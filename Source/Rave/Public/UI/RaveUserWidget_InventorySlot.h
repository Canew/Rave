

#pragma once

#include "CoreMinimal.h"
#include "UI/RaveUserWidget.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpec.h"
#include "RaveUserWidget_InventorySlot.generated.h"

UENUM()
enum class ERaveInventorySlotType : uint8
{
	Skill,
	Module,
	Item
};

/**
 * 
 */
UCLASS()
class RAVE_API URaveUserWidget_InventorySlot : public URaveUserWidget
{
	GENERATED_BODY()

public:
	URaveUserWidget_InventorySlot(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

public:
	ERaveInventorySlotType GetInventorySlotType() const;
	void SetInventorySlotType(ERaveInventorySlotType InInventorySlotType);
	void SetInventoryIndex(int32 InInventoryIndex);
	
protected:
	UFUNCTION()
	FSlateBrush GetInventoryBorderBackground();

	UFUNCTION()
	UWidget* GetInventoryToolTip();

	UFUNCTION()
	ESlateVisibility GetStackTextBlockVisibility();

	UFUNCTION()
	FText GetStackTextBlockText();

	FKey GetInventorySlotKey() const;
	
protected:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class UBorder> InventorySlotBorder;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class UTextBlock> StackTextBlock;

	UPROPERTY(EditInstanceOnly, Category = "Inventory|Slot|Type")
	ERaveInventorySlotType InventorySlotType;

	UPROPERTY(EditInstanceOnly, Category = "Inventory|Slot|Type", meta = (EditCondition = "InventorySlotType == ERaveInventorySlotType::Skill || InventorySlotType == ERaveInventorySlotType::Module", EditConditionHides))
	FGameplayTag InventorySlotInputTag;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory|Slot|ToolTip")
	TSubclassOf<class URaveUserWidget_InventoryToolTip> InventoryToolTipClass;

	TWeakObjectPtr<class URaveInventorySystemComponent> InventorySystemComponent;
	bool bCanClick;
	int32 InventoryIndex;
};
