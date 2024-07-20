

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Styling/SlateBrush.h"
#include "VrmAssetListObject.h"
#include "RaveCharacterSetting.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveCharacterSetting : public UDataAsset
{
	GENERATED_BODY()

public:
	URaveCharacterSetting(const FObjectInitializer& ObjectInitializer);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character")
	TSoftObjectPtr<USkeletalMesh> CharacterMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character")
	TSubclassOf<UAnimInstance> CharacterAnimInstanceClass;

	/** Character illustration portrait to show the player */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character")
	TSoftObjectPtr<UTexture2D> Portrait;

	/** Optional data asset for VRoid character setting */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character")
	TSoftObjectPtr<UVrmAssetListObject> VrmAssetList;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TObjectPtr<const class URaveEquipmentDefinition> EquipmentDefinition;

	// Ability sets to grant to this pawn's ability system.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<TObjectPtr<class URaveAbilitySet>> AbilitySets;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class URaveInputConfig> InputConfig;
};
