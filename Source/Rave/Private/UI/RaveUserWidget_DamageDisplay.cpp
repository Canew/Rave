


#include "UI/RaveUserWidget_DamageDisplay.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"
#include "Components/WidgetComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveUserWidget_DamageDisplay)

void URaveUserWidget_DamageDisplay::SetDamageAmount(float InDamageAmount)
{
	DamageAmount = InDamageAmount;

	if (DamageText)
	{
		DamageText->SetText(FText::FromString(FString::SanitizeFloat(DamageAmount, 0)));
	}
}

void URaveUserWidget_DamageDisplay::SetDamageColor(FSlateColor DamageColor)
{
	if (DamageText)
	{
		DamageText->SetColorAndOpacity(DamageColor);
	}
}

void URaveUserWidget_DamageDisplay::SetOwnerWidgetComponent(UWidgetComponent* WidgetComponent)
{
	OwnerWidgetComponent = WidgetComponent;
}

void URaveUserWidget_DamageDisplay::HandleFadeOutAnimationFinished()
{
	if (IsValid(OwnerWidgetComponent.Get()))
	{
		OwnerWidgetComponent->DestroyComponent();
	}

	Super::HandleFadeOutAnimationFinished();
}
