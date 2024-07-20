


#include "GameplayAbility/RaveAbilitySystemComponent.h"
#include "GameplayAbility/RaveAttributeSet.h"
#include "EnhancedInputSubsystems.h"
#include "RaveGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveAbilitySystemComponent)

URaveAbilitySystemComponent::URaveAbilitySystemComponent()
{
	ActionBufferDuration = 0.2f;
}

void URaveAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
			}
		}
	}
}

void URaveAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.Remove(AbilitySpec.Handle);
			}
		}
	}
}

void URaveAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();

	//
	// Process all abilities that activate when the input is held.
	//
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		if (const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			const URaveGameplayAbility* RaveAbilityCDO = CastChecked<URaveGameplayAbility>(AbilitySpec->Ability);
			if (RaveAbilityCDO->GetActivationPolicy() == ERaveAbilityActivationPolicy::WhileInputActive && RaveAbilityCDO->GetActivationPolicy() != ERaveAbilityActivationPolicy::OnEventReceived)
			{
				AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
			}
		}
	}

	//
	// Process all abilities that had their input pressed this frame.
	//
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			AbilitySpec->InputPressed = true;

			const URaveGameplayAbility* RaveAbilityCDO = CastChecked<URaveGameplayAbility>(AbilitySpec->Ability);
			if (AbilitySpec->IsActive())
			{
				// Ability is active so pass along the input event.
				AbilitySpecInputPressed(*AbilitySpec);
				AbilityInstanceInputPressed(*AbilitySpec);
			}

			if (RaveAbilityCDO->GetActivationPolicy() == ERaveAbilityActivationPolicy::OnInputTriggered && RaveAbilityCDO->GetActivationPolicy() != ERaveAbilityActivationPolicy::OnEventReceived)
			{
				AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
			}
		}
	}

	//
	// Try to activate all the abilities that are from presses and holds.
	// We do it all at once so that held inputs don't activate the ability
	// and then also send a input event to the ability because of the press.
	//
	for (int32 AbilitySpecHandleIndex = 0; AbilitySpecHandleIndex < AbilitiesToActivate.Num(); AbilitySpecHandleIndex++)
	{
		if (!TryActivateAbility(AbilitiesToActivate[AbilitySpecHandleIndex]))
		{
			// Push last pressed ability to action buffer if failed to try activate.
			if (AbilitySpecHandleIndex == AbilitiesToActivate.Num() - 1)
			{
				SetRecentInputAction(AbilitiesToActivate[AbilitySpecHandleIndex]);
			}
		}
	}

	//
	// Process all abilities that had their input released this frame.
	//
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = false;

				if (AbilitySpec->IsActive())
				{
					// Ability is active so pass along the input event.
					AbilitySpecInputReleased(*AbilitySpec);
					AbilityInstanceInputReleased(*AbilitySpec);
				}
			}
		}
	}

	//
	// Clear the cached ability handles.
	//
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void URaveAbilitySystemComponent::ClearAbilityInput()
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

void URaveAbilitySystemComponent::GetActivatableGameplayAbilitySpecsByAllMatchingDynamicTags(const FGameplayTagContainer& GameplayTagContainer, TArray <struct FGameplayAbilitySpec*>& MatchingGameplayAbilities, bool bOnlyAbilitiesThatSatisfyTagRequirements) const
{
	if (!GameplayTagContainer.IsValid())
	{
		return;
	}

	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.Ability && Spec.DynamicAbilityTags.HasAll(GameplayTagContainer))
		{
			// Consider abilities that are blocked by tags currently if we're supposed to (default behavior).  
			// That way, we can use the blocking to find an appropriate ability based on tags when we have more than 
			// one ability that match the GameplayTagContainer.
			if (!bOnlyAbilitiesThatSatisfyTagRequirements || Spec.Ability->DoesAbilitySatisfyTagRequirements(*this))
			{
				MatchingGameplayAbilities.Add(const_cast<FGameplayAbilitySpec*>(&Spec));
			}
		}
	}
}

void URaveAbilitySystemComponent::AssignInputTag(FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& InputTag)
{
	if (AbilitySpec.Ability)
	{
		FGameplayTagContainer DynamicAbilityInputTags = AbilitySpec.DynamicAbilityTags.Filter(FGameplayTagContainer(RaveGameplayTags::Input));
		AbilitySpec.DynamicAbilityTags.RemoveTags(DynamicAbilityInputTags);
		AbilitySpec.DynamicAbilityTags.AddTag(InputTag);
	}
}

int32 URaveAbilitySystemComponent::GetRemainingSkillPoint() const
{
	bool bCharacterLevelFound = false;
	int32 CharacterLevel = GetGameplayAttributeValue(URaveAttributeSet::GetLevelAttribute(), bCharacterLevelFound);
	if (bCharacterLevelFound)
	{
		int32 RemainingSkillPoint = CharacterLevel - 1;
		for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
		{
			FGameplayTagContainer SkillAbilityTags;
			SkillAbilityTags.AddTag(RaveGameplayTags::Ability_Identity);
			SkillAbilityTags.AddTag(RaveGameplayTags::Ability_Skill_1);
			SkillAbilityTags.AddTag(RaveGameplayTags::Ability_Skill_2);
			SkillAbilityTags.AddTag(RaveGameplayTags::Ability_Skill_3);
			SkillAbilityTags.AddTag(RaveGameplayTags::Ability_Skill_4);
			if (Spec.Ability && Spec.Ability->AbilityTags.HasAny(SkillAbilityTags))
			{
				RemainingSkillPoint -= (Spec.Level - 1);
			}
		}

		return RemainingSkillPoint;
	}

	return 0;
}

