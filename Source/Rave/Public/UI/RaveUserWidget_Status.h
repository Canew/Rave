

#pragma once

#include "CoreMinimal.h"
#include "UI/RaveUserWidget.h"
#include "GameplayEffectTypes.h"
#include "RaveUserWidget_Status.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveUserWidget_Status : public URaveUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void InitializeStatus();

	virtual void UpdateHealth(const FOnAttributeChangeData& Data = FOnAttributeChangeData());
	virtual void UpdateMana(const FOnAttributeChangeData& Data = FOnAttributeChangeData());
	virtual void UpdateExperience(const FOnAttributeChangeData& Data = FOnAttributeChangeData());

protected:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class UProgressBar> HealthProgressBar;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class UTextBlock> HealthTextBlock;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class UProgressBar> ManaProgressBar;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class UTextBlock> ManaTextBlock;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class UProgressBar> ExperienceProgressBar;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class UTextBlock> ExperienceTextBlock;
};
