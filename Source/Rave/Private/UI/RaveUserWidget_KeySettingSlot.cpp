


#include "UI/RaveUserWidget_KeySettingSlot.h"
#include "Components/TextBlock.h"
#include "Components/InputKeySelector.h"
#include "Player/RavePlayerController.h"

void URaveUserWidget_KeySettingSlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	InputKeySelector->OnKeySelected.AddDynamic(this, &ThisClass::HandleInputKeySelected);
}

FPlayerKeyMapping URaveUserWidget_KeySettingSlot::GetPlayerKeyMapping() const
{
	return PlayerKeyMapping;
}

void URaveUserWidget_KeySettingSlot::SetPlayerKeyMapping(const FPlayerKeyMapping& KeyMapping)
{
	PlayerKeyMapping = KeyMapping;
	InputKeyTextBlock->SetText(KeyMapping.GetDisplayName());
	InputKeySelector->SetSelectedKey(KeyMapping.GetCurrentKey());
}

void URaveUserWidget_KeySettingSlot::HandleInputKeySelected(FInputChord SelectedKey)
{
	PlayerKeyMapping.SetCurrentKey(SelectedKey.Key);
}
