

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "RaveExecutionCalculation_Force.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveExecutionCalculation_Force : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	URaveExecutionCalculation_Force();

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Rave|Effects")
	TSubclassOf<UGameplayEffect> HitStunEffectClass;
};
