

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "RaveProjectile.generated.h"

UCLASS()
class RAVE_API ARaveProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARaveProjectile();

	UFUNCTION(BlueprintPure, Category = "Rave|Projectile")
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() { return ProjectileMovement; }

	UFUNCTION(BlueprintCallable, Category = "Rave|Projectile")
	void AddImpactGameplayEffectSpec(FGameplayEffectSpecHandle ImpactGameplayEffectSpecHandle);

	UFUNCTION(BlueprintCallable, Category = "Rave|Projectile")
	void AddImpactGameplayEvent(FGameplayTag EventTag);

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TObjectPtr<class USphereComponent> ProjectileCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TObjectPtr<class UStaticMeshComponent> ProjectileMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TObjectPtr<class UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rave|Gameplay Cue", meta = (Categories = "GameplayCue"))
	FGameplayTag ImpactGameplayCueTag;

	UPROPERTY()
	TArray<FGameplayEffectSpecHandle> ImpactGameplayEffectSpecHandles;

	UPROPERTY()
	FGameplayTagContainer ImpactGameplayEventTags;
};
