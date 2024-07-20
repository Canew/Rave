


#include "RaveGameUserSettings.h"
#include "Audio/RaveAudioUserSettings.h"
#include "Kismet/GameplayStatics.h"

URaveGameUserSettings::URaveGameUserSettings()
{
	ResolutionSet.Add(FIntPoint(1280, 720));
	ResolutionSet.Add(FIntPoint(1600, 900));
	ResolutionSet.Add(FIntPoint(1920, 1080));
	ResolutionSet.Add(FIntPoint(2560, 1440));
}

const TArray<FIntPoint>& URaveGameUserSettings::GetResolutionSet() const
{
	return ResolutionSet;
}

URaveAudioUserSettings* URaveGameUserSettings::GetAudioUserSettings()
{
	if (AudioUserSettings)
	{
		return AudioUserSettings;
	}

	AudioUserSettings = Cast<URaveAudioUserSettings>(UGameplayStatics::LoadGameFromSlot(AUDIO_SETTINGS_SLOT_NAME, 0));
	if (!AudioUserSettings)
	{
		AudioUserSettings = Cast<URaveAudioUserSettings>(UGameplayStatics::CreateSaveGameObject(URaveAudioUserSettings::StaticClass()));
	}

	return AudioUserSettings;
}
