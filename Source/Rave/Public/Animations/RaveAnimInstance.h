

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "RaveAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void PushDisableRootMotion(UAnimMontage* Montage, float Duration = 0.0f);
	void PopDisableRootMotion(UAnimMontage* Montage);
	void PlayHitReact();

protected:
	virtual bool ShouldDoFootIK() const;

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Reference")
	TWeakObjectPtr<class ACharacter> Character;

	UPROPERTY(BlueprintReadWrite, Category = "Reference")
	TWeakObjectPtr<class URaveCharacterMovementComponent> CharacterMovementComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float GroundSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	bool ShouldMove;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	bool IsSprinting;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	bool IsFalling;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	FVector Velocity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float MoveDirection;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	FVector InputVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float InputForwardDelta;

	/** Roll delta between control rotation and owning actor rotation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Control")
	float ControlRoll;

	/** Pitch delta between control rotation and owning actor rotation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Control")
	float ControlPitch;

	/** Yaw delta between control rotation and owning actor rotation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Control")
	float ControlYaw;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Control")
	float RotationYawDelta;
	float RotationYawDeltaInterpSpeed = 6.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Control")
	FRotator RotationLastTick;

	/** if character should play full body animation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
	bool bFullBody;

	/** if character should play full body animation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rig")
	bool bShouldDoFootIK;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Hit")
	bool bShouldHitReact;
	bool bShouldHitReactFlag;
};
