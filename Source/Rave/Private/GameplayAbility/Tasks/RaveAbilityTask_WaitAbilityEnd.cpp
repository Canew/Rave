


#include "GameplayAbility/Tasks/RaveAbilityTask_WaitAbilityEnd.h"
#include "AbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveAbilityTask_WaitAbilityEnd)

URaveAbilityTask_WaitAbilityEnd::URaveAbilityTask_WaitAbilityEnd(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilityClass = nullptr;
}

URaveAbilityTask_WaitAbilityEnd* URaveAbilityTask_WaitAbilityEnd::WaitForAbilityEnd(UGameplayAbility* OwningAbility, FGameplayTag InWithTag, FGameplayTag InWithoutTag, bool InTriggerOnce)
{
	URaveAbilityTask_WaitAbilityEnd* MyObj = NewAbilityTask<URaveAbilityTask_WaitAbilityEnd>(OwningAbility);
	MyObj->WithTag = InWithTag;
	MyObj->WithoutTag = InWithoutTag;
	MyObj->TriggerOnce = InTriggerOnce;
	return MyObj;
}

URaveAbilityTask_WaitAbilityEnd* URaveAbilityTask_WaitAbilityEnd::WaitForAbilityEndWithTagRequirements(UGameplayAbility* OwningAbility, FGameplayTagRequirements TagRequirements, bool InTriggerOnce)
{
	URaveAbilityTask_WaitAbilityEnd* MyObj = NewAbilityTask<URaveAbilityTask_WaitAbilityEnd>(OwningAbility);
	MyObj->TagRequirements = TagRequirements;
	MyObj->TriggerOnce = InTriggerOnce;
	return MyObj;
}

URaveAbilityTask_WaitAbilityEnd* URaveAbilityTask_WaitAbilityEnd::WaitForAbilityEnd_Query(UGameplayAbility* OwningAbility, FGameplayTagQuery Query, bool InTriggerOnce)
{
	URaveAbilityTask_WaitAbilityEnd* MyObj = NewAbilityTask<URaveAbilityTask_WaitAbilityEnd>(OwningAbility);
	MyObj->Query = Query;
	MyObj->TriggerOnce = InTriggerOnce;
	return MyObj;
}

URaveAbilityTask_WaitAbilityEnd* URaveAbilityTask_WaitAbilityEnd::WaitForAbilityEndWithClass(UGameplayAbility* OwningAbility, TSubclassOf<UGameplayAbility> AbilityClass, bool InTriggerOnce)
{
	URaveAbilityTask_WaitAbilityEnd* MyObj = NewAbilityTask<URaveAbilityTask_WaitAbilityEnd>(OwningAbility);
	MyObj->AbilityClass = AbilityClass;
	MyObj->TriggerOnce = InTriggerOnce;
	return MyObj;

}

void URaveAbilityTask_WaitAbilityEnd::Activate()
{
	if (AbilitySystemComponent.IsValid())
	{
		OnAbilityEndDelegateHandle = AbilitySystemComponent->AbilityEndedCallbacks.AddUObject(this, &URaveAbilityTask_WaitAbilityEnd::HandleAbilityEnded);
	}
}

void URaveAbilityTask_WaitAbilityEnd::HandleAbilityEnded(UGameplayAbility* EndedAbility)
{
	if (TagRequirements.IsEmpty())
	{
		if ((WithTag.IsValid() && !EndedAbility->AbilityTags.HasTag(WithTag)) ||
			(WithoutTag.IsValid() && EndedAbility->AbilityTags.HasTag(WithoutTag)))
		{
			// Failed tag check
			return;
		}
	}
	else
	{
		if (!TagRequirements.RequirementsMet(EndedAbility->AbilityTags))
		{
			// Failed tag check
			return;
		}
	}

	if (Query.IsEmpty() == false)
	{
		if (Query.Matches(EndedAbility->AbilityTags) == false)
		{
			// Failed query
			return;
		}
	}

	if (AbilityClass)
	{
		if (AbilityClass != EndedAbility->GetClass())
		{
			return;
		}
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnAbilityEnd.Broadcast(EndedAbility);
	}

	if (TriggerOnce)
	{
		EndTask();
	}
}

void URaveAbilityTask_WaitAbilityEnd::OnDestroy(bool AbilityEnded)
{
	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->AbilityEndedCallbacks.Remove(OnAbilityEndDelegateHandle);
	}

	Super::OnDestroy(AbilityEnded);
}

