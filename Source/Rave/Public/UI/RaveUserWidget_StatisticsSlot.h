

#pragma once

#include "CoreMinimal.h"
#include "UI/RaveUserWidget.h"
#include "AttributeSet.h"
#include "GameplayEffectTypes.h"
#include "RaveUserWidget_StatisticsSlot.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveUserWidget_StatisticsSlot : public URaveUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;

	void UpdateStatisticsAttribute();

	void HandleAttributeValueChanged(const FOnAttributeChangeData& Data);

	UFUNCTION()
	UWidget* GetStatisticsToolTip();
	
protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> StatisticsSlotImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> StatisticsSlotTextBlock;

	UPROPERTY(EditInstanceOnly, Category = "Statistics")
	FText AttributeDisplayName;

	UPROPERTY(EditInstanceOnly, Category = "Statistics")
	int32 ValueFractionalDigits;

	UPROPERTY(EditInstanceOnly, Category = "Statistics")
	FSlateBrush StatisticsAttributeBrush;

	UPROPERTY(EditInstanceOnly, Category = "Statistics")
	FGameplayAttribute CurrentStatisticsAttribute;

	UPROPERTY(EditInstanceOnly, Category = "Statistics")
	FGameplayAttribute MaxStatisticsAttribute;

	UPROPERTY(EditDefaultsOnly, Category = "Statistics|ToolTip")
	TSubclassOf<class URaveUserWidget_StatisticsToolTip> StatisticsToolTipClass;
};
