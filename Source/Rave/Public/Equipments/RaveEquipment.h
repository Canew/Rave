

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RaveEquipment.generated.h"

/**
 * URaveEquipment
 *
 *	Equipment actor that is actually attached to the character mesh
 */
UCLASS()
class RAVE_API ARaveEquipment : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ARaveEquipment();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;	

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintPure, Category = "Rave|Equipment")
	virtual class UStaticMeshComponent* const GetMesh() { return EquipmentMesh; };

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TObjectPtr<class UStaticMeshComponent> EquipmentMesh;
};
