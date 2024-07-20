


#include "Animations/AnimNotifies/RaveAnimNotifyState_GameplayTag.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveAnimNotifyState_GameplayTag)

FString URaveAnimNotifyState_GameplayTag::GetNotifyName_Implementation() const
{
	return TEXT("Gameplay Tag");
}

void URaveAnimNotifyState_GameplayTag::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MeshComp->GetOwner()))
	{
		if (bReplicated)
		{
			AbilitySystemComponent->AddReplicatedLooseGameplayTags(GameplayTags);
		}
		else
		{
			AbilitySystemComponent->AddLooseGameplayTags(GameplayTags);
		}
	}
}

void URaveAnimNotifyState_GameplayTag::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MeshComp->GetOwner()))
	{
		if (bReplicated)
		{
			AbilitySystemComponent->RemoveReplicatedLooseGameplayTags(GameplayTags);
		}
		else
		{
			AbilitySystemComponent->RemoveLooseGameplayTags(GameplayTags);
		}
	}
}
