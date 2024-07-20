


#include "AI/RaveBTTask_SetBehaviorState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveBTTask_SetBehaviorState)

URaveBTTask_SetBehaviorState::URaveBTTask_SetBehaviorState()
{
	NodeName = TEXT("Set Behavior State");
}

FString URaveBTTask_SetBehaviorState::GetNodeName_Implementation() const
{
	return FString(TEXT("Set Behavior State"));
}

EBTNodeResult::Type URaveBTTask_SetBehaviorState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ARaveAIController* AIController = Cast<ARaveAIController>(OwnerComp.GetAIOwner());
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	if (BehaviorStateSetChance > FMath::FRand())
	{
		AIController->SetBehaviorState(BehaviorState);
	}

	return EBTNodeResult::Succeeded;
}

FString URaveBTTask_SetBehaviorState::GetStaticDescription() const
{
	return FString::Printf(TEXT("%s: %s, %s chance"), *Super::GetStaticDescription(), *UEnum::GetValueAsString(BehaviorState), *FString::SanitizeFloat(BehaviorStateSetChance));
}

