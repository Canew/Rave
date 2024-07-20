


#include "GameplayAbility/Tasks/RaveAbilityTask_SpawnProjectile.h"
#include "EngineGlobals.h"
#include "Engine/Engine.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveAbilityTask_SpawnProjectile)

URaveAbilityTask_SpawnProjectile::URaveAbilityTask_SpawnProjectile(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

URaveAbilityTask_SpawnProjectile* URaveAbilityTask_SpawnProjectile::SpawnProjectile(UGameplayAbility* OwningAbility, FGameplayAbilityTargetDataHandle TargetData, TSubclassOf<ARaveProjectile> InClass, FVector Velocity, TArray<FGameplayEffectSpecHandle> ProjectileEffectSpecs)
{
	URaveAbilityTask_SpawnProjectile* MyObj = NewAbilityTask<URaveAbilityTask_SpawnProjectile>(OwningAbility);
	MyObj->CachedTargetDataHandle = MoveTemp(TargetData);
	MyObj->Velocity = Velocity;
	MyObj->ProjectileEffectSpecs = ProjectileEffectSpecs;
	return MyObj;
}

// ---------------------------------------------------------------------------------------

bool URaveAbilityTask_SpawnProjectile::BeginSpawningActor(UGameplayAbility* OwningAbility, FGameplayAbilityTargetDataHandle TargetData, TSubclassOf<ARaveProjectile> InClass, ARaveProjectile*& SpawnedActor)
{
	if (Ability && Ability->GetCurrentActorInfo()->IsNetAuthority() && ShouldBroadcastAbilityTaskDelegates())
	{
		UWorld* const World = GEngine->GetWorldFromContextObject(OwningAbility, EGetWorldErrorMode::LogAndReturnNull);
		if (World)
		{
			SpawnedActor = World->SpawnActorDeferred<ARaveProjectile>(InClass, FTransform::Identity, NULL, NULL, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		}
	}

	if (SpawnedActor == nullptr)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			DidNotSpawn.Broadcast(nullptr);
		}

		return false;
	}

	return true;
}

void URaveAbilityTask_SpawnProjectile::FinishSpawningActor(UGameplayAbility* OwningAbility, FGameplayAbilityTargetDataHandle TargetData, ARaveProjectile* SpawnedActor)
{
	if (SpawnedActor)
	{
		bool bTransformSet = false;
		FTransform SpawnTransform;
		if (FGameplayAbilityTargetData* LocationData = CachedTargetDataHandle.Get(0))		//Hardcode to use data 0. It's OK if data isn't useful/valid.
		{
			//Set location. Rotation is unaffected.
			if (LocationData->HasHitResult())
			{
				SpawnTransform.SetLocation(LocationData->GetHitResult()->Location);
				bTransformSet = true;
			}
			else if (LocationData->HasEndPoint())
			{
				SpawnTransform = LocationData->GetEndPointTransform();
				bTransformSet = true;
			}
		}
		if (!bTransformSet)
		{
			if (UAbilitySystemComponent* ASC = AbilitySystemComponent.Get())
			{
				SpawnTransform = ASC->GetOwner()->GetTransform();
			}
		}

		SpawnedActor->SetOwner(GetAvatarActor());
		if (UProjectileMovementComponent* ProjectileMovementComponent = SpawnedActor->GetProjectileMovement())
		{
			if (Velocity.IsZero())
			{
				ProjectileMovementComponent->Velocity = DecideProjectileVelocity(SpawnTransform.GetLocation(), ProjectileMovementComponent);
			}
			else
			{
				ProjectileMovementComponent->Velocity = Velocity;
			}
		}

		for (const FGameplayEffectSpecHandle& ProjectileEffectSpec : ProjectileEffectSpecs)
		{
			SpawnedActor->AddImpactGameplayEffectSpec(ProjectileEffectSpec);
		}

		SpawnedActor->FinishSpawning(SpawnTransform);

		if (ShouldBroadcastAbilityTaskDelegates())
		{
			Success.Broadcast(SpawnedActor);
		}
	}

	EndTask();
}

