

#pragma once

#include "CoreMinimal.h"
#include "UI/RaveUserWidget.h"
#include "RaveUserWidget_WorldMap.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveUserWidget_WorldMap : public URaveUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyPressed(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> MapCanvasPanel;

	UPROPERTY(EditDefaultsOnly, Category = "Rave")
	TObjectPtr<class URaveRegionData> RegionData;

	UPROPERTY(EditDefaultsOnly, Category = "Rave")
	TSubclassOf<class URaveUserWidget_WorldMapRegion> WorldMapRegionWidgetClass;
};
