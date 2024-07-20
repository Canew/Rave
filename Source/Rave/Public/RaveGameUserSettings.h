

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "Audio/RaveAudioUserSettings.h"
#include "RaveGameUserSettings.generated.h"

UENUM(BlueprintType)
enum class ERaveGraphicSettingType : uint8
{
	Overall,
	ViewDistance,
	AntiAliasing,
	Shadow,
	GlobalIllumination,
	Reflection,
	PostProcess,
	Texture,
	Effects,
	Foliage,
	Shading,
};
ENUM_RANGE_BY_FIRST_AND_LAST(ERaveGraphicSettingType, ERaveGraphicSettingType::Overall, ERaveGraphicSettingType::Shading);


/**
 * 
 */
UCLASS()
class RAVE_API URaveGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	URaveGameUserSettings();

	template<class GameUserSettingClass = URaveGameUserSettings>
	static inline GameUserSettingClass* GetGameUserSettings()
	{
		return Cast<GameUserSettingClass>(Super::GetGameUserSettings());
	}

	const TArray<FIntPoint>& GetResolutionSet() const;

	URaveAudioUserSettings* GetAudioUserSettings();
	
protected:
	TArray<FIntPoint> ResolutionSet;

	UPROPERTY()
	TObjectPtr<class URaveAudioUserSettings> AudioUserSettings;
};
