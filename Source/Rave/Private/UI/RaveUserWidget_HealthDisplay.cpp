


#include "UI/RaveUserWidget_HealthDisplay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameplayAbility/RaveAttributeSet.h"
#include "GameplayAbility/RaveAbilitySystemComponent.h"
#include "Character/RaveCharacter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveUserWidget_HealthDisplay)

void URaveUserWidget_HealthDisplay::NativeOnInitialized()
{
	Super::NativeOnInitialized();


}

void URaveUserWidget_HealthDisplay::NativeConstruct()
{
	Super::NativeConstruct();


}

void URaveUserWidget_HealthDisplay::InitializeHealth(const UAttributeSet* InAttributeSet)
{
	AttributeSet = InAttributeSet;
	if (AttributeSet)
	{
		UAbilitySystemComponent* AbilitySystemComponent = AttributeSet->GetOwningAbilitySystemComponent();
		if (AbilitySystemComponent)
		{
			bool bHealthAttributeFound = false;
			float CurrentHealth = AbilitySystemComponent->GetGameplayAttributeValue(URaveAttributeSet::GetHealthAttribute(), bHealthAttributeFound);
			float MaxHealth = AbilitySystemComponent->GetGameplayAttributeValue(URaveAttributeSet::GetMaxHealthAttribute(), bHealthAttributeFound);
			HealthBar->SetPercent(CurrentHealth / MaxHealth);

			FString HealthString = FString::SanitizeFloat(CurrentHealth, 0);
			HealthTextBlock->SetText(FText::FromString(HealthString));

			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(URaveAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::HandleHealthChanged);
		}
	}
}

void URaveUserWidget_HealthDisplay::HandleHealthChanged(const FOnAttributeChangeData& Data)
{
	if (AttributeSet)
	{
		UAbilitySystemComponent* AbilitySystemComponent = AttributeSet->GetOwningAbilitySystemComponent();
		if (AbilitySystemComponent)
		{
			bool bHealthAttributeFound = false;
			float CurrentHealth = AbilitySystemComponent->GetGameplayAttributeValue(URaveAttributeSet::GetHealthAttribute(), bHealthAttributeFound);
			float MaxHealth = AbilitySystemComponent->GetGameplayAttributeValue(URaveAttributeSet::GetMaxHealthAttribute(), bHealthAttributeFound);
			HealthBar->SetPercent(CurrentHealth / MaxHealth);

			//FString HealthString = FString::SanitizeFloat(CurrentHealth, 0) + TEXT(" / ") + FString::SanitizeFloat(MaxHealth, 0);
			FString HealthString = FString::SanitizeFloat(CurrentHealth, 0);
			HealthTextBlock->SetText(FText::FromString(HealthString));
		}
	}
}

