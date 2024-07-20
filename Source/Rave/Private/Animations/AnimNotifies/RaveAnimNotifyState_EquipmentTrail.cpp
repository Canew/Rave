


#include "Animations/AnimNotifies/RaveAnimNotifyState_EquipmentTrail.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Equipments/RaveEquipment.h"
#include "Equipments/RaveEquipmentComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveAnimNotifyState_EquipmentTrail)

URaveAnimNotifyState_EquipmentTrail::URaveAnimNotifyState_EquipmentTrail(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Template = nullptr;
	LocationOffset.Set(0.0f, 0.0f, 0.0f);
	RotationOffset = FRotator(0.0f, 0.0f, 0.0f);
}

UFXSystemComponent* URaveAnimNotifyState_EquipmentTrail::SpawnEffect(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) const
{
	// Only spawn if we've got valid params
	if (ValidateParameters(MeshComp))
	{
		return UNiagaraFunctionLibrary::SpawnSystemAttached(Template, MeshComp, EquipmentAttachedSocketName, LocationOffset, RotationOffset, EAttachLocation::KeepRelativeOffset, !bDestroyAtEnd);
	}

	return nullptr;
}

UFXSystemComponent* URaveAnimNotifyState_EquipmentTrail::GetSpawnedEffect(UMeshComponent* MeshComp)
{
	if (MeshComp)
	{
		TArray<USceneComponent*> Children;
		MeshComp->GetChildrenComponents(false, Children);

		if (Children.Num())
		{
			for (USceneComponent* Component : Children)
			{
				if (Component && Component->ComponentHasTag(GetSpawnedComponentTag()))
				{
					if (UFXSystemComponent* FXComponent = CastChecked<UFXSystemComponent>(Component))
					{
						return FXComponent;
					}
				}
			}
		}
	}

	return nullptr;
}

void URaveAnimNotifyState_EquipmentTrail::NotifyBegin(USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (UFXSystemComponent* FXSystemComponent = SpawnEffect(MeshComp, Animation))
	{
		if (AActor* MeshOwner = MeshComp->GetOwner())
		{
			EquipmentComponent = MeshOwner->FindComponentByClass<URaveEquipmentComponent>();
		}

		// tag the component with the AnimNotify that is triggering the animation so that we can properly clean it up
		FXSystemComponent->ComponentTags.AddUnique(GetSpawnedComponentTag());
	}

	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
}

void URaveAnimNotifyState_EquipmentTrail::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!EquipmentComponent.IsValid())
	{
		return;
	}

	if (UFXSystemComponent* FXSystemComponent = GetSpawnedEffect(MeshComp))
	{
		UMeshComponent* EquipmentMesh = EquipmentComponent->GetEquipmentMesh();
		if (IsValid(EquipmentMesh) && EquipmentMesh->DoesSocketExist(StartEquipmentSocketName) && EquipmentMesh->DoesSocketExist(EndEquipmentSocketName))
		{
			FVector EquipmentTrailStartLocation = EquipmentMesh->GetSocketLocation(StartEquipmentSocketName);
			FXSystemComponent->SetVectorParameter(StartSystemParameterName, EquipmentTrailStartLocation);

			FVector EquipmentTrailEndLocation = EquipmentMesh->GetSocketLocation(EndEquipmentSocketName);
			FXSystemComponent->SetVectorParameter(EndSystemParameterName, EquipmentTrailEndLocation);
		}
	}
}

void URaveAnimNotifyState_EquipmentTrail::NotifyEnd(USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (UFXSystemComponent* FXComponent = GetSpawnedEffect(MeshComp))
	{
		// untag the component
		FXComponent->ComponentTags.Remove(GetSpawnedComponentTag());

		// Either destroy the component or deactivate it to have it's active FXSystems finish.
		// The component will auto destroy once all FXSystem are gone.
		if (bDestroyAtEnd)
		{
			FXComponent->DestroyComponent();
		}
		else
		{
			FXComponent->Deactivate();
		}
	}

	Super::NotifyEnd(MeshComp, Animation, EventReference);
}

bool URaveAnimNotifyState_EquipmentTrail::ValidateParameters(USkeletalMeshComponent* MeshComp) const
{
	bool bValid = true;

	if (!Template)
	{
		bValid = false;
	}
	else if (!MeshComp->DoesSocketExist(EquipmentAttachedSocketName) && MeshComp->GetBoneIndex(EquipmentAttachedSocketName) == INDEX_NONE)
	{
		bValid = false;
	}

	return bValid;
}

FString URaveAnimNotifyState_EquipmentTrail::GetNotifyName_Implementation() const
{
	if (Template)
	{
		return Template->GetName();
	}

	return TEXT("Equipment Trail");
}
