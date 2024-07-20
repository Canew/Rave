

#pragma once

#include "CoreMinimal.h"
#include "UI/RaveUserWidget.h"
#include "RaveUserWidget_MainMenu.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveUserWidget_MainMenu : public URaveUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyPressed(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UFUNCTION()
	virtual void StartGameButtonClickedCallback();
	UFUNCTION()
	virtual void DestructedCallback();

	UFUNCTION()
	virtual void OptionButtonClickedCallback();
	UFUNCTION()
	virtual void QuitButtonClickedCallback();

	void ShowQuitDialog();
	void RemoveQuitDialog();

	UFUNCTION()
	void QuitDialogAcceptButtonClickedCallback();
	UFUNCTION()
	void QuitDialogRejectButtonClickedCallback();
	UFUNCTION()
	void QuitDialogDestructedCallback();

protected:
	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class URaveWidgetSwitcher> MainMenuWidgetSwitcher;

	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class UButton> StartGameButton;

	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class UButton> OptionButton;

	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class UButton> QuitButton;

	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class URaveUserWidget_GameOptions> GameOptionsWidget;

	UPROPERTY()
	TObjectPtr<class URaveUserWidget_Dialog> QuitDialog;
};
