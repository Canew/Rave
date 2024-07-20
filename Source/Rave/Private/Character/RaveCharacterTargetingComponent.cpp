


#include "Character/RaveCharacterTargetingComponent.h"
#include "Character/RaveCharacter.h"
#include "Components/RaveAttributeComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "MotionWarpingComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveCharacterTargetingComponent)

URaveCharacterTargetingComponent::URaveCharacterTargetingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.0f;

	LockOnRadius = 150.0f;
	MaxLockOnDistance = 2500.0f;
	LockCameraInterpSpeed = 4.0f;
}

void URaveCharacterTargetingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsValid(LockOnActor.Get()) && !CanLockOn(LockOnActor.Get()))
	{
		SetLockOnActor(nullptr);
	}

	DesiredOrientation = FVector::ZeroVector;

	UpdateWarpTarget();
	LockCamera(DeltaTime);
}

void URaveCharacterTargetingComponent::ServerUpdateDesiredOrientation_Implementation(FVector Orientation)
{
	UpdateDesiredOrientation(Orientation);
}

void URaveCharacterTargetingComponent::UpdateDesiredOrientation(FVector Orientation)
{
	Orientation.Normalize();
	DesiredOrientation = Orientation;

	UpdateWarpTarget();
}

void URaveCharacterTargetingComponent::LockOn()
{
	if (IsValid(LockOnActor.Get()))
	{
		SetLockOnActor(nullptr);
	}
	else
	{
		AActor* Actor = FindLockOnActor();
		SetLockOnActor(Actor);
	}
}

AActor* URaveCharacterTargetingComponent::GetLockOnActor() const
{
	return LockOnActor.Get();
}

FVector URaveCharacterTargetingComponent::GetOrientationTowardsTarget() const
{
	if (!DesiredOrientation.IsNearlyZero())
	{
		return DesiredOrientation;
	}

	if (GetOwner())
	{
		if (IsValid(LockOnActor.Get()))
		{
			FVector Direction = LockOnActor->GetActorLocation() - GetOwner()->GetActorLocation();
			Direction.Normalize();

			return Direction;
		}
	}

	return FVector::ZeroVector;
}

float URaveCharacterTargetingComponent::GetDistanceToTarget() const
{
	if (GetOwner())
	{
		if (IsValid(LockOnActor.Get()))
		{
			return (LockOnActor->GetActorLocation() - GetOwner()->GetActorLocation()).Length();
		}
	}

	return 0.0f;
}

void URaveCharacterTargetingComponent::SetLockOnActor(AActor* Actor)
{
	if (Actor != LockOnActor)
	{
		LockOnActor = Actor;

		APawn* PawnOwner = GetOwner<APawn>();
		if (PawnOwner && PawnOwner->IsPlayerControlled())
		{
			if (IsValid(Actor))
			{
				ShowLockOnMarker(Actor);
			}
			else
			{
				HiddenLockOnMarker();
			}
		}

		UpdateWarpTarget();

		LockOnActorChanged.Broadcast(Actor);
	}
}

AActor* URaveCharacterTargetingComponent::FindLockOnActor()
{
	APawn* OwnerPawn = GetOwner<APawn>();
	if (!IsValid(OwnerPawn))
	{
		return nullptr;
	}

	UCameraComponent* OwnerCamera = OwnerPawn->FindComponentByClass<UCameraComponent>();
	if (OwnerCamera)
	{
		float SpringArmLength = 0.0f;
		USpringArmComponent* OwnerSpringArm = OwnerPawn->FindComponentByClass<USpringArmComponent>();
		if (OwnerSpringArm)
		{
			SpringArmLength = OwnerSpringArm->TargetArmLength;
		}

		TArray<FHitResult> HitResults;
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(GetOwner());
		GetWorld()->SweepMultiByChannel(
			HitResults,
			OwnerCamera->GetComponentLocation() + (OwnerCamera->GetForwardVector() * SpringArmLength),
			OwnerCamera->GetComponentLocation() + (MaxLockOnDistance * OwnerCamera->GetForwardVector()),
			FQuat::Identity,
			ECC_GameTraceChannel3,
			FCollisionShape::MakeSphere(LockOnRadius),
			CollisionQueryParams);

		for (const FHitResult& HitResult : HitResults)
		{
			if (CanLockOn(HitResult.GetActor()))
			{
				return HitResult.GetActor();
			}
		}
	}

	return nullptr;
}

