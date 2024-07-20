

#pragma once

#include "CoreMinimal.h"
#include "UI/RaveUserWidget.h"
#include "GameplayEffectTypes.h"
#include "RaveUserWidget_HealthDisplay.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveUserWidget_HealthDisplay : public URaveUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable)
	virtual void InitializeHealth(const class UAttributeSet* InTargetAttributeSet);

protected:
	virtual void HandleHealthChanged(const FOnAttributeChangeData& Data);

protected:
	const class UAttributeSet* AttributeSet;

	UPROPERTY(Meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* HealthTextBlock;
};
