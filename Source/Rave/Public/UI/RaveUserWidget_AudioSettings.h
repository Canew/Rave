

#pragma once

#include "CoreMinimal.h"
#include "UI/RaveUserWidget.h"
#include "RaveUserWidget_AudioSettings.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveUserWidget_AudioSettings : public URaveUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	
protected:
	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class UVerticalBox> AudioSettingsVerticalBox;

	UPROPERTY(EditDefaultsOnly, Category = "Slot")
	TSubclassOf<class URaveUserWidget_AudioSettingSlot> AudioSettingSlotWidgetClass;
};
