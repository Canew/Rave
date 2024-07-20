

#pragma once

#include "CoreMinimal.h"
#include "RaveGameMode_Gameplay.h"
#include "RaveGameMode_Arena.generated.h"

USTRUCT(BlueprintType)
struct FRaveArenaSequence
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> SequenceSpawnActorClass;

	UPROPERTY(EditDefaultsOnly)
	int32 SequenceCount;

	UPROPERTY(EditDefaultsOnly)
	float SequenceDelay;
};

USTRUCT(BlueprintType)
struct FRaveArenaWave
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TArray<FRaveArenaSequence> Sequences;
};

/**
 * 
 */
UCLASS()
class RAVE_API ARaveGameMode_Arena : public ARaveGameMode_Gameplay
{
	GENERATED_BODY()

public:
	virtual void StartMatch() override;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Wave")
	TArray<FRaveArenaWave> Waves;

	UPROPERTY(EditDefaultsOnly, Category = "Wave")
	TSubclassOf<AActor> SpawnPointActorClass;
};
