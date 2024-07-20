


#include "GameplayAbility/Abilities/RaveGameplayAbility_Sprint.h"
#include "Character/RaveCharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"

void URaveGameplayAbility_Sprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AActor* AvatarActor = ActorInfo->AvatarActor.Get();
	if (AvatarActor)
	{
		URaveCharacterMovementComponent* CharacterMovementComponent = AvatarActor->FindComponentByClass<URaveCharacterMovementComponent>();
		if (CharacterMovementComponent)
		{
			CharacterMovementComponent->Sprint();
		}
	}

	UAbilityTask_WaitInputRelease* WaitInputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this);
	if (WaitInputReleaseTask)
	{
		WaitInputReleaseTask->OnRelease.AddDynamic(this, &ThisClass::HandleInputRelease);
		WaitInputReleaseTask->ReadyForActivation();
	}
}

void URaveGameplayAbility_Sprint::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	AActor* AvatarActor = ActorInfo->AvatarActor.Get();
	if (AvatarActor)
	{
		URaveCharacterMovementComponent* CharacterMovementComponent = AvatarActor->FindComponentByClass<URaveCharacterMovementComponent>();
		if (CharacterMovementComponent)
		{
			CharacterMovementComponent->UnSprint();
		}
	}
}

void URaveGameplayAbility_Sprint::HandleInputRelease(float TimeWaited)
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}
