// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Components/RaveButton.h"

#include "Binding/States/WidgetStateBitfield.h"
#include "Binding/States/WidgetStateRegistration.h"
#include "Widgets/SNullWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Input/SButton.h"
#include "Components/ButtonSlot.h"
#include "Styling/DefaultStyleCache.h"
#include "Styling/UMGCoreStyle.h"
#include "Blueprint/WidgetTree.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveButton)

#define LOCTEXT_NAMESPACE "UMG"

/////////////////////////////////////////////////////
// UButton

URaveButton::URaveButton(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
		WidgetStyle = UE::Slate::Private::FDefaultStyleCache::GetRuntime().GetButtonStyle();
	PRAGMA_ENABLE_DEPRECATION_WARNINGS

#if WITH_EDITOR 
		if (IsEditorWidget())
		{
			PRAGMA_DISABLE_DEPRECATION_WARNINGS
				WidgetStyle = UE::Slate::Private::FDefaultStyleCache::GetEditor().GetButtonStyle();
			PRAGMA_ENABLE_DEPRECATION_WARNINGS

				// The CDO isn't an editor widget and thus won't use the editor style, call post edit change to mark difference from CDO
				PostEditChange();
		}
#endif // WITH_EDITOR

	PRAGMA_DISABLE_DEPRECATION_WARNINGS
		ColorAndOpacity = FLinearColor::White;
	BackgroundColor = FLinearColor::White;

	ClickMethod = EButtonClickMethod::DownAndUp;
	TouchMethod = EButtonTouchMethod::DownAndUp;

	IsFocusable = true;
	PRAGMA_ENABLE_DEPRECATION_WARNINGS

#if WITH_EDITORONLY_DATA
		AccessibleBehavior = ESlateAccessibleBehavior::Summary;
	bCanChildrenBeAccessible = false;
#endif
}

void URaveButton::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyButton.Reset();
}

TSharedRef<SWidget> URaveButton::RebuildWidget()
{
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
		MyButton = SNew(SButton)
		.OnClicked(BIND_UOBJECT_DELEGATE(FOnClicked, SlateHandleClicked))
		.OnPressed(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandlePressed))
		.OnReleased(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandleReleased))
		.OnHovered_UObject(this, &ThisClass::SlateHandleHovered)
		.OnUnhovered_UObject(this, &ThisClass::SlateHandleUnhovered)
		.ButtonStyle(&WidgetStyle)
		.ClickMethod(ClickMethod)
		.TouchMethod(TouchMethod)
		.PressMethod(PressMethod)
		.IsFocusable(IsFocusable)
		;
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
		if (GetChildrenCount() > 0)
		{
			Cast<UButtonSlot>(GetContentSlot())->BuildSlot(MyButton.ToSharedRef());
		}

	return MyButton.ToSharedRef();
}

void URaveButton::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (!MyButton.IsValid())
	{
		return;
	}

	PRAGMA_DISABLE_DEPRECATION_WARNINGS
		MyButton->SetButtonStyle(&WidgetStyle);
	MyButton->SetColorAndOpacity(ColorAndOpacity);
	MyButton->SetBorderBackgroundColor(BackgroundColor);
	MyButton->SetClickMethod(ClickMethod);
	MyButton->SetTouchMethod(TouchMethod);
	MyButton->SetPressMethod(PressMethod);
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
}

UClass* URaveButton::GetSlotClass() const
{
	return UButtonSlot::StaticClass();
}

void URaveButton::OnSlotAdded(UPanelSlot* InSlot)
{
	// Add the child to the live slot if it already exists
	if (MyButton.IsValid())
	{
		CastChecked<UButtonSlot>(InSlot)->BuildSlot(MyButton.ToSharedRef());
	}
}

void URaveButton::OnSlotRemoved(UPanelSlot* InSlot)
{
	// Remove the widget from the live slot if it exists.
	if (MyButton.IsValid())
	{
		MyButton->SetContent(SNullWidget::NullWidget);
	}
}

PRAGMA_DISABLE_DEPRECATION_WARNINGS
void URaveButton::SetStyle(const FButtonStyle& InStyle)
{
	WidgetStyle = InStyle;
	if (MyButton.IsValid())
	{
		MyButton->SetButtonStyle(&WidgetStyle);
	}
}

const FButtonStyle& URaveButton::GetStyle() const
{
	return WidgetStyle;
}

