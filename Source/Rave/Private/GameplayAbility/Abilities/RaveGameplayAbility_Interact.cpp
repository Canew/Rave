


#include "GameplayAbility/Abilities/RaveGameplayAbility_Interact.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Components/RaveInteractionComponent.h"
#include "Interfaces/RaveInteractionInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveGameplayAbility_Interact)

bool URaveGameplayAbility_Interact::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	URaveInteractionComponent* InteractionComponent = ActorInfo->AvatarActor->FindComponentByClass<URaveInteractionComponent>();
	if (!IsValid(InteractionComponent) || !InteractionComponent->CanInteract())
	{
		return false;
	}

	if (InteractionComponent->GetInteractionActor() && InteractionComponent->GetInteractionActor()->GetClass()->ImplementsInterface(URaveInteractionInterface::StaticClass()))
	{
		return IRaveInteractionInterface::Execute_CanInteract(InteractionComponent->GetInteractionActor());
	}

	return false;
}

void URaveGameplayAbility_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AActor* AvatarActor = ActorInfo->AvatarActor.Get();
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo) && IsValid(AvatarActor))
	{
		URaveInteractionComponent* InteractionComponent = ActorInfo->AvatarActor->FindComponentByClass<URaveInteractionComponent>();
		if (IsValid(InteractionComponent))
		{
			InteractionComponent->Interact();
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}

void URaveGameplayAbility_Interact::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);


}
