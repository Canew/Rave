


#include "GameplayAbility/RaveGameplayAbility.h"
#include "GameplayAbility/RaveAbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "RaveGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveGameplayAbility)

URaveGameplayAbility::URaveGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ActivationBlockedTags.AddTag(RaveGameplayTags::Character_Debuff_Stun);

	bInjectRecentInputWhenEndAbility = false;
	AbilitySequenceType = URaveAbilitySequenceType::None;
}

bool URaveGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();
	if (!IsValid(AbilitySystemComponent))
	{
		return false;
	}

	bool bBlocked = false;
	switch (ActivationGroup)
	{
	case ERaveAbilityActivationGroup::Independent:
		break;

	case ERaveAbilityActivationGroup::Exclusive_Replaceable:
	case ERaveAbilityActivationGroup::Exclusive_Blocking:
	{
		TArray<FGameplayAbilitySpec> ActivatableAbilitySpecs = AbilitySystemComponent->GetActivatableAbilities();
		for (FGameplayAbilitySpec& ActivatableAbilitySpec : ActivatableAbilitySpecs)
		{
			if (ActivatableAbilitySpec.IsActive() && !ActivatableAbilitySpec.DynamicAbilityTags.HasTagExact(RaveGameplayTags::Ability_Property_Cancellable))
			{
				if (URaveGameplayAbility* RaveGameplayAbility = Cast<URaveGameplayAbility>(ActivatableAbilitySpec.Ability))
				{
					if (RaveGameplayAbility->GetActivationGroup() == ERaveAbilityActivationGroup::Exclusive_Blocking)
					{
						bBlocked = true;
						break;
					}
				}
			}
		}
		break;
	}

	default:
		return false;
	}

	if (bBlocked)
	{
		return false;
	}

	return true;
}

float URaveGameplayAbility::GetCostMagnitude(FGameplayAttribute Attribute) const
{
	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	if (!IsValid(AbilitySystemComponent))
	{
		return 0.0f;
	}

	UGameplayEffect* CostGameplayEffect = GetCostGameplayEffect();
	if (IsValid(CostGameplayEffect))
	{
		for (FGameplayModifierInfo Modifier : CostGameplayEffect->Modifiers)
		{
			if (Modifier.Attribute == Attribute)
			{
				FGameplayEffectSpec CostGameplayEffectSpec = FGameplayEffectSpec(CostGameplayEffect, AbilitySystemComponent->MakeEffectContext());
				float CostMagnitude;
				if (Modifier.ModifierMagnitude.AttemptCalculateMagnitude(CostGameplayEffectSpec, CostMagnitude))
				{
					return CostMagnitude;
				}
			}
		}
	}

	return 0.0f;
}

void URaveGameplayAbility::PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	CurrentSpecHandle = Handle;
	CurrentActorInfo = ActorInfo;
	CurrentActivationInfo = ActivationInfo;

	if (TriggerEventData)
	{
		CurrentEventData = *TriggerEventData;
	}
}

void URaveGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* AbilitySystenComponent = ActorInfo->AbilitySystemComponent.Get();
	if (!AbilitySystenComponent || !HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo) || !CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
		return;
	}

	// Grant effects
	for (FRaveGrantedGameplayEffectDef GrantedEffect : GrantedEffects)
	{
		if (GrantedEffect.EffectActivationPolicy == FRaveGrantedEffectActivationPolicy::OnActivated)
		{
			GrantedEffect.ActiveEffectHandle = AbilitySystenComponent->ApplyGameplayEffectToSelf(GrantedEffect.EffectClass.GetDefaultObject(), 0.0f, MakeEffectContext(Handle, ActorInfo), AbilitySystenComponent->ScopedPredictionKey);
		}
		else if (GrantedEffect.EffectActivationPolicy == FRaveGrantedEffectActivationPolicy::NotOnActivated)
		{
			FGameplayEffectQuery EffectQuery;
			EffectQuery.EffectDefinition = GrantedEffect.EffectClass;
			AbilitySystenComponent->RemoveActiveEffects(EffectQuery);
		}
	}
}

void URaveGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (bInjectRecentInputWhenEndAbility)
	{
		GetRaveAbilitySystemComponentFromActorInfo()->InjectRecentInputForAction();
	}

	UAbilitySystemComponent* AbilitySystenComponent = ActorInfo->AbilitySystemComponent.Get();
	if (AbilitySystenComponent && HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		// Grant effects
		for (FRaveGrantedGameplayEffectDef GrantedEffect : GrantedEffects)
		{
			if (GrantedEffect.EffectActivationPolicy == FRaveGrantedEffectActivationPolicy::OnActivated)
			{
				FGameplayEffectQuery EffectQuery;
				EffectQuery.EffectDefinition = GrantedEffect.EffectClass;
				AbilitySystenComponent->RemoveActiveEffects(EffectQuery);
			}
			else if (GrantedEffect.EffectActivationPolicy == FRaveGrantedEffectActivationPolicy::NotOnActivated)
			{
				GrantedEffect.ActiveEffectHandle = AbilitySystenComponent->ApplyGameplayEffectToSelf(GrantedEffect.EffectClass.GetDefaultObject(), 0.0f, MakeEffectContext(Handle, ActorInfo), AbilitySystenComponent->ScopedPredictionKey);
			}
		}
	}

}

