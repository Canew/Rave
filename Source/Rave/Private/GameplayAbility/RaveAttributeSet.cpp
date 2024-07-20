


#include "GameplayAbility/RaveAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "Character/RaveCharacter.h"
#include "Player/RavePlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RaveGameMode_Gameplay.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveAttributeSet)

float URaveAttributeSet::MIN_HEALTH = 0.0f;
float URaveAttributeSet::MIN_MANA = 0.0f;
float URaveAttributeSet::MIN_ATTACK_DAMAGE = 0.0f;
float URaveAttributeSet::MIN_ATTACK_SPEED = 0.0f;
float URaveAttributeSet::MAX_ATTACK_SPEED = 1.4f;
float URaveAttributeSet::MIN_MOVEMENT_SPEED = 100.0f;
float URaveAttributeSet::MAX_MOVEMENT_SPEED = 600.0f;
float URaveAttributeSet::MIN_LEVEL = 1.0f;

URaveAttributeSet::URaveAttributeSet()
	: Health(1.0f)
	, MaxHealth(1.0f)
	, Mana(0.0f)
	, MaxMana(0.0f)
	, AttackPower(0.0f)
	, AttackSpeed(1.0f)
	, MovementSpeed(100.0f)
	, Armor(0.0f)
	, Resistance(0.0f)
	, Level(1.0f)
	, Experience(0.0f)
{
}

void URaveAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(URaveAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URaveAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URaveAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URaveAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URaveAttributeSet, AttackPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URaveAttributeSet, AttackSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URaveAttributeSet, MovementSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URaveAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URaveAttributeSet, Resistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URaveAttributeSet, Level, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URaveAttributeSet, Experience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URaveAttributeSet, MaxExperience, COND_None, REPNOTIFY_Always);
}

UWorld* URaveAttributeSet::GetWorld() const
{
	const UObject* Outer = GetOuter();
	check(Outer);

	return Outer->GetWorld();
}

void URaveAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URaveAttributeSet, Health, OldValue);
}

void URaveAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URaveAttributeSet, MaxHealth, OldValue);
}

void URaveAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URaveAttributeSet, Mana, OldValue);
}

void URaveAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URaveAttributeSet, MaxMana, OldValue);
}

void URaveAttributeSet::OnRep_AttackPower(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URaveAttributeSet, AttackPower, OldValue);
}

void URaveAttributeSet::OnRep_AttackSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URaveAttributeSet, AttackSpeed, OldValue);
}

void URaveAttributeSet::OnRep_MovementSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URaveAttributeSet, MovementSpeed, OldValue);
}

void URaveAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URaveAttributeSet, Armor, OldValue);
}

void URaveAttributeSet::OnRep_Resistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URaveAttributeSet, Resistance, OldValue);
}

void URaveAttributeSet::OnRep_Level(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URaveAttributeSet, Level, OldValue);
}

void URaveAttributeSet::OnRep_Experience(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URaveAttributeSet, Experience, OldValue);
}

void URaveAttributeSet::OnRep_MaxExperience(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URaveAttributeSet, MaxExperience, OldValue);
}

bool URaveAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	return Super::PreGameplayEffectExecute(Data);
}

void URaveAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		HandleDamageAttributeChange(Data);
	}
	else if (Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		// Convert into +Health and then clamo
		SetHealth(FMath::Clamp(GetHealth() + GetHealing(), MIN_HEALTH, GetMaxHealth()));
		SetHealing(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), MIN_HEALTH, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), MIN_MANA, GetMaxMana()));
	}
	else if (Data.EvaluatedData.Attribute == GetAttackPowerAttribute())
	{
		SetAttackPower(FMath::Clamp(GetAttackPower(), MIN_ATTACK_DAMAGE, GetAttackPower()));
	}
	else if (Data.EvaluatedData.Attribute == GetAttackSpeedAttribute())
	{
		SetAttackSpeed(FMath::Clamp(GetAttackSpeed(), MIN_ATTACK_SPEED, MAX_ATTACK_SPEED));
	}
	else if (Data.EvaluatedData.Attribute == GetLevelAttribute())
	{
		HandleLevelAttributeChange(Data);
	}
	else if (Data.EvaluatedData.Attribute == GetExperienceAttribute())
	{
		HandleExperienceAttributeChange(Data);
	}
	else if (Data.EvaluatedData.Attribute == GetMaxExperienceAttribute())
	{
		SetMaxExperience(FMath::Clamp(GetMaxExperience(), 0.0f, GetMaxExperience()));
	}
}

void URaveAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void URaveAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		// Make sure current health is not greater than the new max health.
		if (GetHealth() > NewValue)
		{
			UAbilitySystemComponent* AbilitySystemComponent = GetOwningAbilitySystemComponent();
			check(AbilitySystemComponent);

			AbilitySystemComponent->ApplyModToAttribute(GetHealthAttribute(), EGameplayModOp::Override, NewValue);
		}
	}
	else if (Attribute == GetMaxManaAttribute())
	{
		// Make sure current health is not greater than the new max health.
		if (GetMana() > NewValue)
		{
			UAbilitySystemComponent* AbilitySystemComponent = GetOwningAbilitySystemComponent();
			check(AbilitySystemComponent);

			AbilitySystemComponent->ApplyModToAttribute(GetManaAttribute(), EGameplayModOp::Override, NewValue);
		}
	}
}

void URaveAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		// Do not allow health to go negative or above max health.
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		// Do not allow max health to drop below 1.
		NewValue = FMath::Max(NewValue, 1.0f);
	}
	else if (Attribute == GetManaAttribute())
	{
		// Do not allow mana to go negative or above max mana.
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxMana());
	}
	else if (Attribute == GetMaxManaAttribute())
	{
		// Do not allow max mana to drop below 0.
		NewValue = FMath::Max(NewValue, 0.0f);
	}
	else if (Attribute == GetAttackPowerAttribute())
	{
		// Do not allow attack damage to drop below 0.
		NewValue = FMath::Max(NewValue, 0.0f);
	}
	else if (Attribute == GetAttackSpeedAttribute())
	{
		// Do not allow attack speed to drop below 0.
		NewValue = FMath::Max(NewValue, 0.0f);
	}
	else if (Attribute == GetMovementSpeedAttribute())
	{
		NewValue = FMath::Clamp(NewValue, MIN_MOVEMENT_SPEED, MAX_MOVEMENT_SPEED);
	}
}

void URaveAttributeSet::PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const
{
	Super::PostAttributeBaseChange(Attribute, OldValue, NewValue);
}

void URaveAttributeSet::HandleDamageAttributeChange(const FGameplayEffectModCallbackData& Data)
{
	// Convert into -Health and then clamp
	float DamageDone = GetDamage() - GetArmor();

	if (DamageDone > 0.0f)
	{
		SetHealth(FMath::Clamp(GetHealth() - DamageDone, MIN_HEALTH, GetMaxHealth()));

		// Flick target character material
		TWeakObjectPtr<ARaveCharacter> SourceCharacter = Cast<ARaveCharacter>(Data.EffectSpec.GetEffectContext().GetEffectCauser());
		TWeakObjectPtr<ARaveCharacter> TargetCharacter = Cast<ARaveCharacter>(Data.Target.AbilityActorInfo->AvatarActor);
		if (IsValid(TargetCharacter.Get()))
		{
			TargetCharacter->FlickHitFeedbackMaterial();

			if (IsValid(SourceCharacter.Get()))
			{
				SourceCharacter->ShowHealthDisplayWidgetClient(TargetCharacter.Get());
			}
		}

		TWeakObjectPtr<ARavePlayerController> SourcePlayerController = Cast<ARavePlayerController>(Data.EffectSpec.GetEffectContext().GetOriginalInstigatorAbilitySystemComponent()->AbilityActorInfo->PlayerController);
		TWeakObjectPtr<ARavePlayerController> TargetPlayerController = Cast<ARavePlayerController>(Data.Target.AbilityActorInfo->PlayerController);
		if (IsValid(SourcePlayerController.Get()))
		{
			// Display damage to effect causer player
			bool bCriticalHit = false;
			FColor DamageDisplayColor = bCriticalHit ? FColor::Yellow : FColor::White;
			FVector TargetLocation = Data.EffectSpec.GetEffectContext().GetHitResult() ? Data.EffectSpec.GetEffectContext().GetHitResult()->ImpactPoint : FVector::ZeroVector;
			SourcePlayerController->ShowDamageDisplayWidget(Data.Target.AbilityActorInfo->AvatarActor.Get(), DamageDone, DamageDisplayColor, TargetLocation);
		}

		if (IsValid(TargetPlayerController.Get()) && IsValid(SourceCharacter.Get()))
		{
			TargetPlayerController->ShowHitCircleWidget(SourceCharacter->GetActorLocation());
		}
	}

	SetDamage(0.0f);
}

void URaveAttributeSet::HandleLevelAttributeChange(const FGameplayEffectModCallbackData& Data)
{
#if WITH_SERVER_CODE
	SetLevel(FMath::Clamp(GetLevel(), MIN_LEVEL, GetLevel()));

	AActor* AvatarActor = GetOwningAbilitySystemComponent()->GetAvatarActor();
	if (AvatarActor)
	{
		UWorld* World = AvatarActor->GetWorld();
		if (World)
		{
			ARaveGameMode_Gameplay* GameplayGameMode = World->GetAuthGameMode<ARaveGameMode_Gameplay>();
			if (GameplayGameMode)
			{
				SetMaxExperience(GameplayGameMode->GetExperienceRequireAmountForLevelUp(GetLevel()));
			}
		}
	}

#endif
}

void URaveAttributeSet::HandleExperienceAttributeChange(const FGameplayEffectModCallbackData& Data)
{
#if WITH_SERVER_CODE
	SetExperience(FMath::Clamp(GetExperience(), MIN_LEVEL, GetExperience()));

	AActor* AvatarActor = GetOwningAbilitySystemComponent()->GetAvatarActor();
	if (AvatarActor)
	{
		UWorld* World = AvatarActor->GetWorld();
		if (World)
		{
			ARaveGameMode_Gameplay* GameplayGameMode = World->GetAuthGameMode<ARaveGameMode_Gameplay>();
			if (GameplayGameMode)
			{
				if (GetExperience() >= GameplayGameMode->GetExperienceRequireAmountForLevelUp(GetLevel()))
				{
					float RemainingExperience = GetExperience() - GameplayGameMode->GetExperienceRequireAmountForLevelUp(GetLevel());
					GameplayGameMode->LevelUp(AvatarActor);
					SetExperience(RemainingExperience);
				}
			}
		}
	}

#endif
}
