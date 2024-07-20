

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbility/RaveGameplayAbility.h"
#include "GameplayAbilitySpec.h"
#include "NativeGameplayTags.h"
#include "RaveAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FRaveAbilityAdded, FGameplayAbilitySpec&)
DECLARE_MULTICAST_DELEGATE_OneParam(FRaveAbilityRemoved, FGameplayAbilitySpec&)
DECLARE_MULTICAST_DELEGATE_TwoParams(FRaveAbilityLevelChanged, FGameplayAbilitySpec&, int32)

/**
 * 
 */
UCLASS()
class RAVE_API URaveAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	URaveAbilitySystemComponent();

	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
	void ClearAbilityInput();

	void GetActivatableGameplayAbilitySpecsByAllMatchingDynamicTags(const FGameplayTagContainer& GameplayTagContainer, TArray <struct FGameplayAbilitySpec*>& MatchingGameplayAbilities, bool bOnlyAbilitiesThatSatisfyTagRequirements = true) const;
	void AssignInputTag(FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& InputTag);

	int32 GetRemainingSkillPoint() const;
	void IncreaseAbilityLevel(FGameplayAbilitySpec& AbilitySpec);

protected:
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	virtual void OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec) override;

	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

	virtual void AbilityInstanceInputPressed(FGameplayAbilitySpec& Spec);
	virtual void AbilityInstanceInputReleased(FGameplayAbilitySpec& Spec);

public:
	FRaveAbilityAdded OnAbilityAdded;
	FRaveAbilityRemoved OnAbilityRemoved;
	FRaveAbilityLevelChanged OnAbilityLevelChanged;

protected:
	// Handles to abilities that had their input pressed this frame.
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	// Handles to abilities that had their input released this frame.
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;

	// Handles to abilities that have their input held.
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

#pragma region Action Buffer
public:
	UFUNCTION(BlueprintCallable, Category = "Rave|Input")
	virtual void SetRecentInputAction(const FGameplayAbilitySpecHandle& NewRecentInputAbilitySpecHandle);

	UFUNCTION(BlueprintCallable, Category = "Rave|Input")
	virtual void InjectRecentInputForAction();

	virtual void ClearRecentInputAction();

protected:
	FGameplayAbilitySpecHandle RecentInputAbilitySpecHandle;

	FTimerHandle ActionBufferTimerHandle;
	float ActionBufferDuration;

#pragma endregion
};
