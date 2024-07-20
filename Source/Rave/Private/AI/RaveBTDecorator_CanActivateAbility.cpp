


#include "AI/RaveBTDecorator_CanActivateAbility.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AIController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveBTDecorator_CanActivateAbility)

URaveBTDecorator_CanActivateAbility::URaveBTDecorator_CanActivateAbility()
{
	NodeName = TEXT("Can Activate Ability");
	bNotifyTick = true;
	bNotifyBecomeRelevant = true;

	TSubclassOf<UGameplayAbility> SubclassOfGameplayAbility = UGameplayAbility::StaticClass();
	BlackboardKey.AddClassFilter(this, GetSelectedBlackboardKey(), SubclassOfGameplayAbility);
}

bool URaveBTDecorator_CanActivateAbility::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (!Super::CalculateRawConditionValue(OwnerComp, NodeMemory))
	{
		return false;
	}

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return false;
	}

	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(OwnerComp.GetAIOwner()->GetPawn());
	if (!AbilitySystemInterface)
	{
		return false;
	}

	UAbilitySystemComponent* AbilitySystemComponent = AbilitySystemInterface->GetAbilitySystemComponent();
	if (!AbilitySystemComponent)
	{
		return false;
	}

	UClass* CurrentAbilityClass = GetCurrentAbilityClass(OwnerComp.GetAIOwner());
	if (CurrentAbilityClass)
	{
		if (FGameplayAbilitySpec* GameplayAbilitySpec = AbilitySystemComponent->FindAbilitySpecFromClass(CurrentAbilityClass))
		{
			if (UGameplayAbility* GameplayAbility = GameplayAbilitySpec->Ability)
			{
				if (GameplayAbility->CanActivateAbility(GameplayAbilitySpec->Handle, AbilitySystemComponent->AbilityActorInfo.Get()))
				{
					return true;
				}
			}
		}
	}

	return false;
}

void URaveBTDecorator_CanActivateAbility::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	FBTDecorator_CanActivateAbility_Memory* DecoratorMemory = CastInstanceNodeMemory<FBTDecorator_CanActivateAbility_Memory>(NodeMemory);
	bool bCondition = CalculateRawConditionValue(OwnerComp, NodeMemory);
	if (bCondition != DecoratorMemory->BeforeData)
	{
		DecoratorMemory->BeforeData = bCondition;
		OwnerComp.RequestExecution(this);
	}
}

void URaveBTDecorator_CanActivateAbility::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	FBTDecorator_CanActivateAbility_Memory* DecoratorMemory = CastInstanceNodeMemory<FBTDecorator_CanActivateAbility_Memory>(NodeMemory);
	DecoratorMemory->BeforeData = CalculateRawConditionValue(OwnerComp, NodeMemory);
}

uint16 URaveBTDecorator_CanActivateAbility::GetInstanceMemorySize() const
{
	return sizeof(FBTDecorator_CanActivateAbility_Memory);
}

UClass* URaveBTDecorator_CanActivateAbility::GetCurrentAbilityClass(AAIController* AIController) const
{
	if (UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent())
	{
		return BlackboardComponent->GetValueAsClass(GetSelectedBlackboardKey());
	}

	return nullptr;
}

