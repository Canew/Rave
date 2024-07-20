

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "RaveAbilityTask_WaitCollisionEvent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRaveWaitCollisionEventDelegate, const FHitResult&, HitResult);

/**
 * 
 */
UCLASS()
class RAVE_API URaveAbilityTask_WaitCollisionEvent : public UAbilityTask
{
	GENERATED_BODY()

public:
	URaveAbilityTask_WaitCollisionEvent();

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE", AutoCreateRefTerm = "IgnoreActors"))
	static URaveAbilityTask_WaitCollisionEvent* WaitCollisionEventAtLocation(UGameplayAbility* OwningAbility, float HalfHeight, float Radius, float Duration, FVector Location, FRotator Rotation, TArray<AActor*> IgnoreActors, bool bOnlyTriggerOnce = false, bool bDebug = false);

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE", AutoCreateRefTerm = "IgnoreActors"))
	static URaveAbilityTask_WaitCollisionEvent* WaitCollisionEventAttached(UGameplayAbility* OwningAbility, float HalfHeight, float Radius, float Duration, USceneComponent* AttachComponent, FName AttachSocketName, FVector LocationOffset, FRotator RotationOffset, TArray<AActor*> IgnoreActors, bool bOnlyTriggerOnce = false, bool bDebug = false);

public:
	UPROPERTY(BlueprintAssignable)
	FRaveWaitCollisionEventDelegate OnCollisionDetected;

protected:
	virtual void OnDestroy(bool bAbilityEnded) override;

	UFUNCTION()
	void HandleComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	float HalfHeight;
	float Radius;

	/** Collision event duration. Duration <= 0.0f means infinite */
	float Duration;

	FVector Location;
	FRotator Rotation;
	TWeakObjectPtr<class USceneComponent> AttachComponent;
	FName AttachSocketName;
	TArray<AActor*> IgnoreActors;
	bool bOnlyTriggerOnce;
	bool bDebug;

	TObjectPtr<class UCapsuleComponent> CollisionComponent;
	float AccumulatedTime;

	TArray<AActor*> CollideActors;
};
