

#pragma once

#include "CoreMinimal.h"
#include "UI/RaveUserWidget.h"
#include "RaveGameUserSettings.h"
#include "RaveUserWidget_GraphicSettingSlot.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveUserWidget_GraphicSettingSlot : public URaveUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void SetGraphicSettingSlotType(ERaveGraphicSettingType InGraphicSettingType);

protected:
	void UpdateGraphicSettingValueText(int32 Value);

	UFUNCTION()
	void HandleUpButtonClicked();
	UFUNCTION()
	void HandleDownButtonClicked();
	
protected:
	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class UTextBlock> GraphicSettingTypeTextBlock;

	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class UTextBlock> GraphicSettingValueTextBlock;

	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class UButton> GraphicSettingUpButton;

	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class UButton> GraphicSettingDownButton;

	UPROPERTY(EditInstanceOnly, Category = "Type")
	ERaveGraphicSettingType GraphicSettingType;
};