bool URaveCharacterTargetingComponent::CanLockOn(AActor* Actor) const
{
	APawn* OwnerPawn = GetOwner<APawn>();
	if (!IsValid(OwnerPawn) || !IsValid(Actor) || OwnerPawn == Actor
		|| (OwnerPawn->GetActorLocation() - Actor->GetActorLocation()).Length() > MaxLockOnDistance)
	{
		return false;
	}

	// Unlock if lock on actor is not alive.
	URaveAttributeComponent* OwnerAttributeComponent = Actor->GetComponentByClass<URaveAttributeComponent>();
	if (OwnerAttributeComponent)
	{
		if (!OwnerAttributeComponent->IsAlive())
		{
			return false;
		}
	}

	// Unlock if lock on actor be unseen from both owner and owner's camera.
	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, OwnerPawn->GetActorLocation(), Actor->GetActorLocation(), ECC_GameTraceChannel4))
	{
		UCameraComponent* OwnerCamera = OwnerPawn->FindComponentByClass<UCameraComponent>();
		if (IsValid(OwnerCamera))
		{
			if (GetWorld()->LineTraceSingleByChannel(HitResult, OwnerCamera->GetComponentLocation(), Actor->GetActorLocation(), ECC_GameTraceChannel4))
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	return true;
}

void URaveCharacterTargetingComponent::UpdateWarpTarget()
{
	UMotionWarpingComponent* MotionWarpingComponent = GetOwner()->GetComponentByClass<UMotionWarpingComponent>();
	if (MotionWarpingComponent)
	{
		FName WarpTargetName = TEXT("Target");

		if (!DesiredOrientation.IsNearlyZero())
		{
			MotionWarpingComponent->AddOrUpdateWarpTargetFromLocation(WarpTargetName, GetOwner()->GetActorLocation() + (MaxLockOnDistance * DesiredOrientation));

			return;
		}

		if (IsValid(LockOnActor.Get()))
		{
			MotionWarpingComponent->AddOrUpdateWarpTargetFromComponent(WarpTargetName, LockOnActor->GetRootComponent(), NAME_None, true);

			return;
		}

		MotionWarpingComponent->RemoveWarpTarget(WarpTargetName);
	}
}

void URaveCharacterTargetingComponent::LockCamera(float DeltaTime)
{
	if (!IsValid(LockOnActor.Get()))
	{
		return;
	}

	APawn* PawnOwner = GetOwner<APawn>();
	if (PawnOwner)
	{
		AController* Controller = PawnOwner->GetController();
		if (Controller)
		{
			FVector OwnerLocationOffset = FVector::ZeroVector;
			USpringArmComponent* OwnerSpringArm = PawnOwner->FindComponentByClass<USpringArmComponent>();
			if (OwnerSpringArm)
			{
				OwnerLocationOffset.Z += OwnerSpringArm->GetRelativeLocation().Z;
				OwnerLocationOffset.Z += OwnerSpringArm->SocketOffset.Z;
			}

			FRotator ControlRotation = Controller->GetControlRotation();
			FRotator LockRotation = (LockOnActor->GetActorLocation() - (PawnOwner->GetActorLocation() + OwnerLocationOffset)).Rotation();			
			FRotator NewControlRotation = FMath::RInterpTo(ControlRotation, LockRotation, DeltaTime, LockCameraInterpSpeed);
			Controller->SetControlRotation(NewControlRotation);
		}
	}
}

void URaveCharacterTargetingComponent::ShowLockOnMarker(AActor* Actor)
{
	if (IsValid(LockOnMarker))
	{
		LockOnMarker->GetRootComponent()->SetVisibility(true, true);
		LockOnMarker->AttachToActor(Actor, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
	else
	{
		FTransform SpawnTransform(FRotator::ZeroRotator, Actor->GetActorLocation());
		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.Owner = GetOwner();
		LockOnMarker = GetWorld()->SpawnActor<AActor>(LockOnMarkerClass, SpawnTransform, ActorSpawnParameters);
		if (IsValid(LockOnMarker))
		{
			LockOnMarker->AttachToActor(Actor, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}
	}
}

void URaveCharacterTargetingComponent::HiddenLockOnMarker()
{
	if (IsValid(LockOnMarker))
	{
		LockOnMarker->GetRootComponent()->SetVisibility(false, true);
	}
}
