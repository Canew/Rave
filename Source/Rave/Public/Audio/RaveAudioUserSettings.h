

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"
#include "RaveAudioUserSettings.generated.h"

#define AUDIO_SETTINGS_SLOT_NAME TEXT("AudioUserSettings")
#define DEFAULT_AUDIO_VOLUME 0.8f

USTRUCT(BlueprintType)
struct FRaveSoundGroup
{
	GENERATED_BODY()

public:
	FRaveSoundGroup() {};
	FRaveSoundGroup(FName GroupName)
		: GroupName(GroupName) {};

public:
	UPROPERTY(EditDefaultsOnly)
	FName GroupName;

	UPROPERTY(EditDefaultsOnly)
	USoundMix* SoundMix;

	UPROPERTY(EditDefaultsOnly)
	TArray<USoundClass*> SoundClasses;

	inline bool operator==(const FRaveSoundGroup& Other) const
	{
		return GroupName == Other.GroupName;
	}
};

/**
 * 
 */
UCLASS()
class RAVE_API URaveAudioUserSettings : public USaveGame
{
	GENERATED_BODY()

public:
	void InitializeAudioSettings(const UObject* WorldContextObject);
	void RegisterSoundGroup(FRaveSoundGroup SoundGroup);
	void UnregisterSoundGroup(FRaveSoundGroup SoundGroup);
	void SaveAudioSettings();

	float GetAudioGroupVolume(FName GroupName);
	void SetAudioGroupVolume(const UObject* WorldContextObject, FName GroupName, float Volume);

protected:
	TArray<FRaveSoundGroup> RegisteredSoundGroups;

	UPROPERTY(SaveGame)
	TMap<FName, float> SavedAudioProfile;
};
