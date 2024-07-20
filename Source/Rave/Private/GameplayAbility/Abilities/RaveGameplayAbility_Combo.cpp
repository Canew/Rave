


#include "GameplayAbility/Abilities/RaveGameplayAbility_Combo.h"
#include "GameplayAbility/RaveAbilitySystemComponent.h"
#include "GameplayAbility/Tasks/RaveAbilityTask_WaitAbilityEnd.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "RaveGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveGameplayAbility_Combo)

URaveGameplayAbility_Combo::URaveGameplayAbility_Combo()
{
	AbilitySequenceType = URaveAbilitySequenceType::None;

	CurrentComboAbilityIndex = 0;
}

bool URaveGameplayAbility_Combo::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return CanActivateComboAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void URaveGameplayAbility_Combo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ActivateComboAbility();
}

void URaveGameplayAbility_Combo::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	CurrentComboAbilityIndex = 0;
}

bool URaveGameplayAbility_Combo::CanActivateComboAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ComboAbilities.IsValidIndex(CurrentComboAbilityIndex))
	{
		return false;
	}
	
	FGameplayAbilitySpec* ComboAbilitySpec = ActorInfo->AbilitySystemComponent->FindAbilitySpecFromClass(ComboAbilities[CurrentComboAbilityIndex]);
	if (ComboAbilitySpec)
	{
		if (URaveGameplayAbility* RaveGameplayAbility = Cast<URaveGameplayAbility>(ComboAbilitySpec->Ability))
		{
			return RaveGameplayAbility->CanActivateAbility(Handle, ActorInfo);
		}
	}

	return false;
}

bool URaveGameplayAbility_Combo::ActivateComboAbility()
{
	bool bSucceeded = false;
	if (CanActivateComboAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo()))
	{
		bSucceeded = GetCurrentActorInfo()->AbilitySystemComponent->TryActivateAbilityByClass(ComboAbilities[CurrentComboAbilityIndex]);

		WaitAbilityEndTask = URaveAbilityTask_WaitAbilityEnd::WaitForAbilityEndWithClass(this, ComboAbilities[CurrentComboAbilityIndex]);
		if (WaitAbilityEndTask)
		{
			WaitAbilityEndTask->OnAbilityEnd.AddDynamic(this, &ThisClass::HandleAbilityEnded);
			WaitAbilityEndTask->ReadyForActivation();
		}

		WaitBeginComboEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, RaveGameplayTags::GameplayEvent_Combo_Begin);
		if (WaitBeginComboEventTask)
		{
			WaitBeginComboEventTask->EventReceived.AddDynamic(this, &ThisClass::HandleBeginComboEventReceived);
			WaitBeginComboEventTask->ReadyForActivation();
		}

		CurrentComboAbilityIndex++;
	}

	return bSucceeded;
}

void URaveGameplayAbility_Combo::HandleBeginComboEventReceived(FGameplayEventData Payload)
{
	WaitInputPressTask = UAbilityTask_WaitInputPress::WaitInputPress(this);
	if (WaitInputPressTask)
	{
		WaitInputPressTask->OnPress.AddDynamic(this, &ThisClass::HandleInputPress);
		WaitInputPressTask->ReadyForActivation();
	}

	WaitEndComboEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, RaveGameplayTags::GameplayEvent_Combo_End);
	if (WaitEndComboEventTask)
	{
		WaitEndComboEventTask->EventReceived.AddDynamic(this, &ThisClass::HandleEndComboEventReceived);
		WaitEndComboEventTask->ReadyForActivation();
	}
}

void URaveGameplayAbility_Combo::HandleEndComboEventReceived(FGameplayEventData Payload)
{
	if (WaitBeginComboEventTask && WaitBeginComboEventTask->IsActive())
	{
		WaitBeginComboEventTask->EndTask();
	}

	if (WaitInputPressTask && WaitInputPressTask->IsActive())
	{
		WaitInputPressTask->EndTask();
	}
}

void URaveGameplayAbility_Combo::HandleInputPress(float TimeWaited)
{
	if (WaitAbilityEndTask && WaitAbilityEndTask->IsActive())
	{
		WaitAbilityEndTask->EndTask();
	}

	FGameplayAbilitySpec* GameplayAbilitySpec = GetCurrentActorInfo()->AbilitySystemComponent->FindAbilitySpecFromClass(ComboAbilities[CurrentComboAbilityIndex - 1]);
	if (GameplayAbilitySpec)
	{
		if (UGameplayAbility* GameplayAbility = GameplayAbilitySpec->GetPrimaryInstance())
		{
			GameplayAbility->CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
		}
	}

	ActivateComboAbility();
}

void URaveGameplayAbility_Combo::HandleAbilityEnded(UGameplayAbility* Ability)
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}
