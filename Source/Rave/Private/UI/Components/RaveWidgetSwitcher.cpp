


#include "UI/Components/RaveWidgetSwitcher.h"

void URaveWidgetSwitcher::SetActiveWidgetIndex(int32 Index)
{
	if (IsDesignTime())
	{
		Super::SetActiveWidgetIndex(Index);

		return;
	}

	if (GetActiveWidgetIndex() != Index)
	{
		SetIsEnabled(false);
		if (UWidget* ActiveWidget = GetActiveWidget())
		{
			ActiveWidget->SetIsEnabled(false);
		}

		if (FadeOutSpeed > 0.0f)
		{
			FTimerDelegate FadeOutTimerDelegate;
			FadeOutTimerDelegate.BindUObject(this, &ThisClass::PlayFadeOut, Index);
			GetOwningPlayer()->GetWorldTimerManager().SetTimerForNextTick(FadeOutTimerDelegate);
		}
		else
		{
			HandleFadeOutCompleted(Index);
		}
	}
}

void URaveWidgetSwitcher::SetActiveWidget(UWidget* Widget)
{
	if (IsDesignTime())
	{
		Super::SetActiveWidget(Widget);

		return;
	}

	int32 NewIndex = GetChildIndex(Widget);
	if (GetActiveWidgetIndex() != NewIndex)
	{
		SetIsEnabled(false);
		if (UWidget* ActiveWidget = GetActiveWidget())
		{
			ActiveWidget->SetIsEnabled(false);
		}

		if (FadeOutSpeed > 0.0f)
		{
			FTimerDelegate FadeOutTimerDelegate;
			FadeOutTimerDelegate.BindUObject(this, &ThisClass::PlayFadeOut, NewIndex);
			GetOwningPlayer()->GetWorldTimerManager().SetTimerForNextTick(FadeOutTimerDelegate);
		}
		else
		{
			HandleFadeOutCompleted(NewIndex);
		}
	}
}

void URaveWidgetSwitcher::PlayFadeIn()
{
	float NewOpacity = FMath::Clamp(GetRenderOpacity() + FadeInSpeed, 0.0f, 1.0f);
	SetRenderOpacity(NewOpacity);
	if (GetRenderOpacity() >= 1.0f)
	{
		HandleFadeInCompleted();
	}
	else
	{
		FTimerDelegate FadeInTimerDelegate;
		FadeInTimerDelegate.BindUObject(this, &ThisClass::PlayFadeIn);
		GetOwningPlayer()->GetWorldTimerManager().SetTimerForNextTick(FadeInTimerDelegate);
	}
}

void URaveWidgetSwitcher::PlayFadeOut(int32 Index)
{
	float NewOpacity = FMath::Clamp(GetRenderOpacity() + FadeInSpeed, 0.0f, 1.0f);
	SetRenderOpacity(GetRenderOpacity() - FadeOutSpeed);
	if (GetRenderOpacity() < KINDA_SMALL_NUMBER)
	{
		HandleFadeOutCompleted(Index);
	}
	else
	{
		FTimerDelegate FadeOutTimerDelegate;
		FadeOutTimerDelegate.BindUObject(this, &ThisClass::PlayFadeOut, Index);
		GetOwningPlayer()->GetWorldTimerManager().SetTimerForNextTick(FadeOutTimerDelegate);
	}
}

void URaveWidgetSwitcher::HandleFadeInCompleted()
{
	SetRenderOpacity(1.0f);
	SetIsEnabled(true);
	if (UWidget* ActiveWidget = GetActiveWidget())
	{
		ActiveWidget->SetIsEnabled(true);
	}
}

void URaveWidgetSwitcher::HandleFadeOutCompleted(int32 Index)
{
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	ActiveWidgetIndex = Index;
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
	BroadcastFieldValueChanged(FFieldNotificationClassDescriptor::ActiveWidgetIndex);
	SetActiveWidgetIndexForSlateWidget();

	OnWidgetSwitched.Broadcast();

	SetRenderOpacity(0.0f);
	SetIsEnabled(false);
	if (UWidget* ActiveWidget = GetActiveWidget())
	{
		ActiveWidget->SetIsEnabled(false);
	}

	if (FadeInSpeed > 0.0f)
	{
		FTimerDelegate FadeInTimerDelegate;
		FadeInTimerDelegate.BindUObject(this, &ThisClass::PlayFadeIn);
		GetOwningPlayer()->GetWorldTimerManager().SetTimerForNextTick(FadeInTimerDelegate);
	}
	else
	{
		HandleFadeInCompleted();
	}
}
