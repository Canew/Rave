


#include "Components/RaveAttributeComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameplayAbility/RaveAbilitySystemComponent.h"
#include "GameplayAbility/RaveAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "RaveGameplayTags.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInterface.h"

// Sets default values for this component's properties
URaveAttributeComponent::URaveAttributeComponent()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	AbilitySystemComponent = nullptr;
	HealthAttributeSet = nullptr;
	LifeState = ERaveLifeState::Alive;
}

void URaveAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URaveAttributeComponent, LifeState);
}

void URaveAttributeComponent::InitializeWithAbilitySystem(URaveAbilitySystemComponent* InAbilitySystemComponent)
{
	AActor* Owner = GetOwner();
	check(Owner);

	if (AbilitySystemComponent)
	{
		return;
	}

	AbilitySystemComponent = InAbilitySystemComponent;
	if (!AbilitySystemComponent)
	{
		return;
	}

	HealthAttributeSet = AbilitySystemComponent->GetSet<URaveAttributeSet>();
	if (!HealthAttributeSet)
	{
		return;
	}

	// Register to listen for attribute changes.
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(URaveAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::HandleHealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(URaveAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &ThisClass::HandleMaxHealthChanged);

	// TEMP: Reset attributes to default values.  Eventually this will be driven by a spread sheet.
	AbilitySystemComponent->SetNumericAttributeBase(URaveAttributeSet::GetHealthAttribute(), HealthAttributeSet->GetMaxHealth());

	OnHealthChanged.Broadcast(HealthAttributeSet->GetHealth(), HealthAttributeSet->GetHealth(), nullptr);
	OnMaxHealthChanged.Broadcast(HealthAttributeSet->GetHealth(), HealthAttributeSet->GetHealth(), nullptr);
}

void URaveAttributeComponent::UninitializeFromAbilitySystem()
{
	HealthAttributeSet = nullptr;
	AbilitySystemComponent = nullptr;
}

float URaveAttributeComponent::GetHealth() const
{
	return (HealthAttributeSet ? HealthAttributeSet->GetHealth() : 0.0f);
}

float URaveAttributeComponent::GetMaxHealth() const
{
	return (HealthAttributeSet ? HealthAttributeSet->GetMaxHealth() : 0.0f);
}

float URaveAttributeComponent::GetHealthRatio() const
{
	if (HealthAttributeSet)
	{
		const float Health = HealthAttributeSet->GetHealth();
		const float MaxHealth = HealthAttributeSet->GetMaxHealth();

		return ((MaxHealth > 0.0f) ? (Health / MaxHealth) : 0.0f);
	}

	return 0.0f;
}

void URaveAttributeComponent::StartDeath()
{
	if (LifeState != ERaveLifeState::Alive)
	{
		return;
	}

	LifeState = ERaveLifeState::DeathStarted;

	AActor* Owner = GetOwner();
	check(Owner);

	OnDeathStarted.Broadcast(Owner);

	Owner->ForceNetUpdate();
}

void URaveAttributeComponent::FinishDeath()
{
	if (LifeState != ERaveLifeState::DeathStarted)
	{
		return;
	}

	LifeState = ERaveLifeState::DeathFinished;

	AActor* Owner = GetOwner();
	check(Owner);

	OnDeathFinished.Broadcast(Owner);

	Owner->ForceNetUpdate();
}

void URaveAttributeComponent::OnUnregister()
{
	UninitializeFromAbilitySystem();

	Super::OnUnregister();
}

static AActor* GetInstigatorFromAttrChangeData(const FOnAttributeChangeData& ChangeData)
{
	if (ChangeData.GEModData != nullptr)
	{
		const FGameplayEffectContextHandle& EffectContext = ChangeData.GEModData->EffectSpec.GetEffectContext();
		return EffectContext.GetOriginalInstigator();
	}

	return nullptr;
}

static AActor* GetEffectCauserFromAttrChangeData(const FOnAttributeChangeData& ChangeData)
{
	if (ChangeData.GEModData != nullptr)
	{
		const FGameplayEffectContextHandle& EffectContext = ChangeData.GEModData->EffectSpec.GetEffectContext();
		return EffectContext.GetEffectCauser();
	}

	return nullptr;
}


void URaveAttributeComponent::HandleHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	OnHealthChanged.Broadcast(ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttrChangeData(ChangeData));

	if (ChangeData.NewValue <= 0.0f)
	{
		FGameplayEffectSpec DamageEffectSpec = ChangeData.GEModData ? ChangeData.GEModData->EffectSpec : FGameplayEffectSpec();
		float DamageMagnitude = ChangeData.GEModData ? ChangeData.GEModData->EvaluatedData.Magnitude : 0.0f;
		HandleOutOfHealth(GetInstigatorFromAttrChangeData(ChangeData), GetEffectCauserFromAttrChangeData(ChangeData), DamageEffectSpec, DamageMagnitude);
	}
}

void URaveAttributeComponent::HandleMaxHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	OnMaxHealthChanged.Broadcast(ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttrChangeData(ChangeData));
}

void URaveAttributeComponent::HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude)
{
#if WITH_SERVER_CODE
	if (AbilitySystemComponent)
	{
		// Send the "GameplayEvent.Death" gameplay event through the owner's ability system.  This can be used to trigger a death gameplay ability.
		{
			FGameplayEventData Payload;
			Payload.EventTag = RaveGameplayTags::GameplayEvent_Death;
			Payload.Instigator = DamageInstigator;
			Payload.Target = AbilitySystemComponent->GetAvatarActor();
			Payload.OptionalObject = DamageEffectSpec.Def;
			Payload.ContextHandle = DamageEffectSpec.GetEffectContext();
			Payload.InstigatorTags = *DamageEffectSpec.CapturedSourceTags.GetAggregatedTags();
			Payload.TargetTags = *DamageEffectSpec.CapturedTargetTags.GetAggregatedTags();
			Payload.EventMagnitude = DamageMagnitude;

			FScopedPredictionWindow NewScopedWindow(AbilitySystemComponent, true);
			AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
		}
	}

	StartDeath();

#endif // #if WITH_SERVER_CODE
}

void URaveAttributeComponent::OnRep_LifeState(ERaveLifeState OldLifeState)
{
	const ERaveLifeState NewLifeState = LifeState;

	// Revert the death state for now since we rely on StartDeath and FinishDeath to change it.
	LifeState = OldLifeState;

	if (OldLifeState > NewLifeState)
	{
		// The server is trying to set us back but we've already predicted past the server state.
		return;
	}

	if (OldLifeState == ERaveLifeState::Alive)
	{
		if (NewLifeState == ERaveLifeState::DeathStarted)
		{
			StartDeath();
		}
		else if (NewLifeState == ERaveLifeState::DeathFinished)
		{
			StartDeath();
			FinishDeath();
		}
	}
	else if (OldLifeState == ERaveLifeState::DeathStarted)
	{
		if (NewLifeState == ERaveLifeState::DeathFinished)
		{
			FinishDeath();
		}
	}
}
