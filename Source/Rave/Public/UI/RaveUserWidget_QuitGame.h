

#pragma once

#include "CoreMinimal.h"
#include "UI/RaveUserWidget.h"
#include "RaveUserWidget_QuitGame.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveUserWidget_QuitGame : public URaveUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UFUNCTION()
	void HandleAcceptButtonClicked();

public:
	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class UButton> AcceptButton;

	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class UButton> RejectButton;
};
