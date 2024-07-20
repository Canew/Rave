


#include "UI/RaveUserWidget_GameplayShop.h"
#include "UI/RaveUserWidget_GameplayShopGrid.h"
#include "UI/RaveUserWidget_GameplayShopSlot.h"
#include "UI/RaveUserWidget_InventorySlot.h"
#include "Gameplay/RaveGameplayShop.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Player/RavePlayerState.h"
#include "Inventory/RaveInventorySystemComponent.h"

void URaveUserWidget_GameplayShop::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ARavePlayerState* PlayerState = GetOwningPlayerState<ARavePlayerState>();
	if (PlayerState)
	{
		InventorySystemComponent = PlayerState->GetInventorySystemComponent();
		if (IsValid(InventorySystemComponent.Get()))
		{
			for (int Index = 0; Index < InventorySystemComponent->GetInventoryCapacity(); Index++)
			{
				URaveUserWidget_InventorySlot* InventorySlotWidget = CreateWidget<URaveUserWidget_InventorySlot>(GetOwningPlayer(), InventorySlotWidgetClass);
				if (InventorySlotWidget)
				{
					InventorySlotWidget->SetInventorySlotType(ERaveInventorySlotType::Item);
					InventorySlotWidget->SetInventoryIndex(Index);

					InventoryUniformGridPanel->AddChildToUniformGrid(InventorySlotWidget, Index / InventoryMaxColumn, Index % InventoryMaxColumn);
				}
			}
		}
	}
}

void URaveUserWidget_GameplayShop::InitializeGameplayShopWidget(ARaveGameplayShop* GameplayShop)
{
	OwningGameplayShop = GameplayShop;

	GameplayShopGridWidget->InitializeGameplayShopGrid(GameplayShop);
}
