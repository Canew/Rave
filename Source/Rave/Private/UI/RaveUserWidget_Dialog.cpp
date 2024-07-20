


#include "UI/RaveUserWidget_Dialog.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/VerticalBox.h"

void URaveUserWidget_Dialog::NativeConstruct()
{
	Super::NativeConstruct();

	DialogBorder->OnMouseButtonDownEvent.BindUFunction(this, TEXT("HandleDialogBorderMouseButtonDown"));
	AcceptButton->OnClicked.AddDynamic(this, &ThisClass::HandleAcceptButtonClicked);
	RejectButton->OnClicked.AddDynamic(this, &ThisClass::HandleRejectButtonClicked);
}

FReply URaveUserWidget_Dialog::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	if (!GetIsEnabled())
	{
		return FReply::Unhandled();
	}

	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		RejectButton->OnClicked.Broadcast();
	}

	return FReply::Handled();
}

void URaveUserWidget_Dialog::HandleDialogBorderMouseButtonDown()
{
	RemoveFromParent();
}

void URaveUserWidget_Dialog::HandleAcceptButtonClicked()
{
	RemoveFromParent();
}

void URaveUserWidget_Dialog::HandleRejectButtonClicked()
{
	RemoveFromParent();
}
