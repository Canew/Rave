

#pragma once

#include "CoreMinimal.h"
#include "UI/RaveUserWidget.h"
#include "Data/RaveOperationData.h"
#include "RaveUserWidget_OperationTerminal.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveUserWidget_OperationTerminal : public URaveUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	void SetSelectedOperation(FRaveOperationDefinition Operation);

	UFUNCTION()
	void OperationVerticalBoxSlotClicked(const class URaveButton* Button);
	UFUNCTION()
	void StartOperationButtonClicked();
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UVerticalBox> OperationsVerticalBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> OperationImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> OperationNameTextBlock;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> OperationTypeTextBlock;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> OperationDescriptionTextBlock;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> OperationSceneryImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> StartOperationButton;

	const URaveOperationData* OperationData;
	FRaveOperationDefinition SelectedOperation;
};
