


#include "AI/RaveBTService_Targeting.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "AI/RaveAIController.h"
#include "Character/RaveCharacterTargetingComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveBTService_Targeting)

URaveBTService_Targeting::URaveBTService_Targeting()
{
	NodeName = TEXT("Targeting");
	bNotifyTick = true;
	bNotifyBecomeRelevant = true;

	BlackboardKey.AddObjectFilter(this, GetSelectedBlackboardKey(), AActor::StaticClass());
}

void URaveBTService_Targeting::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	ARaveAIController* AIController = Cast<ARaveAIController>(OwnerComp.GetAIOwner());
	if (!AIController || !AIController->GetPawn())
	{
		return;
	}

	AActor* PerceivedHostileActor = nullptr;
	UAIPerceptionComponent* PerceptionComponent = AIController->GetPerceptionComponent();
	if (PerceptionComponent)
	{
		TArray<AActor*> PerceivedHostileActors;
		PerceptionComponent->GetPerceivedHostileActors(PerceivedHostileActors);

		if (PerceivedHostileActors.Num() > 0)
		{
			int32 PerceivedHostileActorIndex = FMath::RandRange(0, PerceivedHostileActors.Num() - 1);
			PerceivedHostileActor = PerceivedHostileActors[PerceivedHostileActorIndex];

			AIController->SetFocus(PerceivedHostileActor);
			UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent();
			if (BlackboardComponent)
			{
				BlackboardComponent->SetValueAsObject(GetSelectedBlackboardKey(), PerceivedHostileActor);
			}
		}
		else
		{
			AIController->SetFocus(nullptr);
			UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent();
			if (BlackboardComponent)
			{
				BlackboardComponent->SetValueAsObject(GetSelectedBlackboardKey(), nullptr);
			}

			AIController->SetBehaviorState(ERaveBotBehaviorState::Idle);
		}
	}

	URaveCharacterTargetingComponent* CharacterTargetingComponent = AIController->GetPawn()->GetComponentByClass<URaveCharacterTargetingComponent>();
	if (CharacterTargetingComponent)
	{
		CharacterTargetingComponent->SetLockOnActor(PerceivedHostileActor);
	}
}
