


#include "UI/RaveUserWidget_CharacterSelect.h"
#include "UI/RaveUserWidget_CharacterSelectSlot.h"
#include "Components/Button.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/Image.h"
#include "Character/RaveCharacterSetting.h"
#include "Player/RavePlayerState.h"
#include "Player/RavePlayerController.h"
#include "RaveGameInstance.h"

void URaveUserWidget_CharacterSelect::NativeConstruct()
{
	Super::NativeConstruct();

	URaveGameInstance* GameInstance = GetGameInstance<URaveGameInstance>();
	check(GameInstance);

	const URaveCharacterSetting* CurrentPlayerCharacterSetting = nullptr;
	ARavePlayerState* RavePlayerState = GetOwningPlayerState<ARavePlayerState>();
	if (IsValid(RavePlayerState))
	{
		CurrentPlayerCharacterSetting = RavePlayerState->GetCharacterSetting();
	}

	int32 CharacterSettingIndex = 0;
	const TArray<URaveCharacterSetting*> CharacterSettings = GameInstance->GetCharacterSettings();
	for (URaveCharacterSetting* CharacterSetting : CharacterSettings)
	{
		URaveUserWidget_CharacterSelectSlot* CharacterSelectSlot = CreateWidget<URaveUserWidget_CharacterSelectSlot>(GetOwningPlayer(), CharacterSelectSlotClass);
		if (CharacterSelectSlot)
		{
			CharacterSelectSlot->InitializeCharacterSelectSlot(this, CharacterSetting);
			if (IsValid(CurrentPlayerCharacterSetting) && CurrentPlayerCharacterSetting == CharacterSetting)
			{
				SetSelectedCharacterSelectSlot(CharacterSelectSlot);
			}

			UUniformGridSlot* UniformGridSlot = CharactersUniformGrid->AddChildToUniformGrid(CharacterSelectSlot, 0, CharacterSettingIndex);
			if (UniformGridSlot)
			{
				UniformGridSlot->SetHorizontalAlignment(HAlign_Fill);
				UniformGridSlot->SetVerticalAlignment(VAlign_Fill);
			}
			++CharacterSettingIndex;
		}
	}

	CharacterSelectButton->OnClicked.AddDynamic(this, &ThisClass::CharacterSelectButtonClicked);
}

void URaveUserWidget_CharacterSelect::SetSelectedCharacterSelectSlot(URaveUserWidget_CharacterSelectSlot* CharacterSelectSlot)
{
	if (!IsValid(CharacterSelectSlot))
	{
		return;
	}

	if (IsValid(SelectedCharacterSelectSlot))
	{
		SelectedCharacterSelectSlot->UnSelectSlot();
	}

	CharacterSelectSlot->SelectSlot();
	SelectedCharacterSelectSlot = CharacterSelectSlot;
}

void URaveUserWidget_CharacterSelect::CharacterSelectButtonClicked()
{
	if (IsValid(SelectedCharacterSelectSlot))
	{
		ARavePlayerState* RavePlayerState = GetOwningPlayerState<ARavePlayerState>();
		if (IsValid(RavePlayerState))
		{
			RavePlayerState->SetCharacterSetting(SelectedCharacterSelectSlot->GetSlotCharacterSetting());
		}
	}

	ARavePlayerController* RavePlayerController = GetOwningPlayer<ARavePlayerController>();
	if (RavePlayerController)
	{
		FInputModeGameOnly InputModeGameOnly;
		RavePlayerController->SetInputMode(InputModeGameOnly);
		RavePlayerController->SetShowMouseCursor(false);
		RavePlayerController->SetFocusWidget(nullptr);
	}
}
