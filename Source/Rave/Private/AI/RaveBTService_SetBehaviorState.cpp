


#include "AI/RaveBTService_SetBehaviorState.h"
#include "AI/RaveAIController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveBTService_SetBehaviorState)

URaveBTService_SetBehaviorState::URaveBTService_SetBehaviorState()
{
	NodeName = TEXT("Set Behavior State");
	bNotifyTick = true;
	bNotifyBecomeRelevant = true;
}

FString URaveBTService_SetBehaviorState::GetStaticDescription() const
{
	return FString::Printf(TEXT("%s: %s, %s chance"), *Super::GetStaticDescription(), *UEnum::GetValueAsString(BehaviorState), *FString::SanitizeFloat(BehaviorStateSetChance));
}

void URaveBTService_SetBehaviorState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	ARaveAIController* AIController = Cast<ARaveAIController>(OwnerComp.GetAIOwner());
	if (!AIController)
	{
		return;
	}

	if (BehaviorStateSetChance > FMath::FRand())
	{
		AIController->SetBehaviorState(BehaviorState);
	}
}
