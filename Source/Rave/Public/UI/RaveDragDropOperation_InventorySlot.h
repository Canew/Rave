

#pragma once

#include "CoreMinimal.h"
#include "UI/RaveDragDropOperation.h"
#include "Abilities/GameplayAbility.h"
#include "Inventory/RaveGameplayItem.h"
#include "GameplayTagContainer.h"
#include "UI/RaveUserWidget_InventorySlot.h"
#include "RaveDragDropOperation_InventorySlot.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveDragDropOperation_InventorySlot : public URaveDragDropOperation
{
	GENERATED_BODY()
	
public:
	URaveDragDropOperation_InventorySlot();

public:
	ERaveInventorySlotType SlotType;
	TSubclassOf<class URaveGameplayItem> GameplayItemClass;
	TSubclassOf<class UGameplayAbility> AbilityClass;
	FGameplayTag InputTag;
	int32 InventoryIndex;
};
