

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Gameplay/RaveProjectile.h"
#include "GameplayEffectTypes.h"
#include "RaveAbilityTask_SpawnProjectile.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRaveSpawnProjectileDelegate, ARaveProjectile*, SpawnedProjectile);

/**
 * 
 */
UCLASS()
class RAVE_API URaveAbilityTask_SpawnProjectile : public UAbilityTask
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(BlueprintAssignable)
	FRaveSpawnProjectileDelegate Success;

	/** Called when we can't spawn: on clients or potentially on server if they fail to spawn (rare) */
	UPROPERTY(BlueprintAssignable)
	FRaveSpawnProjectileDelegate DidNotSpawn;

	/** Spawn new Actor on the network authority (server) */
	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true", AutoCreateRefTerm = "ProjectileEffectSpecs"), Category = "Rave|Ability|Tasks")
	static URaveAbilityTask_SpawnProjectile* SpawnProjectile(UGameplayAbility* OwningAbility, FGameplayAbilityTargetDataHandle TargetData, TSubclassOf<ARaveProjectile> Class, FVector Velocity, TArray<FGameplayEffectSpecHandle> ProjectileEffectSpecs);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"), Category = "Rave|Abilities")
	bool BeginSpawningActor(UGameplayAbility* OwningAbility, FGameplayAbilityTargetDataHandle TargetData, TSubclassOf<ARaveProjectile> Class, ARaveProjectile*& SpawnedActor);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"), Category = "Rave|Abilities")
	void FinishSpawningActor(UGameplayAbility* OwningAbility, FGameplayAbilityTargetDataHandle TargetData, ARaveProjectile* SpawnedActor);

protected:
	FVector DecideProjectileVelocity(FVector SpawnLocation, class UProjectileMovementComponent* ProjectileMovement) const;

protected:
	FGameplayAbilityTargetDataHandle CachedTargetDataHandle;

	FVector Velocity;
	TArray<FGameplayEffectSpecHandle> ProjectileEffectSpecs;
};
