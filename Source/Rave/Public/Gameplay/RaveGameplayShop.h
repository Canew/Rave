

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/RaveInteractionInterface.h"
#include "RaveGameplayShop.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRaveGameplayShopItemChanged);

UCLASS()
class RAVE_API ARaveGameplayShop : public AActor, public IRaveInteractionInterface
{
	GENERATED_BODY()
	
public:
	ARaveGameplayShop();

	virtual bool Purchase(APlayerController* Player, TSubclassOf<class URaveGameplayItem> ItemClass, int32 Index);
	virtual bool Sell(APlayerController* Player, TSubclassOf<class URaveGameplayItem> ItemClass, int32 Index);

	// Begin IRaveInteractionInterface
public:
	void Interact_Implementation(AActor* Interactor);
	FText GetInteractionDescription_Implementation() const;
	bool CanInteract_Implementation() const;
	float GetInteractionDuration_Implementation() const;
	void InteractionRangeEnter_Implementation(AActor* Interactor);
	void InteractionRangeLeave_Implementation(AActor* Interactor);
	// End IRaveInteractionInterface

	const TArray<TSubclassOf<class URaveGameplayItem>>& GetShopGameplayItemClasses() const;

public:
	FRaveGameplayShopItemChanged ShopItemChanged;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Shop")
	TArray<TSubclassOf<class URaveGameplayItem>> ShopGameplayItemClasses;

	UPROPERTY(EditDefaultsOnly, Category = "Shop")
	FText ShopInteractionDescription;

	UPROPERTY(EditDefaultsOnly, Category = "Shop")
	TSubclassOf<class URaveUserWidget_GameplayShop> GameplayShopWidgetClass;
};
