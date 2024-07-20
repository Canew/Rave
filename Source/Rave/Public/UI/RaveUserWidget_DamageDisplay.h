

#pragma once

#include "CoreMinimal.h"
#include "UI/RaveUserWidget.h"
#include "RaveUserWidget_DamageDisplay.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveUserWidget_DamageDisplay : public URaveUserWidget
{
	GENERATED_BODY()

public:
	virtual void SetDamageAmount(float InDamageAmount);
	virtual void SetDamageColor(FSlateColor DamageColor);

	virtual void SetOwnerWidgetComponent(class UWidgetComponent* WidgetComponent);

protected:
	virtual void HandleFadeOutAnimationFinished() override;

protected:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class UTextBlock> DamageText;

	TWeakObjectPtr<class UWidgetComponent> OwnerWidgetComponent;

	float DamageAmount;
	FTimerHandle DestructTimerHandle;
};
