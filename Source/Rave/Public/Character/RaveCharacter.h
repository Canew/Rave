

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "InputActionValue.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "GameplayEffectTypes.h"
#include "RaveCharacter.generated.h"

UCLASS(Config = "Game")
class RAVE_API ARaveCharacter : public ACharacter, public IAbilitySystemInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	ARaveCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void OnConstruction(const FTransform& Transform);
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	virtual void OnRep_PlayerState() override;

public:
	virtual void PossessedBy(AController* NewController) override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Landed(const FHitResult& Hit) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	template <typename T>
	T* GetAbilitySystemComponent() const
	{
		return Cast<T>(GetAbilitySystemComponent());
	}

	const class URaveCharacterSetting* GetCharacterSetting() const;
	void SetCharacterSetting(const class URaveCharacterSetting* NewCharacterSetting);

protected:
	void InitializeCharacterMesh();
	void InitializeCharacterEquipment();

	void MoveInputTriggered(const FInputActionValue& InputActionValue);
	void MoveInputCompleted(const FInputActionValue& InputActionValue);

	void LookInputTriggered(const FInputActionValue& InputActionValue);

	UFUNCTION()
	void HandleMontageBlendOut(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	void HandleMovementModeChanged(ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode);

	UFUNCTION()
	virtual void HandleHealthChanged(float OldValue, float NewValue, AActor* DamageInstigator);

	// Begins the death sequence for the character (disables collision, disables movement, etc...)
	UFUNCTION()
	virtual void HandleDeathStarted(AActor* OwningActor);

	// Ends the death sequence for the character (detaches controller, destroys pawn, etc...)
	UFUNCTION()
	virtual void HandleDeathFinished(AActor* OwningActor);

	void HandleEvadeGameplayTagEvent(const FGameplayTag GameplayTag, int32 Count);
	void HandleAttackSpeedChanged(const FOnAttributeChangeData& Data);

	void AbilityInputPressed(FGameplayTag InputTag);
	void AbilityInputReleased(FGameplayTag InputTag);

	/** Retrieve team identifier in form of FGenericTeamId */
	virtual FGenericTeamId GetGenericTeamId() const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> SpringArmComponent;
	static FName SpringArmComponentName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> CameraComponent;
	static FName CameraComponentName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class URaveAttributeComponent> AttributeComponent;
	static FName AttributeComponentName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UMotionWarpingComponent> MotionWarpingComponent;
	static FName MotionWarpingComponentName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCharacterTrajectoryComponent> CharacterTrajectoryComponent;
	static FName CharacterTrajectoryComponentName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class URaveEquipmentComponent> EquipmentComponent;
	static FName EquipmentComponentName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class URaveInteractionComponent* InteractionComponent;
	static FName InteractionComponentName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class URaveCharacterTargetingComponent* CharacterTargetingComponent;
	static FName CharacterTargetingComponentName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TArray<TObjectPtr<class UInputMappingContext>> InputMappings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setting")
	TObjectPtr<const class URaveCharacterSetting> CharacterSetting;

	TSharedPtr<struct FStreamableHandle> MeshStreamableHandle;

#pragma region Hit Feedback
public:
	virtual void FlickHitFeedbackMaterial();

protected:
	UPROPERTY()
	TObjectPtr<UMaterialInterface> HitFeedbackMaterial;

	UPROPERTY(Config)
	TSoftObjectPtr<UMaterialInterface> HitFeedbackMaterialPath;

	UPROPERTY(Config)
	float FlickHitFeedbackMaterialTime = 0.025f;

	FTimerHandle FlickHitFeedbackMaterialTimerHandle;
	TSharedPtr<FStreamableHandle> HitFeedbackMaterialStreamableHandle;

#pragma endregion

#pragma region HealthDisplay
public:
	UFUNCTION(Client, Reliable)
	virtual void ShowHealthDisplayWidgetClient(ARaveCharacter* TargetCharacter);
	virtual void ShowHealthDisplayWidget();

protected:
	virtual void InitializeHealthDisplayWidget();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	class UWidgetComponent* HealthDisplayWidgetComponent;
	static FName HealthDisplayWidgetComponentName;

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	FName HealthDisplayWidgetAttachSocketName = TEXT("HealthDisplay");

	FTimerHandle ShowHealthWidgetTimerHandle;
	float HealthDisplayWidgetDuration;

#pragma endregion
};
