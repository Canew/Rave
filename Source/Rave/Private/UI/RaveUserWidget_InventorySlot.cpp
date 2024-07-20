


#include "UI/RaveUserWidget_InventorySlot.h"
#include "UI/RaveUserWidget_InventoryToolTip.h"
#include "UI/RaveDragDropOperation_InventorySlot.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "AbilitySystemGlobals.h"
#include "GameplayAbility/RaveAbilitySystemComponent.h"
#include "GameplayAbility/RaveGameplayAbility.h"
#include "GameplayAbility/RaveAttributeSet.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "RaveGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "Character/RaveCharacter.h"
#include "Character/RaveCharacterSetting.h"
#include "Input/RaveInputConfig.h"
#include "Player/RavePlayerState.h"
#include "Inventory/RaveGameplayItem.h"
#include "Inventory/RaveInventorySystemComponent.h"

URaveUserWidget_InventorySlot::URaveUserWidget_InventorySlot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCanClick = false;
	InventoryIndex = INDEX_NONE;
}

void URaveUserWidget_InventorySlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	InventorySlotBorder->BackgroundDelegate.BindDynamic(this, &ThisClass::GetInventoryBorderBackground);
	InventorySlotBorder->ToolTipWidgetDelegate.BindDynamic(this, &ThisClass::GetInventoryToolTip);
	InventorySlotBorder->SetBrushColor(FLinearColor(0.7f, 0.7f, 0.7f, 1.0f));

	StackTextBlock->VisibilityDelegate.BindDynamic(this, &ThisClass::GetStackTextBlockVisibility);
	StackTextBlock->TextDelegate.BindDynamic(this, &ThisClass::GetStackTextBlockText);
}

void URaveUserWidget_InventorySlot::NativeConstruct()
{
	Super::NativeConstruct();

	ARavePlayerState* PlayerState = GetOwningPlayerState<ARavePlayerState>();
	if (PlayerState)
	{
		InventorySystemComponent = PlayerState->GetInventorySystemComponent();
	}

	if (!IsValid(InventorySystemComponent.Get()))
	{
		RemoveFromParent();
	}
}

FReply URaveUserWidget_InventorySlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (IsHovered())
	{
		InventorySlotBorder->SetBrushColor(FLinearColor(0.4f, 0.4f, 0.4f, 1.0f));

		if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
		{
			bCanClick = true;

			switch (InventorySlotType)
			{
			case ERaveInventorySlotType::Skill:
				break;

			case ERaveInventorySlotType::Module:
			case ERaveInventorySlotType::Item:
				return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
			}
		}
		else if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
		{
			switch (InventorySlotType)
			{
			case ERaveInventorySlotType::Skill:
				break;

			case ERaveInventorySlotType::Module:
				InventorySystemComponent->UnequipModule(InventorySlotInputTag);
				break;

			case ERaveInventorySlotType::Item:
				InventorySystemComponent->EquipModule(InventoryIndex);
				break;
			}
		}
	}

	return FReply::Handled();
}

FReply URaveUserWidget_InventorySlot::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);

	if (IsHovered())
	{
		InventorySlotBorder->SetBrushColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));

		if (bCanClick)
		{
			if (InventorySlotType == ERaveInventorySlotType::Skill)
			{
				if (URaveAbilitySystemComponent* AbilitySystemComponent = Cast<URaveAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwningPlayerPawn())))
				{
					TArray<FGameplayAbilitySpec*> AbilitySpecs;
					AbilitySystemComponent->GetActivatableGameplayAbilitySpecsByAllMatchingDynamicTags(FGameplayTagContainer(InventorySlotInputTag), AbilitySpecs);
					for (FGameplayAbilitySpec* AbilitySpec : AbilitySpecs)
					{
						AbilitySystemComponent->IncreaseAbilityLevel(*AbilitySpec);
					}
				}
			}
		}
	}

	return FReply::Handled();
}

FReply URaveUserWidget_InventorySlot::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);

	return NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void URaveUserWidget_InventorySlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	
	InventorySlotBorder->SetBrushColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
}

void URaveUserWidget_InventorySlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	
	InventorySlotBorder->SetBrushColor(FLinearColor(0.7f, 0.7f, 0.7f, 1.0f));

	bCanClick = false;
}

