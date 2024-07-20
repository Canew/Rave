

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "AI/RaveAIController.h"
#include "RaveBTTask_SetBehaviorState.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveBTTask_SetBehaviorState : public UBTTaskNode
{
	GENERATED_BODY()

public:
	URaveBTTask_SetBehaviorState();

	/** @return name of node */
	FString GetNodeName_Implementation() const;

	/** starts this task, should return Succeeded, Failed or InProgress
 *  (use FinishLatentTask() when returning InProgress)
 * this function should be considered as const (don't modify state of object) if node is not instanced! */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	virtual FString GetStaticDescription() const override;

protected:
	UPROPERTY(EditAnywhere)
	ERaveBotBehaviorState BehaviorState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "1.0"))

	float BehaviorStateSetChance;
};
