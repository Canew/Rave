


#include "UI/RaveUserWidget_StatisticsSlot.h"
#include "UI/RaveUserWidget_StatisticsToolTip.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"

void URaveUserWidget_StatisticsSlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UpdateStatisticsAttribute();

	UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwningPlayerPawn());
	if (AbilitySystemComponent)
	{
		if (CurrentStatisticsAttribute.IsValid())
		{
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(CurrentStatisticsAttribute).AddUObject(this, &ThisClass::HandleAttributeValueChanged);
		}

		if (MaxStatisticsAttribute.IsValid())
		{
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MaxStatisticsAttribute).AddUObject(this, &ThisClass::HandleAttributeValueChanged);
		}
	}

	ToolTipWidgetDelegate.BindDynamic(this, &ThisClass::GetStatisticsToolTip);
}

void URaveUserWidget_StatisticsSlot::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (StatisticsSlotImage)
	{
		if (StatisticsAttributeBrush.HasUObject())
		{
			StatisticsSlotImage->SetBrush(StatisticsAttributeBrush);
			StatisticsSlotImage->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			StatisticsSlotImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void URaveUserWidget_StatisticsSlot::UpdateStatisticsAttribute()
{
	UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwningPlayerPawn());
	if (AbilitySystemComponent && CurrentStatisticsAttribute.IsValid())
	{
		bool bCurrentAttributeValueFound = false;
		float CurrentAttributeValue = AbilitySystemComponent->GetGameplayAttributeValue(CurrentStatisticsAttribute, bCurrentAttributeValueFound);

		FString StatisticsAttributeValueString = FString::SanitizeFloat(CurrentAttributeValue, ValueFractionalDigits);
		if (MaxStatisticsAttribute.IsValid() && bCurrentAttributeValueFound)
		{
			bool bMaxAttributeValueFound = false;
			float MaxAttributeValue = AbilitySystemComponent->GetGameplayAttributeValue(MaxStatisticsAttribute, bMaxAttributeValueFound);
			if (bMaxAttributeValueFound)
			{
				StatisticsAttributeValueString.Append(TEXT(" / ") + FString::SanitizeFloat(MaxAttributeValue, ValueFractionalDigits));
			}
		}

		StatisticsSlotTextBlock->SetText(FText::FromString(StatisticsAttributeValueString));
	}
}

void URaveUserWidget_StatisticsSlot::HandleAttributeValueChanged(const FOnAttributeChangeData& Data)
{
	UpdateStatisticsAttribute();
}

UWidget* URaveUserWidget_StatisticsSlot::GetStatisticsToolTip()
{
	if (StatisticsToolTipClass)
	{
		UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwningPlayerPawn());
		if (AbilitySystemComponent)
		{
			URaveUserWidget_StatisticsToolTip* StatisticsSlotToolTip = CreateWidget<URaveUserWidget_StatisticsToolTip>(GetOwningPlayer(), StatisticsToolTipClass);
			if (StatisticsSlotToolTip)
			{
				StatisticsSlotToolTip->StatisticsToolTipNameTextBlock->SetText(AttributeDisplayName);

				return StatisticsSlotToolTip;
			}
		}
	}

	return nullptr;
}
