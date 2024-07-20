

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "RaveBTDecorator_CanActivateAbility.generated.h"

struct FBTDecorator_CanActivateAbility_Memory
{
	bool BeforeData;
};

/**
 * 
 */
UCLASS()
class RAVE_API URaveBTDecorator_CanActivateAbility : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()
	
public:
	URaveBTDecorator_CanActivateAbility();

protected:
	/** calculates raw, core value of decorator's condition. Should not include calling IsInversed */
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	/** tick function
	  * this function should be considered as const (don't modify state of object) if node is not instanced! */
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);

	/** called when auxiliary node becomes active
	  * this function should be considered as const (don't modify state of object) if node is not instanced! */
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	uint16 GetInstanceMemorySize() const;

	UClass* GetCurrentAbilityClass(class AAIController* AIController) const;
};