void URaveAbilitySystemComponent::IncreaseAbilityLevel(FGameplayAbilitySpec& AbilitySpec)
{
	if (GetRemainingSkillPoint() > 0)
	{
		AbilitySpec.Level += 1;

		OnAbilityLevelChanged.Broadcast(AbilitySpec, AbilitySpec.Level);
	}
}

void URaveAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);

	// Granted effects
	if (URaveGameplayAbility* RaveGameplayAbility = Cast<URaveGameplayAbility>(AbilitySpec.Ability))
	{
		for (FRaveGrantedGameplayEffectDef& GrantedEffect : RaveGameplayAbility->GetGrantedEffects())
		{
			if (GrantedEffect.EffectActivationPolicy == FRaveGrantedEffectActivationPolicy::Always
				|| GrantedEffect.EffectActivationPolicy == FRaveGrantedEffectActivationPolicy::NotOnActivated)
			{
				GrantedEffect.ActiveEffectHandle = ApplyGameplayEffectToSelf(GrantedEffect.EffectClass.GetDefaultObject(), 1.0f, MakeEffectContext(), ScopedPredictionKey);
			}
		}
	}

	OnAbilityAdded.Broadcast(AbilitySpec);
}

void URaveAbilitySystemComponent::OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnRemoveAbility(AbilitySpec);

	// Granted effects
	if (URaveGameplayAbility* RaveGameplayAbility = Cast<URaveGameplayAbility>(AbilitySpec.Ability))
	{
		for (FRaveGrantedGameplayEffectDef& GrantedEffect : RaveGameplayAbility->GetGrantedEffects())
		{
			if (GrantedEffect.bClearOnAbilityRemoved)
			{
				FGameplayEffectQuery EffectQuery;
				EffectQuery.EffectDefinition = GrantedEffect.EffectClass;
				RemoveActiveEffects(EffectQuery, 1);
			}
		}
	}

	OnAbilityRemoved.Broadcast(AbilitySpec);
}

void URaveAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);

	// We don't support UGameplayAbility::bReplicateInputDirectly.
	// Use replicated events instead so that the WaitInputPress ability task works.
	if (Spec.IsActive())
	{
		// Invoke the InputPressed event. This is not replicated here. If someone is listening, they may replicate the InputPressed event to the server.
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

void URaveAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	// We don't support UGameplayAbility::bReplicateInputDirectly.
	// Use replicated events instead so that the WaitInputRelease ability task works.
	if (Spec.IsActive())
	{
		// Invoke the InputReleased event. This is not replicated here. If someone is listening, they may replicate the InputReleased event to the server.
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

void URaveAbilitySystemComponent::AbilityInstanceInputPressed(FGameplayAbilitySpec& Spec)
{
	TArray<UGameplayAbility*> AbilityInstances = Spec.GetAbilityInstances();
	for (UGameplayAbility* AbilityInstance : AbilityInstances)
	{
		AbilityInstance->InputPressed(Spec.Handle, AbilityActorInfo.Get(), AbilityInstance->GetCurrentActivationInfo());
	}
}

void URaveAbilitySystemComponent::AbilityInstanceInputReleased(FGameplayAbilitySpec& Spec)
{
	TArray<UGameplayAbility*> AbilityInstances = Spec.GetAbilityInstances();
	for (UGameplayAbility* AbilityInstance : AbilityInstances)
	{
		AbilityInstance->InputReleased(Spec.Handle, AbilityActorInfo.Get(), AbilityInstance->GetCurrentActivationInfo());
	}
}

void URaveAbilitySystemComponent::SetRecentInputAction(const FGameplayAbilitySpecHandle& NewRecentInputAbilitySpecHandle)
{
	RecentInputAbilitySpecHandle = NewRecentInputAbilitySpecHandle;
	GetWorld()->GetTimerManager().SetTimer(ActionBufferTimerHandle, this, &URaveAbilitySystemComponent::ClearRecentInputAction, ActionBufferDuration, false);
}

void URaveAbilitySystemComponent::InjectRecentInputForAction()
{
	if (FGameplayAbilitySpec* GameplayAbilitySpec = FindAbilitySpecFromHandle(RecentInputAbilitySpecHandle))
	{
		URaveGameplayAbility* RaveGameplayAbility = Cast<URaveGameplayAbility>(GameplayAbilitySpec->GetPrimaryInstance());
		if (RaveGameplayAbility && RaveGameplayAbility->CanActivateAbility(GameplayAbilitySpec->Handle, AbilityActorInfo.Get()))
		{
			InputPressedSpecHandles.AddUnique(RecentInputAbilitySpecHandle);
			InputHeldSpecHandles.AddUnique(RecentInputAbilitySpecHandle);

			GetWorld()->GetTimerManager().ClearTimer(ActionBufferTimerHandle);
			ClearRecentInputAction();
		}
	}
}

void URaveAbilitySystemComponent::ClearRecentInputAction()
{
	RecentInputAbilitySpecHandle = FGameplayAbilitySpecHandle();
}
