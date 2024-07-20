

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "RaveBTTask_TryActivateAbility.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveBTTask_TryActivateAbility : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	URaveBTTask_TryActivateAbility();

	/** starts this task, should return Succeeded, Failed or InProgress
 *  (use FinishLatentTask() when returning InProgress)
 * this function should be considered as const (don't modify state of object) if node is not instanced! */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	/** helper function: finish latent executing */
	void FinishLatentTask(UBehaviorTreeComponent& OwnerComp, EBTNodeResult::Type TaskResult) const;

protected:
	/** ticks this task
	  * this function should be considered as const (don't modify state of object) if node is not instanced! */
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);

	virtual void OnAbilityEnded(class UGameplayAbility* Ability);

protected:
	class UAbilitySystemComponent* AbilitySystemComponent;
	class UBehaviorTreeComponent* OwnerBehaviorComponent;

	TSubclassOf<class UGameplayAbility> CurrentAbilityClass;
	struct FGameplayAbilitySpec* CurrentAbilitySpec;

	FDelegateHandle OnAbilityEndedDelegateHandle;
};
