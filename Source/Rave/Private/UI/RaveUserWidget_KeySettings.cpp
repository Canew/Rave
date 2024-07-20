


#include "UI/RaveUserWidget_KeySettings.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputSubsystems.h"
#include "Player/RavePlayerController.h"
#include "UI/RaveUserWidget_KeySettingSlot.h"
#include "UI/RaveUserWidget_Dialog.h"

void URaveUserWidget_KeySettings::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ApplyButton->OnClicked.AddDynamic(this, &ThisClass::HandleApplyButtonClicked);
	UndoButton->OnClicked.AddDynamic(this, &ThisClass::HandleUndoButtonClicked);
	ResetButton->OnClicked.AddDynamic(this, &ThisClass::HandleResetButtonClicked);
}

void URaveUserWidget_KeySettings::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeKeySettings();
}

void URaveUserWidget_KeySettings::InitializeKeySettings()
{
	KeySettingsVerticalBox->ClearChildren();

	UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalSubsystem = GetOwningPlayer()->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	UEnhancedInputUserSettings* InputUserSettings = EnhancedInputLocalSubsystem->GetUserSettings();
	if (InputUserSettings)
	{
		if (UEnhancedPlayerMappableKeyProfile* CurrentPlayerKeyProfile = InputUserSettings->GetCurrentKeyProfile())
		{
			for (const FName& PlayerMappableRowKey : PlayerMappableRowKeys)
			{
				if (CurrentPlayerKeyProfile->GetPlayerMappingRows().Find(PlayerMappableRowKey))
				{
					FKeyMappingRow KeyMappingRow = CurrentPlayerKeyProfile->GetPlayerMappingRows()[PlayerMappableRowKey];
					AddKeySettingRow(KeyMappingRow);
				}
			}
		}
	}
}

void URaveUserWidget_KeySettings::AddKeySettingRow(const FKeyMappingRow& KeyMappingRow)
{
	for (const FPlayerKeyMapping& PlayerKeyMapping : KeyMappingRow.Mappings.Array())
	{
		if (PlayerKeyMapping.GetSlot() == EPlayerMappableKeySlot::First)
		{
			URaveUserWidget_KeySettingSlot* KeySettingSlotWidget = CreateWidget<URaveUserWidget_KeySettingSlot>(GetOwningPlayer(), KeySettingSlotClass);
			if (KeySettingSlotWidget)
			{
				KeySettingSlotWidget->SetPlayerKeyMapping(PlayerKeyMapping);

				if (UVerticalBoxSlot* VerticalBoxSlot = KeySettingsVerticalBox->AddChildToVerticalBox(KeySettingSlotWidget))
				{
					VerticalBoxSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
				}
			}
		}
	}
}

void URaveUserWidget_KeySettings::HandleApplyButtonClicked()
{
	UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalSubsystem = GetOwningPlayer()->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	UEnhancedInputUserSettings* InputUserSettings = EnhancedInputLocalSubsystem->GetUserSettings();
	if (InputUserSettings)
	{
		for (UWidget* Child : KeySettingsVerticalBox->GetAllChildren())
		{
			if (URaveUserWidget_KeySettingSlot* KeySettingSlotWidget = Cast<URaveUserWidget_KeySettingSlot>(Child))
			{
				FPlayerKeyMapping PlayerKeyMapping = KeySettingSlotWidget->GetPlayerKeyMapping();
				FMapPlayerKeyArgs MapPlayerKeyArgs;
				MapPlayerKeyArgs.MappingName = PlayerKeyMapping.GetMappingName();
				MapPlayerKeyArgs.Slot = EPlayerMappableKeySlot::First;
				MapPlayerKeyArgs.NewKey = PlayerKeyMapping.GetCurrentKey();

				FGameplayTagContainer FailureReasonTags;
				InputUserSettings->MapPlayerKey(MapPlayerKeyArgs, FailureReasonTags);
			}
		}

		InputUserSettings->ApplySettings();
		InputUserSettings->SaveSettings();
	}
}

void URaveUserWidget_KeySettings::HandleUndoButtonClicked()
{
	InitializeKeySettings();
}

void URaveUserWidget_KeySettings::HandleResetButtonClicked()
{
	ShowResetConfirmationDialog();
}

