

#pragma once

#include "CoreMinimal.h"
#include "UI/RaveUserWidget.h"
#include "RaveUserWidget_GraphicSettings.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveUserWidget_GraphicSettings : public URaveUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

protected:
	void SetFullscreenModeText(int32 FullscreenMode);
	void SetResolutionText(FIntPoint ScreenResolution);

	UFUNCTION()
	void HandleFullscreenModeUpButtonClicked();
	UFUNCTION()
	void HandleFullscreenModeDownButtonClicked();

	UFUNCTION()
	void HandleResolutionUpButtonClicked();
	UFUNCTION()
	void HandleResolutionDownButtonClicked();

protected:
	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class UTextBlock> FullscreenModeTextBlock;
	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class UButton> FullscreenModeUpButton;
	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class UButton> FullscreenModeDownButton;

	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class UTextBlock> ResolutionTextBlock;
	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class UButton> ResolutionUpButton;
	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class UButton> ResolutionDownButton;

	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class UVerticalBox> GraphicSettingsVerticalBox;

	UPROPERTY(EditDefaultsOnly, Category = "Slot")
	TSubclassOf<class URaveUserWidget_GraphicSettingSlot> GraphicSettingSlotWidgetClass;
};
