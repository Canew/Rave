


#include "GameplayAbility/Abilities/RaveGameplayAbility_Evade.h"
#include "Character/RaveCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "RaveGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveGameplayAbility_Evade)

URaveGameplayAbility_Evade::URaveGameplayAbility_Evade()
{
	ActivationOwnedTags.AddTag(RaveGameplayTags::Character_State_Evade);
}

bool URaveGameplayAbility_Evade::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	if (!IsValid(EvadeMontage))
	{
		return false;
	}

	APawn* AvatarPawn = Cast<APawn>(ActorInfo->AvatarActor);
	if (!IsValid(AvatarPawn) || !AvatarPawn->GetMovementComponent() || AvatarPawn->GetMovementComponent()->IsFalling())
	{
		return false;
	}

	return true;
}

void URaveGameplayAbility_Evade::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	if (UAbilityTask_PlayMontageAndWait* PlayEvadeMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, EvadeMontage, EvadePlayRate, NAME_None, true, EvadeRootMotionTranslationScale))
	{
		PlayEvadeMontageTask->OnCompleted.AddDynamic(this, &ThisClass::HandleMontageComplete);
		PlayEvadeMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::HandleMontageBlendOut);
		PlayEvadeMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::HandleMontageInterrupt);
		PlayEvadeMontageTask->OnCancelled.AddDynamic(this, &ThisClass::HandleMontageCancel);
		PlayEvadeMontageTask->ReadyForActivation();
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void URaveGameplayAbility_Evade::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void URaveGameplayAbility_Evade::HandleMontageComplete()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void URaveGameplayAbility_Evade::HandleMontageBlendOut()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void URaveGameplayAbility_Evade::HandleMontageInterrupt()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void URaveGameplayAbility_Evade::HandleMontageCancel()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
}
