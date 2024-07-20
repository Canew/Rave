


#include "RaveGameMode_Gameplay.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayAbility/RaveAttributeSet.h"

ARaveGameMode_Gameplay::ARaveGameMode_Gameplay(const FObjectInitializer& ObjectInitializer)
{
	ExperienceRequireAmountRowName = TEXT("Experience");
}

void ARaveGameMode_Gameplay::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (NewPlayer)
	{
		NewPlayer->SetInputMode(FInputModeGameOnly());
		NewPlayer->SetShowMouseCursor(false);
	}
}

void ARaveGameMode_Gameplay::DistributeExperience(float ExperienceAmount)
{
	for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
	{
		const APlayerState* PlayerState = PlayerController->GetPlayerState<APlayerState>();
		const IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(PlayerState);
		if (AbilitySystemInterface)
		{
			UAbilitySystemComponent* AbilitySystemComponent = AbilitySystemInterface->GetAbilitySystemComponent();
			if (AbilitySystemComponent)
			{
				UGameplayEffect* ExperienceGameplayEffect = NewObject<UGameplayEffect>();
				if (ExperienceGameplayEffect)
				{
					FGameplayModifierInfo ExperienceGameplayModifierInfo;
					ExperienceGameplayModifierInfo.Attribute = URaveAttributeSet::GetExperienceAttribute();
					ExperienceGameplayModifierInfo.ModifierOp = EGameplayModOp::Additive;
					ExperienceGameplayModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(ExperienceAmount);
					ExperienceGameplayEffect->Modifiers.Add(ExperienceGameplayModifierInfo);

					FGameplayEffectSpec ExperienceGameplayEffectSpec(ExperienceGameplayEffect, AbilitySystemComponent->MakeEffectContext());
					AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(ExperienceGameplayEffectSpec, AbilitySystemComponent->ScopedPredictionKey);
				}
			}
		}
	}
}

void ARaveGameMode_Gameplay::LevelUp(AActor* Actor)
{
	check(LevelUpGameplayEffect);

	UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
	if (AbilitySystemComponent)
	{
		FGameplayEffectSpec FGameplayEffectSpec(LevelUpGameplayEffect.GetDefaultObject(), AbilitySystemComponent->MakeEffectContext());
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(FGameplayEffectSpec, AbilitySystemComponent->ScopedPredictionKey);
	}
}

float ARaveGameMode_Gameplay::GetExperienceRequireAmountForLevelUp(float CurrentLevel) const
{
	check(ExperienceRequireAmountCurveTable);

	FRealCurve* ExperienceCurve = ExperienceRequireAmountCurveTable->GetRowMap()[ExperienceRequireAmountRowName];
	check(ExperienceCurve);

	return ExperienceCurve->GetKeyValue(ExperienceCurve->FindKey(CurrentLevel + 1));
}
