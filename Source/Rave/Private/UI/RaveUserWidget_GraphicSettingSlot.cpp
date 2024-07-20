


#include "UI/RaveUserWidget_GraphicSettingSlot.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "RaveGameUserSettings.h"

#define LOCTEXT_NAMESPACE "GraphicSettings"

#define GRAPHIC_SETTING_MIN 0
#define GRAPHIC_SETTING_MAX 4

void URaveUserWidget_GraphicSettingSlot::NativeConstruct()
{
	Super::NativeConstruct();

	URaveGameUserSettings* GameUserSettings = URaveGameUserSettings::GetGameUserSettings();
	check(GameUserSettings);

	FText GraphicSettingTypeText;
	int32 GraphicSettingValue = -1;
	switch (GraphicSettingType)
	{
		case ERaveGraphicSettingType::Overall:
			GraphicSettingTypeText = LOCTEXT("GraphicSetting", "Overall");
			GraphicSettingValue = GameUserSettings->GetOverallScalabilityLevel();
			break;

		case ERaveGraphicSettingType::ViewDistance:
			GraphicSettingTypeText = LOCTEXT("GraphicSetting", "View Distance");
			GraphicSettingValue = GameUserSettings->GetViewDistanceQuality();
			break;

		case ERaveGraphicSettingType::AntiAliasing:
			GraphicSettingTypeText = LOCTEXT("GraphicSetting", "Anti Aliasing");
			GraphicSettingValue = GameUserSettings->GetAntiAliasingQuality();
			break;

		case ERaveGraphicSettingType::Shadow:
			GraphicSettingTypeText = LOCTEXT("GraphicSetting", "Shadow");
			GraphicSettingValue = GameUserSettings->GetShadowQuality();
			break;

		case ERaveGraphicSettingType::GlobalIllumination:
			GraphicSettingTypeText = LOCTEXT("GraphicSetting", "Global Illumination");
			GraphicSettingValue = GameUserSettings->GetGlobalIlluminationQuality();
			break;

		case ERaveGraphicSettingType::Reflection:
			GraphicSettingTypeText = LOCTEXT("GraphicSetting", "Reflection");
			GraphicSettingValue = GameUserSettings->GetReflectionQuality();
			break;

		case ERaveGraphicSettingType::PostProcess:
			GraphicSettingTypeText = LOCTEXT("GraphicSetting", "Post Process");
			GraphicSettingValue = GameUserSettings->GetPostProcessingQuality();
			break;

		case ERaveGraphicSettingType::Texture:
			GraphicSettingTypeText = LOCTEXT("GraphicSetting", "Texture");
			GraphicSettingValue = GameUserSettings->GetTextureQuality();
			break;

		case ERaveGraphicSettingType::Effects:
			GraphicSettingTypeText = LOCTEXT("GraphicSetting", "Effects");
			GraphicSettingValue = GameUserSettings->GetVisualEffectQuality();
			break;

		case ERaveGraphicSettingType::Foliage:
			GraphicSettingTypeText = LOCTEXT("GraphicSetting", "Foliage");
			GraphicSettingValue = GameUserSettings->GetFoliageQuality();
			break;

		case ERaveGraphicSettingType::Shading:
			GraphicSettingTypeText = LOCTEXT("GraphicSetting", "Shading");
			GraphicSettingValue = GameUserSettings->GetShadingQuality();
			break;
	}

	GraphicSettingTypeTextBlock->SetText(GraphicSettingTypeText);
	UpdateGraphicSettingValueText(GraphicSettingValue);

	GraphicSettingUpButton->OnClicked.AddDynamic(this, &ThisClass::HandleUpButtonClicked);
	GraphicSettingDownButton->OnClicked.AddDynamic(this, &ThisClass::HandleDownButtonClicked);
}

void URaveUserWidget_GraphicSettingSlot::NativeDestruct()
{
	Super::NativeDestruct();

	GraphicSettingUpButton->OnClicked.RemoveDynamic(this, &ThisClass::HandleUpButtonClicked);
	GraphicSettingDownButton->OnClicked.RemoveDynamic(this, &ThisClass::HandleDownButtonClicked);
}

void URaveUserWidget_GraphicSettingSlot::SetGraphicSettingSlotType(ERaveGraphicSettingType InGraphicSettingType)
{
	GraphicSettingType = InGraphicSettingType;
}

