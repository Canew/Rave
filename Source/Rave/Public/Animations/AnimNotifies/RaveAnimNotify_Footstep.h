

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "RaveAnimNotify_Footstep.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Footstep"))
class RAVE_API URaveAnimNotify_Footstep : public UAnimNotify
{
	GENERATED_BODY()

public:
	FString GetNotifyName_Implementation() const;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Particle")
	TObjectPtr<class UNiagaraSystem> FootstepParticle;

	UPROPERTY(EditAnywhere, Category = "Audio")
	TObjectPtr<class USoundBase> FootstepSound;

	UPROPERTY(EditAnywhere, Category = "Audio")
	FName FootSocketName;

	float FootTraceDistance = 50.0f;
};
