


#include "UI/RaveUserWidget_GameplayShopSlot.h"
#include "UI/RaveUserWidget_InventoryToolTip.h"
#include "UI/RaveUserWidget_Dialog.h"
#include "Gameplay/RaveGameplayShop.h"
#include "Inventory/RaveGameplayItem.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "GameplayAbility/RaveAttributeSet.h"
#include "GameplayAbility/RaveGameplayAbility.h"
#include "Blueprint/WidgetTree.h"

void URaveUserWidget_GameplayShopSlot::InitializeGameplayShopSlot(ARaveGameplayShop* GameplayShop, int32 Index)
{
	OwningGameplayShop = GameplayShop;
	SlotIndex = Index;

	if (GameplayShop && GameplayShop->GetShopGameplayItemClasses().IsValidIndex(Index))
	{
		TSubclassOf<class URaveGameplayItem> ShopGameplayItemClass = GameplayShop->GetShopGameplayItemClasses()[Index];
		URaveGameplayItem* GameplayItem = ShopGameplayItemClass.GetDefaultObject();
		if (GameplayItem)
		{
			ShopSlotImage->SetBrushFromTexture(GameplayItem->ItemIcon);
			ItemNameTextBlock->SetText(GameplayItem->ItemDisplayName);
			ItemPriceTextBlock->SetText(FText::FromString(FString::FromInt(GameplayItem->PurchasePrice)));
		}

		FSlateBrushOutlineSettings ShopSlotBorderOutlineSettings;
		ShopSlotBorderOutlineSettings.CornerRadii = FVector4(8.0f, 8.0f, 8.0f, 8.0f);
		ShopSlotBorderOutlineSettings.Color = FLinearColor(0.01f, 0.01f, 0.01f, 0.9f);
		ShopSlotBorderOutlineSettings.Width = 1.0f;
		ShopSlotBorderOutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
		ShopSlotBorderOutlineSettings.bUseBrushTransparency = false;

		FSlateBrush ShopSlotBorderBrush;
		ShopSlotBorderBrush.TintColor = FLinearColor(0.01f, 0.01f, 0.01f, 1.0f);
		ShopSlotBorderBrush.DrawAs = ESlateBrushDrawType::RoundedBox;
		ShopSlotBorderBrush.OutlineSettings = ShopSlotBorderOutlineSettings;
		ShopSlotBorder->SetBrush(ShopSlotBorderBrush);
		ShopSlotBorder->SetBrushColor(FLinearColor(0.7f, 0.7f, 0.7f, 1.0f));

		ShopSlotButton->ToolTipWidgetDelegate.BindDynamic(this, &ThisClass::GetShopSlotToolTip);
		ShopSlotButton->OnHovered.AddDynamic(this, &ThisClass::SlotButtonHovered);
		ShopSlotButton->OnUnhovered.AddDynamic(this, &ThisClass::SlotButtonUnhovered);
		ShopSlotButton->OnClicked.AddDynamic(this, &ThisClass::SlotButtonClicked);
		ShopSlotButton->OnPressed.AddDynamic(this, &ThisClass::SlotButtonPressed);
		ShopSlotButton->OnReleased.AddDynamic(this, &ThisClass::SlotButtonReleased);
	}
	else
	{
		ShopSlotImage->SetBrush(FSlateBrush());
	}

}

void URaveUserWidget_GameplayShopSlot::NativeDestruct()
{
	Super::NativeDestruct();

	if (IsValid(PurchaseConfirmationDialogWidget))
	{
		PurchaseConfirmationDialogWidget->RemoveFromParent();
		PurchaseConfirmationDialogWidget = nullptr;	
	}
}