void URaveUserWidget_GraphicSettingSlot::UpdateGraphicSettingValueText(int32 Value)
{
	FText GraphicSettingValueText;
	switch (Value)
	{
	case 0:
		GraphicSettingValueText = LOCTEXT("GraphicSetting", "Low");
		break;

	case 1:
		GraphicSettingValueText = LOCTEXT("GraphicSetting", "Medium");
		break;

	case 2:
		GraphicSettingValueText = LOCTEXT("GraphicSetting", "High");
		break;

	case 3:
		GraphicSettingValueText = LOCTEXT("GraphicSetting", "Very High");
		break;

	case 4:
		GraphicSettingValueText = LOCTEXT("GraphicSetting", "Ultra");
		break;

	default:
		GraphicSettingValueText = LOCTEXT("GraphicSetting", "Invalid");
		break;
	}

	GraphicSettingValueTextBlock->SetText(GraphicSettingValueText);
}

void URaveUserWidget_GraphicSettingSlot::HandleUpButtonClicked()
{
	URaveGameUserSettings* GameUserSettings = URaveGameUserSettings::GetGameUserSettings();
	check(GameUserSettings);

	int32 GraphicSettingValue = -1;
	switch (GraphicSettingType)
	{
	case ERaveGraphicSettingType::Overall:
		GraphicSettingValue = FMath::Clamp(GameUserSettings->GetOverallScalabilityLevel() + 1, GRAPHIC_SETTING_MIN, GRAPHIC_SETTING_MAX);
		GameUserSettings->SetOverallScalabilityLevel(GraphicSettingValue);
		break;

	case ERaveGraphicSettingType::ViewDistance:
		GraphicSettingValue = FMath::Clamp(GameUserSettings->GetViewDistanceQuality() + 1, GRAPHIC_SETTING_MIN, GRAPHIC_SETTING_MAX);
		GameUserSettings->SetViewDistanceQuality(GraphicSettingValue);
		break;

	case ERaveGraphicSettingType::AntiAliasing:
		GraphicSettingValue = FMath::Clamp(GameUserSettings->GetAntiAliasingQuality() + 1, GRAPHIC_SETTING_MIN, GRAPHIC_SETTING_MAX);
		GameUserSettings->SetAntiAliasingQuality(GraphicSettingValue);
		break;

	case ERaveGraphicSettingType::Shadow:
		GraphicSettingValue = FMath::Clamp(GameUserSettings->GetShadowQuality() + 1, GRAPHIC_SETTING_MIN, GRAPHIC_SETTING_MAX);
		GameUserSettings->SetShadowQuality(GraphicSettingValue);
		break;

	case ERaveGraphicSettingType::GlobalIllumination:
		GraphicSettingValue = FMath::Clamp(GameUserSettings->GetGlobalIlluminationQuality() + 1, GRAPHIC_SETTING_MIN, GRAPHIC_SETTING_MAX);
		GameUserSettings->SetGlobalIlluminationQuality(GraphicSettingValue);
		break;

	case ERaveGraphicSettingType::Reflection:
		GraphicSettingValue = FMath::Clamp(GameUserSettings->GetReflectionQuality() + 1, GRAPHIC_SETTING_MIN, GRAPHIC_SETTING_MAX);
		GameUserSettings->SetReflectionQuality(GraphicSettingValue);
		break;

	case ERaveGraphicSettingType::PostProcess:
		GraphicSettingValue = FMath::Clamp(GameUserSettings->GetPostProcessingQuality() + 1, GRAPHIC_SETTING_MIN, GRAPHIC_SETTING_MAX);
		GameUserSettings->SetPostProcessingQuality(GraphicSettingValue);
		break;

	case ERaveGraphicSettingType::Texture:
		GraphicSettingValue = FMath::Clamp(GameUserSettings->GetTextureQuality() + 1, GRAPHIC_SETTING_MIN, GRAPHIC_SETTING_MAX);
		GameUserSettings->SetTextureQuality(GraphicSettingValue);
		break;

	case ERaveGraphicSettingType::Effects:
		GraphicSettingValue = FMath::Clamp(GameUserSettings->GetVisualEffectQuality()  + 1, GRAPHIC_SETTING_MIN, GRAPHIC_SETTING_MAX);
		GameUserSettings->SetVisualEffectQuality(GraphicSettingValue);
		break;

	case ERaveGraphicSettingType::Foliage:
		GraphicSettingValue = FMath::Clamp(GameUserSettings->GetFoliageQuality() + 1, GRAPHIC_SETTING_MIN, GRAPHIC_SETTING_MAX);
		GameUserSettings->SetFoliageQuality(GraphicSettingValue);
		break;

	case ERaveGraphicSettingType::Shading:
		GraphicSettingValue = FMath::Clamp(GameUserSettings->GetShadingQuality() + 1, GRAPHIC_SETTING_MIN, GRAPHIC_SETTING_MAX);
		GameUserSettings->SetShadingQuality(GraphicSettingValue);
		break;
	}

	GameUserSettings->ApplyNonResolutionSettings();
	UpdateGraphicSettingValueText(GraphicSettingValue);
}

