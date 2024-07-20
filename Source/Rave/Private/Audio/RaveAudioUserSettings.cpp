


#include "Audio/RaveAudioUserSettings.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"
#include "Kismet/GameplayStatics.h"

void URaveAudioUserSettings::InitializeAudioSettings(const UObject* WorldContextObject)
{
	for (const FRaveSoundGroup& SoundGroup : RegisteredSoundGroups)
	{
		float Volume = DEFAULT_AUDIO_VOLUME;
		if (const float* SavedAudioVolume = SavedAudioProfile.Find(SoundGroup.GroupName))
		{
			Volume = *SavedAudioVolume;
		}

		SetAudioGroupVolume(WorldContextObject, SoundGroup.GroupName, Volume);
	}
}

void URaveAudioUserSettings::RegisterSoundGroup(FRaveSoundGroup SoundGroup)
{
	RegisteredSoundGroups.AddUnique(SoundGroup);
}

void URaveAudioUserSettings::UnregisterSoundGroup(FRaveSoundGroup SoundGroup)
{
	RegisteredSoundGroups.Remove(SoundGroup);
}

void URaveAudioUserSettings::SaveAudioSettings()
{
	UGameplayStatics::SaveGameToSlot(this, AUDIO_SETTINGS_SLOT_NAME, 0);
}

float URaveAudioUserSettings::GetAudioGroupVolume(FName GroupName)
{
	int32 SoundGroupIndex = RegisteredSoundGroups.Find(GroupName);
	if (RegisteredSoundGroups.IsValidIndex(SoundGroupIndex))
	{
		const FRaveSoundGroup& SoundGroup = RegisteredSoundGroups[SoundGroupIndex];
		if (SavedAudioProfile.Find(SoundGroup.GroupName))
		{
			return SavedAudioProfile[SoundGroup.GroupName];
		}
	}

	return DEFAULT_AUDIO_VOLUME;
}

void URaveAudioUserSettings::SetAudioGroupVolume(const UObject* WorldContextObject, FName GroupName, float Volume)
{
	int32 SoundGroupIndex = RegisteredSoundGroups.Find(GroupName);
	if (RegisteredSoundGroups.IsValidIndex(SoundGroupIndex))
	{
		const FRaveSoundGroup& SoundGroup = RegisteredSoundGroups[SoundGroupIndex];
		for (USoundClass* SoundClass : SoundGroup.SoundClasses)
		{
			UGameplayStatics::SetSoundMixClassOverride(WorldContextObject, SoundGroup.SoundMix, SoundClass, Volume);
		}

		UGameplayStatics::PushSoundMixModifier(WorldContextObject, SoundGroup.SoundMix);

		float SaveVolume = Volume < KINDA_SMALL_NUMBER ? -1.0f : Volume;
		SavedAudioProfile.Add(SoundGroup.GroupName, SaveVolume);
	}
}
