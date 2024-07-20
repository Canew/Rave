


#include "UI/RaveUserWidget_OperationTerminal.h"
#include "UI/Components/RaveButton.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "RaveGameInstance.h"
#include "Blueprint/WidgetTree.h"

void URaveUserWidget_OperationTerminal::NativeConstruct()
{
	Super::NativeConstruct();

	URaveGameInstance* GameInstance = GetGameInstance<URaveGameInstance>();
	check(GameInstance);

	OperationData = GameInstance->GetOperationData();
	check(OperationData);

	for (FRaveOperationDefinition OperationDefinition : OperationData->OperationDefinitions)
	{
		URaveButton* OperationVerticalSlotButton = WidgetTree->ConstructWidget<URaveButton>(URaveButton::StaticClass());
		if (OperationVerticalSlotButton)
		{
			UTextBlock* OperationVerticalSlotTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
			if (OperationVerticalSlotTextBlock)
			{
				OperationVerticalSlotTextBlock->SetText(OperationDefinition.OperationName);
				OperationVerticalSlotTextBlock->SetFont(OperationTypeTextBlock->GetFont());

				OperationVerticalSlotButton->SetStyle(StartOperationButton->GetStyle());
				OperationVerticalSlotButton->IsFocusable = false;
				OperationVerticalSlotButton->OnClicked.AddDynamic(this, &ThisClass::OperationVerticalBoxSlotClicked);
				OperationVerticalSlotButton->AddChild(OperationVerticalSlotTextBlock);

				UVerticalBoxSlot* VerticalBoxSlot = OperationsVerticalBox->AddChildToVerticalBox(OperationVerticalSlotButton);
				if (VerticalBoxSlot)
				{
					VerticalBoxSlot->SetPadding(FMargin(0.0f, 4.0f));
				}
			}
		}
	}

	StartOperationButton->OnClicked.AddDynamic(this, &ThisClass::StartOperationButtonClicked);
	StartOperationButton->SetIsEnabled(false);
}

void URaveUserWidget_OperationTerminal::SetSelectedOperation(FRaveOperationDefinition Operation)
{
	OperationImage->SetBrushFromTexture(Operation.OperationImage);
	OperationNameTextBlock->SetText(Operation.OperationName);
	OperationTypeTextBlock->SetText(FText::FromString(FString::FromInt(Operation.OperationType)));
	OperationDescriptionTextBlock->SetText(Operation.OperationDescription);
	OperationSceneryImage->SetBrushFromTexture(Operation.OperationSceneryImage);
	OperationSceneryImage->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));

	SelectedOperation = Operation;
}

void URaveUserWidget_OperationTerminal::OperationVerticalBoxSlotClicked(const URaveButton* Button)
{
	UTextBlock* ButtonTextBlock = Cast<UTextBlock>(Button->GetChildAt(0));
	if (ButtonTextBlock)
	{
		const FRaveOperationDefinition* OperationDefinition = OperationData->FindOperationDefinition(FName(*ButtonTextBlock->GetText().ToString()));
		if (OperationDefinition)
		{
			SetSelectedOperation(*OperationDefinition);
			StartOperationButton->SetIsEnabled(true);
		}
	}
}

void URaveUserWidget_OperationTerminal::StartOperationButtonClicked()
{
	const FRaveOperationDefinition* OperationDefinition = OperationData->FindOperationDefinition(FName(*SelectedOperation.OperationName.ToString()));
	if (OperationDefinition)
	{
		APlayerController* OwningPlayer = GetOwningPlayer();
		if (IsValid(OwningPlayer) && OwningPlayer->HasAuthority())
		{
			UWorld* World = OwningPlayer->GetWorld();
			if (World)
			{
				World->ServerTravel(SelectedOperation.OperationURL);
			}
		}
	}
}
