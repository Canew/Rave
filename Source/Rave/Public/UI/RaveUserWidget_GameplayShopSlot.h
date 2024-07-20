

#pragma once

#include "CoreMinimal.h"
#include "UI/RaveUserWidget.h"
#include "RaveUserWidget_GameplayShopSlot.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveUserWidget_GameplayShopSlot : public URaveUserWidget
{
	GENERATED_BODY()
	
public:
	void InitializeGameplayShopSlot(class ARaveGameplayShop* GameplayShop, int32 Index);

protected:
	virtual void NativeDestruct() override;

	UFUNCTION()
	UWidget* GetShopSlotToolTip();

	UFUNCTION()
	void SlotButtonHovered();
	UFUNCTION()
	void SlotButtonUnhovered();
	UFUNCTION()
	void SlotButtonClicked();
	UFUNCTION()
	void SlotButtonPressed();
	UFUNCTION()
	void SlotButtonReleased();

	UFUNCTION()
	void PurchaseAcceptButtonClicked();
	UFUNCTION()
	void PurchaseRejectButtonClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UBorder> ShopSlotBorder;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> ShopSlotButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> ShopSlotImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> ItemNameTextBlock;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> ItemPriceTextBlock;

	TWeakObjectPtr<class ARaveGameplayShop> OwningGameplayShop;
	int32 SlotIndex;

	UPROPERTY(EditDefaultsOnly, Category = "Rave")
	TSubclassOf<class URaveUserWidget_InventoryToolTip> InventoryToolTipClass;

	UPROPERTY(EditDefaultsOnly, Category = "Rave")
	TObjectPtr<UTexture2D> CurrencyTexture;

	UPROPERTY()
	TObjectPtr<class URaveUserWidget_Dialog> PurchaseConfirmationDialogWidget;
};
