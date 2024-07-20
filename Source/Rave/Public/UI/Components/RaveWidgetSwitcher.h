

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetSwitcher.h"
#include "RaveWidgetSwitcher.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWidgetSwitchEvent);

/**
 * 
 */
UCLASS()
class RAVE_API URaveWidgetSwitcher : public UWidgetSwitcher
{
	GENERATED_BODY()
	
public:
	/** Activates the widget at the specified index. */
	virtual void SetActiveWidgetIndex(int32 Index) override;

	/** Activates the widget and makes it the active index. */
	virtual void SetActiveWidget(UWidget* Widget) override;

protected:
	void PlayFadeIn();
	void PlayFadeOut(int32 Index);

	void HandleFadeInCompleted();
	void HandleFadeOutCompleted(int32 Index);

public:
	UPROPERTY(BlueprintAssignable, Category = "Switcher")
	FOnWidgetSwitchEvent OnWidgetSwitched;

protected:
	UPROPERTY(EditInstanceOnly, Category = "Switcher")
	float FadeInSpeed;
	UPROPERTY(EditInstanceOnly, Category = "Switcher")
	float FadeOutSpeed;
};
 