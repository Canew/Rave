

#pragma once

#include "CoreMinimal.h"
#include "UI/RaveUserWidget.h"
#include "RaveUserWidget_InventoryToolTip.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveUserWidget_InventoryToolTip : public URaveUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> ToolTipImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> NameTextBlock;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> LevelTextBlock;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> KeyTextBlock;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> CostTextBlock;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> CooldownTextBlock;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> DescriptionTextBlock;
};
