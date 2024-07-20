


#include "Inventory/RaveInventorySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayAbility/RaveAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveInventorySystemComponent)

URaveInventorySystemComponent::URaveInventorySystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

const TPair<TSubclassOf<URaveGameplayItem>, int32>* URaveInventorySystemComponent::GetInventorySlot(int32 InventoryIndex) const
{
	return Inventory.Find(InventoryIndex);
}

const TSubclassOf<URaveGameplayItem>* URaveInventorySystemComponent::GetModuleEquipment(FGameplayTag ModuleTag) const
{
	return ModuleEquipmentInventory.Find(ModuleTag);
}

int32 URaveInventorySystemComponent::GetInventoryCapacity() const
{
	return InventoryCapacity;
}

bool URaveInventorySystemComponent::AddItem(TSubclassOf<URaveGameplayItem> ItemClassToAdd, int32 CountToAdd, int32 InventoryIndexToAdd)
{
	if (!ItemClassToAdd || CountToAdd <= 0)
	{
		return false;
	}

	if (InventoryIndexToAdd == INDEX_NONE)
	{
		int32 FirstEmptySlotIndex = INDEX_NONE;
		for (int Index = 0; Index < InventoryCapacity; Index++)
		{
			TPair<TSubclassOf<URaveGameplayItem>, int32>* InventorySlot = Inventory.Find(Index);
			if (InventorySlot && InventorySlot->Key && ItemClassToAdd.GetDefaultObject()->CanStack() && ItemClassToAdd.Get() == InventorySlot->Key.Get())
			{
				InventorySlot->Value += CountToAdd;

				return true;
			}
			else if ((!InventorySlot || !InventorySlot->Key) && FirstEmptySlotIndex == INDEX_NONE)
			{
				FirstEmptySlotIndex = Index;
			}
		}

		if (FirstEmptySlotIndex > INDEX_NONE)
		{
			Inventory.Add(FirstEmptySlotIndex, TPair<TSubclassOf<URaveGameplayItem>, int32>(ItemClassToAdd, CountToAdd));

			return true;
		}
	}
	else
	{
		TPair<TSubclassOf<URaveGameplayItem>, int32>* InventorySlot = Inventory.Find(InventoryIndexToAdd);
		if (!InventorySlot || !IsValid((InventorySlot->Key)))
		{
			Inventory.Add(InventoryIndexToAdd, TPair<TSubclassOf<URaveGameplayItem>, int32>(ItemClassToAdd, CountToAdd));

			return true;
		}
	}

	return false;
}

bool URaveInventorySystemComponent::RemoveItem(TSubclassOf<URaveGameplayItem> ItemClassToRemove, int32 CountToRemove)
{
	if (!ItemClassToRemove || CountToRemove < 0)
	{
		return false;
	}

	for (int InventoryIndex = 0; InventoryIndex < InventoryCapacity; InventoryIndex++)
	{
		TPair<TSubclassOf<URaveGameplayItem>, int32>* InventorySlot = Inventory.Find(InventoryIndex);
		if (InventorySlot && InventorySlot->Key == ItemClassToRemove)
		{
			Inventory.Remove(InventoryIndex);

			return true;
		}
	}

	return false;
}

bool URaveInventorySystemComponent::RemoveItemByIndex(int32 InventoryIndexToRemove, int32 CountToRemove)
{
	TPair<TSubclassOf<URaveGameplayItem>, int32>* InventorySlot = Inventory.Find(InventoryIndexToRemove);
	if (InventorySlot && InventorySlot->Key)
	{
		Inventory.Remove(InventoryIndexToRemove);

		return true;
	}

	return false;
}

