

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RaveGameplayItem.generated.h"

/**
 * 
 */
UCLASS(abstract, BlueprintType, Blueprintable)
class RAVE_API URaveGameplayItem : public UDataAsset
{
	GENERATED_BODY()
	
public:
	bool CanStack() { return GrantedAbilityClass == nullptr; }
	bool CanUsable() { return GrantedAbilityClass != nullptr; }
	bool CanEquipable() { return GrantedAbilityClass != nullptr; }

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText ItemDisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 PurchasePrice;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 SalesPrice;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSubclassOf<class URaveGameplayAbility> GrantedAbilityClass;
};