void URaveUserWidget_InventorySlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	if (!IsValid(InventorySystemComponent.Get()))
	{
		return;
	}

	URaveDragDropOperation_InventorySlot* DragDropOperation = NewObject<URaveDragDropOperation_InventorySlot>(GetTransientPackage(), URaveDragDropOperation_InventorySlot::StaticClass());
	switch (InventorySlotType)
	{
	case ERaveInventorySlotType::Skill:
		return;

	case ERaveInventorySlotType::Module:
	{
		const TSubclassOf<URaveGameplayItem>* ModuleEquipment = InventorySystemComponent->GetModuleEquipment(InventorySlotInputTag);
		if (ModuleEquipment)
		{
			URaveGameplayItem* GameplayItem = ModuleEquipment->GetDefaultObject();
			if (GameplayItem)
			{
				DragDropOperation->SlotType = InventorySlotType;
				DragDropOperation->GameplayItemClass = GameplayItem->GetClass();
				DragDropOperation->AbilityClass = GameplayItem->GrantedAbilityClass;
				DragDropOperation->InputTag = InventorySlotInputTag;
			}
			else
			{
				return;
			}
		}
		else
		{
			return;
		}
		break;
	}

	case ERaveInventorySlotType::Item:
	{
		const TPair<TSubclassOf<URaveGameplayItem>, int32>* InventorySlot = InventorySystemComponent->GetInventorySlot(InventoryIndex);
		if (InventorySlot)
		{
			URaveGameplayItem* GameplayItem = InventorySlot->Key.GetDefaultObject();
			if (GameplayItem)
			{
				DragDropOperation->SlotType = InventorySlotType;
				DragDropOperation->GameplayItemClass = GameplayItem->GetClass();
				DragDropOperation->AbilityClass = InventorySlot->Key.GetDefaultObject()->GrantedAbilityClass;
				DragDropOperation->InventoryIndex = InventoryIndex;
			}
			else
			{
				return;
			}
		}
		else
		{
			return;
		}
		break;
	}
	}

	URaveUserWidget_InventorySlot* DragVisualInventorySlotWidget = DuplicateObject<URaveUserWidget_InventorySlot>(this, this);
	if (DragVisualInventorySlotWidget)
	{
		DragVisualInventorySlotWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		DragVisualInventorySlotWidget->SetInventorySlotType(InventorySlotType);
		DragVisualInventorySlotWidget->SetInventoryIndex(InventoryIndex);
		DragDropOperation->DefaultDragVisual = DragVisualInventorySlotWidget;
		DragDropOperation->Pivot = EDragPivot::CenterCenter;

		OutOperation = DragDropOperation;
	}
}

bool URaveUserWidget_InventorySlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	URaveDragDropOperation_InventorySlot* InventorySlotDragDropOperation = Cast<URaveDragDropOperation_InventorySlot>(InOperation);
	if (InventorySlotDragDropOperation)
	{
		if ((InventoryIndex != INDEX_NONE && InventoryIndex == InventorySlotDragDropOperation->InventoryIndex)
			|| (InventorySlotInputTag.IsValid() && InventorySlotInputTag == InventorySlotDragDropOperation->InputTag))
		{
			return false;
		}

		switch (InventorySlotType)
		{
		case ERaveInventorySlotType::Skill:
			return false;

		case ERaveInventorySlotType::Module:

			if (InventorySlotDragDropOperation->SlotType == ERaveInventorySlotType::Module)
			{
				InventorySystemComponent->MoveModule(InventorySlotDragDropOperation->InputTag, InventorySlotInputTag);
			}
			else if (InventorySlotDragDropOperation->SlotType == ERaveInventorySlotType::Item)
			{
				InventorySystemComponent->EquipModule(InventorySlotDragDropOperation->InventoryIndex, InventorySlotInputTag);
			}
			break;

		case ERaveInventorySlotType::Item:
			if (InventorySlotDragDropOperation->SlotType == ERaveInventorySlotType::Module)
			{
				InventorySystemComponent->UnequipModule(InventorySlotDragDropOperation->InputTag, InventoryIndex);
			}
			else if (InventorySlotDragDropOperation->SlotType == ERaveInventorySlotType::Item)
			{
				InventorySystemComponent->MoveItem(InventorySlotDragDropOperation->InventoryIndex, InventoryIndex);
			}
			break;

		default:
			return false;
		}

		return true;
	}

	return false;
}

ERaveInventorySlotType URaveUserWidget_InventorySlot::GetInventorySlotType() const
{
	return InventorySlotType;
}

