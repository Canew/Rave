


#include "Equipments/RaveEquipment.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveEquipment)

ARaveEquipment::ARaveEquipment()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	EquipmentMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EquipmentMesh"));
	EquipmentMesh->SetGenerateOverlapEvents(false);
	EquipmentMesh->SetCollisionProfileName(TEXT("NoCollision"));
	SetRootComponent(EquipmentMesh);
}

void ARaveEquipment::BeginPlay()
{
	Super::BeginPlay();

}

void ARaveEquipment::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
