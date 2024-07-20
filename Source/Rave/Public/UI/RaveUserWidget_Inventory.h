

#pragma once

#include "CoreMinimal.h"
#include "UI/RaveUserWidget.h"
#include "RaveUserWidget_Inventory.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveUserWidget_Inventory : public URaveUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

protected:
	UFUNCTION()
	FText GetRemainingSkillPointText();

public:
	UPROPERTY(EditDefaultsOnly, Category = "Rave|Inventory|Slot")
	TSubclassOf<class URaveUserWidget_InventorySlot> InventorySlotWidgetClass;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> RemainingSkillPointTextBlock;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UUniformGridPanel> ItemUniformGridPanel;

	TWeakObjectPtr<class URaveInventorySystemComponent> InventorySystemComponent;
	int32 ItemMaxColumnPerRow = 10;
};
