

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RaveAIController.generated.h"

UENUM(BlueprintType)
enum class ERaveBotBehaviorState : uint8
{
	Idle = 0,
	Patrol = 1,
	Guard = 2,
	Chase = 3
};

/**
 * 
 */
UCLASS()
class RAVE_API ARaveAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ARaveAIController(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

public:
	ERaveBotBehaviorState GetBehaviorState() const;
	void SetBehaviorState(ERaveBotBehaviorState NewBehaviorState);

	void RunAI();
	void RestartAI();
	void StopAI();

protected:
	UFUNCTION()
	void TargetPerceptionUpdatedCallback(AActor* Actor, FAIStimulus Stimulus);
	UFUNCTION()
	void TargetPerceptionForgottenCallback(AActor* Actor);

public:
	static FName HomeLocationKeyName;
	static FName MoveLocationKeyName;
	static FName TargetActorKeyName;
	static FName BehaviorStateKeyName;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Rave|Blackboard")
	class UBlackboardData* BlackboardData;

	UPROPERTY(EditDefaultsOnly, Category = "Rave|BehaviorTree")
	class UBehaviorTree* BehaviorTree;	

	UPROPERTY()
	class UAISenseConfig_Sight* SightSenseConfig;
	UPROPERTY()
	class UAISenseConfig_Damage* DamageSenseConfig;
	UPROPERTY()
	class UAISenseConfig_Hearing* HearingSenseConfig;

	UPROPERTY(VisibleInstanceOnly, Category = "Rave|BehaviorTree")
	ERaveBotBehaviorState BehaviorState;
};