void URaveUserWidget_InventorySlot::SetInventorySlotType(ERaveInventorySlotType InInventorySlotType)
{
	InventorySlotType = InInventorySlotType;
}

void URaveUserWidget_InventorySlot::SetInventoryIndex(int32 InInventoryIndex)
{
	InventoryIndex = InInventoryIndex;
}

FSlateBrush URaveUserWidget_InventorySlot::GetInventoryBorderBackground()
{
	if (InventorySlotType == ERaveInventorySlotType::Skill)
	{
		if (URaveAbilitySystemComponent* AbilitySystemComponent = Cast<URaveAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwningPlayerPawn())))
		{
			TArray<FGameplayAbilitySpec*> AbilitySpecs;
			AbilitySystemComponent->GetActivatableGameplayAbilitySpecsByAllMatchingDynamicTags(FGameplayTagContainer(InventorySlotInputTag), AbilitySpecs, false);
			if (AbilitySpecs.Num() > 0)
			{
				FGameplayAbilitySpec* AbilitySpec = AbilitySpecs[0];
				if (AbilitySpec)
				{
					if (URaveGameplayAbility* Ability = Cast<URaveGameplayAbility>(AbilitySpec->Ability))
					{
						FSlateBrushOutlineSettings InventorySlotBorderOutlineSettings;
						InventorySlotBorderOutlineSettings.CornerRadii = FVector4(16.0f, 16.0f, 16.0f, 16.0f);
						InventorySlotBorderOutlineSettings.Color = FLinearColor(0.01f, 0.01f, 0.01f, 0.9f);
						InventorySlotBorderOutlineSettings.Width = 1.0f;
						InventorySlotBorderOutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
						InventorySlotBorderOutlineSettings.bUseBrushTransparency = false;

						FSlateBrush InventorySlotBorderBrush;
						InventorySlotBorderBrush.SetResourceObject(Ability->GetAbilityDescription().Icon);
						InventorySlotBorderBrush.TintColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
						InventorySlotBorderBrush.DrawAs = ESlateBrushDrawType::RoundedBox;
						InventorySlotBorderBrush.OutlineSettings = InventorySlotBorderOutlineSettings;

						return InventorySlotBorderBrush;
					}
				}
			}
		}
	}
	else if (InventorySlotType == ERaveInventorySlotType::Module)
	{
		const TSubclassOf<URaveGameplayItem>* ModuleEquipment = InventorySystemComponent->GetModuleEquipment(InventorySlotInputTag);
		if (ModuleEquipment)
		{
			URaveGameplayItem* GameplayItem = ModuleEquipment->GetDefaultObject();
			if (GameplayItem)
			{
				FSlateBrushOutlineSettings InventorySlotBorderOutlineSettings;
				InventorySlotBorderOutlineSettings.CornerRadii = FVector4(16.0f, 16.0f, 16.0f, 16.0f);
				InventorySlotBorderOutlineSettings.Color = FLinearColor(0.01f, 0.01f, 0.01f, 0.9f);
				InventorySlotBorderOutlineSettings.Width = 1.0f;
				InventorySlotBorderOutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
				InventorySlotBorderOutlineSettings.bUseBrushTransparency = false;

				FSlateBrush InventorySlotBorderBrush;
				InventorySlotBorderBrush.SetResourceObject(GameplayItem->ItemIcon);
				InventorySlotBorderBrush.TintColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
				InventorySlotBorderBrush.DrawAs = ESlateBrushDrawType::RoundedBox;
				InventorySlotBorderBrush.OutlineSettings = InventorySlotBorderOutlineSettings;

				return InventorySlotBorderBrush;
			}
		}
	}
	else if (InventorySlotType == ERaveInventorySlotType::Item)
	{
		const TPair<TSubclassOf<URaveGameplayItem>, int32>* InventorySlot = InventorySystemComponent->GetInventorySlot(InventoryIndex);
		if (InventorySlot)
		{
			URaveGameplayItem* GameplayItem = (InventorySlot->Key.GetDefaultObject());
			if (IsValid(GameplayItem))
			{
				FSlateBrushOutlineSettings InventorySlotBorderOutlineSettings;
				InventorySlotBorderOutlineSettings.CornerRadii = FVector4(16.0f, 16.0f, 16.0f, 16.0f);
				InventorySlotBorderOutlineSettings.Color = FLinearColor(0.01f, 0.01f, 0.01f, 0.9f);
				InventorySlotBorderOutlineSettings.Width = 1.0f;
				InventorySlotBorderOutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
				InventorySlotBorderOutlineSettings.bUseBrushTransparency = false;

				FSlateBrush InventorySlotBorderBrush;
				InventorySlotBorderBrush.SetResourceObject(GameplayItem->ItemIcon);
				InventorySlotBorderBrush.TintColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
				InventorySlotBorderBrush.DrawAs = ESlateBrushDrawType::RoundedBox;
				InventorySlotBorderBrush.OutlineSettings = InventorySlotBorderOutlineSettings;

				return InventorySlotBorderBrush;
			}
		}
	}

	FSlateBrushOutlineSettings DefaultInventorySlotBorderOutlineSettings;
	DefaultInventorySlotBorderOutlineSettings.CornerRadii = FVector4(16.0f, 16.0f, 16.0f, 16.0f);
	DefaultInventorySlotBorderOutlineSettings.Color = FLinearColor(0.01f, 0.01f, 0.01f, 0.9f);
	DefaultInventorySlotBorderOutlineSettings.Width = 1.0f;
	DefaultInventorySlotBorderOutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
	DefaultInventorySlotBorderOutlineSettings.bUseBrushTransparency = false;

	FSlateBrush DefaultInventorySlotBorderBrush;
	DefaultInventorySlotBorderBrush.TintColor = FLinearColor(0.01f, 0.01f, 0.01f, 1.0f);
	DefaultInventorySlotBorderBrush.DrawAs = ESlateBrushDrawType::RoundedBox;
	DefaultInventorySlotBorderBrush.OutlineSettings = DefaultInventorySlotBorderOutlineSettings;

	return DefaultInventorySlotBorderBrush;
}

