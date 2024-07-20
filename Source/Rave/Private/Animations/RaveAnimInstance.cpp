


#include "Animations/RaveAnimInstance.h"
#include "GameFramework/Character.h"
#include "Character/RaveCharacterMovementComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveAnimInstance)

void URaveAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<ACharacter>(GetOwningActor());
	if (Character.IsValid())
	{
		CharacterMovementComponent = Cast<URaveCharacterMovementComponent>(Character->GetCharacterMovement());
	}
}

void URaveAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{	
	Super::NativeUpdateAnimation(DeltaSeconds);

	FName FullBodyCurveName = TEXT("FullBody");
	bFullBody = GetCurveValue(FullBodyCurveName) > KINDA_SMALL_NUMBER;

	if (Character.IsValid() && CharacterMovementComponent.IsValid())
	{
		if (CharacterMovementComponent->bOrientRotationToMovement)
		{
			ControlRoll = 0.0f;
			ControlPitch = 0.0f;
			ControlYaw = 0.0f;
		}
		else
		{
			FRotator RotatorDeltaActorAndControl = Character->GetBaseAimRotation() - Character->GetActorRotation();
			RotatorDeltaActorAndControl.Normalize();
			ControlRoll = RotatorDeltaActorAndControl.Roll;
			ControlPitch = RotatorDeltaActorAndControl.Pitch;
			ControlYaw = RotatorDeltaActorAndControl.Yaw;
		}

		if (!RotationLastTick.IsZero())
		{
			FRotator RotationDelta = RotationLastTick - Character->GetActorRotation();
			RotationDelta.Normalize();

			RotationYawDelta = FMath::FInterpTo(RotationYawDelta, RotationDelta.Yaw / (4.0f * DeltaSeconds), DeltaSeconds, RotationYawDeltaInterpSpeed);
		}

		RotationLastTick = Character->GetActorRotation();

		Velocity = CharacterMovementComponent->Velocity;
		GroundSpeed = Velocity.Size();

		FVector CurrentAcceleration = CharacterMovementComponent->GetCurrentAcceleration();
		ShouldMove = (!CurrentAcceleration.IsNearlyZero()) && (GroundSpeed > 1.0f);

		IsSprinting = CharacterMovementComponent->IsSprinting();
		IsFalling = CharacterMovementComponent->IsFalling();

		FVector ForwardVector = FVector(Character->GetActorForwardVector().X, Character->GetActorForwardVector().Y, 0.0f);
		FVector MoveVector = FVector(Character->GetVelocity().X, Character->GetVelocity().Y, 0.0f);
		ForwardVector.Normalize();
		MoveVector.Normalize();
		if (GroundSpeed > 0.0f)
		{
			MoveDirection = FMath::RadiansToDegrees(FMath::Acos(ForwardVector | MoveVector));
		}

		float DistanceToFrontBackPlane = FVector::PointPlaneDist(Character->GetActorLocation() + Character->GetVelocity(), Character->GetActorLocation(), Character->GetActorRightVector());
		if (DistanceToFrontBackPlane < 0)
		{
			MoveDirection *= -1.0f;
		}

		InputVector = CharacterMovementComponent->GetLastInputVector().GetClampedToSize(0.0f, 1.0f);
		InputForwardDelta = (Character->GetActorRotation() - InputVector.Rotation()).Yaw;

		bShouldDoFootIK = ShouldDoFootIK();

		bShouldHitReact = false;
		if (bShouldHitReactFlag)
		{
			bShouldHitReactFlag = false;
			bShouldHitReact = true;
		}
	}
}

void URaveAnimInstance::PushDisableRootMotion(UAnimMontage* Montage, float Duration)
{
	FAnimMontageInstance* AnimMontageInstance = GetActiveInstanceForMontage(Montage);
	if (AnimMontageInstance)
	{
		AnimMontageInstance->PushDisableRootMotion();
		if (Duration > 0.0f)
		{
			FTimerHandle PushDisableRootMotionTimerHandle;
			FTimerDelegate PushDisableRootMotionTimerDelegate;
			PushDisableRootMotionTimerDelegate.BindUObject(this, &ThisClass::PopDisableRootMotion, Montage);
			GetWorld()->GetTimerManager().SetTimer(PushDisableRootMotionTimerHandle, PushDisableRootMotionTimerDelegate, Duration, false);
		}
	}
}

void URaveAnimInstance::PopDisableRootMotion(UAnimMontage* Montage)
{
	FAnimMontageInstance* AnimMontageInstance = GetActiveInstanceForMontage(Montage);
	if (AnimMontageInstance)
	{
		AnimMontageInstance->PopDisableRootMotion();
	}
}

void URaveAnimInstance::PlayHitReact()
{
	bShouldHitReactFlag = true;
}

bool URaveAnimInstance::ShouldDoFootIK() const
{
#if WITH_SERVER_CODE

#endif
	if (!Character.IsValid() || !Character->IsLocallyControlled())
	{
		return false;
	}

	UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Character.Get());
	if (AbilitySystemComponent && AbilitySystemComponent->GetAnimatingAbility())
	{
		return false;
	}

	return true;
}
