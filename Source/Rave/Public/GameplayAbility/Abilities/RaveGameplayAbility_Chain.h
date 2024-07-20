

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbility/RaveGameplayAbility.h"
#include "RaveGameplayAbility_Chain.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveGameplayAbility_Chain : public URaveGameplayAbility
{
	GENERATED_BODY()
	
public:
	URaveGameplayAbility_Chain();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	float GetMaxChainInterval() const;
	float GetRemainingChainDuration() const;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	bool CanActivateChainAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const;
	bool ActivateChainAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);

	UFUNCTION()
	void HandleInputPress(float TimeWaited);
	UFUNCTION()
	void HandleAbilityEnded(UGameplayAbility* Ability);

	void WaitInputPress();
	void HandleChainTimerElapsed();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Rave|Ability")
	TArray<TSubclassOf<URaveGameplayAbility>> ChainAbilities;

	// MaxChainInterval == 0.0f, instantly end ability when end sub ability.
	// MaxChainInterval < 0.0f, has infinite chain interval.
	UPROPERTY(EditDefaultsOnly, Category = "Rave|Ability")
	float MaxChainInterval;

	FTimerHandle ChainTimerHandle;
	bool bCanActivateChainAbility;

	TObjectPtr<class UAbilityTask_WaitInputPress> WaitInputPressTask;
	TObjectPtr<class URaveAbilityTask_WaitAbilityEnd> WaitAbilityEndTask;

	int32 CurrentChainAbilityIndex;
};
