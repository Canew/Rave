

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RaveEquipmentComponent.generated.h"

/**
 * URaveiEquipmentComponent
 *
 * Component that manage the character's equipment
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RAVE_API URaveEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URaveEquipmentComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	void EquipEquipment(const class URaveEquipmentDefinition* EquipmentDefinition);
	class ARaveEquipment* GetEquipment() const;
	UMeshComponent* GetEquipmentMesh() const;

protected:
	UPROPERTY(Replicated)
	TObjectPtr<class ARaveEquipment> SpawnedEquipment;
};
