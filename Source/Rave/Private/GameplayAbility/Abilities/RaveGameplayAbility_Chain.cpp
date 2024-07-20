


#include "GameplayAbility/Abilities/RaveGameplayAbility_Chain.h"
#include "GameplayAbility/RaveAbilitySystemComponent.h"
#include "GameplayAbility/Tasks/RaveAbilityTask_WaitAbilityEnd.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "RaveGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveGameplayAbility_Chain)

URaveGameplayAbility_Chain::URaveGameplayAbility_Chain()
{
	AbilitySequenceType = URaveAbilitySequenceType::Chain;

	CurrentChainAbilityIndex = 0;
	MaxChainInterval = 0.0f;
	bCanActivateChainAbility = true;
}

bool URaveGameplayAbility_Chain::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!IsActive())
	{
		if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
		{
			return false;
		}
	}

	return CanActivateChainAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

float URaveGameplayAbility_Chain::GetMaxChainInterval() const
{
	return MaxChainInterval;
}

float URaveGameplayAbility_Chain::GetRemainingChainDuration() const
{
	UWorld* World = GetWorld();
	if (World)
	{
		if (World->GetTimerManager().TimerExists(ChainTimerHandle))
		{
			return World->GetTimerManager().GetTimerRemaining(ChainTimerHandle);
		}
	}

	return 0.0f;
}

void URaveGameplayAbility_Chain::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ActivateChainAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void URaveGameplayAbility_Chain::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	CurrentChainAbilityIndex = 0;
	bCanActivateChainAbility = true;
}

bool URaveGameplayAbility_Chain::CanActivateChainAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!bCanActivateChainAbility || !ChainAbilities.IsValidIndex(CurrentChainAbilityIndex))
	{
		return false;
	}

	FGameplayAbilitySpec* ChainAbilitySpec = ActorInfo->AbilitySystemComponent->FindAbilitySpecFromClass(ChainAbilities[CurrentChainAbilityIndex]);
	if (ChainAbilitySpec)
	{
		if (URaveGameplayAbility* RaveGameplayAbility = Cast<URaveGameplayAbility>(ChainAbilitySpec->Ability))
		{
			return RaveGameplayAbility->CanActivateAbility(ChainAbilitySpec->Handle, ActorInfo);
		}
	}

	return false;
}

bool URaveGameplayAbility_Chain::ActivateChainAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	bool bSucceeded = false;
	if (ChainAbilities.IsValidIndex(CurrentChainAbilityIndex))
	{
		bSucceeded = GetCurrentActorInfo()->AbilitySystemComponent->TryActivateAbilityByClass(ChainAbilities[CurrentChainAbilityIndex]);

		bCanActivateChainAbility = false;

		WaitAbilityEndTask = URaveAbilityTask_WaitAbilityEnd::WaitForAbilityEndWithClass(this, ChainAbilities[CurrentChainAbilityIndex]);
		if (WaitAbilityEndTask)
		{
			WaitAbilityEndTask->OnAbilityEnd.AddDynamic(this, &ThisClass::HandleAbilityEnded);
			WaitAbilityEndTask->ReadyForActivation();
		}

		WaitInputPress();
		
		CurrentChainAbilityIndex++;
	}

	return bSucceeded;
}

void URaveGameplayAbility_Chain::HandleInputPress(float TimeWaited)
{
	if (CanActivateChainAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo()))
	{
		if (WaitAbilityEndTask && WaitAbilityEndTask->IsActive())
		{
			WaitAbilityEndTask->EndTask();
		}

		if (GetCurrentActorInfo()->AbilitySystemComponent->GetWorld()->GetTimerManager().TimerExists(ChainTimerHandle))
		{
			GetCurrentActorInfo()->AbilitySystemComponent->GetWorld()->GetTimerManager().ClearTimer(ChainTimerHandle);
		}

		FGameplayAbilitySpec* GameplayAbilitySpec = GetCurrentActorInfo()->AbilitySystemComponent->FindAbilitySpecFromClass(ChainAbilities[CurrentChainAbilityIndex - 1]);
		if (GameplayAbilitySpec)
		{
			if (UGameplayAbility* GameplayAbility = GameplayAbilitySpec->GetPrimaryInstance())
			{
				GameplayAbility->CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), false);
			}
		}

		ActivateChainAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), nullptr);
	}
	else
	{
		WaitInputPress();
	}
}

void URaveGameplayAbility_Chain::HandleAbilityEnded(UGameplayAbility* Ability)
{
	if (CurrentChainAbilityIndex == ChainAbilities.Num() || MaxChainInterval == 0.0f)
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
	}
	else if (bCanActivateChainAbility && CurrentChainAbilityIndex < ChainAbilities.Num())
	{
		GetCurrentActorInfo()->AbilitySystemComponent->GetWorld()->GetTimerManager().SetTimer(ChainTimerHandle, this, &ThisClass::HandleChainTimerElapsed, MaxChainInterval, false);
	}
	else
	{
		WaitInputPress();
	}
}

void URaveGameplayAbility_Chain::WaitInputPress()
{
	WaitInputPressTask = UAbilityTask_WaitInputPress::WaitInputPress(this);
	if (WaitInputPressTask)
	{
		WaitInputPressTask->OnPress.AddDynamic(this, &ThisClass::HandleInputPress);
		WaitInputPressTask->ReadyForActivation();

		bCanActivateChainAbility = true;
	}
}

void URaveGameplayAbility_Chain::HandleChainTimerElapsed()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}
