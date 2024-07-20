

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RaveCharacterTargetingComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRaveLockOnActorChangedDelegate, AActor*, Actor);

/**
 * URaveCharacterTargetingComponent
 * 
 * Helper component that decides character abilities direction or projectile velocity
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RAVE_API URaveCharacterTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URaveCharacterTargetingComponent();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UFUNCTION(Server, Reliable)
	virtual void ServerUpdateDesiredOrientation(FVector Orientation);
	virtual void UpdateDesiredOrientation(FVector Orientation);

	UFUNCTION(BlueprintCallable, Category = "Rave|Targeting")
	virtual void LockOn();

	UFUNCTION(BlueprintPure, Category = "Rave|Targeting")
	virtual AActor* GetLockOnActor() const;

	UFUNCTION(BlueprintCallable, Category = "Rave|Targeting")
	virtual void SetLockOnActor(AActor* Actor);

	/* Return normalized vector towards locked on actor. If doesn't locked on, return zero vector. */
	UFUNCTION(BlueprintPure, Category = "Rave|Targeting")
	virtual FVector GetOrientationTowardsTarget() const;

	/* Return distance to locked on actor. If doesn't locked on, return zero. */
	UFUNCTION(BlueprintPure, Category = "Rave|Targeting")
	virtual float GetDistanceToTarget() const;

protected:
	virtual AActor* FindLockOnActor();
	virtual bool CanLockOn(AActor* Actor) const;
	virtual void UpdateWarpTarget();
	virtual void LockCamera(float DeltaTime);
	virtual void ShowLockOnMarker(AActor* Actor);
	virtual void HiddenLockOnMarker();

public:
	UPROPERTY(BlueprintAssignable, Category = "Rave|Targeting")
	FRaveLockOnActorChangedDelegate LockOnActorChanged;

protected:
	UPROPERTY()
	FVector DesiredOrientation;

	UPROPERTY()
	TWeakObjectPtr<AActor> LockOnActor;

	UPROPERTY()
	TObjectPtr<AActor> LockOnMarker;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<AActor> LockOnMarkerClass;

	float MaxLockOnDistance;
	float LockOnRadius;
	float LockCameraInterpSpeed;
};
