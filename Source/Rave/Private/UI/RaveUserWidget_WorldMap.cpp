


#include "UI/RaveUserWidget_WorldMap.h"
#include "UI/RaveUserWidget_WorldMapRegion.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Data/RaveRegionData.h"

void URaveUserWidget_WorldMap::NativeConstruct()
{
	Super::NativeConstruct();

	check(WorldMapRegionWidgetClass);

	for (FRaveRegion Region : RegionData->Regions)
	{
		URaveUserWidget_WorldMapRegion* WorldMapRegionWidget = CreateWidget<URaveUserWidget_WorldMapRegion>(GetOwningPlayer(), WorldMapRegionWidgetClass);
		if (WorldMapRegionWidget)
		{
			WorldMapRegionWidget->SetOwningWorldMapWidget(this);
			WorldMapRegionWidget->SetRegion(Region);

			UCanvasPanelSlot* CanvasPanelSlot = MapCanvasPanel->AddChildToCanvas(WorldMapRegionWidget);
			if (CanvasPanelSlot)
			{
				CanvasPanelSlot->SetPosition(Region.RegionMapCoordinate);
				CanvasPanelSlot->SetSize(FVector2D(Region.RegionImage->GetSizeX() * 2.0f, Region.RegionImage->GetSizeY() * 2.0f));
			}
		}
	}

	SetKeyboardFocus();
}

FReply URaveUserWidget_WorldMap::NativeOnKeyPressed(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FReply Handled = Super::NativeOnKeyPressed(InGeometry, InKeyEvent);

	if (!GetIsEnabled())
	{
		return FReply::Unhandled();
	}

	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		SetIsEnabled(false);
		SetIsFocusable(false);

		RemoveFromParent();
	}

	return Handled;
}