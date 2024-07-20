


#include "UI/RaveUserWidget_GameplayShopGrid.h"
#include "UI/RaveUserWidget_GameplayShopSlot.h"
#include "UI/RaveUserWidget_Dialog.h"
#include "UI/RaveDragDropOperation_InventorySlot.h"
#include "Gameplay/RaveGameplayShop.h"
#include "Inventory/RaveGameplayItem.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBox.h" 
#include "Components/HorizontalBoxSlot.h" 
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Blueprint/WidgetTree.h"

void URaveUserWidget_GameplayShopGrid::InitializeGameplayShopGrid(ARaveGameplayShop* GameplayShop)
{
	OwningGameplayShop = GameplayShop;

	if (!IsValid(GameplayShop))
	{
		ShopUniformGridPanel->ClearChildren();
		return;
	}

	ShopBorder->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.5f));
	const TArray<TSubclassOf<class URaveGameplayItem>>& ShopGameplayItemClasses = GameplayShop->GetShopGameplayItemClasses();
	for (int SlotIndex = 0; SlotIndex < ShopGameplayItemClasses.Num(); ++SlotIndex)
	{
		URaveUserWidget_GameplayShopSlot* GameplayShopSlotWidget = CreateWidget<URaveUserWidget_GameplayShopSlot>(GetOwningPlayer(), GameplayShopSlotWidgetClass);
		if (GameplayShopSlotWidget)
		{
			GameplayShopSlotWidget->InitializeGameplayShopSlot(GameplayShop, SlotIndex);
			UUniformGridSlot* UniformGridSlot = ShopUniformGridPanel->AddChildToUniformGrid(GameplayShopSlotWidget, SlotIndex / ShopMaxColumn, SlotIndex % ShopMaxColumn);
			{
				UniformGridSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
				UniformGridSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
			}
		}
	}
}

void URaveUserWidget_GameplayShopGrid::NativeDestruct()
{
	Super::Destruct();

	if (IsValid(SaleConfirmationDialogWidget))
	{
		SaleConfirmationDialogWidget->RemoveFromParent();
		SaleConfirmationDialogWidget = nullptr;
	}
}

void URaveUserWidget_GameplayShopGrid::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);

	ShopBorder->SetBrushColor(FLinearColor(0.01f, 0.01f, 0.01f, 0.5f));
}

void URaveUserWidget_GameplayShopGrid::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	ShopBorder->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.5f));
}

