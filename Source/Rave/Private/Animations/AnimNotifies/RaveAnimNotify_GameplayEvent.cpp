


#include "Animations/AnimNotifies/RaveAnimNotify_GameplayEvent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveAnimNotify_GameplayEvent)

FString URaveAnimNotify_GameplayEvent::GetNotifyName_Implementation() const
{
	return TEXT("Gameplay Event");
}

void URaveAnimNotify_GameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MeshComp->GetOwner()))
	{
		FGameplayEventData GameplayEventData;
		GameplayEventData.ContextHandle = ASC->MakeEffectContext();
		GameplayEventData.OptionalObject = Animation;
		ASC->HandleGameplayEvent(GameplayTag, &GameplayEventData);
	}
}
