

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbility/RaveGameplayAbility.h"
#include "RaveGameplayAbility_Evade.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveGameplayAbility_Evade : public URaveGameplayAbility
{
	GENERATED_BODY()

public:
	URaveGameplayAbility_Evade();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void HandleMontageComplete();
	UFUNCTION()
	void HandleMontageBlendOut();
	UFUNCTION()
	void HandleMontageInterrupt();
	UFUNCTION()
	void HandleMontageCancel();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<class UAnimMontage> EvadeMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float EvadePlayRate = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float EvadeRootMotionTranslationScale = 1.0f;
};
