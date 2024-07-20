

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RaveGameplayAbility.generated.h"

/**
 * ERaveAbilityActivationPolicy
 *
 *	Defines how an ability is meant to activate.
 */
UENUM(BlueprintType)
enum class ERaveAbilityActivationPolicy : uint8
{
	// Try to activate the ability when the input is triggered.
	OnInputTriggered,

	// Continually try to activate the ability while the input is active.
	WhileInputActive,

	// Try to activate the ability when an avatar is assigned.
	OnSpawn,

	// Try to activate only when event is received.
	OnEventReceived
};


/**
 * ERaveAbilityActivationGroup
 *
 *	Defines how an ability activates in relation to other abilities.
 */
UENUM(BlueprintType)
enum class ERaveAbilityActivationGroup : uint8
{
	// Ability runs independently of all other abilities.
	Independent,

	// Ability is canceled and replaced by other exclusive abilities.
	Exclusive_Replaceable,

	// Ability blocks all other exclusive abilities from activating.
	Exclusive_Blocking,

	MAX	UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FRaveAbilityDescription
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (MultiLine = "true"))
	FText Description;
};

UENUM(BlueprintType)
enum class URaveAbilitySequenceType : uint8
{
	None,
	Chain
};

UENUM(BlueprintType)
enum class FRaveGrantedEffectActivationPolicy : uint8
{
	Always,
	OnActivated,
	NotOnActivated,
};

USTRUCT(BlueprintType)
struct FRaveGrantedGameplayEffectDef
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> EffectClass;

	UPROPERTY(EditDefaultsOnly)
	FScalableFloat EffectLevel;

	UPROPERTY(EditDefaultsOnly)
	FRaveGrantedEffectActivationPolicy EffectActivationPolicy;

	UPROPERTY(EditDefaultsOnly)
	bool bClearOnAbilityRemoved = true;

	UPROPERTY()
	FActiveGameplayEffectHandle ActiveEffectHandle;
};

/**
 * 
 */
UCLASS()
class RAVE_API URaveGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	URaveGameplayAbility();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	ERaveAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }
	ERaveAbilityActivationGroup GetActivationGroup() const { return ActivationGroup; }
	FRaveAbilityDescription GetAbilityDescription() const { return AbilityDescription; }
	URaveAbilitySequenceType GetAbilitySequenceType() const { return AbilitySequenceType; }
	TArray<FRaveGrantedGameplayEffectDef> GetGrantedEffects() const { return GrantedEffects; }
	float GetCooldownDuration() const { return CooldownTime; };
	float GetCostMagnitude(FGameplayAttribute Attribute) const;

protected:
	virtual void PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData = nullptr) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual const FGameplayTagContainer* GetCooldownTags() const override;
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

	UFUNCTION(BlueprintPure, Category = "Rave|Ability")
	virtual class URaveAbilitySystemComponent* GetRaveAbilitySystemComponentFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "Rave|Ability")
	virtual int32 GetPredictionKey() const;

	UFUNCTION(BlueprintPure, Category = "Rave|Ability")
	FGameplayEffectSpecHandle MakeGameplayEffectSpecFromHitResult(TSubclassOf<UGameplayEffect> GameplayEffectClass, float GameplayEffectLevel = 1.0f, const FHitResult& HitResult = FHitResult());

	UFUNCTION(BlueprintPure, Category = "Rave|Ability", meta = (AdvancedDisplay = "OptionalObject, OptionalObject2"))
	FGameplayEventData MakeGameplayEventData(FGameplayTag EventTag, float EventMagnitude = 0.0f, const FHitResult& HitResult = FHitResult(), UObject* OptionalObject = nullptr, UObject* OptionalObject2 = nullptr) const;

	UFUNCTION(BlueprintCallable, Category = "Rave|Ability")
	void AddDynamicAbilityTag(FGameplayTag DynamicAbilityTag);
	UFUNCTION(BlueprintCallable, Category = "Rave|Ability")
	void AddDynamicAbilityTags(FGameplayTagContainer DynamicAbilityTags);
	UFUNCTION(BlueprintCallable, Category = "Rave|Ability")
	void RemoveDynamicAbilityTag(FGameplayTag DynamicAbilityTag);
	UFUNCTION(BlueprintCallable, Category = "Rave|Ability")
	void RemoveDynamicAbilityTags(FGameplayTagContainer DynamicAbilityTags);

protected:
	// Defines how this ability is meant to activate.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rave|Ability Activation")
	ERaveAbilityActivationPolicy ActivationPolicy;

	// Defines the relationship between this ability activating and other abilities activating.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rave|Ability Activation")
	ERaveAbilityActivationGroup ActivationGroup;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rave|Ability")
	bool bInjectRecentInputWhenEndAbility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rave|Description")
	FRaveAbilityDescription AbilityDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rave|Sequence")
	URaveAbilitySequenceType AbilitySequenceType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cooldowns")
	float CooldownTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Granted Effects")
	TArray<FRaveGrantedGameplayEffectDef> GrantedEffects;

	// Temp container that we will return the pointer to in GetCooldownTags().
	// This will be a union of our CooldownTags and the Cooldown GE's cooldown tags.
	UPROPERTY(Transient)
	FGameplayTagContainer CooldownTagsContainer;


};
