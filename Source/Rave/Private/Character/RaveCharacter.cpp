


#include "Character/RaveCharacter.h"
#include "Character/RaveCharacterSetting.h"
#include "Character/RaveCharacterMovementComponent.h"
#include "Character/RaveCharacterTargetingComponent.h"
#include "RaveGameInstance.h"
#include "Animations/RaveAnimInstance.h"
#include "Player/RavePlayerState.h"
#include "Engine/LocalPlayer.h"
#include "Engine/StreamableManager.h"
#include "RaveGameMode_Gameplay.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/RaveInteractionComponent.h"
#include "Components/RaveAttributeComponent.h"
#include "Input/RaveEnhancedInputComponent.h"
#include "Input/RaveInputConfig.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayAbility/RaveAbilitySystemComponent.h"
#include "GameplayAbility/RaveAttributeSet.h"
#include "AI/RaveAIController.h"
#include "RaveGameplayTags.h"
#include "MotionWarpingComponent.h"
#include "Equipments/RaveEquipmentComponent.h"
#include "Equipments/RaveEquipment.h"
#include "UI/RaveUserWidget_HealthDisplay.h"
#include "CharacterTrajectoryComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveCharacter)

FName ARaveCharacter::SpringArmComponentName = TEXT("SpringArm");
FName ARaveCharacter::CameraComponentName = TEXT("Camera");
FName ARaveCharacter::AttributeComponentName = TEXT("Attribute");
FName ARaveCharacter::MotionWarpingComponentName = TEXT("MotionWarping");
FName ARaveCharacter::CharacterTrajectoryComponentName = TEXT("CharacterTrajectory");
FName ARaveCharacter::EquipmentComponentName = TEXT("Equipment");
FName ARaveCharacter::InteractionComponentName = TEXT("Interaction");
FName ARaveCharacter::CharacterTargetingComponentName = TEXT("CharacterTargeting");
FName ARaveCharacter::HealthDisplayWidgetComponentName = TEXT("HealthDisplay");

ARaveCharacter::ARaveCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<URaveCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	SetReplicates(true);

	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("RaveCharacterCapsule"));
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetCapsuleComponent()->SetCapsuleRadius(32.0f);
	GetCapsuleComponent()->SetIsReplicated(true);

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight()), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetCollisionProfileName(TEXT("RaveCharacterMesh"));
	GetMesh()->SetGenerateOverlapEvents(true);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->GravityScale = 1.5f;
	GetCharacterMovement()->MaxAcceleration = 1024.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1024.0f;
	GetCharacterMovement()->BrakingFrictionFactor = 0.5f;
	GetCharacterMovement()->bUseSeparateBrakingFriction = false;
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;
	GetCharacterMovement()->JumpZVelocity = 450.0f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 1000.0f, 0.0f);

	SpringArmComponent = CreateOptionalDefaultSubobject<USpringArmComponent>(SpringArmComponentName);
	SpringArmComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 80.0f));
	SpringArmComponent->SocketOffset = FVector(0.0f, 0.0f, 20.0f);
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->TargetArmLength = 300.0f;
	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->CameraLagSpeed = 10.0f;
	SpringArmComponent->bEnableCameraRotationLag = true;
	SpringArmComponent->CameraRotationLagSpeed = 20.0f;
	SpringArmComponent->SetupAttachment(GetRootComponent());

	CameraComponent = CreateOptionalDefaultSubobject<UCameraComponent>(CameraComponentName);
	CameraComponent->SetupAttachment(SpringArmComponent);

	AttributeComponent = CreateDefaultSubobject<URaveAttributeComponent>(TEXT("AttributeComponent"));

	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(MotionWarpingComponentName);

	CharacterTrajectoryComponent = CreateDefaultSubobject<UCharacterTrajectoryComponent>(CharacterTrajectoryComponentName);

	EquipmentComponent = CreateOptionalDefaultSubobject<URaveEquipmentComponent>(EquipmentComponentName);

	InteractionComponent = CreateDefaultSubobject<URaveInteractionComponent>(InteractionComponentName);

	CharacterTargetingComponent = CreateDefaultSubobject<URaveCharacterTargetingComponent>(CharacterTargetingComponentName);

	HealthDisplayWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthDisplay"));
	HealthDisplayWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HealthDisplayWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HealthDisplayWidgetComponent->SetDrawAtDesiredSize(true);
	HealthDisplayWidgetComponent->SetupAttachment(GetMesh(), TEXT("HealthWidget"));
	HealthDisplayWidgetComponent->SetVisibility(false);
	HealthDisplayWidgetComponent->SetIsReplicated(true);
	HealthDisplayWidgetComponent->SetGenerateOverlapEvents(false);

	AIControllerClass = ARaveAIController::StaticClass();
}

void ARaveCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	InitializeCharacterMesh();
}

void ARaveCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (CharacterSetting)
	{
		GetMesh()->GetAnimInstance()->OnMontageBlendingOut.AddDynamic(this, &ThisClass::HandleMontageBlendOut);
		MovementModeChangedDelegate.AddDynamic(this, &ThisClass::HandleMovementModeChanged);

		if (AttributeComponent)
		{
			AttributeComponent->OnHealthChanged.AddDynamic(this, &ThisClass::HandleHealthChanged);
			AttributeComponent->OnDeathStarted.AddDynamic(this, &ThisClass::HandleDeathStarted);
			AttributeComponent->OnDeathFinished.AddDynamic(this, &ThisClass::HandleDeathFinished);
		}
	}
}

void ARaveCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ARaveCharacter::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearAllTimersForObject(this);
}

void ARaveCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (ARavePlayerState* RavePlayerState = GetPlayerState<ARavePlayerState>())
	{
		if (URaveAbilitySystemComponent* AbilitySystemComponent = RavePlayerState->GetAbilitySystemComponent<URaveAbilitySystemComponent>())
		{
			AbilitySystemComponent->InitAbilityActorInfo(RavePlayerState, this);

			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(URaveAttributeSet::GetAttackSpeedAttribute()).AddUObject(this, &ThisClass::HandleAttackSpeedChanged);
			AbilitySystemComponent->RegisterGameplayTagEvent(RaveGameplayTags::Character_State_Evade).AddUObject(this, &ThisClass::HandleEvadeGameplayTagEvent);

			AttributeComponent->InitializeWithAbilitySystem(AbilitySystemComponent);
		}
	}

	InitializeHealthDisplayWidget();
}

void ARaveCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (ARavePlayerState* RavePlayerState = GetPlayerState<ARavePlayerState>())
	{
		if (URaveAbilitySystemComponent* AbilitySystemComponent = RavePlayerState->GetAbilitySystemComponent<URaveAbilitySystemComponent>())
		{
			AbilitySystemComponent->InitAbilityActorInfo(RavePlayerState, this);

			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(URaveAttributeSet::GetAttackSpeedAttribute()).AddUObject(this, &ThisClass::HandleAttackSpeedChanged);
			AbilitySystemComponent->RegisterGameplayTagEvent(RaveGameplayTags::Character_State_Evade).AddUObject(this, &ThisClass::HandleEvadeGameplayTagEvent);

			AttributeComponent->InitializeWithAbilitySystem(AbilitySystemComponent);
		}

		if (CharacterSetting)
		{
			RavePlayerState->SetCharacterSetting(CharacterSetting);
		}
	}

	InitializeHealthDisplayWidget();
}

void ARaveCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARaveCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	const APlayerController* PlayerController = GetController<APlayerController>();
	check(PlayerController);

	const ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PlayerController->GetLocalPlayer());
	check(LocalPlayer);

	UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(EnhancedInputSubsystem);

	if (CharacterSetting)
	{
		if (const URaveInputConfig* InputConfig = CharacterSetting->InputConfig)
		{
			for (const UInputMappingContext* InputMappingContext : InputMappings)
			{
				FModifyContextOptions Options = {};
				Options.bIgnoreAllPressedKeysUntilRelease = false;
				EnhancedInputSubsystem->AddMappingContext(InputMappingContext, 0, Options);
			}

			if (URaveEnhancedInputComponent* RaveEnhancedInputComponent = Cast<URaveEnhancedInputComponent>(PlayerInputComponent))
			{
				RaveEnhancedInputComponent->AddInputMappings(InputConfig, EnhancedInputSubsystem);

				TArray<uint32> BindHandles;
				RaveEnhancedInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputPressed, &ThisClass::AbilityInputReleased, BindHandles);

				RaveEnhancedInputComponent->BindNativeAction(InputConfig, RaveGameplayTags::Input_Move, ETriggerEvent::Triggered, this, &ThisClass::MoveInputTriggered);
				RaveEnhancedInputComponent->BindNativeAction(InputConfig, RaveGameplayTags::Input_Move, ETriggerEvent::Completed, this, &ThisClass::MoveInputCompleted);
				RaveEnhancedInputComponent->BindNativeAction(InputConfig, RaveGameplayTags::Input_Look, ETriggerEvent::Triggered, this, &ThisClass::LookInputTriggered);
			}
		}
	}
}

void ARaveCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
	if (AbilitySystemComponent)
	{
		FGameplayEventData LandGameplayEventData;
		LandGameplayEventData.EventMagnitude = GetCharacterMovement()->Velocity.Z;
		LandGameplayEventData.ContextHandle.AddHitResult(Hit);
		AbilitySystemComponent->HandleGameplayEvent(RaveGameplayTags::GameplayEvent_Land, &LandGameplayEventData);
	}
}

UAbilitySystemComponent* ARaveCharacter::GetAbilitySystemComponent() const
{
	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(GetPlayerState());
	if (AbilitySystemInterface)
	{
		return AbilitySystemInterface->GetAbilitySystemComponent();
	}

	return nullptr;
}

const URaveCharacterSetting* ARaveCharacter::GetCharacterSetting() const
{
	return CharacterSetting;
}

void ARaveCharacter::SetCharacterSetting(const URaveCharacterSetting* NewCharacterSetting)
{
	if (!NewCharacterSetting)
	{
		return;
	}

	CharacterSetting = NewCharacterSetting;

	InitializeCharacterMesh();
	InitializeCharacterEquipment();
}

void ARaveCharacter::InitializeCharacterMesh()
{
	if (CharacterSetting)
	{
		if (IsValid(CharacterSetting->CharacterMesh.Get()))
		{
			GetMesh()->SetSkeletalMesh(CharacterSetting->CharacterMesh.Get());
		}
		else if (!GIsEditor || GIsPlayInEditorWorld)
		{
			URaveGameInstance* RaveGameInstance = GetGameInstance<URaveGameInstance>();
			check(RaveGameInstance);

			MeshStreamableHandle = RaveGameInstance->StreamableManager.RequestAsyncLoad(CharacterSetting->CharacterMesh.ToSoftObjectPath(), FStreamableDelegate::CreateLambda(
				[this]() -> void
				{
					if (MeshStreamableHandle.IsValid())
					{
						USkeletalMesh* LoadedMeshAsset = Cast<USkeletalMesh>(MeshStreamableHandle->GetLoadedAsset());
						if (LoadedMeshAsset)
						{
							GetMesh()->SetSkeletalMesh(LoadedMeshAsset);
						}

						MeshStreamableHandle.Reset();
					}
				}));
		}

		GetMesh()->SetAnimInstanceClass(CharacterSetting->CharacterAnimInstanceClass);
	}
	else
	{
		GetMesh()->SetSkeletalMesh(nullptr);
		GetMesh()->SetAnimInstanceClass(nullptr);
	}
}

void ARaveCharacter::InitializeCharacterEquipment()
{
	if (CharacterSetting)
	{
		EquipmentComponent->EquipEquipment(CharacterSetting->EquipmentDefinition);
	}
}

