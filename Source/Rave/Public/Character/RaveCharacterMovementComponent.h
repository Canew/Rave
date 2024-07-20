

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RaveCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	URaveCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);

	virtual float GetMaxSpeed() const override;
	virtual float GetMaxAcceleration() const override;
	virtual bool HandlePendingLaunch() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	bool IsSprinting() const;
	void Sprint();
	void UnSprint();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Rave|Movement")
	float GuardSpeedMultiplier;

	UPROPERTY(EditDefaultsOnly, Category = "Rave|Movement")
	float PatrolSpeedMultiplier;

	UPROPERTY(Replicated)
	bool bIsSprinting;

	float SprintSpeedMultiplier;
};
