

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbility/RaveGameplayAbility.h"
#include "RaveGameplayAbility_Climb.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveGameplayAbility_Climb : public URaveGameplayAbility
{
	GENERATED_BODY()

public:
	URaveGameplayAbility_Climb();

	/** Returns true if this ability can be activated right now. Has no side effects */
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const;

protected:
	/** Actually activate ability, do not call this directly */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);

	UFUNCTION()
	void MontageCompletedCallback();
	UFUNCTION()
	void MontageBlendOutCallback();
	UFUNCTION()
	void MontageInterruptedCallback();
	UFUNCTION()
	void MontageCancelledCallback();

private:
	bool CheckClimbRoute(const FHitResult& HorizontalHitResult, const FHitResult& VerticalHitResult) const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Climb")
	class UAnimMontage* LowClimbMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Climb")
	class UAnimMontage* HighClimbMontage;

	TWeakObjectPtr<class ACharacter> AvatarCharacter;
	TWeakObjectPtr<class UCapsuleComponent> CharacterCapsule;
	TWeakObjectPtr<class UCharacterMovementComponent> CharacterMovement;

	float MaxClimbHeight;
	float TraceDistance;
	float TraceHeight;
	FName ClimbStartWarpTargetName;
	FName ClimbDestinationWarpTargetName;

	bool bClimbSucceeded = false;
};