UWidget* URaveUserWidget_GameplayShopSlot::GetShopSlotToolTip()
{
	if (IsValid(OwningGameplayShop.Get()) && OwningGameplayShop->GetShopGameplayItemClasses().IsValidIndex(SlotIndex))
	{
		TSubclassOf<class URaveGameplayItem> ShopGameplayItemClass = OwningGameplayShop->GetShopGameplayItemClasses()[SlotIndex];
		URaveGameplayItem* GameplayItem = ShopGameplayItemClass.GetDefaultObject();
		URaveUserWidget_InventoryToolTip* InventorySlotToolTip = CreateWidget<URaveUserWidget_InventoryToolTip>(GetOwningPlayer(), InventoryToolTipClass);
		if (InventorySlotToolTip && GameplayItem)
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

	return nullptr;
}

void URaveUserWidget_GameplayShopSlot::SlotButtonHovered()
{
	ShopSlotBorder->SetBrushColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
}

void URaveUserWidget_GameplayShopSlot::SlotButtonUnhovered()
{
	ShopSlotBorder->SetBrushColor(FLinearColor(0.7f, 0.7f, 0.7f, 1.0f));
}

void URaveUserWidget_GameplayShopSlot::SlotButtonClicked()
{
	if (IsValid(OwningGameplayShop.Get()) && OwningGameplayShop->GetShopGameplayItemClasses().IsValidIndex(SlotIndex))
	{
		TSubclassOf<class URaveGameplayItem> ShopGameplayItemClass = OwningGameplayShop->GetShopGameplayItemClasses()[SlotIndex];
		URaveGameplayItem* GameplayItem = ShopGameplayItemClass.GetDefaultObject();
		if (DialogWidgetClass && GameplayItem)
		{
			PurchaseConfirmationDialogWidget = CreateWidget<URaveUserWidget_Dialog>(GetOwningPlayer(), DialogWidgetClass);
			if (PurchaseConfirmationDialogWidget)
			{
				PurchaseConfirmationDialogWidget->TitleTextBlock->SetText(FText::FromString(TEXT("Purchase")));
				PurchaseConfirmationDialogWidget->DialogTextBlock->SetText(FText::FromString(TEXT("Are you sure you want to purchase this item?")));
				PurchaseConfirmationDialogWidget->AcceptButton->OnClicked.AddDynamic(this, &ThisClass::PurchaseAcceptButtonClicked);
				PurchaseConfirmationDialogWidget->RejectButton->OnClicked.AddDynamic(this, &ThisClass::PurchaseRejectButtonClicked);

				UImage* PurchaseItemImage = PurchaseConfirmationDialogWidget->WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("PurchaseItemImage"));
				if (PurchaseItemImage)
				{
					FSlateBrush PurchaseItemImageBrush;
					PurchaseItemImageBrush.SetImageSize(FVector2D(48.0f, 48.0f));
					PurchaseItemImageBrush.SetResourceObject(GameplayItem->ItemIcon);
					PurchaseItemImage->SetBrush(PurchaseItemImageBrush);
					PurchaseItemImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
					UVerticalBoxSlot* VerticalBoxSlot = PurchaseConfirmationDialogWidget->DialogDetailVerticalBox->AddChildToVerticalBox(PurchaseItemImage);
					if (VerticalBoxSlot)
					{
						VerticalBoxSlot->SetHorizontalAlignment(HAlign_Center);
						VerticalBoxSlot->SetVerticalAlignment(VAlign_Center);
					}
				}

				UTextBlock* PurchaseItemTextBlock = PurchaseConfirmationDialogWidget->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("PurchaseItemTextBlock"));
				if (PurchaseItemTextBlock)
				{
					PurchaseItemTextBlock->SetFont(PurchaseConfirmationDialogWidget->DialogTextBlock->GetFont());
					PurchaseItemTextBlock->SetText(GameplayItem->ItemDisplayName);
					PurchaseItemTextBlock->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
					UVerticalBoxSlot* VerticalBoxSlot = PurchaseConfirmationDialogWidget->DialogDetailVerticalBox->AddChildToVerticalBox(PurchaseItemTextBlock);
					if (VerticalBoxSlot)
					{
						VerticalBoxSlot->SetHorizontalAlignment(HAlign_Center);
						VerticalBoxSlot->SetVerticalAlignment(VAlign_Center);
					}
				}

				// Price Box
				UHorizontalBox* PurchasePriceHorizontalBox = PurchaseConfirmationDialogWidget->WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("PurchasePriceHorizontalBox"));
				if (PurchasePriceHorizontalBox)
				{
					PurchasePriceHorizontalBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
					UVerticalBoxSlot* VerticalBoxSlot = PurchaseConfirmationDialogWidget->DialogDetailVerticalBox->AddChildToVerticalBox(PurchasePriceHorizontalBox);
					if (VerticalBoxSlot)
					{
						VerticalBoxSlot->SetPadding(FMargin(0.0f, 8.0f, 0.0f, 2.0f));
						VerticalBoxSlot->SetHorizontalAlignment(HAlign_Center);
						VerticalBoxSlot->SetVerticalAlignment(VAlign_Center);
					}

					UTextBlock* PurchasePriceTextBlock = PurchaseConfirmationDialogWidget->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("PurchasePriceTextBlock"));
					if (PurchasePriceTextBlock)
					{
						PurchasePriceTextBlock->SetFont(PurchaseConfirmationDialogWidget->DialogTextBlock->GetFont());
						PurchasePriceTextBlock->SetText(FText::FromString(TEXT("Purchase Price ")));
						PurchasePriceTextBlock->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
						UHorizontalBoxSlot* HorizontalBoxSlot = PurchasePriceHorizontalBox->AddChildToHorizontalBox(PurchasePriceTextBlock);
						if (HorizontalBoxSlot)
						{
							HorizontalBoxSlot->SetHorizontalAlignment(HAlign_Center);
							HorizontalBoxSlot->SetVerticalAlignment(VAlign_Center);
						}
					}

					UImage* PurchasePriceCurrencyImage = PurchaseConfirmationDialogWidget->WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("PurchasePriceCurrencyImage"));
					if (PurchasePriceCurrencyImage)
					{
						FSlateBrush PurchasePriceCurrencyImageBrush;
						PurchasePriceCurrencyImageBrush.SetImageSize(FVector2D(18.0f, 18.0f));
						PurchasePriceCurrencyImageBrush.SetResourceObject(CurrencyTexture);
						PurchasePriceCurrencyImage->SetBrush(PurchasePriceCurrencyImageBrush);
						PurchasePriceCurrencyImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
						UHorizontalBoxSlot* HorizontalBoxSlot = PurchasePriceHorizontalBox->AddChildToHorizontalBox(PurchasePriceCurrencyImage);
						if (HorizontalBoxSlot)
						{
							HorizontalBoxSlot->SetHorizontalAlignment(HAlign_Center);
							HorizontalBoxSlot->SetVerticalAlignment(VAlign_Center);
						}
					}

					UTextBlock* PurchasePriceValueTextBlock = PurchaseConfirmationDialogWidget->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("PurchasePriceValueTextBlock"));
					if (PurchasePriceValueTextBlock)
					{
						PurchasePriceValueTextBlock->SetFont(PurchaseConfirmationDialogWidget->DialogTextBlock->GetFont());
						PurchasePriceValueTextBlock->SetText(FText::FromString(FString::FromInt(GameplayItem->PurchasePrice)));
						PurchasePriceValueTextBlock->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
						UHorizontalBoxSlot* HorizontalBoxSlot = PurchasePriceHorizontalBox->AddChildToHorizontalBox(PurchasePriceValueTextBlock);
						if (HorizontalBoxSlot)
						{
							HorizontalBoxSlot->SetHorizontalAlignment(HAlign_Center);
							HorizontalBoxSlot->SetVerticalAlignment(VAlign_Center);
						}
					}
				}

				PurchaseConfirmationDialogWidget->AddToViewport();
			}
		}
	}
}

void URaveUserWidget_GameplayShopSlot::SlotButtonPressed()
{
	ShopSlotBorder->SetBrushColor(FLinearColor(0.4f, 0.4f, 0.4f, 1.0f));
}

void URaveUserWidget_GameplayShopSlot::SlotButtonReleased()
{
	ShopSlotBorder->SetBrushColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
}

void URaveUserWidget_GameplayShopSlot::PurchaseAcceptButtonClicked()
{
	if (IsValid(OwningGameplayShop.Get()) && OwningGameplayShop->GetShopGameplayItemClasses().IsValidIndex(SlotIndex))
	{
		OwningGameplayShop->Purchase(GetOwningPlayer(), OwningGameplayShop->GetShopGameplayItemClasses()[SlotIndex], SlotIndex);
	}

	if (IsValid(PurchaseConfirmationDialogWidget))
	{
		PurchaseConfirmationDialogWidget->RemoveFromParent();
	}
}

void URaveUserWidget_GameplayShopSlot::PurchaseRejectButtonClicked()
{
	if (IsValid(PurchaseConfirmationDialogWidget))
	{
		PurchaseConfirmationDialogWidget->RemoveFromParent();
	}
}
