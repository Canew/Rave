

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "RaveExecutionCalculation_Cooldown.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveExecutionCalculation_Cooldown : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	URaveExecutionCalculation_Cooldown();

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
