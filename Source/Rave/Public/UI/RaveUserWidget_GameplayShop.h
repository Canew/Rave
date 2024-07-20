

#pragma once

#include "CoreMinimal.h"
#include "UI/RaveUserWidget.h"
#include "RaveUserWidget_GameplayShop.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveUserWidget_GameplayShop : public URaveUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

public:
	void InitializeGameplayShopWidget(class ARaveGameplayShop* GameplayShop);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class URaveUserWidget_GameplayShopGrid> GameplayShopGridWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UUniformGridPanel> InventoryUniformGridPanel;

	TWeakObjectPtr<class ARaveGameplayShop> OwningGameplayShop;
	TWeakObjectPtr<class URaveInventorySystemComponent> InventorySystemComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Rave|Widget")
	TSubclassOf<class URaveUserWidget_InventorySlot> InventorySlotWidgetClass;

	int32 ShopMaxColumn = 5;
	int32 InventoryMaxColumn = 10;
};
