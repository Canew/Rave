


#include "UI/RaveUserWidget_AudioSettingSlot.h"
#include "Components/TextBlock.h"
#include "Components/Slider.h"
#include "Kismet/GameplayStatics.h"
#include "RaveGameUserSettings.h"
#include "Audio/RaveAudioUserSettings.h"

void URaveUserWidget_AudioSettingSlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	AudioSettingSlider->OnValueChanged.AddDynamic(this, &ThisClass::HandleAudioSliderValueChanged);
}

void URaveUserWidget_AudioSettingSlot::NativeConstruct()
{
	Super::NativeConstruct();

	URaveGameUserSettings* GameUserSettings = URaveGameUserSettings::GetGameUserSettings();
	check(GameUserSettings);

	URaveAudioUserSettings* AudioUserSettings = GameUserSettings->GetAudioUserSettings();
	check(AudioUserSettings);

	AudioSettingTypeTextBlock->SetText(FText::FromName(SoundGroup.GroupName));

	float VolumeValue = AudioUserSettings->GetAudioGroupVolume(SoundGroup.GroupName);
	AudioSettingSlider->SetValue(VolumeValue);
}

void URaveUserWidget_AudioSettingSlot::SetSoundGroup(FRaveSoundGroup InSoundGroup)
{
	SoundGroup = InSoundGroup;
}

void URaveUserWidget_AudioSettingSlot::HandleAudioSliderValueChanged(float Value)
{
	URaveGameUserSettings* GameUserSettings = URaveGameUserSettings::GetGameUserSettings();
	check(GameUserSettings);

	URaveAudioUserSettings* AudioUserSettings = GameUserSettings->GetAudioUserSettings();
	check(AudioUserSettings);

	AudioUserSettings->SetAudioGroupVolume(GetOwningPlayer(), SoundGroup.GroupName, Value);
	AudioUserSettings->SaveAudioSettings();

	float VolumePercentage = FMath::Clamp(Value * 100.0f, 0.0f, 100.0f);
	FString VolumePercentageString = FString::SanitizeFloat(VolumePercentage);
	VolumePercentageString = VolumePercentageString.Left(VolumePercentageString.Find("."));
	AudioVolumeTextBlock->SetText(FText::FromString(VolumePercentageString + "%"));
}
