

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "RaveExecutionCalculation_Healing.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveExecutionCalculation_Healing : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	URaveExecutionCalculation_Healing();

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
