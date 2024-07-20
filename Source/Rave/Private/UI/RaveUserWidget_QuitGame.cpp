


#include "UI/RaveUserWidget_QuitGame.h"
#include "Components/Button.h"

void URaveUserWidget_QuitGame::NativeConstruct()
{
	Super::NativeConstruct();

	AcceptButton->OnClicked.AddDynamic(this, &ThisClass::HandleAcceptButtonClicked);
}

void URaveUserWidget_QuitGame::NativeDestruct()
{
	Super::NativeDestruct();

	AcceptButton->OnClicked.RemoveDynamic(this, &ThisClass::HandleAcceptButtonClicked);
}

void URaveUserWidget_QuitGame::HandleAcceptButtonClicked()
{
	GetOwningPlayer()->ConsoleCommand(TEXT("Quit"));
}