void URaveUserWidget_GraphicSettingSlot::HandleDownButtonClicked()
{
	URaveGameUserSettings* GameUserSettings = URaveGameUserSettings::GetGameUserSettings();
	check(GameUserSettings);

	int32 GraphicSettingValue = -1;
	switch (GraphicSettingType)
	{
	case ERaveGraphicSettingType::Overall:
		GraphicSettingValue = FMath::Clamp(GameUserSettings->GetOverallScalabilityLevel() - 1, GRAPHIC_SETTING_MIN, GRAPHIC_SETTING_MAX);
		GameUserSettings->SetOverallScalabilityLevel(GraphicSettingValue);
		break;

	case ERaveGraphicSettingType::ViewDistance:
		GraphicSettingValue = FMath::Clamp(GameUserSettings->GetViewDistanceQuality() - 1, GRAPHIC_SETTING_MIN, GRAPHIC_SETTING_MAX);
		GameUserSettings->SetViewDistanceQuality(GraphicSettingValue);
		break;

	case ERaveGraphicSettingType::AntiAliasing:
		GraphicSettingValue = FMath::Clamp(GameUserSettings->GetAntiAliasingQuality() - 1, GRAPHIC_SETTING_MIN, GRAPHIC_SETTING_MAX);
		GameUserSettings->SetAntiAliasingQuality(GraphicSettingValue);
		break;

	case ERaveGraphicSettingType::Shadow:
		GraphicSettingValue = FMath::Clamp(GameUserSettings->GetShadowQuality() - 1, GRAPHIC_SETTING_MIN, GRAPHIC_SETTING_MAX);
		GameUserSettings->SetShadowQuality(GraphicSettingValue);
		break;

	case ERaveGraphicSettingType::GlobalIllumination:
		GraphicSettingValue = FMath::Clamp(GameUserSettings->GetGlobalIlluminationQuality() - 1, GRAPHIC_SETTING_MIN, GRAPHIC_SETTING_MAX);
		GameUserSettings->SetGlobalIlluminationQuality(GraphicSettingValue);
		break;

	case ERaveGraphicSettingType::Reflection:
		GraphicSettingValue = FMath::Clamp(GameUserSettings->GetReflectionQuality() - 1, GRAPHIC_SETTING_MIN, GRAPHIC_SETTING_MAX);
		GameUserSettings->SetReflectionQuality(GraphicSettingValue);
		break;

	case ERaveGraphicSettingType::PostProcess:
		GraphicSettingValue = FMath::Clamp(GameUserSettings->GetPostProcessingQuality() - 1, GRAPHIC_SETTING_MIN, GRAPHIC_SETTING_MAX);
		GameUserSettings->SetPostProcessingQuality(GraphicSettingValue);
		break;

	case ERaveGraphicSettingType::Texture:
		GraphicSettingValue = FMath::Clamp(GameUserSettings->GetTextureQuality() - 1, GRAPHIC_SETTING_MIN, GRAPHIC_SETTING_MAX);
		GameUserSettings->SetTextureQuality(GraphicSettingValue);
		break;

	case ERaveGraphicSettingType::Effects:
		GraphicSettingValue = FMath::Clamp(GameUserSettings->GetVisualEffectQuality() - 1, GRAPHIC_SETTING_MIN, GRAPHIC_SETTING_MAX);
		GameUserSettings->SetVisualEffectQuality(GraphicSettingValue);
		break;

	case ERaveGraphicSettingType::Foliage:
		GraphicSettingValue = FMath::Clamp(GameUserSettings->GetFoliageQuality() - 1, GRAPHIC_SETTING_MIN, GRAPHIC_SETTING_MAX);
		GameUserSettings->SetFoliageQuality(GraphicSettingValue);
		break;

	case ERaveGraphicSettingType::Shading:
		GraphicSettingValue = FMath::Clamp(GameUserSettings->GetShadingQuality() - 1, GRAPHIC_SETTING_MIN, GRAPHIC_SETTING_MAX);
		GameUserSettings->SetShadingQuality(GraphicSettingValue);
		break;
	}

	GameUserSettings->ApplyNonResolutionSettings();
	UpdateGraphicSettingValueText(GraphicSettingValue);
}

#undef LOCTEXT_NAMESPACE