void URaveButton::SetColorAndOpacity(FLinearColor InColorAndOpacity)
{
	ColorAndOpacity = InColorAndOpacity;
	if (MyButton.IsValid())
	{
		MyButton->SetColorAndOpacity(InColorAndOpacity);
	}
}

FLinearColor URaveButton::GetColorAndOpacity() const
{
	return ColorAndOpacity;
}

void URaveButton::SetBackgroundColor(FLinearColor InBackgroundColor)
{
	BackgroundColor = InBackgroundColor;
	if (MyButton.IsValid())
	{
		MyButton->SetBorderBackgroundColor(InBackgroundColor);
	}
}

FLinearColor URaveButton::GetBackgroundColor() const
{
	return BackgroundColor;
}
PRAGMA_ENABLE_DEPRECATION_WARNINGS

bool URaveButton::IsPressed() const
{
	if (MyButton.IsValid())
	{
		return MyButton->IsPressed();
	}

	return false;
}

PRAGMA_DISABLE_DEPRECATION_WARNINGS
void URaveButton::SetClickMethod(EButtonClickMethod::Type InClickMethod)
{
	ClickMethod = InClickMethod;
	if (MyButton.IsValid())
	{
		MyButton->SetClickMethod(ClickMethod);
	}
}

EButtonClickMethod::Type URaveButton::GetClickMethod() const
{
	return ClickMethod;
}

void URaveButton::SetTouchMethod(EButtonTouchMethod::Type InTouchMethod)
{
	TouchMethod = InTouchMethod;
	if (MyButton.IsValid())
	{
		MyButton->SetTouchMethod(TouchMethod);
	}
}

EButtonTouchMethod::Type URaveButton::GetTouchMethod() const
{
	return TouchMethod;
}

void URaveButton::SetPressMethod(EButtonPressMethod::Type InPressMethod)
{
	PressMethod = InPressMethod;
	if (MyButton.IsValid())
	{
		MyButton->SetPressMethod(PressMethod);
	}
}

EButtonPressMethod::Type URaveButton::GetPressMethod() const
{
	return PressMethod;
}

bool URaveButton::GetIsFocusable() const
{
	return IsFocusable;
}

void URaveButton::InitIsFocusable(bool InIsFocusable)
{
	IsFocusable = InIsFocusable;
}

PRAGMA_ENABLE_DEPRECATION_WARNINGS

void URaveButton::PostLoad()
{
	Super::PostLoad();

	if (GetChildrenCount() > 0)
	{
		//TODO UMG Pre-Release Upgrade, now buttons have slots of their own.  Convert existing slot to new slot.
		if (UPanelSlot* PanelSlot = GetContentSlot())
		{
			UButtonSlot* ButtonSlot = Cast<UButtonSlot>(PanelSlot);
			if (ButtonSlot == NULL)
			{
				ButtonSlot = NewObject<UButtonSlot>(this);
				ButtonSlot->Content = GetContentSlot()->Content;
				ButtonSlot->Content->Slot = ButtonSlot;
				Slots[0] = ButtonSlot;
			}
		}
	}
}

FReply URaveButton::SlateHandleClicked()
{
	OnClicked.Broadcast(this);

	return FReply::Handled();
}

void URaveButton::SlateHandlePressed()
{
	OnPressed.Broadcast(this);
	BroadcastBinaryPostStateChange(UWidgetPressedStateRegistration::Bit, true);


}

void URaveButton::SlateHandleReleased()
{
	OnReleased.Broadcast(this);
	BroadcastBinaryPostStateChange(UWidgetPressedStateRegistration::Bit, false);
}

void URaveButton::SlateHandleHovered()
{
	OnHovered.Broadcast(this);
	BroadcastBinaryPostStateChange(UWidgetHoveredStateRegistration::Bit, true);
}

void URaveButton::SlateHandleUnhovered()
{
	OnUnhovered.Broadcast(this);
	BroadcastBinaryPostStateChange(UWidgetHoveredStateRegistration::Bit, false);
}

#if WITH_ACCESSIBILITY
TSharedPtr<SWidget> URaveButton::GetAccessibleWidget() const
{
	return MyButton;
}
#endif

#if WITH_EDITOR

const FText URaveButton::GetPaletteCategory()
{
	return LOCTEXT("Common", "Common");
}

#endif

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE

