

#pragma once

#include "CoreMinimal.h"
#include "UI/RaveUserWidget.h"
#include "RaveUserWidget_GameplayShopGrid.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveUserWidget_GameplayShopGrid : public URaveUserWidget
{
	GENERATED_BODY()
	
public:
	void InitializeGameplayShopGrid(class ARaveGameplayShop* GameplayShop);

protected:
	virtual void NativeDestruct() override;
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	UFUNCTION()
	void SaleAcceptButtonClicked();
	UFUNCTION()
	void SaleRejectButtonClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UBorder> ShopBorder;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UUniformGridPanel> ShopUniformGridPanel;

	UPROPERTY(EditDefaultsOnly, Category = "Rave|Widget")
	TSubclassOf<class URaveUserWidget_GameplayShopSlot> GameplayShopSlotWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Rave")
	TObjectPtr<UTexture2D> CurrencyTexture;

	TWeakObjectPtr<class ARaveGameplayShop> OwningGameplayShop;

	UPROPERTY()
	TObjectPtr<class URaveUserWidget_Dialog> SaleConfirmationDialogWidget;
	TSubclassOf<class URaveGameplayItem> GameplayItemClassToSell;
	int32 InventoryIndexToSell;

	int32 ShopMaxColumn = 5;
};
