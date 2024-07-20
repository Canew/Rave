


#include "GameplayAbility/Abilities/RaveGameplayAbility_Climb.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MotionWarpingComponent.h"
#include "DrawDebugHelpers.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveGameplayAbility_Climb)

URaveGameplayAbility_Climb::URaveGameplayAbility_Climb()
{
	ActivationGroup = ERaveAbilityActivationGroup::Exclusive_Blocking;

	MaxClimbHeight = 150.0f;
	TraceDistance = 50.0f;
	TraceHeight = 250.0f;
	ClimbStartWarpTargetName = TEXT("ClimbStart");
	ClimbDestinationWarpTargetName = TEXT("ClimbDestination");
}

bool URaveGameplayAbility_Climb::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	if (!LowClimbMontage || !HighClimbMontage)
	{
		return false;
	}

	return true;
}

void URaveGameplayAbility_Climb::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	bClimbSucceeded = false;

	AvatarCharacter = Cast<ACharacter>(ActorInfo->AvatarActor);
	if (!IsValid(AvatarCharacter.Get()))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	CharacterCapsule = AvatarCharacter->GetCapsuleComponent();
	CharacterMovement = AvatarCharacter->GetCharacterMovement();
	if (!IsValid(CharacterCapsule.Get()) || !IsValid(CharacterMovement.Get()))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// Horizontal trace
	FHitResult HorizontalHitResult;
	FCollisionObjectQueryParams CollisionObjectQueryParams;
	CollisionObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
	CollisionObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldDynamic);
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(GetAvatarActorFromActorInfo());
	bool bHitHorizontalTrace = AvatarCharacter->GetWorld()->SweepSingleByObjectType(
		HorizontalHitResult,
		AvatarCharacter->GetActorLocation(),
		AvatarCharacter->GetActorLocation() + AvatarCharacter->GetActorForwardVector() * TraceDistance,
		FQuat::Identity,
		CollisionObjectQueryParams,
		FCollisionShape::MakeSphere(0.5f * CharacterCapsule->GetScaledCapsuleRadius()),
		CollisionQueryParams);

	// Vertical trace
	FHitResult VerticalHitResult;
	bool bHitVerticalTrace = AvatarCharacter->GetWorld()->SweepSingleByObjectType(
		VerticalHitResult,
		AvatarCharacter->GetActorLocation() + AvatarCharacter->GetActorUpVector() * TraceHeight + AvatarCharacter->GetActorForwardVector() * TraceDistance,
		AvatarCharacter->GetActorLocation() + AvatarCharacter->GetActorForwardVector() * TraceDistance,
		FQuat::Identity,
		CollisionObjectQueryParams,
		FCollisionShape::MakeSphere(0.5f * CharacterCapsule->GetScaledCapsuleRadius()),
		CollisionQueryParams);

	float HeightDifference = VerticalHitResult.ImpactPoint.Z - CharacterCapsule->GetComponentLocation().Z;
	if (!bHitHorizontalTrace || !bHitVerticalTrace || FMath::Abs(HeightDifference) > MaxClimbHeight || !CheckClimbRoute(HorizontalHitResult, VerticalHitResult))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// Play climb montage
	UAnimMontage* ClimbMontage =
		(CharacterMovement->IsFalling() || HeightDifference > CharacterCapsule->GetScaledCapsuleHalfHeight() / 2.0f)
		? HighClimbMontage : LowClimbMontage;

	float ClimbMontageStartTime = 0.0f;
	if (ClimbMontage == HighClimbMontage)
	{
		ClimbMontageStartTime = FMath::GetMappedRangeValueClamped(FVector2D(CharacterCapsule->GetScaledCapsuleHalfHeight() * 2.0f, 0.0f), FVector2D(0.0f, 0.2f), HeightDifference);
	}

	if (UAbilityTask_PlayMontageAndWait* PlayClimbMontageAndWait = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ClimbMontage, 1.0f, NAME_None, true, 1.0f, ClimbMontageStartTime))
	{
		CharacterMovement->SetMovementMode(MOVE_Flying);

		UMotionWarpingComponent* MotionWarpingComponent = AvatarCharacter->GetComponentByClass<UMotionWarpingComponent>();
		if (MotionWarpingComponent)
		{
			FVector ClimbStartWarpTargetLocation = HorizontalHitResult.ImpactPoint - (AvatarCharacter->GetActorForwardVector() * CharacterCapsule->GetScaledCapsuleRadius());
			ClimbStartWarpTargetLocation.Z = VerticalHitResult.ImpactPoint.Z;
			FVector ClimbDestinationWarpTargetLocation = HorizontalHitResult.ImpactPoint + (AvatarCharacter->GetActorForwardVector() * CharacterCapsule->GetScaledCapsuleRadius());
			ClimbDestinationWarpTargetLocation.Z = VerticalHitResult.ImpactPoint.Z;

			MotionWarpingComponent->AddOrUpdateWarpTargetFromLocation(ClimbStartWarpTargetName, ClimbStartWarpTargetLocation);
			MotionWarpingComponent->AddOrUpdateWarpTargetFromLocation(ClimbDestinationWarpTargetName, ClimbDestinationWarpTargetLocation);
		}

		PlayClimbMontageAndWait->OnCompleted.AddDynamic(this, &ThisClass::MontageCompletedCallback);
		PlayClimbMontageAndWait->OnBlendOut.AddDynamic(this, &ThisClass::MontageBlendOutCallback);
		PlayClimbMontageAndWait->OnInterrupted.AddDynamic(this, &ThisClass::MontageInterruptedCallback);
		PlayClimbMontageAndWait->OnCancelled.AddDynamic(this, &ThisClass::MontageCancelledCallback);
		PlayClimbMontageAndWait->ReadyForActivation();

		bClimbSucceeded = true;
	}
}

