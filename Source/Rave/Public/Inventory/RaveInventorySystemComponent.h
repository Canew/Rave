

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/RaveGameplayItem.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpec.h"
#include "RaveInventorySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FRaveCurrentCurrencyChanged, int32);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RAVE_API URaveInventorySystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URaveInventorySystemComponent();

public:
	const TPair<TSubclassOf<URaveGameplayItem>, int32>* GetInventorySlot(int32 InventoryIndex) const;
	const TSubclassOf<URaveGameplayItem>* GetModuleEquipment(FGameplayTag ModuleTag) const;

	UFUNCTION(BlueprintPure, Category = "Rave|Inventory")
	int32 GetInventoryCapacity() const;

	/** Add item to inventory at specified index. If index is not valid, add at first empty index. return true if you succeed in adding. */
	UFUNCTION(BlueprintCallable, Category = "Rave|Inventory")
	bool AddItem(TSubclassOf<URaveGameplayItem> ItemClassToAdd, int32 CountToAdd, int32 InventoryIndexToAdd = -1);

	/** Remove first item that equal to ability class from inventory. return true if you succeed in removing. */
	UFUNCTION(BlueprintCallable, Category = "Rave|Inventory")
	bool RemoveItem(TSubclassOf<URaveGameplayItem> ItemClassToRemove, int32 CountToRemove);
	/** Remove module ability from inventory at specified index. return true if you succeed in removing. */
	UFUNCTION(BlueprintCallable, Category = "Rave|Inventory")
	bool RemoveItemByIndex(int32 InventoryIndexToRemove, int32 CountToRemove);

	UFUNCTION(BlueprintCallable, Category = "Rave|Inventory")
	void MoveItem(int32 StartInventoryIndex, int32 DestinationInventoryIndex);

	/** Equip module item to speicfied module index. If module index is not valid, equip to first empty index */
	UFUNCTION(BlueprintCallable, Category = "Rave|Inventory")
	void EquipModule(int32 InventoryIndex, FGameplayTag ModuleTag = FGameplayTag());

	/** Unequip module item to specified inventory index. If inventory index is not valid, unequip to first empty index */
	UFUNCTION(BlueprintCallable, Category = "Rave|Inventory")
	void UnequipModule(FGameplayTag ModuleTag, int32 InventoryIndex = -1);

	UFUNCTION(BlueprintCallable, Category = "Rave|Inventory")
	void MoveModule(FGameplayTag StartTag, FGameplayTag DestinationTag);

	UFUNCTION(BlueprintCallable, Category = "Rave|Inventory")
	int32 GetCurrency() const;

	UFUNCTION(BlueprintCallable, Category = "Rave|Inventory")
	void AddCurrency(int32 Amount);

	// Get rid of Currency as much as amount. if current Currency greater equal than return true.
	// if bForce true, then remove and return true regardless of remaining amount condition.
	UFUNCTION(BlueprintCallable, Category = "Rave|Inventory")
	bool RemoveCurrency(int32 Amount, bool bForce = false);
	
protected:
	UFUNCTION(BlueprintPure)
	bool IsValidInventoryIndex(int32 Index);

public:
	FRaveCurrentCurrencyChanged CurrentCurrencyChanged;

protected:
	TMap<int32, TPair<TSubclassOf<URaveGameplayItem>, int32>> Inventory;
	TMap<FGameplayTag, TSubclassOf<URaveGameplayItem>> ModuleEquipmentInventory;
	const int32 InventoryCapacity = 40;

	UPROPERTY()
	int32 CurrentCurrency;
};