void URaveInventorySystemComponent::MoveItem(int32 StartInventoryIndex, int32 DestinationInventoryIndex)
{
	if (!IsValidInventoryIndex(StartInventoryIndex) && !IsValidInventoryIndex(DestinationInventoryIndex) && StartInventoryIndex == DestinationInventoryIndex)
	{
		return;
	}

	TPair<TSubclassOf<URaveGameplayItem>, int32>* StartInventoySlot = Inventory.Find(StartInventoryIndex);
	if (!StartInventoySlot || !StartInventoySlot->Key)
	{
		return;
	}

	TSubclassOf<URaveGameplayItem> StartGameplayItemClass = StartInventoySlot->Key;
	int32 StartGameplayItemCount = StartInventoySlot->Value;

	TPair<TSubclassOf<URaveGameplayItem>, int32>* DestinationInventorySlot = Inventory.Find(DestinationInventoryIndex);
	if (DestinationInventorySlot && DestinationInventorySlot->Key)
	{
		if (StartInventoySlot->Key == DestinationInventorySlot->Key && StartGameplayItemClass.GetDefaultObject()->CanStack())
		{
			// Merge item
			int32 DestinationGameplayItemCount = DestinationInventorySlot->Value;

			Inventory.Remove(StartInventoryIndex);
			Inventory.Add(DestinationInventoryIndex, TPair<TSubclassOf<URaveGameplayItem>, int32>(StartGameplayItemClass, StartGameplayItemCount + DestinationGameplayItemCount));
		}
		else
		{
			// Swap item
			Inventory.Add(StartInventoryIndex, *DestinationInventorySlot);
			Inventory.Add(DestinationInventoryIndex, TPair<TSubclassOf<URaveGameplayItem>, int32>(StartGameplayItemClass, StartGameplayItemCount));
		}
	}
	else
	{
		// Move item
		Inventory.Remove(StartInventoryIndex);
		Inventory.Add(DestinationInventoryIndex, TPair<TSubclassOf<URaveGameplayItem>, int32>(StartGameplayItemClass, StartGameplayItemCount));
	}
}

void URaveInventorySystemComponent::EquipModule(int32 InventoryIndex, FGameplayTag ModuleTag)
{
	URaveAbilitySystemComponent* AbilitySystemComponent = Cast<URaveAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()));
	if (!AbilitySystemComponent)
	{
		return;
	}

	TPair<TSubclassOf<URaveGameplayItem>, int32>* InventorySlot = Inventory.Find(InventoryIndex);
	if (InventorySlot && InventorySlot->Key)
	{
		URaveGameplayItem* GameplayItem = InventorySlot->Key.GetDefaultObject();
		if (GameplayItem && GameplayItem->CanEquipable())
		{
			// Check can equip module
			FGameplayTag FirstEmptyModuleTag;
			for (int Index = 1; Index <= 6; Index++)
			{
				FString ModuleInputTagString = TEXT("Input.Module_") + FString::FromInt(Index);
				FGameplayTag ModuleInputTag = FGameplayTag::RequestGameplayTag(FName(ModuleInputTagString));
				TSubclassOf<URaveGameplayItem>* CurrentEquipGameplayItemClass = ModuleEquipmentInventory.Find(ModuleInputTag);
				if (CurrentEquipGameplayItemClass && GameplayItem->GetClass() == CurrentEquipGameplayItemClass->Get())
				{
					return;
				}
				else if (!CurrentEquipGameplayItemClass && !FirstEmptyModuleTag.IsValid())
				{
					FirstEmptyModuleTag = ModuleInputTag;
				}
			}

			// Equip module
			FGameplayAbilitySpec AbilitySpecToEquip(GameplayItem->GrantedAbilityClass, 1);
			if (ModuleTag.IsValid())
			{
				TSubclassOf<URaveGameplayItem>* CurrentEquipGameplayItemClassPtr = ModuleEquipmentInventory.Find(ModuleTag);
				if (CurrentEquipGameplayItemClassPtr)
				{
					TSubclassOf<URaveGameplayItem> CurrentEquipGameplayItemClass = *CurrentEquipGameplayItemClassPtr;
					FGameplayAbilitySpec* CurrentEquipAbilitySpec = AbilitySystemComponent->FindAbilitySpecFromClass(CurrentEquipGameplayItemClass.GetDefaultObject()->GrantedAbilityClass);
					if (CurrentEquipAbilitySpec && CurrentEquipAbilitySpec->GetPrimaryInstance() && !CurrentEquipAbilitySpec->IsActive() && CurrentEquipAbilitySpec->GetPrimaryInstance()->CheckCooldown(CurrentEquipAbilitySpec->Handle, AbilitySystemComponent->AbilityActorInfo.Get()))
					{
						// Swap item if module is already equipped in slot
						AbilitySystemComponent->ClearAbility(CurrentEquipAbilitySpec->Handle);

						ModuleEquipmentInventory.Add(ModuleTag, InventorySlot->Key);

						AbilitySpecToEquip.DynamicAbilityTags.AddTag(ModuleTag);
						AbilitySystemComponent->GiveAbility(AbilitySpecToEquip);

						Inventory.Add(InventoryIndex, TPair<TSubclassOf<URaveGameplayItem>, int32>(CurrentEquipGameplayItemClass, 1));
					}
				}
				else
				{
					ModuleEquipmentInventory.Add(ModuleTag, GameplayItem->GetClass());

					AbilitySpecToEquip.DynamicAbilityTags.AddTag(ModuleTag);
					AbilitySystemComponent->GiveAbility(AbilitySpecToEquip);

					Inventory.Remove(InventoryIndex);
				}
			}
			else if (FirstEmptyModuleTag.IsValid())
			{
				ModuleEquipmentInventory.Add(FirstEmptyModuleTag, GameplayItem->GetClass());

				AbilitySpecToEquip.DynamicAbilityTags.AddTag(FirstEmptyModuleTag);
				AbilitySystemComponent->GiveAbility(AbilitySpecToEquip);

				Inventory.Remove(InventoryIndex);
			}
		}
	}
}

