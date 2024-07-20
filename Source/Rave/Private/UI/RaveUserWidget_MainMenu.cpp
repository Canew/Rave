


#include "UI/RaveUserWidget_MainMenu.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "UI/Components/RaveWidgetSwitcher.h"
#include "GameFramework/GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "UI/RaveUserWidget_GameOptions.h"
#include "UI/RaveUserWidget_QuitGame.h"
#include "UI/RaveUserWidget_Dialog.h"
#include "RaveGameSession.h"

void URaveUserWidget_MainMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	StartGameButton->OnClicked.AddDynamic(this, &URaveUserWidget_MainMenu::StartGameButtonClickedCallback);
	OptionButton->OnClicked.AddDynamic(this, &URaveUserWidget_MainMenu::OptionButtonClickedCallback);
	QuitButton->OnClicked.AddDynamic(this, &URaveUserWidget_MainMenu::QuitButtonClickedCallback);
}

void URaveUserWidget_MainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if (FadeInAnimation)
	{
		PlayAnimation(FadeInAnimation);
	}

	SetKeyboardFocus();
}

FReply URaveUserWidget_MainMenu::NativeOnKeyPressed(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FReply Handled = Super::NativeOnKeyPressed(InGeometry, InKeyEvent);

	UWidget* ActiveWidget = MainMenuWidgetSwitcher->GetActiveWidget();
	if (!GetIsEnabled() || (ActiveWidget && !ActiveWidget->GetIsEnabled()))
	{
		return FReply::Unhandled();
	}

	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		if (MainMenuWidgetSwitcher->GetActiveWidgetIndex() == 0)
		{
			ShowQuitDialog();
		}
		else
		{
			MainMenuWidgetSwitcher->SetActiveWidgetIndex(0);
		}
	}

	return Handled;
}

void URaveUserWidget_MainMenu::StartGameButtonClickedCallback()
{
	OnDestructed.AddDynamic(this, &ThisClass::DestructedCallback);

	SetIsEnabled(false);
	RemoveFromParent();
}

void URaveUserWidget_MainMenu::DestructedCallback()
{
	OnDestructed.RemoveDynamic(this, &ThisClass::DestructedCallback);

	FName LevelName = TEXT("/Game/Maps/Conflux/Conflux");
	UWorld* World = GetOwningPlayer()->GetWorld();
	if (!World)
	{
		return;
	}

	AGameModeBase* GameMode = World->GetAuthGameMode();
	if (!GameMode)
	{
		return;
	}

	if (ARaveGameSession* GameSession = Cast<ARaveGameSession>(GameMode->GameSession))
	{
		if (GameSession->HostSession(GetOwningPlayer(), ARaveGameSession::DEFAULT_NUM_PLAYERS, false))
		{
			UGameplayStatics::OpenLevel(GetOwningPlayer(), LevelName, true, TEXT("listen"));
		}
	}
}

void URaveUserWidget_MainMenu::OptionButtonClickedCallback()
{
	MainMenuWidgetSwitcher->SetActiveWidget(GameOptionsWidget);
}

void URaveUserWidget_MainMenu::QuitButtonClickedCallback()
{
	ShowQuitDialog();
}

void URaveUserWidget_MainMenu::ShowQuitDialog()
{
	if (DialogWidgetClass)
	{
		QuitDialog = CreateWidget<URaveUserWidget_Dialog>(GetOwningPlayer(), DialogWidgetClass);
		if (QuitDialog)
		{
			QuitDialog->TitleTextBlock->SetText(FText::FromString(TEXT("Quit Game")));
			QuitDialog->DialogTextBlock->SetText(FText::FromString(TEXT("Are you sure you want to quit the game?")));
			QuitDialog->AcceptButton->OnClicked.AddDynamic(this, &ThisClass::QuitDialogAcceptButtonClickedCallback);
			QuitDialog->RejectButton->OnClicked.AddDynamic(this, &ThisClass::QuitDialogRejectButtonClickedCallback);

			if (!QuitDialog->OnDestructed.IsAlreadyBound(this, &ThisClass::QuitDialogDestructedCallback))
			{
				QuitDialog->OnDestructed.AddDynamic(this, &ThisClass::QuitDialogDestructedCallback);
			}

			QuitDialog->AddToViewport();

			SetIsEnabled(false);
		}
	}
}

void URaveUserWidget_MainMenu::RemoveQuitDialog()
{
	if (QuitDialog)
	{
		QuitDialog->AcceptButton->OnClicked.RemoveDynamic(this, &ThisClass::QuitDialogAcceptButtonClickedCallback);
		QuitDialog->RejectButton->OnClicked.RemoveDynamic(this, &ThisClass::QuitDialogRejectButtonClickedCallback);

		QuitDialog->RemoveFromParent();
	}
	else
	{
		QuitDialogDestructedCallback();
	}
}

void URaveUserWidget_MainMenu::QuitDialogAcceptButtonClickedCallback()
{
	GetOwningPlayer()->ConsoleCommand(TEXT("Quit"));

	RemoveQuitDialog();
}

void URaveUserWidget_MainMenu::QuitDialogRejectButtonClickedCallback()
{
	RemoveQuitDialog();
}

void URaveUserWidget_MainMenu::QuitDialogDestructedCallback()
{
	SetIsEnabled(true);
	SetKeyboardFocus();

	if (QuitDialog)
	{
		QuitDialog->OnDestructed.RemoveDynamic(this, &ThisClass::QuitDialogDestructedCallback);
		QuitDialog = nullptr;
	}
}
