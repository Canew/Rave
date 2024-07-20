

#pragma once

#include "CoreMinimal.h"
#include "RaveGameMode.h"
#include "RaveGameMode_MainMenu.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API ARaveGameMode_MainMenu : public ARaveGameMode
{
	GENERATED_BODY()
	
public:
	void PostLogin(APlayerController* NewPlayer) override;
};