void URaveInventorySystemComponent::UnequipModule(FGameplayTag ModuleTag, int32 InventoryIndex)
{
	URaveAbilitySystemComponent* AbilitySystemComponent = Cast<URaveAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()));
	if (!AbilitySystemComponent || !ModuleTag.IsValid())
	{
		return;
	}

	TSubclassOf<URaveGameplayItem>* CurrentEquipGameplayItemClassPtr = ModuleEquipmentInventory.Find(ModuleTag);
	if (CurrentEquipGameplayItemClassPtr)
	{
		TSubclassOf<URaveGameplayItem> CurrentEquipGameplayItemClass = *CurrentEquipGameplayItemClassPtr;
		FGameplayAbilitySpec* CurrentEquipAbilitySpec = AbilitySystemComponent->FindAbilitySpecFromClass(CurrentEquipGameplayItemClass.GetDefaultObject()->GrantedAbilityClass);
		if (CurrentEquipAbilitySpec && CurrentEquipAbilitySpec->GetPrimaryInstance() && !CurrentEquipAbilitySpec->IsActive() && CurrentEquipAbilitySpec->GetPrimaryInstance()->CheckCooldown(CurrentEquipAbilitySpec->Handle, AbilitySystemComponent->AbilityActorInfo.Get()))
		{
			if (InventoryIndex == INDEX_NONE)
			{
				for (int Index = 0; Index < InventoryCapacity; Index++)
				{
					TPair<TSubclassOf<URaveGameplayItem>, int32>* InventorySlot = Inventory.Find(Index);
					if (!InventorySlot || !InventorySlot->Key)
					{
						AbilitySystemComponent->ClearAbility(CurrentEquipAbilitySpec->Handle);
						ModuleEquipmentInventory.Remove(ModuleTag);

						Inventory.Add(Index, TPair<TSubclassOf<URaveGameplayItem>, int32>(CurrentEquipGameplayItemClass, 1));

						break;
					}
				}
			}
			else if (IsValidInventoryIndex(InventoryIndex))
			{
				TPair<TSubclassOf<URaveGameplayItem>, int32>* InventorySlot = Inventory.Find(InventoryIndex);
				if (InventorySlot && InventorySlot->Key)
				{
					// Check can swap
					FGameplayTag FirstEmptyModuleTag;
					for (int Index = 1; Index <= 6; Index++)
					{
						FString ModuleInputTagString = TEXT("Input.Module_") + FString::FromInt(Index);
						FGameplayTag ModuleInputTag = FGameplayTag::RequestGameplayTag(FName(ModuleInputTagString));
						if (ModuleInputTag == ModuleTag)
						{
							continue;
						}

						TSubclassOf<URaveGameplayItem>* EquipGameplayItemClass = ModuleEquipmentInventory.Find(ModuleInputTag);
						if (EquipGameplayItemClass && InventorySlot->Key == EquipGameplayItemClass->Get())
						{
							return;
						}
					}

					AbilitySystemComponent->ClearAbility(CurrentEquipAbilitySpec->Handle);
					ModuleEquipmentInventory[ModuleTag] = InventorySlot->Key;

					FGameplayAbilitySpec InventoryAbilitySpec(InventorySlot->Key.GetDefaultObject()->GrantedAbilityClass, 1);
					InventoryAbilitySpec.DynamicAbilityTags.AddTag(ModuleTag);
					AbilitySystemComponent->GiveAbility(InventoryAbilitySpec);

					Inventory[InventoryIndex] = TPair<TSubclassOf<URaveGameplayItem>, int32>(CurrentEquipGameplayItemClass, 1);
				}
				else
				{
					AbilitySystemComponent->ClearAbility(CurrentEquipAbilitySpec->Handle);
					ModuleEquipmentInventory.Remove(ModuleTag);

					Inventory.Add(InventoryIndex, TPair<TSubclassOf<URaveGameplayItem>, int32>(CurrentEquipGameplayItemClass, 1));
				}
			}
		}
	}
}

