


#include "UI/RaveUserWidget_Currency.h"
#include "Components/TextBlock.h"
#include "Player/RavePlayerState.h"
#include "Inventory/RaveInventorySystemComponent.h"

void URaveUserWidget_Currency::NativeConstruct()
{
	Super::NativeConstruct();
	
	RequestInvetorySystemComponent();
}

void URaveUserWidget_Currency::NativeDestruct()
{
	Super::NativeDestruct();

	if (IsValid(GetOwningPlayer()))
	{
		GetOwningPlayer()->GetWorldTimerManager().ClearAllTimersForObject(this);
	}
}

void URaveUserWidget_Currency::RequestInvetorySystemComponent()
{
	ARavePlayerState* RavePlayerState = GetOwningPlayerState<ARavePlayerState>();
	if (IsValid(RavePlayerState))
	{
		InventorySystemComponent = RavePlayerState->GetInventorySystemComponent();
	}

	if (IsValid(InventorySystemComponent.Get()))
	{
		UpdateCurrency(InventorySystemComponent->GetCurrency());
		InventorySystemComponent->CurrentCurrencyChanged.AddUObject(this, &ThisClass::UpdateCurrency);
	}
	else if (IsValid(GetOwningPlayer()))
	{
		GetOwningPlayer()->GetWorldTimerManager().SetTimer(RequestTimerHandle, this, &ThisClass::RequestInvetorySystemComponent, RequestDelay, false);
	}
	else
	{
		RemoveFromParent();
	}
}

void URaveUserWidget_Currency::UpdateCurrency(int32 CurrentCurrency)
{
	FString CurrencyString = FString::FromInt(CurrentCurrency);
	CurrencyTextBlock->SetText(FText::FromString(CurrencyString));
}
