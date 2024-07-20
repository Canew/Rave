


#include "Animations/AnimNotifies/RaveAnimNotify_Footstep.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Sound/SoundBase.h"

FString URaveAnimNotify_Footstep::GetNotifyName_Implementation() const
{
	return TEXT("Footstep");
}

void URaveAnimNotify_Footstep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (APawn* Pawn = MeshComp->GetOwner<APawn>())
	{
		if (USkeletalMeshComponent* SkeletalMeshComponent = Pawn->FindComponentByClass<USkeletalMeshComponent>())
		{
			if (SkeletalMeshComponent->DoesSocketExist(FootSocketName))
			{
				FHitResult HitResult;
				FVector TraceStart = SkeletalMeshComponent->GetSocketLocation(FootSocketName);
				FVector TraceEnd = TraceStart - FVector(0.0f, 0.0f, FootTraceDistance);
				FCollisionQueryParams CollisionQueryParams;
				CollisionQueryParams.AddIgnoredActor(Pawn);
				CollisionQueryParams.bReturnPhysicalMaterial = true;
				if (Pawn->GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility))
				{
					UGameplayStatics::PlaySoundAtLocation(Pawn, FootstepSound, HitResult.ImpactPoint);
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(Pawn, FootstepParticle, HitResult.ImpactPoint);
				}
			}
		}
	}
}
