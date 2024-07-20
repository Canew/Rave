

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "AI/RaveAIController.h"
#include "RaveBTService_SetBehaviorState.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveBTService_SetBehaviorState : public UBTService
{
	GENERATED_BODY()

public:
	URaveBTService_SetBehaviorState();

	virtual FString GetStaticDescription() const override;

protected:
	/** update next tick interval
  * this function should be considered as const (don't modify state of object) if node is not instanced! */
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
protected:
	UPROPERTY(EditAnywhere)
	ERaveBotBehaviorState BehaviorState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "1.0"))

	float BehaviorStateSetChance;
};
