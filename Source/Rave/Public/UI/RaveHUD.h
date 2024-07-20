

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RaveHUD.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API ARaveHUD : public AHUD
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Rave|Widget")
	TArray<TSubclassOf<class URaveUserWidget>> DefaultWidgetClasses;

	UPROPERTY()
	TArray<TObjectPtr<class URaveUserWidget>> DefaultWidgets;
};
