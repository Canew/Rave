


#include "UI/RaveUserWidget_Status.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbility/RaveAttributeSet.h"
#include "RaveGameState_Gameplay.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveUserWidget_Status)

void URaveUserWidget_Status::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeStatus();
}

void URaveUserWidget_Status::InitializeStatus()
{
	if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwningPlayerPawn()))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(URaveAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::UpdateHealth);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(URaveAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &ThisClass::UpdateHealth);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(URaveAttributeSet::GetManaAttribute()).AddUObject(this, &ThisClass::UpdateMana);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(URaveAttributeSet::GetMaxManaAttribute()).AddUObject(this, &ThisClass::UpdateMana);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(URaveAttributeSet::GetExperienceAttribute()).AddUObject(this, &ThisClass::UpdateExperience);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(URaveAttributeSet::GetMaxExperienceAttribute()).AddUObject(this, &ThisClass::UpdateExperience);

		UpdateHealth();
		UpdateMana();
		UpdateExperience();
	}
	else
	{
		APlayerController* PlayerController = GetOwningPlayer();
		if (PlayerController)
		{
			PlayerController->GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::InitializeStatus);
		}
	}
}

void URaveUserWidget_Status::UpdateHealth(const FOnAttributeChangeData& Data)
{
	if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwningPlayerPawn()))
	{
		FGameplayAttribute HealthAttribute = URaveAttributeSet::GetHealthAttribute();
		FGameplayAttribute MaxHealthAttribute = URaveAttributeSet::GetMaxHealthAttribute();
		if (AbilitySystemComponent->HasAttributeSetForAttribute(HealthAttribute) && AbilitySystemComponent->HasAttributeSetForAttribute(MaxHealthAttribute))
		{
			float HealthValue = AbilitySystemComponent->GetNumericAttribute(HealthAttribute);
			float MaxHealthValue = AbilitySystemComponent->GetNumericAttribute(MaxHealthAttribute);

			if (MaxHealthValue == 0.0f)
			{
				HealthProgressBar->SetPercent(0.0f);
			}
			else
			{
				HealthProgressBar->SetPercent(HealthValue / MaxHealthValue);
			}

			FString HealthString = FString::SanitizeFloat(HealthValue);
			HealthString = HealthString.Left(HealthString.Find("."));
			FString MaxHealthString = FString::SanitizeFloat(MaxHealthValue);
			MaxHealthString = MaxHealthString.Left(MaxHealthString.Find("."));

			FString HealthText = FString::Printf(TEXT("%s   /   %s"), *HealthString, *MaxHealthString);
			HealthTextBlock->SetText(FText::FromString(HealthText));
		}
	}
}

void URaveUserWidget_Status::UpdateMana(const FOnAttributeChangeData& Data)
{
	if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwningPlayerPawn()))
	{
		FGameplayAttribute ManaAttribute = URaveAttributeSet::GetManaAttribute();
		FGameplayAttribute MaxManaAttribute = URaveAttributeSet::GetMaxManaAttribute();
		if (AbilitySystemComponent->HasAttributeSetForAttribute(ManaAttribute) && AbilitySystemComponent->HasAttributeSetForAttribute(MaxManaAttribute))
		{
			float ManaValue = AbilitySystemComponent->GetNumericAttribute(ManaAttribute);
			float MaxManaValue = AbilitySystemComponent->GetNumericAttribute(MaxManaAttribute);

			if (MaxManaValue == 0.0f)
			{
				ManaProgressBar->SetPercent(0.0f);
			}
			else
			{
				ManaProgressBar->SetPercent(ManaValue / MaxManaValue);
			}

			FString ManaString = FString::SanitizeFloat(ManaValue);
			ManaString = ManaString.Left(ManaString.Find("."));
			FString MaxManaString = FString::SanitizeFloat(MaxManaValue);
			MaxManaString = MaxManaString.Left(MaxManaString.Find("."));

			FString ManaText = FString::Printf(TEXT("%s   /   %s"), *ManaString, *MaxManaString);
			ManaTextBlock->SetText(FText::FromString(ManaText));
		}
	}
}

void URaveUserWidget_Status::UpdateExperience(const FOnAttributeChangeData& Data)
{
	if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwningPlayerPawn()))
	{
		FGameplayAttribute ExperienceAttribute = URaveAttributeSet::GetExperienceAttribute();
		FGameplayAttribute MaxExperienceAttribute = URaveAttributeSet::GetMaxExperienceAttribute();
		if (AbilitySystemComponent->HasAttributeSetForAttribute(ExperienceAttribute) && AbilitySystemComponent->HasAttributeSetForAttribute(MaxExperienceAttribute))
		{
			float ExperienceValue = AbilitySystemComponent->GetNumericAttribute(ExperienceAttribute);
			float MaxExperienceValue = AbilitySystemComponent->GetNumericAttribute(MaxExperienceAttribute);

			if (MaxExperienceValue == 0.0f)
			{
				ExperienceProgressBar->SetPercent(0.0f);
			}
			else
			{
				ExperienceProgressBar->SetPercent(ExperienceValue / MaxExperienceValue);
			}

			FString ExperienceString = FString::SanitizeFloat(ExperienceValue);
			ExperienceString = ExperienceString.Left(ExperienceString.Find("."));
			FString MaxExperienceString = FString::SanitizeFloat(MaxExperienceValue);
			MaxExperienceString = MaxExperienceString.Left(MaxExperienceString.Find("."));

			FString ExperienceText = FString::Printf(TEXT("%s   /   %s"), *ExperienceString, *MaxExperienceString);
			ExperienceTextBlock->SetText(FText::FromString(ExperienceText));
		}
	}
}
