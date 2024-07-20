

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "RaveExecutionCalculation_Damage.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveExecutionCalculation_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	URaveExecutionCalculation_Damage();

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
