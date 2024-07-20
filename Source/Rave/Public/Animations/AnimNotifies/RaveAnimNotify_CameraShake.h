

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "RaveAnimNotify_CameraShake.generated.h"

/**
 * ERaveCameraShakePlaySpace
 *
 *	Used by some effects (like camera shakes) to specify what coordinate space the they should be applied in.
 */
UENUM(BlueprintType)
enum class ERaveCameraShakePlaySpace : uint8
{
	// Play the effect in camera space.
	CameraSpace,

	// Play the effect in world space.
	WorldSpace,
};

/**
 * 
 */
UCLASS(meta = (DisplayName = "Camera Shake"))
class RAVE_API URaveAnimNotify_CameraShake : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	FString GetNotifyName_Implementation() const;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	ECameraShakePlaySpace GetCameraShakePlaySpace();
	float CalculateFalloffIntensity(const FVector& SourceLocation, const FVector& TargetLocation, float InnerRadius, float OuterRadius, float FalloffExponent) const;

protected:
	UPROPERTY(EditAnywhere, Category = "Camera Shake")
	TSubclassOf<class UCameraShakeBase> CameraShakeClass;

	// What coordinate space to play the camera shake relative to.
	UPROPERTY(EditAnywhere, Category = "Camera Shake")
	ERaveCameraShakePlaySpace PlaySpace;

	// Scale applied to the camera shake.
	UPROPERTY(EditAnywhere, Category = "Camera Shake")
	float ShakeScale = 1.0f;

	// Target's socket (or bone) used for location and rotation.  If "None", it uses the target's root.
	UPROPERTY(EditAnywhere, Category = "Camera Shake")
	FName SocketName;

	// If enabled, the camera shake will be played in the world and affect all players.
	UPROPERTY(EditAnywhere, Category = "Camera Shake")
	uint32 bPlayInWorld : 1;

	// Players inside this radius get the full intensity camera shake.
	UPROPERTY(EditAnywhere, Category = "Camera Shake", Meta = (EditCondition = "bPlayInWorld", UIMin = "0.0", ClampMin = "0.0"))
	float WorldInnerRadius;

	// Players outside this radius do not get the camera shake applied.
	UPROPERTY(EditAnywhere, Category = "Camera Shake", Meta = (EditCondition = "bPlayInWorld", UIMin = "0.0", ClampMin = "0.0"))
	float WorldOuterRadius;

	// Exponent that describes the shake intensity falloff curve between the inner and outer radii.  (1.0 is linear)
	UPROPERTY(EditAnywhere, Category = "Camera Shake", Meta = (EditCondition = "bPlayInWorld", UIMin = "0.0", ClampMin = "0.0"))
	float WorldFalloffExponent = 1.0f;
};
