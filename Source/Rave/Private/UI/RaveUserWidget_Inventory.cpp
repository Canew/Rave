


#include "UI/RaveUserWidget_Inventory.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "AbilitySystemGlobals.h"
#include "GameplayAbility/RaveAbilitySystemComponent.h"
#include "Player/RavePlayerState.h"
#include "UI/RaveUserWidget_InventorySlot.h"
#include "Inventory/RaveInventorySystemComponent.h"

void URaveUserWidget_Inventory::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RemainingSkillPointTextBlock->TextDelegate.BindDynamic(this, &ThisClass::GetRemainingSkillPointText);
}

void URaveUserWidget_Inventory::NativeConstruct()
{
	Super::NativeConstruct();

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

					ItemUniformGridPanel->AddChildToUniformGrid(InventorySlotWidget, Index / ItemMaxColumnPerRow, Index % ItemMaxColumnPerRow);
				}
			}
		}
	}
}

FText URaveUserWidget_Inventory::GetRemainingSkillPointText()
{
	if (URaveAbilitySystemComponent* AbilitySystemComponent = Cast<URaveAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwningPlayerPawn())))
	{
		return FText::FromString(FString::FromInt(AbilitySystemComponent->GetRemainingSkillPoint()));
	}

	return FText();
}
