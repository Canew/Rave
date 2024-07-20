

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "RaveAnimNotify_GameplayEvent.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Gameplay Event"))
class RAVE_API URaveAnimNotify_GameplayEvent : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Gameplay Tag")
	FGameplayTag GameplayTag;
};
