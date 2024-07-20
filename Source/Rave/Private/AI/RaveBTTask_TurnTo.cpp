


#include "AI/RaveBTTask_TurnTo.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "AIController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveBTTask_TurnTo)

URaveBTTask_TurnTo::URaveBTTask_TurnTo()
{
	NodeName = FString(TEXT("Turn To"));
	TurnInterpSpeed = 30.0f;
	bUseCharacterRotationRate = false;

	// accept only actors and vectors
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(URaveBTTask_TurnTo, BlackboardKey), AActor::StaticClass());
	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(URaveBTTask_TurnTo, BlackboardKey));
}

FString URaveBTTask_TurnTo::GetNodeName_Implementation() const
{
	return FString(TEXT("Turn To"));
}

EBTNodeResult::Type URaveBTTask_TurnTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	APawn* Pawn = AIController->GetPawn();
	if (!Pawn)
	{
		return EBTNodeResult::Failed;
	}

	AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BlackboardKey.SelectedKeyName));
	if (!TargetActor)
	{
		return EBTNodeResult::Failed;
	}

	float RotationRate = TurnInterpSpeed;
	if (bUseCharacterRotationRate)
	{
		UCharacterMovementComponent* CharacterMovementComponent = Pawn->GetComponentByClass<UCharacterMovementComponent>();
		if (CharacterMovementComponent)
		{
			RotationRate = CharacterMovementComponent->RotationRate.Yaw;
		}
	}

	FRotator PawnToTargetRotator = (TargetActor->GetActorLocation() - Pawn->GetActorLocation()).Rotation();
	PawnToTargetRotator.Roll = 0.0f;
	PawnToTargetRotator.Pitch = 0.0f;
	FRotator NewRotation = FMath::RInterpConstantTo(Pawn->GetActorRotation(), PawnToTargetRotator, Pawn->GetWorld()->GetDeltaSeconds(), RotationRate);
	Pawn->SetActorRotation(NewRotation);

	return EBTNodeResult::Succeeded;
}

FString URaveBTTask_TurnTo::GetStaticDescription() const
{
	FString KeyDesc("invalid");
	if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass() ||
		BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		KeyDesc = BlackboardKey.SelectedKeyName.ToString();
	}

	return FString::Printf(TEXT("%s: %s"), *Super::GetStaticDescription(), *KeyDesc);
}