void ARaveCharacter::MoveInputTriggered(const FInputActionValue& InputActionValue)
{
	AController* ControllerLocal = GetController();
	if (ControllerLocal)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator ControlRotation(0.0f, ControllerLocal->GetControlRotation().Yaw, 0.0f);

		if (Value.X != 0.0f)
		{
			const FVector MovementDirection = FRotationMatrix(ControlRotation).GetUnitAxis(EAxis::Y);
			AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			const FVector MovementDirection = FRotationMatrix(ControlRotation).GetUnitAxis(EAxis::X);
			AddMovementInput(MovementDirection, Value.Y);
		}

		if (IsValid(CharacterTargetingComponent))
		{
			CharacterTargetingComponent->UpdateDesiredOrientation(ControlInputVector);
			if (GetLocalRole() == ROLE_AutonomousProxy)
			{
				CharacterTargetingComponent->ServerUpdateDesiredOrientation(ControlInputVector);
			}
		}
	}
}

void ARaveCharacter::MoveInputCompleted(const FInputActionValue& InputActionValue)
{
	if (IsValid(CharacterTargetingComponent))
	{
		CharacterTargetingComponent->UpdateDesiredOrientation(ControlInputVector);
		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			CharacterTargetingComponent->ServerUpdateDesiredOrientation(ControlInputVector);
		}
	}
}

void ARaveCharacter::LookInputTriggered(const FInputActionValue& InputActionValue)
{
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	if (CharacterTargetingComponent && IsValid(CharacterTargetingComponent->GetLockOnActor()))
	{
		return;
	}

	if (Value.X != 0.0f)
	{
		AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		AddControllerPitchInput(Value.Y);
	}
}

void ARaveCharacter::HandleMontageBlendOut(UAnimMontage* Montage, bool bInterrupted)
{
	FVector NewVelocity = GetCharacterMovement()->Velocity;
	float CurrentSpeed2D = GetCharacterMovement()->Velocity.Size2D();
	CurrentSpeed2D = FMath::Clamp(CurrentSpeed2D, 0.0f, GetCharacterMovement()->MaxWalkSpeed);
	NewVelocity.Z = 0.0f;
	NewVelocity.Normalize();
	NewVelocity *= CurrentSpeed2D;

	NewVelocity.Z = GetCharacterMovement()->Velocity.Z;
	GetCharacterMovement()->Velocity = NewVelocity;
}

void ARaveCharacter::HandleMovementModeChanged(ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Walking)
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetMesh()->GetAnimInstance()->SetRootMotionMode(ERootMotionMode::RootMotionFromMontagesOnly);
	}
	else if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Falling)
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetMesh()->GetAnimInstance()->SetRootMotionMode(ERootMotionMode::IgnoreRootMotion);
	}
	else if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Flying)
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->GetAnimInstance()->SetRootMotionMode(ERootMotionMode::RootMotionFromMontagesOnly);
	}

	FVector NewVelocity = GetCharacterMovement()->Velocity;
	float CurrentSpeed2D = GetCharacterMovement()->Velocity.Size2D();
	CurrentSpeed2D = FMath::Clamp(CurrentSpeed2D, 0.0f, GetCharacterMovement()->MaxWalkSpeed);
	NewVelocity.Z = 0.0f;
	NewVelocity.Normalize();
	NewVelocity *= CurrentSpeed2D;

	NewVelocity.Z = GetCharacterMovement()->Velocity.Z;
	GetCharacterMovement()->Velocity = NewVelocity;
}

void ARaveCharacter::HandleHealthChanged(float OldValue, float NewValue, AActor* DamageInstigator)
{
	if (OldValue > NewValue)
	{
		URaveAnimInstance* AnimInstance = Cast<URaveAnimInstance>(GetMesh()->GetAnimInstance());
		if (AnimInstance)
		{
			AnimInstance->PlayHitReact();
		}
	}
}

