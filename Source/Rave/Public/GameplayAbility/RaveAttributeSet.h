

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "RaveAttributeSet.generated.h"

/**
 * macro defines a set of helper functions for accessing and initializing attributes.
 */
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class RAVE_API URaveAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	URaveAttributeSet();

	UWorld* GetWorld() const override;

	ATTRIBUTE_ACCESSORS(URaveAttributeSet, Health);
	ATTRIBUTE_ACCESSORS(URaveAttributeSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(URaveAttributeSet, Mana);
	ATTRIBUTE_ACCESSORS(URaveAttributeSet, MaxMana);
	ATTRIBUTE_ACCESSORS(URaveAttributeSet, AttackPower);
	ATTRIBUTE_ACCESSORS(URaveAttributeSet, AttackSpeed);
	ATTRIBUTE_ACCESSORS(URaveAttributeSet, MovementSpeed);
	ATTRIBUTE_ACCESSORS(URaveAttributeSet, Armor);
	ATTRIBUTE_ACCESSORS(URaveAttributeSet, Resistance);
	ATTRIBUTE_ACCESSORS(URaveAttributeSet, Level);
	ATTRIBUTE_ACCESSORS(URaveAttributeSet, Experience);
	ATTRIBUTE_ACCESSORS(URaveAttributeSet, MaxExperience);

	ATTRIBUTE_ACCESSORS(URaveAttributeSet, Healing);
	ATTRIBUTE_ACCESSORS(URaveAttributeSet, Damage);

protected:
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_AttackPower(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_AttackSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MovementSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Resistance(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Level(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Experience(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxExperience(const FGameplayAttributeData& OldValue);

	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const override;

protected:
	void HandleDamageAttributeChange(const struct FGameplayEffectModCallbackData& Data);
	void HandleLevelAttributeChange(const struct FGameplayEffectModCallbackData& Data);
	void HandleExperienceAttributeChange(const struct FGameplayEffectModCallbackData& Data);

public:
	static float MIN_HEALTH;
	static float MIN_MANA;
	static float MIN_ATTACK_DAMAGE;
	static float MIN_ATTACK_SPEED;
	static float MAX_ATTACK_SPEED;
	static float MIN_MOVEMENT_SPEED;
	static float MAX_MOVEMENT_SPEED;
	static float MIN_LEVEL;

protected:
	// The current health attribute. The health will be capped by the max health attribute. Health is hidden from modifiers so only executions can modify it.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Rave|Health", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	// The current max health attribute. Max health is an attribute since gameplay effects can modify it.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Rave|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	// The current mana attribute. The mana will be capped by the max mana attribute. Mana is hidden from modifiers so only executions can modify it.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Rave|Mana", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Mana;

	// The current max mana attribute. Max mana is an attribute since gameplay effects can modify it.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Rave|Mana", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxMana;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackPower, Category = "Rave|Attack", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackPower;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackSpeed, Category = "Rave|Attack", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackSpeed;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MovementSpeed, Category = "Rave|Movement", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MovementSpeed;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor, Category = "Rave|Defense", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Armor;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Resistance, Category = "Rave|Defense", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Resistance;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Level, Category = "Rave|Level", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Level;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Experience, Category = "Rave|Level", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Experience;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxExperience, Category = "Rave|Level", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxExperience;

protected:
	//	Meta Attribute (please keep attributes that aren't 'stateful' below 

	// Incoming damage. This is mapped directly to -Health
	UPROPERTY(BlueprintReadOnly, Category = "Rave|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Damage;

	// Incoming healing. This is mapped directly to +Health
	UPROPERTY(BlueprintReadOnly, Category = "Rave|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Healing;
};