bool URaveUserWidget_GameplayShopGrid::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	ShopBorder->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.5f));

	URaveDragDropOperation_InventorySlot* InventorySlotDragDropOperation = Cast<URaveDragDropOperation_InventorySlot>(InOperation);
	if (InventorySlotDragDropOperation)
	{
		if (InventorySlotDragDropOperation->SlotType != ERaveInventorySlotType::Item)
		{
			return false;
		}

		if (IsValid(OwningGameplayShop.Get()))
		{
			GameplayItemClassToSell = InventorySlotDragDropOperation->GameplayItemClass;
			InventoryIndexToSell = InventorySlotDragDropOperation->InventoryIndex;

			URaveGameplayItem* GameplayItemToSell = GameplayItemClassToSell.GetDefaultObject();
			if (DialogWidgetClass && GameplayItemToSell)
			{
				SaleConfirmationDialogWidget = CreateWidget<URaveUserWidget_Dialog>(GetOwningPlayer(), DialogWidgetClass);
				if (SaleConfirmationDialogWidget)
				{
					SaleConfirmationDialogWidget->TitleTextBlock->SetText(FText::FromString(TEXT("Sale")));
					SaleConfirmationDialogWidget->DialogTextBlock->SetText(FText::FromString(TEXT("Are you sure you want to sell this item?")));
					SaleConfirmationDialogWidget->AcceptButton->OnClicked.AddDynamic(this, &ThisClass::SaleAcceptButtonClicked);
					SaleConfirmationDialogWidget->RejectButton->OnClicked.AddDynamic(this, &ThisClass::SaleRejectButtonClicked);

					UImage* SaleItemImage = SaleConfirmationDialogWidget->WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("SaleItemImage"));
					if (SaleItemImage)
					{
						FSlateBrush SaleItemImageBrush;
						SaleItemImageBrush.SetImageSize(FVector2D(48.0f, 48.0f));
						SaleItemImageBrush.SetResourceObject(GameplayItemToSell->ItemIcon);
						SaleItemImage->SetBrush(SaleItemImageBrush);
						SaleItemImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
						UVerticalBoxSlot* VerticalBoxSlot = SaleConfirmationDialogWidget->DialogDetailVerticalBox->AddChildToVerticalBox(SaleItemImage);
						if (VerticalBoxSlot)
						{
							VerticalBoxSlot->SetHorizontalAlignment(HAlign_Center);
							VerticalBoxSlot->SetVerticalAlignment(VAlign_Center);
						}
					}

					UTextBlock* SaleItemTextBlock = SaleConfirmationDialogWidget->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SaleItemTextBlock"));
					if (SaleItemTextBlock)
					{
						SaleItemTextBlock->SetFont(SaleConfirmationDialogWidget->DialogTextBlock->GetFont());
						SaleItemTextBlock->SetText(GameplayItemToSell->ItemDisplayName);
						SaleItemTextBlock->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
						UVerticalBoxSlot* VerticalBoxSlot = SaleConfirmationDialogWidget->DialogDetailVerticalBox->AddChildToVerticalBox(SaleItemTextBlock);
						if (VerticalBoxSlot)
						{
							VerticalBoxSlot->SetHorizontalAlignment(HAlign_Center);
							VerticalBoxSlot->SetVerticalAlignment(VAlign_Center);
						}
					}

					// Price Box
					UHorizontalBox* SalePriceHorizontalBox = SaleConfirmationDialogWidget->WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("SalePriceHorizontalBox"));
					if (SalePriceHorizontalBox)
					{
						SalePriceHorizontalBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
						UVerticalBoxSlot* VerticalBoxSlot = SaleConfirmationDialogWidget->DialogDetailVerticalBox->AddChildToVerticalBox(SalePriceHorizontalBox);
						if (VerticalBoxSlot)
						{
							VerticalBoxSlot->SetPadding(FMargin(0.0f, 8.0f, 0.0f, 2.0f));
							VerticalBoxSlot->SetHorizontalAlignment(HAlign_Center);
							VerticalBoxSlot->SetVerticalAlignment(VAlign_Center);
						}

						UTextBlock* SalePriceTextBlock = SaleConfirmationDialogWidget->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SalePriceTextBlock"));
						if (SalePriceTextBlock)
						{
							SalePriceTextBlock->SetFont(SaleConfirmationDialogWidget->DialogTextBlock->GetFont());
							SalePriceTextBlock->SetText(FText::FromString(TEXT("Sales Price ")));
							SalePriceTextBlock->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
							UHorizontalBoxSlot* HorizontalBoxSlot = SalePriceHorizontalBox->AddChildToHorizontalBox(SalePriceTextBlock);
							if (HorizontalBoxSlot)
							{
								HorizontalBoxSlot->SetHorizontalAlignment(HAlign_Center);
								HorizontalBoxSlot->SetVerticalAlignment(VAlign_Center);
							}
						}

						UImage* SalePriceCurrencyImage = SaleConfirmationDialogWidget->WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("SalePriceCurrencyImage"));
						if (SalePriceCurrencyImage)
						{
							FSlateBrush SalePriceCurrencyImageBrush;
							SalePriceCurrencyImageBrush.SetImageSize(FVector2D(18.0f, 18.0f));
							SalePriceCurrencyImageBrush.SetResourceObject(CurrencyTexture);
							SalePriceCurrencyImage->SetBrush(SalePriceCurrencyImageBrush);
							SalePriceCurrencyImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
							UHorizontalBoxSlot* HorizontalBoxSlot = SalePriceHorizontalBox->AddChildToHorizontalBox(SalePriceCurrencyImage);
							if (HorizontalBoxSlot)
							{
								HorizontalBoxSlot->SetHorizontalAlignment(HAlign_Center);
								HorizontalBoxSlot->SetVerticalAlignment(VAlign_Center);
							}
						}

						UTextBlock* SalePriceValueTextBlock = SaleConfirmationDialogWidget->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SalePriceValueTextBlock"));
						if (SalePriceValueTextBlock)
						{
							SalePriceValueTextBlock->SetFont(SaleConfirmationDialogWidget->DialogTextBlock->GetFont());
							SalePriceValueTextBlock->SetText(FText::FromString(FString::FromInt(GameplayItemToSell->SalesPrice)));
							SalePriceValueTextBlock->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
							UHorizontalBoxSlot* HorizontalBoxSlot = SalePriceHorizontalBox->AddChildToHorizontalBox(SalePriceValueTextBlock);
							if (HorizontalBoxSlot)
							{
								HorizontalBoxSlot->SetHorizontalAlignment(HAlign_Center);
								HorizontalBoxSlot->SetVerticalAlignment(VAlign_Center);
							}
						}
					}

					SaleConfirmationDialogWidget->AddToViewport();
				}
			}
		}
	}

	return true;
}

void URaveUserWidget_GameplayShopGrid::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);

	ShopBorder->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.5f));
}

void URaveUserWidget_GameplayShopGrid::SaleAcceptButtonClicked()
{
	if (IsValid(OwningGameplayShop.Get()))
	{
		OwningGameplayShop->Sell(GetOwningPlayer(), GameplayItemClassToSell, InventoryIndexToSell);
	}

	if (IsValid(SaleConfirmationDialogWidget))
	{
		SaleConfirmationDialogWidget->RemoveFromParent();
	}

	GameplayItemClassToSell = nullptr;
	InventoryIndexToSell = -1;
}

void URaveUserWidget_GameplayShopGrid::SaleRejectButtonClicked()
{
	if (IsValid(SaleConfirmationDialogWidget))
	{
		SaleConfirmationDialogWidget->RemoveFromParent();
	}

	GameplayItemClassToSell = nullptr;
	InventoryIndexToSell = -1;
}