void URaveInventorySystemComponent::MoveModule(FGameplayTag StartModuleTag, FGameplayTag DestinationModuleTag)
{
	URaveAbilitySystemComponent* AbilitySystemComponent = Cast<URaveAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()));
	if (!AbilitySystemComponent || StartModuleTag == DestinationModuleTag)
	{
		return;
	}

	TSubclassOf<URaveGameplayItem>* StartGameplayItemClassPtr = ModuleEquipmentInventory.Find(StartModuleTag);
	if (StartGameplayItemClassPtr)
	{
		TSubclassOf<URaveGameplayItem> StartGameplayItemClass = *StartGameplayItemClassPtr;
		URaveGameplayItem* StartGameplayItem = StartGameplayItemClass.GetDefaultObject();
		FGameplayAbilitySpec* StartAbilitySpec = AbilitySystemComponent->FindAbilitySpecFromClass(StartGameplayItem->GrantedAbilityClass);
		if (StartAbilitySpec && StartAbilitySpec->GetPrimaryInstance() && !StartAbilitySpec->IsActive() && StartAbilitySpec->GetPrimaryInstance()->CheckCooldown(StartAbilitySpec->Handle, AbilitySystemComponent->AbilityActorInfo.Get()))
		{
			TSubclassOf<URaveGameplayItem>* DestinationGameplayItemClass = ModuleEquipmentInventory.Find(DestinationModuleTag);
			if (DestinationGameplayItemClass)
			{
				URaveGameplayItem* DestinationGameplayItem = DestinationGameplayItemClass->GetDefaultObject();
				FGameplayAbilitySpec* DestinationAbilitySpec = AbilitySystemComponent->FindAbilitySpecFromClass(DestinationGameplayItem->GrantedAbilityClass);
				if (DestinationAbilitySpec && DestinationAbilitySpec->GetPrimaryInstance() && !DestinationAbilitySpec->IsActive() && DestinationAbilitySpec->GetPrimaryInstance()->CheckCooldown(DestinationAbilitySpec->Handle, AbilitySystemComponent->AbilityActorInfo.Get()))
				{
					ModuleEquipmentInventory[DestinationModuleTag] = StartGameplayItem->GetClass();
					ModuleEquipmentInventory[StartModuleTag] = DestinationGameplayItem->GetClass();

					AbilitySystemComponent->AssignInputTag(*StartAbilitySpec, DestinationModuleTag);
					AbilitySystemComponent->AssignInputTag(*DestinationAbilitySpec, StartModuleTag);
				}
			}
			else
			{
				ModuleEquipmentInventory.Add(DestinationModuleTag, StartGameplayItem->GetClass());
				ModuleEquipmentInventory.Remove(StartModuleTag);

				AbilitySystemComponent->AssignInputTag(*StartAbilitySpec, DestinationModuleTag);
			}
		}
	}
}

int32 URaveInventorySystemComponent::GetCurrency() const
{
	return CurrentCurrency;
}

void URaveInventorySystemComponent::AddCurrency(int32 Amount)
{
	if (Amount > 0)
	{
		CurrentCurrency = CurrentCurrency + Amount;

		CurrentCurrencyChanged.Broadcast(CurrentCurrency);
	}
}

bool URaveInventorySystemComponent::RemoveCurrency(int32 Amount, bool bForce)
{
	if (Amount > 0)
	{
		if (CurrentCurrency >= Amount || bForce)
		{
			CurrentCurrency = FMath::Max(CurrentCurrency - Amount, 0);

			CurrentCurrencyChanged.Broadcast(CurrentCurrency);

			return true;
		}
	}

	return false;
}

bool URaveInventorySystemComponent::IsValidInventoryIndex(int32 Index)
{
	return Index >= 0 && Index < InventoryCapacity;
}
