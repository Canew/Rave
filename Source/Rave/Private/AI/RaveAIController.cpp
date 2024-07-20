


#include "AI/RaveAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "MotionWarpingComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveAIController)

#define BOT_TEAM 2

FName ARaveAIController::HomeLocationKeyName = FName(TEXT("HomeLocation"));
FName ARaveAIController::MoveLocationKeyName = FName(TEXT("MoveLocation"));
FName ARaveAIController::TargetActorKeyName = FName(TEXT("TargetActor"));
FName ARaveAIController::BehaviorStateKeyName = FName(TEXT("BehaviorState"));

ARaveAIController::ARaveAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	bAttachToPawn = true;
	bWantsPlayerState = true;

	// Setup the perception component
	SetPerceptionComponent(*CreateOptionalDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception")));

	SightSenseConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightSenseConfig"));
	SightSenseConfig->SightRadius = 2000.0f;
	SightSenseConfig->LoseSightRadius = 2000.0f;
	SightSenseConfig->SetMaxAge(8.0f);
	SightSenseConfig->PeripheralVisionAngleDegrees = 180.0f;
	GetPerceptionComponent()->ConfigureSense(*SightSenseConfig);

	DamageSenseConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageSenseConfig"));
	DamageSenseConfig->SetMaxAge(8.0f);
	GetPerceptionComponent()->ConfigureSense(*DamageSenseConfig);

	HearingSenseConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingSenseConfig"));
	HearingSenseConfig->HearingRange = 350.0f;
	HearingSenseConfig->SetMaxAge(8.0f);
	GetPerceptionComponent()->ConfigureSense(*HearingSenseConfig);

	GetPerceptionComponent()->SetDominantSense(SightSenseConfig->GetSenseImplementation());

	BehaviorState = ERaveBotBehaviorState::Idle;
}

void ARaveAIController::BeginPlay()
{
	Super::BeginPlay();

	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &ARaveAIController::TargetPerceptionUpdatedCallback);
	GetPerceptionComponent()->OnTargetPerceptionForgotten.AddDynamic(this, &ThisClass::TargetPerceptionForgottenCallback);
	UAIPerceptionSystem::GetCurrent(GetWorld())->UpdateListener(*GetAIPerceptionComponent());

	RunAI();
	if (APawn* MyPawn = GetPawn())
	{
		if (Blackboard && Blackboard->IsValidKey(Blackboard->GetKeyID(HomeLocationKeyName)))
		{
			Blackboard->SetValueAsVector(HomeLocationKeyName, MyPawn->GetActorLocation());
		}
	}

	// Generic team
	SetGenericTeamId(BOT_TEAM);
}

ERaveBotBehaviorState ARaveAIController::GetBehaviorState() const
{
	return BehaviorState;
}

void ARaveAIController::SetBehaviorState(ERaveBotBehaviorState NewBehaviorState)
{
	if (NewBehaviorState == GetBehaviorState())
	{
		return;
	}

	if (!IsValid(GetPawn()))
	{
		return;
	}

	UCharacterMovementComponent* CharacterMovementComponent = GetPawn()->GetComponentByClass<UCharacterMovementComponent>();
	if (!IsValid(CharacterMovementComponent))
	{
		return;
	}

	switch (NewBehaviorState)
	{
	case ERaveBotBehaviorState::Idle:
		CharacterMovementComponent->bOrientRotationToMovement = true;
		break;

	case ERaveBotBehaviorState::Patrol:
		CharacterMovementComponent->bOrientRotationToMovement = true;
		break;

	case ERaveBotBehaviorState::Guard:
		CharacterMovementComponent->bOrientRotationToMovement = false;
		break;

	case ERaveBotBehaviorState::Chase:
		CharacterMovementComponent->bOrientRotationToMovement = true;
		break;
	}

	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		BlackboardComponent->SetValueAsEnum(BehaviorStateKeyName, static_cast<uint8>(NewBehaviorState));
	}

	BehaviorState = NewBehaviorState;
}

void ARaveAIController::RunAI()
{
	if (BlackboardData && BehaviorTree)
	{
		UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
		if (UseBlackboard(BlackboardData, BlackboardComponent))
		{
			RunBehaviorTree(BehaviorTree);
		}
	}
}

void ARaveAIController::RestartAI()
{
	UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (BehaviorTreeComponent)
	{
		BehaviorTreeComponent->RestartTree();
	}
}

void ARaveAIController::StopAI()
{
	UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (BehaviorTreeComponent)
	{
		BehaviorTreeComponent->StopTree();
	}
}

void ARaveAIController::TargetPerceptionUpdatedCallback(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		if (IsValid(Actor) && (GetBehaviorState() == ERaveBotBehaviorState::Patrol || GetBehaviorState() == ERaveBotBehaviorState::Idle))
		{
			SetBehaviorState(ERaveBotBehaviorState::Guard);
		}
	}
}

void ARaveAIController::TargetPerceptionForgottenCallback(AActor* Actor)
{
}