const FGameplayTagContainer* URaveGameplayAbility::GetCooldownTags() const
{
	if (!CooldownGameplayEffectClass)
	{
		return Super::GetCooldownTags();
	}

	// Get cooldown tag from input tag
	FGameplayTagContainer* CooldownTags = const_cast<FGameplayTagContainer*>(&CooldownTagsContainer);
	CooldownTags->Reset();
	const FGameplayTagContainer* ParentCooldownTags = Super::GetCooldownTags();
	if (ParentCooldownTags)
	{
		CooldownTags->AppendTags(*ParentCooldownTags);
	}

	if (FGameplayAbilitySpec* AbilitySpec = GetCurrentAbilitySpec())
	{
		FGameplayTagContainer InputTags;
		InputTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Input")));
		InputTags = AbilitySpec->DynamicAbilityTags.Filter(InputTags);

		CooldownTags->AppendTags(InputTags);
	}

	return CooldownTags;
}

void URaveGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	const FGameplayTagContainer* CooldownTags = GetCooldownTags();
	if (CooldownGE && CooldownTags)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
		SpecHandle.Data.Get()->DynamicGrantedTags.AppendTags(*CooldownTags);

		SpecHandle.Data.Get()->SetSetByCallerMagnitude(RaveGameplayTags::Data_Cooldown, CooldownTime);
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}

URaveAbilitySystemComponent* URaveGameplayAbility::GetRaveAbilitySystemComponentFromActorInfo() const
{
	return Cast<URaveAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
}

int32 URaveGameplayAbility::GetPredictionKey() const
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		return ASC->ScopedPredictionKey.Current;
	}

	return 0;
}

FGameplayEffectSpecHandle URaveGameplayAbility::MakeGameplayEffectSpecFromHitResult(TSubclassOf<UGameplayEffect> GameplayEffectClass, float GameplayEffectLevel, const FHitResult& HitResult)
{
	UAbilitySystemComponent* AbilitySystemComponent = CurrentActorInfo->AbilitySystemComponent.Get();

	FGameplayEffectContextHandle GameplayEffectContextHandle = MakeEffectContext(CurrentSpecHandle, CurrentActorInfo);
	GameplayEffectContextHandle.AddHitResult(HitResult, true);
	FGameplayEffectSpecHandle GameplayEffectSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffectClass, GameplayEffectLevel, GameplayEffectContextHandle);
	if (GameplayEffectSpecHandle.IsValid())
	{
		FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromHandle(CurrentSpecHandle);
		ApplyAbilityTagsToGameplayEffectSpec(*GameplayEffectSpecHandle.Data.Get(), AbilitySpec);

		// Copy over set by caller magnitudes
		if (AbilitySpec)
		{
			GameplayEffectSpecHandle.Data->SetByCallerTagMagnitudes = AbilitySpec->SetByCallerTagMagnitudes;
		}
	}

	return GameplayEffectSpecHandle;
}

FGameplayEventData URaveGameplayAbility::MakeGameplayEventData(FGameplayTag EventTag, float EventMagnitude, const FHitResult& HitResult, UObject* OptionalObject, UObject* OptionalObject2) const
{
	FGameplayEventData GameplayEventData;
	GameplayEventData.EventTag = EventTag;
	GameplayEventData.EventMagnitude = EventMagnitude;
	GameplayEventData.Instigator = CurrentActorInfo->OwnerActor.Get();
	GameplayEventData.OptionalObject = OptionalObject;
	GameplayEventData.OptionalObject2 = OptionalObject2;
	GameplayEventData.Target = HitResult.GetActor();
	if (HitResult.GetActor())
	{
		FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit();
		TargetData->ReplaceHitWith(HitResult.GetActor(), &HitResult);
		GameplayEventData.TargetData = TargetData;
	}

	GameplayEventData.ContextHandle = MakeEffectContext(CurrentSpecHandle, CurrentActorInfo);
	GameplayEventData.ContextHandle.AddInstigator(CurrentActorInfo->OwnerActor.Get(), CurrentActorInfo->AvatarActor.Get());
	GameplayEventData.ContextHandle.AddHitResult(HitResult, true);

	return GameplayEventData;
}

void URaveGameplayAbility::AddDynamicAbilityTag(FGameplayTag DynamicAbilityTag)
{
	if (FGameplayAbilitySpec* GameplayAbilitySpec = GetCurrentAbilitySpec())
	{
		GameplayAbilitySpec->DynamicAbilityTags.AddTag(DynamicAbilityTag);
	}
}

void URaveGameplayAbility::AddDynamicAbilityTags(FGameplayTagContainer DynamicAbilityTags)
{
	if (FGameplayAbilitySpec* GameplayAbilitySpec = GetCurrentAbilitySpec())
	{
		for (FGameplayTag DynamicAbilityTag : DynamicAbilityTags)
		{
			GameplayAbilitySpec->DynamicAbilityTags.AddTag(DynamicAbilityTag);
		}
	}
}

void URaveGameplayAbility::RemoveDynamicAbilityTag(FGameplayTag DynamicAbilityTag)
{
	if (FGameplayAbilitySpec* GameplayAbilitySpec = GetCurrentAbilitySpec())
	{
		GameplayAbilitySpec->DynamicAbilityTags.RemoveTag(DynamicAbilityTag);
	}
}

void URaveGameplayAbility::RemoveDynamicAbilityTags(FGameplayTagContainer DynamicAbilityTags)
{
	if (FGameplayAbilitySpec* GameplayAbilitySpec = GetCurrentAbilitySpec())
	{
		for (FGameplayTag DynamicAbilityTag : DynamicAbilityTags)
		{
			GameplayAbilitySpec->DynamicAbilityTags.RemoveTag(DynamicAbilityTag);
		}
	}
}
