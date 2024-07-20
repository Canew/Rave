


#include "Player/RavePlayerController.h"
#include "Engine/LocalPlayer.h"
#include "Input/RaveEnhancedInputComponent.h"
#include "Input/RaveInputConfig.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayAbility/RaveAbilitySystemComponent.h"
#include "Player/RavePlayerState.h"
#include "RaveGameplayTags.h"
#include "Components/WidgetComponent.h"
#include "UI/RaveUserWidget_DamageDisplay.h"
#include "UI/RaveUserWidget_HitCircle.h"
#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RavePlayerController)

#define PLAYER_TEAM 1

ARavePlayerController::ARavePlayerController()
{
	bAttachToPawn = true;

	SetGenericTeamId(PLAYER_TEAM);
}

void ARavePlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ARavePlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (PlayerCameraManager)
	{
		PlayerCameraManager->ViewPitchMin = -65.0f;
		PlayerCameraManager->ViewPitchMax = 60.0f;
	}
}

void ARavePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	const ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(GetLocalPlayer());
	check(LocalPlayer);

	UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(EnhancedInputLocalSubsystem);

	for (const UInputMappingContext* InputMappingContext : InputMappings)
	{
		FModifyContextOptions Options = {};
		Options.bIgnoreAllPressedKeysUntilRelease = false;
		EnhancedInputLocalSubsystem->AddMappingContext(InputMappingContext, 0, Options);
	}

	if (URaveEnhancedInputComponent* RaveEnhancedInputComponent = Cast<URaveEnhancedInputComponent>(InputComponent))
	{
		for (const URaveInputConfig* InputConfig : InputConfigs)
		{
			RaveEnhancedInputComponent->BindNativeAction(InputConfig, RaveGameplayTags::Input_Escape, ETriggerEvent::Triggered, this, &ThisClass::EscapeInputTriggered);
			RaveEnhancedInputComponent->BindNativeAction(InputConfig, RaveGameplayTags::Input_CharacterSelect, ETriggerEvent::Triggered, this, &ThisClass::CharacterSelectInputTriggered);
		}
	}

	// Register input mapping to user settings
	if (UEnhancedInputUserSettings* EnhancedInputUserSettings = EnhancedInputLocalSubsystem->GetUserSettings())
	{
		for (UInputMappingContext* UserSettingsInputMapping : UserSettingsInputMappings)
		{
			if (!EnhancedInputUserSettings->IsMappingContextRegistered(UserSettingsInputMapping))
			{
				EnhancedInputUserSettings->RegisterInputMappingContext(UserSettingsInputMapping);
			}
		}
	}

	// Initialize input user settings
	UEnhancedInputUserSettings* InputUserSettings = EnhancedInputLocalSubsystem->GetUserSettings();
	if (InputUserSettings)
	{
		if (UEnhancedPlayerMappableKeyProfile* CurrentPlayerKeyProfile = InputUserSettings->GetCurrentKeyProfile())
		{
			for (const TPair<FName, FKeyMappingRow>& PlayerMappingRow : CurrentPlayerKeyProfile->GetPlayerMappingRows())
			{
				FKeyMappingRow KeyMappingRow = PlayerMappingRow.Value;
				for (const FPlayerKeyMapping& PlayerKeyMapping : KeyMappingRow.Mappings.Array())
				{
					if (PlayerKeyMapping.GetSlot() == EPlayerMappableKeySlot::First)
					{
						FMapPlayerKeyArgs MapPlayerKeyArgs;
						MapPlayerKeyArgs.MappingName = PlayerKeyMapping.GetMappingName();
						MapPlayerKeyArgs.NewKey = PlayerKeyMapping.GetCurrentKey();
						MapPlayerKeyArgs.Slot = EPlayerMappableKeySlot::First;
						FGameplayTagContainer FailureReasonTags;

						InputUserSettings->MapPlayerKey(MapPlayerKeyArgs, FailureReasonTags);
					}
				}
			}
		}

		InputUserSettings->ApplySettings();
		InputUserSettings->SaveSettings();
	}
}

void ARavePlayerController::PreProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PreProcessInput(DeltaTime, bGamePaused);
}

void ARavePlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (URaveAbilitySystemComponent* RaveAbilitySystemComponent = GetRaveAbilitySystemComponent())
	{
		RaveAbilitySystemComponent->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

URaveAbilitySystemComponent* ARavePlayerController::GetRaveAbilitySystemComponent() const
{
	const ARavePlayerState* RavePlayerState = GetPlayerState<ARavePlayerState>();

	return (RavePlayerState ? RavePlayerState->GetAbilitySystemComponent<URaveAbilitySystemComponent>() : nullptr);
}

void ARavePlayerController::EscapeInputTriggered(const FInputActionValue& InputActionValue)
{
	const bool Value = InputActionValue.Get<bool>();
	if (Value)
	{
		if (IsValid(FocusWidget))
		{
			FocusWidget->RemoveFromParent();
			FocusWidget = nullptr;

			FInputModeGameOnly InputModeGameOnly;
			SetInputMode(InputModeGameOnly);
			SetShowMouseCursor(false);
		}
		else
		{
			SetFocusWidget(EscapeMenuWidgetClass);
		}
	}
}

void ARavePlayerController::CharacterSelectInputTriggered(const FInputActionValue& InputActionValue)
{
	const bool Value = InputActionValue.Get<bool>();
	if (Value)
	{
		SetFocusWidget(CharacterSelectWidgetClass);
	}
}

void ARavePlayerController::SetGenericTeamId(const FGenericTeamId& NewTeamId)
{
	if (TeamId != NewTeamId)
	{
		TeamId = NewTeamId;
	}
}

FGenericTeamId ARavePlayerController::GetGenericTeamId() const
{
	return TeamId;
}

UUserWidget* ARavePlayerController::GetFocusWidget() const
{
	return FocusWidget;
}

UUserWidget* ARavePlayerController::SetFocusWidget(TSubclassOf<UUserWidget> FocusWidgetClass)
{
	if (FocusWidgetClass)
	{
		if (IsValid(FocusWidget))
		{
			if (FocusWidget->GetClass() != FocusWidgetClass)
			{
				FocusWidget->RemoveFromParent();
				FocusWidget = CreateWidget(this, FocusWidgetClass);
				if (IsValid(FocusWidget))
				{
					FocusWidget->AddToViewport();

					FInputModeGameAndUI InputModeGameAndUI;
					InputModeGameAndUI.SetWidgetToFocus(FocusWidget->TakeWidget());
					SetInputMode(InputModeGameAndUI);
					SetShowMouseCursor(true);

					return FocusWidget;
				}
			}
		}
		else
		{
			FocusWidget = CreateWidget(this, FocusWidgetClass);
			if (IsValid(FocusWidget))
			{
				FocusWidget->AddToViewport();

				FInputModeGameAndUI InputModeGameAndUI;
				InputModeGameAndUI.SetWidgetToFocus(FocusWidget->TakeWidget());
				SetInputMode(InputModeGameAndUI);
				SetShowMouseCursor(true);

				int32 ViewportSizeX;
				int32 ViewportSizeY;
				GetViewportSize(ViewportSizeX, ViewportSizeY);
				SetMouseLocation(ViewportSizeX / 2.0f, ViewportSizeY / 2.0f);

				return FocusWidget;
			}
		}
	}

	if (IsValid(FocusWidget))
	{
		FocusWidget->RemoveFromParent();
		FocusWidget = nullptr;
	}

	FInputModeGameOnly InputModeGameOnly;
	SetInputMode(InputModeGameOnly);
	SetShowMouseCursor(false);

	return nullptr;
}

void ARavePlayerController::ShowDamageDisplayWidget_Implementation(AActor* TargetActor, float DamageAmount, FSlateColor DamageColor, FVector TargetLocation)
{
	if (!DamageDisplayWidgetClass)
	{
		return;
	}

	if (TargetActor == nullptr)
	{
		return;
	}

	UWidgetComponent* DamageDisplayWidgetComponent = NewObject<UWidgetComponent>(TargetActor);
	URaveUserWidget_DamageDisplay* DamageDisplayWidget = CreateWidget<URaveUserWidget_DamageDisplay>(this, DamageDisplayWidgetClass);
	if (DamageDisplayWidgetComponent && DamageDisplayWidget)
	{
		DamageDisplayWidgetComponent->RegisterComponent();
		DamageDisplayWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		DamageDisplayWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
		DamageDisplayWidgetComponent->SetWidget(DamageDisplayWidget);
		DamageDisplayWidgetComponent->SetDrawAtDesiredSize(true);
		DamageDisplayWidgetComponent->SetIsReplicated(true);

		DamageDisplayWidget->SetDamageAmount(DamageAmount);
		DamageDisplayWidget->SetDamageColor(DamageColor);
		DamageDisplayWidget->SetOwnerWidgetComponent(DamageDisplayWidgetComponent);

		if (TargetLocation.IsNearlyZero())
		{
			DamageDisplayWidgetComponent->SetWorldLocation(TargetActor->GetActorLocation());
		}
		else
		{
			DamageDisplayWidgetComponent->SetWorldLocation(TargetLocation);
		}

		float DistanceSqrt = FMath::Sqrt((GetPawn()->GetActorLocation() - TargetActor->GetActorLocation()).Size());
		float HorizontalWeight = 2.5f;
		float VerticalWeight = 10.0f;
		FVector DamageWidgetLocationOffset = DistanceSqrt * FMath::FRandRange(-HorizontalWeight, HorizontalWeight) * GetPawn()->GetActorRightVector() + DistanceSqrt * FMath::FRandRange(VerticalWeight - HorizontalWeight, VerticalWeight) * GetPawn()->GetActorUpVector();
		DamageDisplayWidgetComponent->SetWorldLocation(DamageDisplayWidgetComponent->GetComponentLocation() + DamageWidgetLocationOffset);
	}
}

void ARavePlayerController::ShowHitCircleWidget_Implementation(const FVector& HitLocation)
{
	if (HitCircleWidgetClass)
	{
		if (URaveUserWidget_HitCircle* HitCircleWidget = CreateWidget<URaveUserWidget_HitCircle>(this, HitCircleWidgetClass))
		{
			HitCircleWidget->SetHitLocation(HitLocation);
			HitCircleWidget->AddToViewport();
		}
	}
}