void ARaveCharacter::HandleDeathStarted(AActor* OwningActor)
{
	if (Controller)
	{
		Controller->SetIgnoreMoveInput(true);
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);

	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetAllBodiesSimulatePhysics(true);

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	ARaveAIController* AIController = GetController<ARaveAIController>();
	if (AIController)
	{
		AIController->StopAI();
	}

	FTimerHandle FinishDeathTimerHandle;
	float FinishDeathTime = 1.0f;
	GetWorldTimerManager().SetTimer(FinishDeathTimerHandle, FTimerDelegate::CreateLambda(
		[this]() -> void
		{
			AttributeComponent->FinishDeath();
		}), FinishDeathTime, false);
}

void ARaveCharacter::HandleDeathFinished(AActor* OwningActor)
{
	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
	URaveAbilitySystemComponent* RaveAbilitySystemComponent = GetAbilitySystemComponent<URaveAbilitySystemComponent>();
	if (RaveAbilitySystemComponent && RaveAbilitySystemComponent->GetAvatarActor() == this)
	{
		// Distribute experience to all player
		if (HasAuthority() && !IsPlayerControlled())
		{
			UWorld* World = GetWorld();
			if (World)
			{
				ARaveGameMode_Gameplay* GameplayGameMode = World->GetAuthGameMode<ARaveGameMode_Gameplay>();
				if (GameplayGameMode)
				{
					bool bExperienceAttributeFound = false;
					float ExperienceValue = RaveAbilitySystemComponent->GetGameplayAttributeValue(URaveAttributeSet::GetExperienceAttribute(), bExperienceAttributeFound);
					if (bExperienceAttributeFound)
					{
						GameplayGameMode->DistributeExperience(ExperienceValue);
					}
				}
			}
		}

		// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
		if (RaveAbilitySystemComponent->GetAvatarActor() == GetOwner())
		{
			RaveAbilitySystemComponent->CancelAbilities();
			RaveAbilitySystemComponent->ClearAbilityInput();
			RaveAbilitySystemComponent->RemoveAllGameplayCues();

			if (RaveAbilitySystemComponent->GetOwnerActor())
			{
				RaveAbilitySystemComponent->SetAvatarActor(nullptr);
			}
			else
			{
				// If the ASC doesn't have a valid owner, we need to clear *all* actor info, not just the avatar pairing
				RaveAbilitySystemComponent->ClearActorInfo();
			}
		}
	}

	if (HasAuthority())
	{
		if (EquipmentComponent)
		{
			if (ARaveEquipment* Equipment = EquipmentComponent->GetEquipment())
			{
				Equipment->Destroy();
			}
		}

		DetachFromControllerPendingDestroy();
		SetLifeSpan(1.0f);
		SetActorHiddenInGame(true);
	}
}

void ARaveCharacter::HandleEvadeGameplayTagEvent(const FGameplayTag GameplayTag, int32 Count)
{
	if (GameplayTag == RaveGameplayTags::Character_State_Evade)
	{
		if (Count > 0)
		{
			GetMesh()->SetGenerateOverlapEvents(false);
			GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
		}
		else
		{
			GetMesh()->SetGenerateOverlapEvents(true);
			GetMesh()->SetCollisionProfileName(TEXT("RaveCharacterMesh"));
		}
	}
}

void ARaveCharacter::HandleAttackSpeedChanged(const FOnAttributeChangeData& Data)
{
	float NewAttackSpeed = Data.NewValue;
	NewAttackSpeed = FMath::Clamp(NewAttackSpeed, 0.0f, URaveAttributeSet::MAX_ATTACK_SPEED);
	
	GetMesh()->GetAnimInstance()->Montage_SetPlayRate(GetMesh()->GetAnimInstance()->GetCurrentActiveMontage(), NewAttackSpeed);
}

void ARaveCharacter::AbilityInputPressed(FGameplayTag InputTag)
{
	if (URaveAbilitySystemComponent* RaveASC = GetAbilitySystemComponent<URaveAbilitySystemComponent>())
	{
		RaveASC->AbilityInputTagPressed(InputTag);
	}
}

