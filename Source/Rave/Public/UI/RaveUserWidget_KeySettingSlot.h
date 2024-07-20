

#pragma once

#include "CoreMinimal.h"
#include "UI/RaveUserWidget.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "RaveUserWidget_KeySettingSlot.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveUserWidget_KeySettingSlot : public URaveUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	FPlayerKeyMapping GetPlayerKeyMapping() const;
	void SetPlayerKeyMapping(const FPlayerKeyMapping& KeyMapping);

	UFUNCTION()
	void HandleInputKeySelected(FInputChord SelectedKey);

protected:
	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class UTextBlock> InputKeyTextBlock;

	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class UInputKeySelector> InputKeySelector;

	FPlayerKeyMapping PlayerKeyMapping;
};
