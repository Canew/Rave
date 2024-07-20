


#include "Equipments/RaveEquipmentComponent.h"
#include "Equipments/RaveEquipmentDefinition.h"
#include "Equipments/RaveEquipment.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveEquipmentComponent)

URaveEquipmentComponent::URaveEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void URaveEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URaveEquipmentComponent, SpawnedEquipment);
}

void URaveEquipmentComponent::EquipEquipment(const URaveEquipmentDefinition* EquipmentDefinition)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	if (SpawnedEquipment)
	{
		SpawnedEquipment->Destroy();
		SpawnedEquipment = nullptr;
	}

	if (EquipmentDefinition)
	{
		FActorSpawnParameters EquipmentSpawnParameters;
		EquipmentSpawnParameters.bNoFail = true;
		EquipmentSpawnParameters.Owner = GetOwner();
		EquipmentSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnedEquipment = GetWorld()->SpawnActor<ARaveEquipment>(EquipmentDefinition->SpawnToEquipmentActorClass, EquipmentSpawnParameters);
		if (SpawnedEquipment)
		{
			if (USkeletalMeshComponent* OwnerSkeletalMeshComponent = GetOwner()->FindComponentByClass<USkeletalMeshComponent>())
			{
				SpawnedEquipment->AttachToComponent(OwnerSkeletalMeshComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, EquipmentDefinition->AttachSocketName);
			}
		}
	}
}

ARaveEquipment* URaveEquipmentComponent::GetEquipment() const
{
	return SpawnedEquipment;
}

UMeshComponent* URaveEquipmentComponent::GetEquipmentMesh() const
{
	ARaveEquipment* RaveEquipment = SpawnedEquipment;
	if (RaveEquipment)
	{
		return RaveEquipment->GetMesh();
	}

	return nullptr;
}
