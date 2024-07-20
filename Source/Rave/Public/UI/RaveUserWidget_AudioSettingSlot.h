

#pragma once

#include "CoreMinimal.h"
#include "UI/RaveUserWidget.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"
#include "Audio/RaveAudioUserSettings.h"
#include "RaveUserWidget_AudioSettingSlot.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveUserWidget_AudioSettingSlot : public URaveUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	void SetSoundGroup(struct FRaveSoundGroup InSoundGroup);

protected:
	UFUNCTION()
	void HandleAudioSliderValueChanged(float Value);

protected:
	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class UTextBlock> AudioSettingTypeTextBlock;

	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class UTextBlock> AudioVolumeTextBlock;

	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class USlider> AudioSettingSlider;

	UPROPERTY(EditInstanceOnly, Category = "Sound")
	FName SoundTypeName;

	UPROPERTY(EditInstanceOnly, Category = "Sound")
	TObjectPtr<USoundMix> SoundMix;

	UPROPERTY(EditInstanceOnly, Category = "Sound")
	TArray<TObjectPtr<USoundClass>> SoundClasses;

	struct FRaveSoundGroup SoundGroup;
};

