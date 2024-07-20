

#pragma once

#include "CoreMinimal.h"
#include "UI/RaveUserWidget.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "RaveUserWidget_KeySettings.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveUserWidget_KeySettings : public URaveUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

protected:
	void InitializeKeySettings();
	void AddKeySettingRow(const FKeyMappingRow& KeyMappingRow);

	UFUNCTION()
	void HandleApplyButtonClicked();
	UFUNCTION()
	void HandleUndoButtonClicked();
	UFUNCTION()
	void HandleResetButtonClicked();

	void ShowResetConfirmationDialog();
	void RemoveResetConfirmationDialog();
	UFUNCTION()
	void HandleResetAcceptButtonClicked();
	UFUNCTION()
	void HandleResetRejectButtonClicked();
	UFUNCTION()
	void HandleResetConfirmationDialogDestructed();

protected:
	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class UVerticalBox> KeySettingsVerticalBox;

	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class UButton> ApplyButton;

	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class UButton> UndoButton;

	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class UButton> ResetButton;

	UPROPERTY(EditDefaultsOnly, Category = "Rave|Slot")
	TSubclassOf<class URaveUserWidget_KeySettingSlot> KeySettingSlotClass;

	UPROPERTY(EditDefaultsOnly, Category = "Rave|Keys")
	TSet<FName> PlayerMappableRowKeys;

	UPROPERTY()
	TObjectPtr<class URaveUserWidget_Dialog> ResetConfirmationDialog;
};
