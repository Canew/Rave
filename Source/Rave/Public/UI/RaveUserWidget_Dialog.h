

#pragma once

#include "CoreMinimal.h"
#include "UI/RaveUserWidget.h"
#include "RaveUserWidget_Dialog.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveUserWidget_Dialog : public URaveUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

protected:
	UFUNCTION()
	void HandleDialogBorderMouseButtonDown();
	UFUNCTION()
	void HandleAcceptButtonClicked();
	UFUNCTION()
	void HandleRejectButtonClicked();

public:
	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class UBorder> DialogBorder;

	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class UTextBlock> TitleTextBlock;

	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class UTextBlock> DialogTextBlock;

	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class UVerticalBox> DialogDetailVerticalBox;

	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class UButton> AcceptButton;

	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class UButton> RejectButton;
};
