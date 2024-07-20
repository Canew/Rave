

#pragma once

#include "CoreMinimal.h"
#include "UI/RaveUserWidget.h"
#include "Data/RaveRegionData.h"
#include "RaveUserWidget_WorldMapRegion.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveUserWidget_WorldMapRegion : public URaveUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void SetOwningWorldMapWidget(URaveUserWidget* WorldMapWidget);
	void SetRegion(FRaveRegion InRegion);

protected:
	void ShowWarpDialog();

	UFUNCTION()
	void HandleRegionButtonClicked();

	UFUNCTION()
	void HandleWarpDialogAcceptButtonClicked();
	UFUNCTION()
	void HandleWarpDialogRejectButtonClicked();
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> RegionButton;

	TWeakObjectPtr<class URaveUserWidget> OwningWorldMapWidget;

	FRaveRegion Region;
};
