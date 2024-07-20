

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/StreamableManager.h"
#include "OnlineSubsystemUtils.h"
#include "AdvancedFriendsGameInstance.h"
#include "RaveGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveGameInstance : public UAdvancedFriendsGameInstance
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;

public:
	const TArray<struct FRaveSoundGroup>& GetGameSoundGroups() const;
	const class URaveOperationData* GetOperationData() const;
	const TArray<class URaveCharacterSetting*> GetCharacterSettings() const;

protected:
	void InitializeAudioUserSettings();

public:
	FStreamableManager StreamableManager;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TArray<struct FRaveSoundGroup> GameSoundGroups;

	UPROPERTY(EditDefaultsOnly, Category = "Operation")
	TObjectPtr<class URaveOperationData> OperationData;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	TArray<class URaveCharacterSetting*> CharacterSettings;
};
