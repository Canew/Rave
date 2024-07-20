


#include "Animations/AnimNotifies/RaveAnimNotify_CameraShake.h"
#include "Camera/CameraShakeBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveAnimNotify_CameraShake)

FString URaveAnimNotify_CameraShake::GetNotifyName_Implementation() const
{
	return TEXT("Camera Shake");
}

void URaveAnimNotify_CameraShake::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp || !CameraShakeClass || (ShakeScale <= 0.0f))
	{
		return;
	}

	const ECameraShakePlaySpace CameraShakePlaySpace = GetCameraShakePlaySpace();

	// Check if the camera shake should be played in world for all players.
	if (bPlayInWorld)
	{
		const FVector SpawnLocation = MeshComp->DoesSocketExist(SocketName) ? MeshComp->GetSocketLocation(SocketName) : MeshComp->GetComponentLocation();

		for (FConstPlayerControllerIterator Iterator = MeshComp->GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			APlayerController* PC = Iterator->Get();
			if (!PC || !PC->PlayerCameraManager || !PC->IsLocalController())
			{
				continue;
			}

			const APawn* Pawn = PC->GetPawn();
			if (!Pawn)
			{
				continue;
			}

			const FVector PawnLocation = Pawn->GetActorLocation();
			const float FalloffIntensity = CalculateFalloffIntensity(SpawnLocation, PawnLocation, WorldInnerRadius, WorldOuterRadius, WorldFalloffExponent);

			if (FalloffIntensity <= 0.0f)
			{
				continue;
			}

			UCameraShakeBase* CameraShakeInstance = PC->PlayerCameraManager->StartCameraShake(CameraShakeClass, (ShakeScale * FalloffIntensity), CameraShakePlaySpace, FRotator::ZeroRotator);
		}
	}
	else
	{
		// Play the camera shake directly on the player controller. No intensity falloff is applied when playing directly on the target.
		if (APawn* Pawn = MeshComp->GetOwner<APawn>())
		{
			if (APlayerController* PlayerController = Pawn->GetController<APlayerController>())
			{
				if (PlayerController || PlayerController->PlayerCameraManager || PlayerController->IsLocalController())
				{
					PlayerController->PlayerCameraManager->StartCameraShake(CameraShakeClass, ShakeScale, CameraShakePlaySpace, FRotator::ZeroRotator);
				}
			}
		}
	}

}

ECameraShakePlaySpace URaveAnimNotify_CameraShake::GetCameraShakePlaySpace()
{
	switch (PlaySpace)
	{
	case ERaveCameraShakePlaySpace::WorldSpace:
		return ECameraShakePlaySpace::World;

	case ERaveCameraShakePlaySpace::CameraSpace:
		return ECameraShakePlaySpace::CameraLocal;

	default:
		checkf(false, TEXT("CameraShake: Invalid play space [%d]\n"), (uint8)PlaySpace);
	}

	return ECameraShakePlaySpace::CameraLocal;
}

float URaveAnimNotify_CameraShake::CalculateFalloffIntensity(const FVector& SourceLocation, const FVector& TargetLocation, float InnerRadius, float OuterRadius, float FalloffExponent) const
{
	if ((InnerRadius <= 0.0f) && (OuterRadius <= 0.0f))
	{
		return 1.0f;
	}

	const float DistanceSqr = FVector::DistSquared(SourceLocation, TargetLocation);

	if (InnerRadius < OuterRadius)
	{
		const float Distance = FMath::Sqrt(DistanceSqr);
		const float Percent = 1.0f - FMath::Clamp(((Distance - InnerRadius) / (OuterRadius - InnerRadius)), 0.0f, 1.0f);

		return FMath::Pow(Percent, FalloffExponent);
	}

	// Ignore the outer radius and do a cliff falloff at the inner radius.
	return (DistanceSqr < FMath::Square(InnerRadius)) ? 1.0f : 0.0f;
}