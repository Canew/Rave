

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RaveEquipmentDefinition.generated.h"

/**
 * URaveEquipmentDefinition
 *
 * Define properties of equipment.
 */
UCLASS()
class RAVE_API URaveEquipmentDefinition : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	TSubclassOf<class ARaveEquipment> SpawnToEquipmentActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	FName AttachSocketName;

	UPROPERTY(EditAnywhere, Category = "Equipment")
	FTransform AttachTransform;
};
