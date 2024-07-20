

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RaveUserWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFadeInAnimationFinishedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFadeOutAnimationFinishedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDestructedDelegate);

/**
 * 
 */
UCLASS()
class RAVE_API URaveUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	URaveUserWidget(const FObjectInitializer& ObjectInitializer);

	virtual void RemoveFromParent() override;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent);
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnKeyPressed(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) { return FReply::Unhandled(); };

public:
	UFUNCTION(BlueprintCallable)
	virtual void PlayFadeInAnimation();
	UFUNCTION(BlueprintCallable)
	virtual void PlayFadeOutAnimation(bool bRemoveWhenCompleted = false);

protected:
	UFUNCTION()
	virtual void HandleFadeInAnimationFinished();
	UFUNCTION()
	virtual void HandleFadeOutAnimationFinished();

public:
	UPROPERTY(BlueprintAssignable)
	FOnDestructedDelegate OnDestructed;

	UPROPERTY(BlueprintAssignable)
	FOnFadeInAnimationFinishedDelegate OnFadeInAnimationFinished;

	UPROPERTY(BlueprintAssignable)
	FOnFadeOutAnimationFinishedDelegate OnFadeOutAnimationFinished;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Rave|Dialog")
	TSubclassOf<URaveUserWidget> DialogWidgetClass;

	UPROPERTY(Transient, Meta = (BindWidgetAnimOptional))
	TObjectPtr<class UWidgetAnimation> FadeInAnimation;

	UPROPERTY(Transient, Meta = (BindWidgetAnimOptional))
	TObjectPtr<class UWidgetAnimation> FadeOutAnimation;

	TMap<FKey, bool> KeyDownMap;

	bool bRemoveWhenFadeOutCompleted;
};
