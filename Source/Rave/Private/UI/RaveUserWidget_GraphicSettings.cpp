


#include "UI/RaveUserWidget_GraphicSettings.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "RaveGameUserSettings.h"
#include "UI/RaveUserWidget_GraphicSettingSlot.h"

#define LOCTEXT_NAMESPACE "GraphicSettings"

void URaveUserWidget_GraphicSettings::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	check(GraphicSettingSlotWidgetClass);

	for (ERaveGraphicSettingType GraphicSettingType : TEnumRange<ERaveGraphicSettingType>())
	{
		URaveUserWidget_GraphicSettingSlot* GraphicSettingSlotWidget = CreateWidget<URaveUserWidget_GraphicSettingSlot>(GetOwningPlayer(), GraphicSettingSlotWidgetClass);
		if (GraphicSettingSlotWidget)
		{
			GraphicSettingSlotWidget->SetGraphicSettingSlotType(GraphicSettingType);

			GraphicSettingsVerticalBox->AddChild(GraphicSettingSlotWidget);
		}
	}

	FullscreenModeUpButton->OnClicked.AddDynamic(this, &ThisClass::HandleFullscreenModeUpButtonClicked);
	FullscreenModeDownButton->OnClicked.AddDynamic(this, &ThisClass::HandleFullscreenModeDownButtonClicked);

	ResolutionUpButton->OnClicked.AddDynamic(this, &ThisClass::HandleResolutionUpButtonClicked);
	ResolutionDownButton->OnClicked.AddDynamic(this, &ThisClass::HandleResolutionDownButtonClicked);
}

void URaveUserWidget_GraphicSettings::NativeConstruct()
{
	Super::NativeConstruct();

	URaveGameUserSettings* GameUserSettings = URaveGameUserSettings::GetGameUserSettings();
	check(GameUserSettings);

	SetFullscreenModeText(GameUserSettings->GetFullscreenMode());
	SetResolutionText(GameUserSettings->GetScreenResolution());
}

void URaveUserWidget_GraphicSettings::SetFullscreenModeText(int32 FullscreenMode)
{
	FText FullscreenModeText;
	switch (FullscreenMode)
	{
	case EWindowMode::Fullscreen:
		FullscreenModeText = LOCTEXT("GraphicSettings", "Fullscreen");
		break;

	case EWindowMode::WindowedFullscreen:
		FullscreenModeText = LOCTEXT("GraphicSettings", "Windowed Fullscreen");
		break;

	case EWindowMode::Windowed:
		FullscreenModeText = LOCTEXT("GraphicSettings", "Windowed");
		break;
	}

	FullscreenModeTextBlock->SetText(FullscreenModeText);
}

void URaveUserWidget_GraphicSettings::SetResolutionText(FIntPoint ScreenResolution)
{
	FText ResolutionText = FText::FromString(FString::FromInt(ScreenResolution.X) + TEXT(" x ") + FString::FromInt(ScreenResolution.Y));
	ResolutionTextBlock->SetText(ResolutionText);
}

void URaveUserWidget_GraphicSettings::HandleFullscreenModeUpButtonClicked()
{
	URaveGameUserSettings* GameUserSettings = URaveGameUserSettings::GetGameUserSettings();
	check(GameUserSettings);

	int32 CurrentFullscreenMode = GameUserSettings->GetFullscreenMode();
	int32 NewFullscreenMode = FMath::Clamp(CurrentFullscreenMode + 1, 0, EWindowMode::NumWindowModes - 1);
	GameUserSettings->SetFullscreenMode(EWindowMode::ConvertIntToWindowMode(NewFullscreenMode));
	
	GameUserSettings->ApplyResolutionSettings(false);

	SetFullscreenModeText(NewFullscreenMode);
}

void URaveUserWidget_GraphicSettings::HandleFullscreenModeDownButtonClicked()
{
	URaveGameUserSettings* GameUserSettings = URaveGameUserSettings::GetGameUserSettings();
	check(GameUserSettings);

	int32 CurrentFullscreenMode = GameUserSettings->GetFullscreenMode();
	int32 NewFullscreenMode = FMath::Clamp(CurrentFullscreenMode - 1, 0, EWindowMode::NumWindowModes - 1);
	GameUserSettings->SetFullscreenMode(EWindowMode::ConvertIntToWindowMode(NewFullscreenMode));

	GameUserSettings->ApplyResolutionSettings(false);

	SetFullscreenModeText(NewFullscreenMode);
}

void URaveUserWidget_GraphicSettings::HandleResolutionUpButtonClicked()
{
	URaveGameUserSettings* GameUserSettings = URaveGameUserSettings::GetGameUserSettings();
	check(GameUserSettings);

	const TArray<FIntPoint>& ResolutionSet = GameUserSettings->GetResolutionSet();
	if (ResolutionSet.Num() <= 0)
	{
		return;
	}

	int32 CurrentResolutionSetIndex = ResolutionSet.Find(GameUserSettings->GetScreenResolution());
	if (CurrentResolutionSetIndex == INDEX_NONE)
	{
		GameUserSettings->SetScreenResolution(ResolutionSet[0]);
	}
	else
	{
		int32 NewResolutionSetIndex = FMath::Clamp(CurrentResolutionSetIndex + 1, 0, ResolutionSet.Num() - 1);
		GameUserSettings->SetScreenResolution(ResolutionSet[NewResolutionSetIndex]);
	}

	GameUserSettings->ApplyResolutionSettings(false);
	SetResolutionText(GameUserSettings->GetScreenResolution());
}

void URaveUserWidget_GraphicSettings::HandleResolutionDownButtonClicked()
{
	URaveGameUserSettings* GameUserSettings = URaveGameUserSettings::GetGameUserSettings();
	check(GameUserSettings);

	const TArray<FIntPoint>& ResolutionSet = GameUserSettings->GetResolutionSet();
	if (ResolutionSet.Num() <= 0)
	{
		return;
	}

	int32 CurrentResolutionSetIndex = ResolutionSet.Find(GameUserSettings->GetScreenResolution());
	if (CurrentResolutionSetIndex == INDEX_NONE)
	{
		GameUserSettings->SetScreenResolution(ResolutionSet[ResolutionSet.Num() - 1]);
	}
	else
	{
		int32 NewResolutionSetIndex = FMath::Clamp(CurrentResolutionSetIndex - 1, 0, ResolutionSet.Num() - 1);
		GameUserSettings->SetScreenResolution(ResolutionSet[NewResolutionSetIndex]);
	}

	GameUserSettings->ApplyResolutionSettings(false);
	SetResolutionText(GameUserSettings->GetScreenResolution());
}
