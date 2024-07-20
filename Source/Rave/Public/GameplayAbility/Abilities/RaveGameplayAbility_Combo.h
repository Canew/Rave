

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbility/RaveGameplayAbility.h"
#include "RaveGameplayAbility_Combo.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveGameplayAbility_Combo : public URaveGameplayAbility
{
	GENERATED_BODY()

public:
	URaveGameplayAbility_Combo();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	bool CanActivateComboAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const;
	bool ActivateComboAbility();

	UFUNCTION()
	void HandleBeginComboEventReceived(FGameplayEventData Payload);
	UFUNCTION()
	void HandleEndComboEventReceived(FGameplayEventData Payload);
	UFUNCTION()
	void HandleInputPress(float TimeWaited);
	UFUNCTION()
	void HandleAbilityEnded(UGameplayAbility* Ability);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Rave|Ability")
	TArray<TSubclassOf<URaveGameplayAbility>> ComboAbilities;

	TObjectPtr<class UAbilityTask_WaitGameplayEvent> WaitBeginComboEventTask;
	TObjectPtr<class UAbilityTask_WaitGameplayEvent> WaitEndComboEventTask;
	TObjectPtr<class UAbilityTask_WaitInputPress> WaitInputPressTask;
	TObjectPtr<class URaveAbilityTask_WaitAbilityEnd> WaitAbilityEndTask;

	int32 CurrentComboAbilityIndex;
};
