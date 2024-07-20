

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "RaveAnimNotifyState_GameplayTag.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Gameplay Tag"))
class RAVE_API URaveAnimNotifyState_GameplayTag : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	FString GetNotifyName_Implementation() const override;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Gameplay Tag")
	FGameplayTagContainer GameplayTags;

	UPROPERTY(EditAnywhere, Category = "Gameplay Tag")
	bool bReplicated;
};
