


#include "UI/RaveUserWidget_AudioSettings.h"
#include "UI/RaveUserWidget_AudioSettingSlot.h"
#include "Components/VerticalBox.h"
#include "RaveGameInstance.h"
#include "Audio/RaveAudioUserSettings.h"

void URaveUserWidget_AudioSettings::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	URaveGameInstance* GameInstance = GetGameInstance<URaveGameInstance>();
	check(GameInstance);

	for (const FRaveSoundGroup& SoundGroup : GameInstance->GetGameSoundGroups())
	{
		URaveUserWidget_AudioSettingSlot* AudioSettingSlotWidget = CreateWidget<URaveUserWidget_AudioSettingSlot>(GetOwningPlayer(), AudioSettingSlotWidgetClass);
		if (AudioSettingSlotWidget)
		{
			AudioSettingSlotWidget->SetSoundGroup(SoundGroup);
		}

		AudioSettingsVerticalBox->AddChild(AudioSettingSlotWidget);
	}
}

void URaveUserWidget_AudioSettings::NativeConstruct()
{
	Super::NativeConstruct();
}
