


#include "Gameplay/RaveGameplayShop.h"
#include "Inventory/RaveInventorySystemComponent.h"
#include "Inventory/RaveGameplayItem.h"
#include "Blueprint/UserWidget.h"
#include "Player/RavePlayerController.h"
#include "UI/RaveUserWidget_GameplayShop.h"
#include "Player/RavePlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveGameplayShop)

ARaveGameplayShop::ARaveGameplayShop()
{
	PrimaryActorTick.bCanEverTick = false;

	ShopInteractionDescription = FText::FromString(("Open Shop"));
}

bool ARaveGameplayShop::Purchase(APlayerController* Player, TSubclassOf<class URaveGameplayItem> ItemClass, int32 Index)
{
	if (!IsValid(Player) || !ShopGameplayItemClasses.IsValidIndex(Index))
	{
		return false;
	}

	TSubclassOf<URaveGameplayItem> ShopGameplayItemClass = ShopGameplayItemClasses[Index];
	URaveGameplayItem* GameplayItem = ItemClass.GetDefaultObject();
	if (GameplayItem && ItemClass == ShopGameplayItemClass)
	{
		ARavePlayerState* PlayerState = Player->GetPlayerState<ARavePlayerState>();
		if (IsValid(PlayerState))
		{
			URaveInventorySystemComponent* InventorySystemComponent = PlayerState->GetInventorySystemComponent();
			if (IsValid(InventorySystemComponent))
			{
				if (GameplayItem->PurchasePrice == 0 || InventorySystemComponent->RemoveCurrency(GameplayItem->PurchasePrice))
				{
					InventorySystemComponent->AddItem(ItemClass, 1);

					return true;
				}
			}
		}
	}

	return false;
}

bool ARaveGameplayShop::Sell(APlayerController* Player, TSubclassOf<class URaveGameplayItem> ItemClass, int32 Index)
{
	if (!IsValid(Player))
	{
		return true;
	}

	URaveGameplayItem* GameplayItem = ItemClass.GetDefaultObject();
	if (GameplayItem)
	{
		ARavePlayerState* PlayerState = Player->GetPlayerState<ARavePlayerState>();
		if (IsValid(PlayerState))
		{
			URaveInventorySystemComponent* InventorySystemComponent = PlayerState->GetInventorySystemComponent();
			if (IsValid(InventorySystemComponent) && InventorySystemComponent->GetInventorySlot(Index)->Key == ItemClass)
			{
				if (InventorySystemComponent->RemoveItemByIndex(Index, 1))
				{
					InventorySystemComponent->AddCurrency(GameplayItem->SalesPrice);

					return true;
				}
			}
		}
	}

	return false;
}

void ARaveGameplayShop::Interact_Implementation(AActor* Interactor)
{
	APawn* InteractorPawn = Cast<APawn>(Interactor);
	if (IsValid(InteractorPawn))
	{
		ARavePlayerController* InteractorPlayerController = InteractorPawn->GetController<ARavePlayerController>();
		if (IsValid(InteractorPlayerController))
		{
			URaveUserWidget_GameplayShop* GameplayShopWidget = Cast<URaveUserWidget_GameplayShop>(InteractorPlayerController->SetFocusWidget(GameplayShopWidgetClass));
			if (GameplayShopWidget)
			{
				GameplayShopWidget->InitializeGameplayShopWidget(this);
			}
		}
	}
}

FText ARaveGameplayShop::GetInteractionDescription_Implementation() const
{
	return ShopInteractionDescription;
}

bool ARaveGameplayShop::CanInteract_Implementation() const
{
	return true;
}

float ARaveGameplayShop::GetInteractionDuration_Implementation() const
{
	return 0.0f;
}

void ARaveGameplayShop::InteractionRangeEnter_Implementation(AActor* Interactor)
{
}

void ARaveGameplayShop::InteractionRangeLeave_Implementation(AActor* Interactor)
{
	APawn* InteractorPawn = Cast<APawn>(Interactor);
	if (IsValid(InteractorPawn))
	{
		ARavePlayerController* InteractorPlayerController = InteractorPawn->GetController<ARavePlayerController>();
		if (IsValid(InteractorPlayerController))
		{
			UUserWidget* FocusWidget = InteractorPlayerController->GetFocusWidget();
			if (FocusWidget && FocusWidget->GetClass()  == GameplayShopWidgetClass)
			{
				InteractorPlayerController->SetFocusWidget(nullptr);
			}
		}
	}
}

const TArray<TSubclassOf<class URaveGameplayItem>>& ARaveGameplayShop::GetShopGameplayItemClasses() const
{
	return ShopGameplayItemClasses;
}