void ARaveCharacter::AbilityInputReleased(FGameplayTag InputTag)
{
	if (URaveAbilitySystemComponent* RaveASC = GetAbilitySystemComponent<URaveAbilitySystemComponent>())
	{
		RaveASC->AbilityInputTagReleased(InputTag);
	}
}

FGenericTeamId ARaveCharacter::GetGenericTeamId() const
{
	if (IGenericTeamAgentInterface* GenericTeamAgentInterface = Cast<IGenericTeamAgentInterface>(GetController()))
	{
		return GenericTeamAgentInterface->GetGenericTeamId();
	}

	return FGenericTeamId::NoTeam;
}

void ARaveCharacter::FlickHitFeedbackMaterial()
{
	if (HitFeedbackMaterial)
	{
		GetMesh()->SetOverlayMaterial(HitFeedbackMaterial);

		FTimerDelegate FlickHitFeedbackMaterialDelegate = FTimerDelegate::CreateLambda([this]() -> void
			{
				if (IsValid(this) && IsValid(GetMesh()))
				{
					GetMesh()->SetOverlayMaterial(nullptr);
				}
			});

		GetWorldTimerManager().SetTimer(FlickHitFeedbackMaterialTimerHandle, FlickHitFeedbackMaterialDelegate, FlickHitFeedbackMaterialTime, false);
	}
	else
	{
		URaveGameInstance* RaveGameInstance = GetGameInstance<URaveGameInstance>();
		check(RaveGameInstance);

		
		HitFeedbackMaterialStreamableHandle = RaveGameInstance->StreamableManager.RequestAsyncLoad(HitFeedbackMaterialPath.ToSoftObjectPath(), FStreamableDelegate::CreateLambda(
			[this]() -> void
			{
				if (HitFeedbackMaterialStreamableHandle.IsValid())
				{
					UMaterialInterface* LoadedMaterialInterface = Cast<UMaterialInterface>(HitFeedbackMaterialStreamableHandle->GetLoadedAsset());
					if (LoadedMaterialInterface)
					{
						HitFeedbackMaterial = LoadedMaterialInterface;
					}

					HitFeedbackMaterialStreamableHandle.Reset();
				}
			}));
	}
}

void ARaveCharacter::ShowHealthDisplayWidgetClient_Implementation(ARaveCharacter* TargetCharacter)
{
	TargetCharacter->ShowHealthDisplayWidget();
}

void ARaveCharacter::ShowHealthDisplayWidget()
{
	if (!AttributeComponent->IsAlive())
	{
		return;
	}

	if (HealthDisplayWidgetComponent)
	{
		HealthDisplayWidgetComponent->SetVisibility(true);
		GetWorldTimerManager().ClearTimer(ShowHealthWidgetTimerHandle);
		GetWorldTimerManager().SetTimer(ShowHealthWidgetTimerHandle, FTimerDelegate::CreateLambda(
			[this]() -> void
			{
				if (this && HealthDisplayWidgetComponent)
				{
					HealthDisplayWidgetComponent->SetVisibility(false);
				}
			}), HealthDisplayWidgetDuration, false);
	}
}

void ARaveCharacter::InitializeHealthDisplayWidget()
{
	if (HealthDisplayWidgetComponent)
	{
		if (GetMesh()->DoesSocketExist(HealthDisplayWidgetAttachSocketName))
		{
			HealthDisplayWidgetComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, HealthDisplayWidgetAttachSocketName);
		}

		HealthDisplayWidgetComponent->InitWidget();
		if (URaveUserWidget_HealthDisplay* HealthDisplayWidget = Cast<URaveUserWidget_HealthDisplay>(HealthDisplayWidgetComponent->GetUserWidgetObject()))
		{
			UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
			if (AbilitySystemComponent)
			{
				HealthDisplayWidget->InitializeHealth(GetAbilitySystemComponent()->GetAttributeSet(URaveAttributeSet::StaticClass()));
				HealthDisplayWidgetDuration = 4.0f;
			}
		}
	}
}
