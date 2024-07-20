


#include "RaveGameInstance.h"
#include "RaveGameUserSettings.h"
#include "Audio/RaveAudioUserSettings.h"
#include "Data/RaveOperationData.h"
#include "Character/RaveCharacterSetting.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveGameInstance)

void URaveGameInstance::Init()
{
	Super::Init();

	InitializeAudioUserSettings();
}

const TArray<struct FRaveSoundGroup>& URaveGameInstance::GetGameSoundGroups() const
{
	return GameSoundGroups;
}

const URaveOperationData* URaveGameInstance::GetOperationData() const
{
	return OperationData;
}

const TArray<class URaveCharacterSetting*> URaveGameInstance::GetCharacterSettings() const
{
	return CharacterSettings;
}

void URaveGameInstance::InitializeAudioUserSettings()
{
	URaveGameUserSettings* RaveGameUserSettings = URaveGameUserSettings::GetGameUserSettings();
	check(RaveGameUserSettings);

	URaveAudioUserSettings* RaveAudioUserSettings = RaveGameUserSettings->GetAudioUserSettings();
	check(RaveAudioUserSettings);

	for (const FRaveSoundGroup& GameSoundGroup : GameSoundGroups)
	{
		RaveAudioUserSettings->RegisterSoundGroup(GameSoundGroup);
	}

	RaveAudioUserSettings->InitializeAudioSettings(this);
	RaveAudioUserSettings->SaveAudioSettings();
}
