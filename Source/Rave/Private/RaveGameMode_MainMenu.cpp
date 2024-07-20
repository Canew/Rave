


#include "RaveGameMode_MainMenu.h"
#include "GameFramework/PlayerController.h"

void ARaveGameMode_MainMenu::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (NewPlayer)
	{
		NewPlayer->SetInputMode(FInputModeUIOnly());
		NewPlayer->SetShowMouseCursor(true);
	}
}
