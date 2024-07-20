

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpec.h"
#include "Character/RaveCharacterSetting.h"
#include "Inventory/RaveGameplayItem.h"
#include "RavePlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRaveCharacterSettingChanged, const URaveCharacterSetting*, CharacterSetting);

/**
 * 
 */
UCLASS()
class RAVE_API ARavePlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	ARavePlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	//~End of AActor interface

	UFUNCTION(BlueprintCallable, Category = "Rave|PlayerState")
	class ARavePlayerController* GetRavePlayerController() const;

	UFUNCTION(BlueprintCallable, Category = "Rave|PlayerState")
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	template <typename T>
	T* GetAbilitySystemComponent() const
	{
		return Cast<T>(GetAbilitySystemComponent());
	}

	UFUNCTION(BlueprintPure, Category = "Rave|PlayerState")
	const URaveCharacterSetting* GetCharacterSetting() const { return CharacterSetting; }
	UFUNCTION(BlueprintCallable, Category = "Rave|PlayerState")
	void SetCharacterSetting(const class URaveCharacterSetting* InCharacterSetting);

	UFUNCTION(BlueprintPure, Category = "Rave|PlayerState")
	virtual class URaveInventorySystemComponent* GetInventorySystemComponent() const;

public:
	UPROPERTY(BlueprintAssignable, Category = "Rave|PlayerState")
	FRaveCharacterSettingChanged CharacterSettingChanged;

protected:
	UPROPERTY()
	TObjectPtr<const class URaveCharacterSetting> CharacterSetting;

private:
	// The ability system component sub-object used by player characters.
	UPROPERTY(VisibleAnywhere, Category = "PlayerState")
	TObjectPtr<class URaveAbilitySystemComponent> AbilitySystemComponent;

	// The inventory system component sub-object used by player characters.
	UPROPERTY(VisibleAnywhere, Category = "PlayerState")
	TObjectPtr<class URaveInventorySystemComponent> InventorySystemComponent;
};