UWidget* URaveUserWidget_InventorySlot::GetInventoryToolTip()
{
	if (!IsValid(InventoryToolTipClass))
	{
		return nullptr;
	}

	if (InventorySlotType == ERaveInventorySlotType::Skill || InventorySlotType == ERaveInventorySlotType::Module)
	{
		if (URaveAbilitySystemComponent* AbilitySystemComponent = Cast<URaveAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwningPlayerPawn())))
		{
			TArray<FGameplayAbilitySpec*> AbilitySpecs;
			AbilitySystemComponent->GetActivatableGameplayAbilitySpecsByAllMatchingDynamicTags(FGameplayTagContainer(InventorySlotInputTag), AbilitySpecs);
			if (AbilitySpecs.Num() > 0)
			{
				FGameplayAbilitySpec* GameplayAbilitySpec = AbilitySpecs[0];
				if (GameplayAbilitySpec)
				{
					if (URaveGameplayAbility* GameplayAbility = Cast<URaveGameplayAbility>(GameplayAbilitySpec->Ability))
					{
						URaveUserWidget_InventoryToolTip* InventorySlotToolTip = CreateWidget<URaveUserWidget_InventoryToolTip>(GetOwningPlayer(), InventoryToolTipClass);
						if (InventorySlotToolTip)
						{
							InventorySlotToolTip->ToolTipImage->SetBrushFromTexture(GameplayAbility->GetAbilityDescription().Icon);
							InventorySlotToolTip->NameTextBlock->SetText(GameplayAbility->GetAbilityDescription().Name);
							InventorySlotToolTip->LevelTextBlock->SetText(FText::FromString(TEXT("(") + FString::FromInt(GameplayAbilitySpec->Level) + TEXT(")")));
							InventorySlotToolTip->KeyTextBlock->SetText(FText::FromString((TEXT("[") + GetInventorySlotKey().GetDisplayName().ToString() + TEXT("]"))));
							InventorySlotToolTip->CostTextBlock->SetText(FText::FromString(FString::SanitizeFloat(FMath::Abs(GameplayAbility->GetCostMagnitude(URaveAttributeSet::GetManaAttribute())), 0) + TEXT(" Mana")));
							InventorySlotToolTip->CooldownTextBlock->SetText(FText::FromString(FString::SanitizeFloat(GameplayAbility->GetCooldownDuration(), 0) + TEXT("s")));
							InventorySlotToolTip->DescriptionTextBlock->SetText(GameplayAbility->GetAbilityDescription().Description);

							return InventorySlotToolTip;
						}
					}
				}
			}
		}
	}
	else if (InventorySlotType == ERaveInventorySlotType::Item)
	{
		ARavePlayerState* PlayerState = GetOwningPlayerState<ARavePlayerState>();
		check(PlayerState);

		const TPair<TSubclassOf<URaveGameplayItem>, int32>* InventorySlot = InventorySystemComponent->GetInventorySlot(InventoryIndex);
		if (InventorySlot)
		{
			URaveGameplayItem* GameplayItem = InventorySlot->Key.GetDefaultObject();
			if (GameplayItem)
			{
				URaveUserWidget_InventoryToolTip* InventorySlotToolTip = CreateWidget<URaveUserWidget_InventoryToolTip>(GetOwningPlayer(), InventoryToolTipClass);
				if (InventorySlotToolTip)
				{
					InventorySlotToolTip->ToolTipImage->SetBrushFromTexture(GameplayItem->ItemIcon);
					InventorySlotToolTip->NameTextBlock->SetText(GameplayItem->ItemDisplayName);
					InventorySlotToolTip->DescriptionTextBlock->SetText(GameplayItem->ItemDescription);

					URaveGameplayAbility* GameplayAbility = Cast<URaveGameplayAbility>(GameplayItem->GrantedAbilityClass.GetDefaultObject());
					if (GameplayAbility)
					{
						InventorySlotToolTip->KeyTextBlock->SetText(FText::FromString((TEXT(""))));
						InventorySlotToolTip->CostTextBlock->SetText(FText::FromString(FString::SanitizeFloat(FMath::Abs(GameplayAbility->GetCostMagnitude(URaveAttributeSet::GetManaAttribute())), 0) + TEXT(" Mana")));
						InventorySlotToolTip->CooldownTextBlock->SetText(FText::FromString(FString::SanitizeFloat(GameplayAbility->GetCooldownDuration(), 0) + TEXT("s")));

						InventorySlotToolTip->LevelTextBlock->SetVisibility(ESlateVisibility::Hidden);
						InventorySlotToolTip->KeyTextBlock->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
						InventorySlotToolTip->CostTextBlock->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
						InventorySlotToolTip->CooldownTextBlock->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
					}
					else
					{
						InventorySlotToolTip->LevelTextBlock->SetVisibility(ESlateVisibility::Hidden);
						InventorySlotToolTip->KeyTextBlock->SetVisibility(ESlateVisibility::Hidden);
						InventorySlotToolTip->CostTextBlock->SetVisibility(ESlateVisibility::Hidden);
						InventorySlotToolTip->CooldownTextBlock->SetVisibility(ESlateVisibility::Hidden);
					}

					return InventorySlotToolTip;
				}
			}
		}
	}

	return nullptr;
}

