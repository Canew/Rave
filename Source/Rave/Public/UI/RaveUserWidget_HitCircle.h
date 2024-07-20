

#pragma once

#include "CoreMinimal.h"
#include "UI/RaveUserWidget.h"
#include "RaveUserWidget_HitCircle.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveUserWidget_HitCircle : public URaveUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

public:
	void SetHitLocation(const FVector& InHitLocation);

protected:
	UFUNCTION()
	virtual void HandleDisplayDurationFinished();

protected:
	class UMaterialInstanceDynamic* HitCircleMaterialInstance;
	FVector HitLocation;

	UPROPERTY(Meta = (BindWidget))
	class UBorder* HitCircleBorder;

	FTimerHandle DisplayTimerHandle;
	float DisplayDuration;
};