FVector URaveAbilityTask_SpawnProjectile::DecideProjectileVelocity(FVector SpawnLocation, UProjectileMovementComponent* ProjectileMovement) const
{
	FVector ProjectileVelocity = FVector::ZeroVector;
	if (AActor* AvatarActor = GetAvatarActor())
	{
		ProjectileVelocity = AvatarActor->GetActorForwardVector();
		if (UCameraComponent* AvatarCamera = AvatarActor->FindComponentByClass<UCameraComponent>())
		{
			FHitResult HitResult;
			FVector TraceStart = AvatarCamera->GetComponentLocation();
			if (USpringArmComponent* AvatarSpringArm = Cast<USpringArmComponent>(AvatarCamera->GetAttachParent()))
			{
				// Trace camera probe collision
				FVector CameraTowardsDirection = (AvatarCamera->GetComponentLocation() - AvatarSpringArm->GetComponentLocation()).GetSafeNormal();
				FVector CameraTowardsDirection2D = CameraTowardsDirection;
				CameraTowardsDirection2D.Z = 0.0f;
				CameraTowardsDirection.X = -AvatarActor->GetActorForwardVector().X * CameraTowardsDirection2D.Length();
				CameraTowardsDirection.Y = -AvatarActor->GetActorForwardVector().Y * CameraTowardsDirection2D.Length();
				AvatarActor->GetWorld()->LineTraceSingleByChannel(HitResult, AvatarSpringArm->GetComponentLocation(), AvatarSpringArm->GetComponentLocation() + (CameraTowardsDirection * AvatarSpringArm->TargetArmLength), ECC_Visibility, FCollisionQueryParams());
				TraceStart = HitResult.bBlockingHit ? HitResult.ImpactPoint : HitResult.TraceEnd;

				// Trace towards target
				constexpr float MaxTraceDistance = 20000.0f;
				float ProjectileGravityScale = ProjectileMovement->ProjectileGravityScale;
				float TraceDistance = (ProjectileGravityScale == 0.0f) ? MaxTraceDistance : ((1.0f / ProjectileGravityScale) * 1.5f * ProjectileMovement->MaxSpeed);
				FVector TraceEnd = TraceStart + ((FMath::Clamp(TraceDistance, 0.0f, MaxTraceDistance) * ((AvatarSpringArm->GetComponentLocation() - TraceStart).GetSafeNormal())));
				FCollisionQueryParams CollisionQueryParams;
				CollisionQueryParams.AddIgnoredActor(AvatarActor);
				CollisionQueryParams.AddIgnoredActor(HitResult.GetActor());
				AvatarActor->GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, CollisionQueryParams);

				// Predict projectile velocity
				FVector OutLaunchVelocity;
				FVector EndLocation = (HitResult.bBlockingHit ? HitResult.ImpactPoint : HitResult.TraceEnd);
				if (!UGameplayStatics::SuggestProjectileVelocity(AvatarActor, OutLaunchVelocity, SpawnLocation, EndLocation, ProjectileMovement->MaxSpeed, false, 0.0f, ProjectileMovement->GetGravityZ(), ESuggestProjVelocityTraceOption::DoNotTrace))
				{
					OutLaunchVelocity = (EndLocation - SpawnLocation).GetSafeNormal();
					OutLaunchVelocity.Z = OutLaunchVelocity.Length();
					OutLaunchVelocity = OutLaunchVelocity.GetSafeNormal() * ProjectileMovement->MaxSpeed;
				}

				ProjectileVelocity = OutLaunchVelocity;
			}
		}
		else
		{
			if (APlayerController* PlayerController = AvatarActor->GetInstigatorController<APlayerController>())
			{
				ProjectileVelocity.Z = PlayerController->GetControlRotation().Vector().Z;
			}
		}
	}

	return ProjectileVelocity;
}

// ---------------------------------------------------------------------------------------
