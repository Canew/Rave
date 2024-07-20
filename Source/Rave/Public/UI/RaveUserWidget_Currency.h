

#pragma once

#include "CoreMinimal.h"
#include "UI/RaveUserWidget.h"
#include "GameplayEffectTypes.h"
#include "RaveUserWidget_Currency.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveUserWidget_Currency : public URaveUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void RequestInvetorySystemComponent();
	void UpdateCurrency(int32 CurrentCurrency);
	
protected:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class UTextBlock> CurrencyTextBlock;

	TWeakObjectPtr<class URaveInventorySystemComponent> InventorySystemComponent;
	FTimerHandle RequestTimerHandle;
	float RequestDelay = 1.0f;
};