void URaveUserWidget_KeySettings::ShowResetConfirmationDialog()
{
	if (DialogWidgetClass)
	{
		ResetConfirmationDialog = CreateWidget<URaveUserWidget_Dialog>(GetOwningPlayer(), DialogWidgetClass);
		if (ResetConfirmationDialog)
		{
			ResetConfirmationDialog->TitleTextBlock->SetText(FText::FromString(TEXT("Key Reset")));
			ResetConfirmationDialog->DialogTextBlock->SetText(FText::FromString(TEXT("Are you sure you want to reset key settings?")));
			ResetConfirmationDialog->AcceptButton->OnClicked.AddDynamic(this, &ThisClass::HandleResetAcceptButtonClicked);
			ResetConfirmationDialog->RejectButton->OnClicked.AddDynamic(this, &ThisClass::HandleResetRejectButtonClicked);

			if (!ResetConfirmationDialog->OnDestructed.IsAlreadyBound(this, &ThisClass::HandleResetConfirmationDialogDestructed))
			{
				ResetConfirmationDialog->OnDestructed.AddDynamic(this, &ThisClass::HandleResetConfirmationDialogDestructed);
			}

			ResetConfirmationDialog->AddToViewport();
			
			SetIsEnabled(false);
		}
	}
}

void URaveUserWidget_KeySettings::RemoveResetConfirmationDialog()
{
	if (ResetConfirmationDialog)
	{
		ResetConfirmationDialog->AcceptButton->OnClicked.RemoveDynamic(this, &ThisClass::HandleResetAcceptButtonClicked);
		ResetConfirmationDialog->RejectButton->OnClicked.RemoveDynamic(this, &ThisClass::HandleResetRejectButtonClicked);

		ResetConfirmationDialog->RemoveFromParent();
	}
	else
	{
		HandleResetConfirmationDialogDestructed();
	}
}

void URaveUserWidget_KeySettings::HandleResetAcceptButtonClicked()
{
	UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalSubsystem = GetOwningPlayer()->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	UEnhancedInputUserSettings* InputUserSettings = EnhancedInputLocalSubsystem->GetUserSettings();
	if (InputUserSettings)
	{
		if (UEnhancedPlayerMappableKeyProfile* CurrentPlayerKeyProfile = InputUserSettings->GetCurrentKeyProfile())
		{
			CurrentPlayerKeyProfile->ResetToDefault();

			for (const FName& PlayerMappableRowKey : PlayerMappableRowKeys)
			{
				if (CurrentPlayerKeyProfile->GetPlayerMappingRows().Find(PlayerMappableRowKey))
				{
					FKeyMappingRow KeyMappingRow = CurrentPlayerKeyProfile->GetPlayerMappingRows()[PlayerMappableRowKey];
					for (const FPlayerKeyMapping& PlayerKeyMapping : KeyMappingRow.Mappings.Array())
					{
						if (PlayerKeyMapping.GetSlot() == EPlayerMappableKeySlot::First)
						{
							FMapPlayerKeyArgs MapPlayerKeyArgs;
							MapPlayerKeyArgs.MappingName = PlayerKeyMapping.GetMappingName();
							MapPlayerKeyArgs.Slot = EPlayerMappableKeySlot::First;
							MapPlayerKeyArgs.NewKey = PlayerKeyMapping.GetCurrentKey();

							FGameplayTagContainer FailureReasonTags;
							InputUserSettings->MapPlayerKey(MapPlayerKeyArgs, FailureReasonTags);
						}
					}
				}
			}
		}
	}

	InitializeKeySettings();

	RemoveResetConfirmationDialog();
}

void URaveUserWidget_KeySettings::HandleResetRejectButtonClicked()
{
	RemoveResetConfirmationDialog();
}

void URaveUserWidget_KeySettings::HandleResetConfirmationDialogDestructed()
{
	SetIsEnabled(true);
	SetKeyboardFocus();

	if (ResetConfirmationDialog)
	{
		ResetConfirmationDialog->OnDestructed.RemoveDynamic(this, &ThisClass::HandleResetConfirmationDialogDestructed);
		ResetConfirmationDialog = nullptr;
	}
}
