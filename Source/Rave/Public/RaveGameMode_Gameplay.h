

#pragma once

#include "CoreMinimal.h"
#include "RaveGameMode.h"
#include "RaveGameMode_Gameplay.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API ARaveGameMode_Gameplay : public ARaveGameMode
{
	GENERATED_BODY()

public:
	ARaveGameMode_Gameplay(const FObjectInitializer& ObjectInitializer);

	void PostLogin(APlayerController* NewPlayer) override;

#pragma region Character Level
public:
	virtual void DistributeExperience(float ExperienceAmount);
	virtual void LevelUp(AActor* Actor);
	virtual float GetExperienceRequireAmountForLevelUp(float CurrentLevel) const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Level")
	TSubclassOf<class UGameplayEffect> LevelUpGameplayEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Level")
	TObjectPtr<UCurveTable> ExperienceRequireAmountCurveTable;

	UPROPERTY(EditDefaultsOnly, Category = "Level")
	FName ExperienceRequireAmountRowName;

#pragma endregion
};