void URaveGameplayAbility_Climb::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (bClimbSucceeded)
	{
		if (IsValid(AvatarCharacter.Get()))
		{
			if (IsValid(CharacterMovement.Get()))
			{
				CharacterMovement->SetMovementMode(MOVE_Walking);
			}

			UMotionWarpingComponent* MotionWarpingComponent = AvatarCharacter->GetComponentByClass<UMotionWarpingComponent>();
			if (MotionWarpingComponent)
			{
				MotionWarpingComponent->RemoveWarpTarget(ClimbStartWarpTargetName);
				MotionWarpingComponent->RemoveWarpTarget(ClimbDestinationWarpTargetName);
			}
		}
	}
}

void URaveGameplayAbility_Climb::MontageCompletedCallback()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void URaveGameplayAbility_Climb::MontageBlendOutCallback()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void URaveGameplayAbility_Climb::MontageInterruptedCallback()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void URaveGameplayAbility_Climb::MontageCancelledCallback()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

bool URaveGameplayAbility_Climb::CheckClimbRoute(const FHitResult& HorizontalHitResult, const FHitResult& VerticalHitResult) const
{
	FCollisionObjectQueryParams CollisionObjectQueryParams;
	CollisionObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
	CollisionObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldDynamic);

	FHitResult ClimbRouteHitResult;
	FVector VerticalRouteStart = AvatarCharacter->GetActorLocation() + AvatarCharacter->GetActorUpVector();
	FVector VerticalRouteEnd = VerticalRouteStart + AvatarCharacter->GetActorUpVector() * (VerticalHitResult.ImpactPoint.Z - AvatarCharacter->GetActorLocation().Z + CharacterCapsule->GetScaledCapsuleHalfHeight() + CharacterCapsule->GetScaledCapsuleRadius());
	bool bHitClimbRouteTrace = AvatarCharacter->GetWorld()->SweepSingleByObjectType(
		ClimbRouteHitResult,
		VerticalRouteStart,
		VerticalRouteEnd,
		FQuat::Identity,
		CollisionObjectQueryParams,
		FCollisionShape::MakeSphere(0.5f * CharacterCapsule->GetScaledCapsuleRadius()));

	if (!bHitClimbRouteTrace)
	{
		FVector HorizontalRouteStart = FVector(HorizontalHitResult.ImpactPoint.X, HorizontalHitResult.ImpactPoint.Y, VerticalHitResult.ImpactPoint.Z + CharacterCapsule->GetScaledCapsuleHalfHeight() + CharacterCapsule->GetScaledCapsuleRadius());
		FVector HorizontalRouteEnd = HorizontalRouteStart + AvatarCharacter->GetActorForwardVector() * CharacterCapsule->GetScaledCapsuleRadius();
		return !AvatarCharacter->GetWorld()->SweepSingleByObjectType(
			ClimbRouteHitResult,
			VerticalRouteEnd,
			HorizontalRouteEnd,
			FQuat::Identity,
			CollisionObjectQueryParams,
			FCollisionShape::MakeSphere(0.5f * CharacterCapsule->GetScaledCapsuleRadius()));
	}

	return false;
}
