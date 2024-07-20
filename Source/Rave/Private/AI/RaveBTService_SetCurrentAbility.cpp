


#include "AI/RaveBTService_SetCurrentAbility.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AIController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveBTService_SetCurrentAbility)

URaveBTService_SetCurrentAbility::URaveBTService_SetCurrentAbility()
{
	NodeName = TEXT("Random Ability");
	bNotifyTick = true;
	bNotifyBecomeRelevant = true;

	TSubclassOf<UGameplayAbility> SubclassOfGameplayAbility = UGameplayAbility::StaticClass();
	BlackboardKey.AddClassFilter(this, GetSelectedBlackboardKey(), SubclassOfGameplayAbility);
}

void URaveBTService_SetCurrentAbility::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController || !AIController->GetPawn() || !AIController->GetWorld())
	{
		return;
	}

	UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent();
	if (!BlackboardComponent)
	{
		return;
	}

	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(OwnerComp.GetAIOwner()->GetPawn());
	if (!AbilitySystemInterface)
	{
		return;
	}

	UAbilitySystemComponent* AbilitySystemComponent = AbilitySystemInterface->GetAbilitySystemComponent();
	if (!AbilitySystemComponent)
	{
		return;
	}

	TArray<UClass*> ActivatableAbilityClasses;
	for (const FGameplayAbilitySpec& AbilitySpec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if (AbilitySpec.Ability->CheckCooldown(AbilitySpec.Handle, AbilitySystemComponent->AbilityActorInfo.Get()))
		{
			ActivatableAbilityClasses.Add(AbilitySpec.Ability->GetClass());
		}
	}

	int32 AbilityIndex = FMath::RandRange(0, ActivatableAbilityClasses.Num() - 1);
	if (ActivatableAbilityClasses.IsValidIndex(AbilityIndex))
	{
		BlackboardComponent->SetValueAsClass(GetSelectedBlackboardKey(), ActivatableAbilityClasses[AbilityIndex]);
	}
}

void URaveBTService_SetCurrentAbility::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController || !AIController->GetPawn() || !AIController->GetWorld())
	{
		return;
	}

	UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent();
	if (!BlackboardComponent)
	{
		return;
	}

	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(OwnerComp.GetAIOwner()->GetPawn());
	if (!AbilitySystemInterface)
	{
		return;
	}

	UAbilitySystemComponent* AbilitySystemComponent = AbilitySystemInterface->GetAbilitySystemComponent();
	if (!AbilitySystemComponent)
	{
		return;
	}

	TArray<UClass*> ActivatableAbilityClasses;
	for (const FGameplayAbilitySpec& AbilitySpec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if (AbilitySpec.Ability->CheckCooldown(AbilitySpec.Handle, AbilitySystemComponent->AbilityActorInfo.Get()))
		{
			ActivatableAbilityClasses.Add(AbilitySpec.Ability->GetClass());
		}
	}

	int32 AbilityIndex = FMath::RandRange(0, ActivatableAbilityClasses.Num() - 1);
	if (ActivatableAbilityClasses.IsValidIndex(AbilityIndex))
	{
		BlackboardComponent->SetValueAsClass(GetSelectedBlackboardKey(), ActivatableAbilityClasses[AbilityIndex]);
	}
}

