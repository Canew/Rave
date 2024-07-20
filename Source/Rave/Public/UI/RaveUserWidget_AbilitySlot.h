

#pragma once

#include "CoreMinimal.h"
#include "UI/RaveUserWidget.h"
#include "GameplayAbilitySpec.h"
#include "GameplayTagContainer.h"
#include "RaveUserWidget_AbilitySlot.generated.h"

UENUM(BlueprintType)
enum class ENoahAbilitySlotType : uint8
{
	Skill,
	Module,
	Spell,
};

/**
 * 
 */
UCLASS()
class RAVE_API URaveUserWidget_AbilitySlot : public URaveUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	void UpdateAbilitySlot();
	void UpdateCooldown();

	// Handler method
	void HandleAbilityAdded(FGameplayAbilitySpec& AbilitySpec);
	void HandleAbilityRemoved(FGameplayAbilitySpec& AbilitySpec);

protected:
	UPROPERTY(EditInstanceOnly, Category = "Rave|Type")
	ENoahAbilitySlotType AbilitySlotType;

	UPROPERTY(EditInstanceOnly, Category = "Rave|Input", Meta = (Categories = "Input"))
	FGameplayTag AbilitySlotInputTag;

	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class UImage> AbilitySlotImage;

	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<class UTextBlock> AbilityCooldownTextBlock;

	TObjectPtr<class UMaterialInstanceDynamic> ImageMID;

	UPROPERTY(EditDefaultsOnly, Category = "Rave|Appearance")
	TObjectPtr<class UTexture2D> DefaultBackgroundTexture;

	FGameplayAbilitySpec SlotAbilitySpec;

	const float ChainBorderShade = 75.0f;
};
