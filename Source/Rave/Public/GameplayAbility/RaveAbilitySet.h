

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AttributeSet.h"
#include "GameplayAbilitySpecHandle.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayTagContainer.h"
#include "RaveAbilitySet.generated.h"

/**
 * FRaveAbilitySet_GameplayAbility
 *
 *	Data used by the ability set to grant gameplay abilities.
 */
USTRUCT(BlueprintType)
struct FRaveAbilitySet_GameplayAbility
{
	GENERATED_BODY()

public:

	// Gameplay ability to grant.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UGameplayAbility> Ability;

	// Level of ability to grant.
	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;

	// Tag used to process input for the ability.
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};

/**
 * FRaveAbilitySet_GameplayEffect
 *
 *	Data used by the ability set to grant gameplay effects.
 */
USTRUCT(BlueprintType)
struct FRaveAbilitySet_GameplayEffect
{
	GENERATED_BODY()

public:

	// Gameplay effect to grant.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UGameplayEffect> GameplayEffect;

	// Level of gameplay effect to grant.
	UPROPERTY(EditDefaultsOnly)
	float EffectLevel = 1.0f;
};

/**
 * FRaveAbilitySet_AttributeSet
 *
 *	Data used by the ability set to grant attribute sets.
 */
USTRUCT(BlueprintType)
struct FRaveAbilitySet_AttributeSet
{
	GENERATED_BODY()

public:
	// Gameplay effect to grant.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAttributeSet> AttributeSet;

};

/**
 * FRaveAbilitySet_GrantedHandles
 *
 *	Data used to store handles to what has been granted by the ability set.
 */
USTRUCT(BlueprintType)
struct FRaveAbilitySet_GrantedHandles
{
	GENERATED_BODY()

public:

	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle);
	void AddAttributeSet(UAttributeSet* Set);

	void TakeFromAbilitySystem(class URaveAbilitySystemComponent* ASC);

protected:

	// Handles to the granted abilities.
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	// Handles to the granted gameplay effects.
	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

	// Pointers to the granted attribute sets
	UPROPERTY()
	TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;
};

/**
 * 
 */
UCLASS()
class RAVE_API URaveAbilitySet : public UDataAsset
{
	GENERATED_BODY()
	
public:
	URaveAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Grants the ability set to the specified ability system component.
	// The returned handles can be used later to take away anything that was granted.
	void GiveToAbilitySystem(class UAbilitySystemComponent* ASC, FRaveAbilitySet_GrantedHandles* OutGrantedHandles = nullptr, UObject* SourceObject = nullptr) const;

protected:

	// Gameplay abilities to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities", meta = (TitleProperty = Ability))
	TArray<FRaveAbilitySet_GameplayAbility> GrantedGameplayAbilities;

	// Gameplay effects to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects", meta = (TitleProperty = GameplayEffect))
	TArray<FRaveAbilitySet_GameplayEffect> GrantedGameplayEffects;

	// Attribute sets to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Attribute Sets", meta = (TitleProperty = AttributeSet))
	TArray<FRaveAbilitySet_AttributeSet> GrantedAttributes;
};
