

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "RaveAnimNotifyState_EquipmentTrail.generated.h"

class UMeshComponent;
class UNiagaraSystem;
class USkeletalMeshComponent;
class UFXSystemAsset;
class UFXSystemComponent;
class UAnimInstance;

/**
 * 
 */
UCLASS(Blueprintable, meta = (DisplayName = "Equipment Trail"))
class RAVE_API URaveAnimNotifyState_EquipmentTrail : public UAnimNotifyState
{
	GENERATED_UCLASS_BODY()

	// The niagara system template to use when spawning the niagara component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = NiagaraSystem, meta = (DisplayName = "Niagara System", ToolTip = "The niagara system to spawn for the notify state"))
	TObjectPtr<UNiagaraSystem> Template;

	// The socket within our mesh component to attach to when we spawn the Niagara component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = NiagaraSystem, meta = (ToolTip = "The socket or bone to attach the system to", AnimNotifyBoneName = "true"))
	FName EquipmentAttachedSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = NiagaraSystem)
	FName StartEquipmentSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = NiagaraSystem)
	FName EndEquipmentSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = NiagaraSystem)
	FName StartSystemParameterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = NiagaraSystem)
	FName EndSystemParameterName;

	// Offset from the socket / bone location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = NiagaraSystem, meta = (ToolTip = "Offset from the socket or bone to place the Niagara system"))
	FVector LocationOffset;

	// Offset from the socket / bone rotation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = NiagaraSystem, meta = (ToolTip = "Rotation offset from the socket or bone for the Niagara system"))
	FRotator RotationOffset;

	// Whether or not we destroy the component at the end of the notify or instead just stop
	// the emitters.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = NiagaraSystem, meta = (DisplayName = "Destroy Immediately", ToolTip = "Whether the Niagara system should be immediately destroyed at the end of the notify state or be allowed to finish"))
	bool bDestroyAtEnd;

	TWeakObjectPtr<class URaveEquipmentComponent> EquipmentComponent;

	virtual void NotifyBegin(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	// Overridden from UAnimNotifyState to provide custom notify name.
	FString GetNotifyName_Implementation() const override;

	// Return FXSystemComponent created from SpawnEffect
	UFUNCTION(BlueprintCallable, Category = "AnimNotify")
	UFXSystemComponent* GetSpawnedEffect(UMeshComponent* MeshComp);

protected:
	// Spawns the NiagaraSystemComponent. Called from Notify.
	virtual UFXSystemComponent* SpawnEffect(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) const;

	bool ValidateParameters(USkeletalMeshComponent* MeshComp) const;

	FORCEINLINE FName GetSpawnedComponentTag()const { return GetFName(); }
};