ESlateVisibility URaveUserWidget_InventorySlot::GetStackTextBlockVisibility()
{
	if (InventorySlotType == ERaveInventorySlotType::Item)
	{
		const TPair<TSubclassOf<URaveGameplayItem>, int32>* InventorySlot = InventorySystemComponent->GetInventorySlot(InventoryIndex);
		if (InventorySlot)
		{
			URaveGameplayItem* GameplayItem = InventorySlot->Key.GetDefaultObject();
			if (GameplayItem)
			{
				if (GameplayItem->CanStack())
				{
					return ESlateVisibility::SelfHitTestInvisible;
				}
			}
		}
	}

	return ESlateVisibility::Collapsed;
}

FText URaveUserWidget_InventorySlot::GetStackTextBlockText()
{
	if (InventorySlotType == ERaveInventorySlotType::Item)
	{
		const TPair<TSubclassOf<URaveGameplayItem>, int32>* InventorySlot = InventorySystemComponent->GetInventorySlot(InventoryIndex);
		if (InventorySlot)
		{
			return FText::FromString(FString::FromInt(InventorySlot->Value));
		}
	}

	return FText();
}

FKey URaveUserWidget_InventorySlot::GetInventorySlotKey() const
{
	ARaveCharacter* OwningPlayerCharacter = Cast<ARaveCharacter>(GetOwningPlayerPawn());
	if (OwningPlayerCharacter)
	{
		const UInputAction* InputAction = OwningPlayerCharacter->GetCharacterSetting()->InputConfig->FindAbilityInputActionForTag(InventorySlotInputTag);

		UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalSubsystem = GetOwningPlayer()->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		if (EnhancedInputLocalSubsystem)
		{
			TArray<FKey> Keys = EnhancedInputLocalSubsystem->QueryKeysMappedToAction(InputAction);
			if (Keys.Num() > 0)
			{
				return Keys[0];
			}
		}
	}

	return FKey();
}
