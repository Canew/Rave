


#include "UI/RaveUserWidget_WorldMapRegion.h"
#include "UI/RaveUserWidget_Dialog.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void URaveUserWidget_WorldMapRegion::NativeConstruct()
{
	Super::NativeConstruct();

	FButtonStyle RegionButtonStyle;

	FSlateBrushOutlineSettings RegionButtonOutlineSettings;
	RegionButtonOutlineSettings.CornerRadii = FVector4(4.0f, 4.0f, 4.0f, 4.0f);
	RegionButtonOutlineSettings.Color = FLinearColor(1.0f, 1.0f, 1.0f, 0.25f);
	RegionButtonOutlineSettings.Width = 12.0f;
	RegionButtonOutlineSettings.RoundingType = ESlateBrushRoundingType::HalfHeightRadius;
	RegionButtonOutlineSettings.bUseBrushTransparency = false;

	FSlateBrush RegionButtonNormal;
	RegionButtonNormal.SetResourceObject(Region.RegionImage);
	RegionButtonNormal.TintColor = FLinearColor(0.6f, 0.6f, 0.6f, 1.0f);
	RegionButtonNormal.DrawAs = ESlateBrushDrawType::RoundedBox;
	RegionButtonNormal.OutlineSettings = RegionButtonOutlineSettings;
	RegionButtonStyle.SetNormal(RegionButtonNormal);

	FSlateBrush RegionButtonHovered;
	RegionButtonHovered.SetResourceObject(Region.RegionImage);
	RegionButtonHovered.TintColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	RegionButtonHovered.DrawAs = ESlateBrushDrawType::RoundedBox;
	RegionButtonHovered.OutlineSettings = RegionButtonOutlineSettings;
	RegionButtonStyle.SetHovered(RegionButtonHovered);

	FSlateBrush RegionButtonPressed;
	RegionButtonPressed.SetResourceObject(Region.RegionImage);
	RegionButtonPressed.TintColor = FLinearColor(0.8f, 0.8f, 0.8f, 1.0f);
	RegionButtonPressed.DrawAs = ESlateBrushDrawType::RoundedBox;
	RegionButtonPressed.OutlineSettings = RegionButtonOutlineSettings;
	RegionButtonStyle.SetPressed(RegionButtonPressed);

	RegionButton->SetStyle(RegionButtonStyle);

	RegionButton->OnClicked.AddDynamic(this, &ThisClass::HandleRegionButtonClicked);
}

void URaveUserWidget_WorldMapRegion::SetOwningWorldMapWidget(URaveUserWidget* WorldMapWidget)
{
	OwningWorldMapWidget = WorldMapWidget;
}

void URaveUserWidget_WorldMapRegion::SetRegion(FRaveRegion InRegion)
{
	Region = InRegion;
}

void URaveUserWidget_WorldMapRegion::HandleRegionButtonClicked()
{
	ShowWarpDialog();
}

void URaveUserWidget_WorldMapRegion::ShowWarpDialog()
{
	if (DialogWidgetClass)
	{
		URaveUserWidget_Dialog* RegionWarpDialog = CreateWidget<URaveUserWidget_Dialog>(GetOwningPlayer(), DialogWidgetClass);
		if (RegionWarpDialog)
		{
			RegionWarpDialog->TitleTextBlock->SetText(FText::FromString(TEXT("Warp")));
			RegionWarpDialog->DialogTextBlock->SetText(FText::FromString(TEXT("Do you want to warp to ") + Region.RegionDisplayName.ToString() + TEXT("?")));
			RegionWarpDialog->AcceptButton->OnClicked.AddDynamic(this, &ThisClass::HandleWarpDialogAcceptButtonClicked);
			RegionWarpDialog->RejectButton->OnClicked.AddDynamic(this, &ThisClass::HandleWarpDialogRejectButtonClicked);

			RegionWarpDialog->AddToViewport();
		}
	}
}

void URaveUserWidget_WorldMapRegion::HandleWarpDialogAcceptButtonClicked()
{
	if (IsValid(OwningWorldMapWidget.Get()))
	{
		OwningWorldMapWidget->RemoveFromParent();
	}

	GetWorld()->ServerTravel(Region.RegionName.ToString());
}

void URaveUserWidget_WorldMapRegion::HandleWarpDialogRejectButtonClicked()
{
	if (IsValid(OwningWorldMapWidget.Get()))
	{
		OwningWorldMapWidget->SetFocus();
	}
}
