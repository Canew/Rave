


#include "UI/RaveUserWidget.h"
#include "Animation/WidgetAnimation.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveUserWidget)

URaveUserWidget::URaveUserWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bRemoveWhenFadeOutCompleted = false;
}

void URaveUserWidget::RemoveFromParent()
{
	if (!IsValid(this) || IsUnreachable())
	{
		Super::RemoveFromParent();

		return;
	}

	bRemoveWhenFadeOutCompleted = true;
	PlayFadeOutAnimation();
}

void URaveUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	bRemoveWhenFadeOutCompleted = false;
	PlayFadeInAnimation();
}

void URaveUserWidget::NativeDestruct()
{
	OnDestructed.Broadcast();

	Super::NativeDestruct();
}

void URaveUserWidget::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusLost(InFocusEvent);

	for (TPair<FKey, bool> KeyDownPair : KeyDownMap)
	{
		KeyDownMap[KeyDownPair.Key] = false;
	}
}

FReply URaveUserWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FReply Handled = Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	if (KeyDownMap.Find(InKeyEvent.GetKey()))
	{
		if (!KeyDownMap[InKeyEvent.GetKey()])
		{
			NativeOnKeyPressed(InGeometry, InKeyEvent);
		}

		KeyDownMap[InKeyEvent.GetKey()] = true;
	}
	else
	{
		KeyDownMap.Add(InKeyEvent.GetKey(), true);
		NativeOnKeyPressed(InGeometry, InKeyEvent);
	}

	return Handled;
}

FReply URaveUserWidget::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FReply Handled = Super::NativeOnKeyUp(InGeometry, InKeyEvent);

	if (KeyDownMap.Find(InKeyEvent.GetKey()))
	{
		KeyDownMap[InKeyEvent.GetKey()] = false;
	}
	else
	{
		KeyDownMap.Add(InKeyEvent.GetKey(), false);
	}

	return Handled;
}

void URaveUserWidget::PlayFadeInAnimation()
{
	if (FadeInAnimation && (FadeInAnimation->GetEndTime() - FadeInAnimation->GetStartTime() > KINDA_SMALL_NUMBER))
	{
		FWidgetAnimationDynamicEvent FadeInAnimationDynamicEvent;
		FadeInAnimationDynamicEvent.BindDynamic(this, &ThisClass::HandleFadeInAnimationFinished);
		BindToAnimationFinished(FadeInAnimation, FadeInAnimationDynamicEvent);
			
		PlayAnimation(FadeInAnimation);
	}
	else
	{
		HandleFadeInAnimationFinished();
	}
}

void URaveUserWidget::PlayFadeOutAnimation(bool bRemoveWhenCompleted)
{
	if (FadeOutAnimation && (FadeOutAnimation->GetEndTime() - FadeOutAnimation->GetStartTime() > KINDA_SMALL_NUMBER))
	{
		FWidgetAnimationDynamicEvent FadeOutAnimationDynamicEvent;
		FadeOutAnimationDynamicEvent.BindDynamic(this, &ThisClass::HandleFadeOutAnimationFinished);
		BindToAnimationFinished(FadeOutAnimation, FadeOutAnimationDynamicEvent);

		PlayAnimation(FadeOutAnimation);
	}
	else
	{
		HandleFadeOutAnimationFinished();
	}
}

void URaveUserWidget::HandleFadeInAnimationFinished()
{
	FWidgetAnimationDynamicEvent FadeInAnimationDynamicEvent;
	FadeInAnimationDynamicEvent.BindDynamic(this, &ThisClass::HandleFadeInAnimationFinished);
	UnbindFromAnimationFinished(FadeInAnimation, FadeInAnimationDynamicEvent);

	OnFadeInAnimationFinished.Broadcast();
}

void URaveUserWidget::HandleFadeOutAnimationFinished()
{
	FWidgetAnimationDynamicEvent FadeOutAnimationDynamicEvent;
	FadeOutAnimationDynamicEvent.BindDynamic(this, &ThisClass::HandleFadeOutAnimationFinished);
	UnbindFromAnimationFinished(FadeOutAnimation, FadeOutAnimationDynamicEvent);

	OnFadeOutAnimationFinished.Broadcast();

	if (bRemoveWhenFadeOutCompleted)
	{
		Super::RemoveFromParent();
	}
}
