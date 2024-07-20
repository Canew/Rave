

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "InputActionValue.h"
#include "RavePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API ARavePlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
public:
	ARavePlayerController();

	//~APlayerController interface
	virtual void PostInitializeComponents() override;
	virtual void SetupInputComponent() override;
	virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
protected:
	virtual void BeginPlay() override;
	//~End of APlayerController interface

public:
	UFUNCTION(BlueprintCallable, Category = "Rave|Ability")
	class URaveAbilitySystemComponent* GetRaveAbilitySystemComponent() const;

protected:
	void EscapeInputTriggered(const FInputActionValue& InputActionValue);
	void CharacterSelectInputTriggered(const FInputActionValue& InputActionValue);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TArray<TObjectPtr<class UInputMappingContext>> InputMappings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TArray<TObjectPtr<class URaveInputConfig>> InputConfigs;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TArray<TObjectPtr<class UInputMappingContext>> UserSettingsInputMappings;

#pragma region GenericTeamAgentInterface
public:
	/** Assigns Team Agent to given TeamID */
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamId) override;

	/** Retrieve team identifier in form of FGenericTeamId */
	virtual FGenericTeamId GetGenericTeamId() const override;

protected:
	UPROPERTY()
	FGenericTeamId TeamId;

#pragma endregion

#pragma region Widget
public:
	UFUNCTION(BlueprintPure, Category = "Rave|Widget")
	virtual UUserWidget* GetFocusWidget() const;

	UFUNCTION(BlueprintCallable, Category = "Rave|Widget")
	virtual UUserWidget* SetFocusWidget(TSubclassOf<UUserWidget> FocusWidgetClass);

	UFUNCTION(Client, Reliable)
	void ShowDamageDisplayWidget(AActor* TargetActor, float DamageAmount, FSlateColor DamageColor, FVector TargetLocation = FVector::ZeroVector);

	UFUNCTION(Client, Reliable)
	void ShowHitCircleWidget(const FVector& HitLocation);

protected:
	UPROPERTY()
	TObjectPtr<UUserWidget> FocusWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Widget|Class")
	TSubclassOf<class UUserWidget> EscapeMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widget|Class")
	TSubclassOf<class UUserWidget> DamageDisplayWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widget|Class")
	TSubclassOf<class UUserWidget> HitCircleWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widget|Class")
	TSubclassOf<class UUserWidget> CharacterSelectWidgetClass;

#pragma endregion


};
