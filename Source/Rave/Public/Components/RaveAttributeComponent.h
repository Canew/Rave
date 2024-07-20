

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayEffectTypes.h"
#include "RaveAttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRaveHealthComponentDead, AActor*, OwningActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FRaveHealthComponentAttributeChanged, float, OldValue, float, NewValue, AActor*, Instigator);

/**
 * ERaveLifeState
 *
 *	Defines current state of death.
 */
UENUM(BlueprintType)
enum class ERaveLifeState : uint8
{
	Alive = 0,
	DeathStarted,
	DeathFinished
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class RAVE_API URaveAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URaveAttributeComponent();

	// Initialize the component using an ability system component.
	UFUNCTION(BlueprintCallable, Category = "Rave|Health")
	void InitializeWithAbilitySystem(URaveAbilitySystemComponent* InAbilitySystemComponent);

	// Uninitialize the component, clearing any references to the ability system.
	UFUNCTION(BlueprintCallable, Category = "Rave|Health")
	void UninitializeFromAbilitySystem();

	// Returns the current health value.
	UFUNCTION(BlueprintCallable, Category = "Rave|Health")
	float GetHealth() const;

	// Returns the current maximum health value.
	UFUNCTION(BlueprintCallable, Category = "Rave|Health")
	float GetMaxHealth() const;

	// Returns the current health in the range [0.0, 1.0].
	UFUNCTION(BlueprintCallable, Category = "Rave|Health")
	float GetHealthRatio() const;

	UFUNCTION(BlueprintCallable, Category = "Rave|Health")
	ERaveLifeState GetLifeState() const { return LifeState; }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Rave|Health", Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool IsAlive() const { return (LifeState == ERaveLifeState::Alive); }

	// Begins the death sequence for the owner.
	virtual void StartDeath();

	// Ends the death sequence for the owner.
	virtual void FinishDeath();

public:
	// Delegate fired when the health value has changed.
	UPROPERTY(BlueprintAssignable)
	FRaveHealthComponentAttributeChanged OnHealthChanged;

	// Delegate fired when the max health value has changed.
	UPROPERTY(BlueprintAssignable)
	FRaveHealthComponentAttributeChanged OnMaxHealthChanged;

	// Delegate fired when the death sequence has started.
	UPROPERTY(BlueprintAssignable)
	FRaveHealthComponentDead OnDeathStarted;

	// Delegate fired when the death sequence has finished.
	UPROPERTY(BlueprintAssignable)
	FRaveHealthComponentDead OnDeathFinished;

protected:
	virtual void OnUnregister() override;

	virtual void HandleHealthChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandleMaxHealthChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude);

	UFUNCTION()
	virtual void OnRep_LifeState(ERaveLifeState OldDeathState);

protected:
	// Ability system used by this component.
	UPROPERTY()
	TObjectPtr<URaveAbilitySystemComponent> AbilitySystemComponent;

	// Health set used by this component.
	UPROPERTY()
	TObjectPtr<const class URaveAttributeSet> HealthAttributeSet;

	// Replicated state used to handle dying.
	UPROPERTY(ReplicatedUsing = OnRep_LifeState)
	ERaveLifeState LifeState;
};
