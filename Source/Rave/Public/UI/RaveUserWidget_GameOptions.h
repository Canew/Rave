

#pragma once

#include "CoreMinimal.h"
#include "UI/RaveUserWidget.h"
#include "RaveUserWidget_GameOptions.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveUserWidget_GameOptions : public URaveUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class URaveUserWidget_GraphicSettings> GraphicSettingsWidget;

	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class URaveUserWidget_AudioSettings> AudioSettingsWidget;

	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class URaveUserWidget_KeySettings> KeySettingsWidget;
};
