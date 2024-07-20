


#include "UI/RaveUserWidget_CharacterSelectSlot.h"
#include "UI/RaveUserWidget_CharacterSelect.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "RaveGameInstance.h"
#include "Character/RaveCharacterSetting.h"
#include "Player/RavePlayerState.h"

URaveUserWidget_CharacterSelectSlot::URaveUserWidget_CharacterSelectSlot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bSelected = false;
	SelectColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	UnselectColor = FLinearColor(0.2f, 0.2f, 0.2f, 1.0f);
}

void URaveUserWidget_CharacterSelectSlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	CharacterSelectSlotButton->SetColorAndOpacity(UnselectColor);
	CharacterSelectSlotButton->OnClicked.AddDynamic(this, &ThisClass::CharacterSelectSlotButtonClicked);
}

void URaveUserWidget_CharacterSelectSlot::InitializeCharacterSelectSlot(URaveUserWidget_CharacterSelect* CharacterSelectWidget, URaveCharacterSetting* CharacterSetting)
{
	OwningCharacterSelectWidget = CharacterSelectWidget;
	SlotCharacterSetting = CharacterSetting;

	if (IsValid(CharacterSetting))
	{
		if (CharacterSetting->Portrait.Get())
		{
			CharacterImage->SetBrushFromTexture(CharacterSetting->Portrait.Get());
		}
		else
		{
			URaveGameInstance* RaveGameInstance = GetGameInstance<URaveGameInstance>();
			if (RaveGameInstance)
			{
				PortraitStreamableHandle = RaveGameInstance->StreamableManager.RequestAsyncLoad(
					CharacterSetting->Portrait.ToSoftObjectPath(),
					FStreamableDelegate::CreateUObject(this, &ThisClass::PortraitAsyncLoadCompleted));
			}
		}
	}
}

URaveCharacterSetting* URaveUserWidget_CharacterSelectSlot::GetSlotCharacterSetting() const
{
	return SlotCharacterSetting.Get();
}

void URaveUserWidget_CharacterSelectSlot::SelectSlot()
{
	CharacterSelectSlotButton->SetColorAndOpacity(SelectColor);
	bSelected = true;
}

void URaveUserWidget_CharacterSelectSlot::UnSelectSlot()
{
	CharacterSelectSlotButton->SetColorAndOpacity(UnselectColor);
	bSelected = false;
}

void URaveUserWidget_CharacterSelectSlot::CharacterSelectSlotButtonClicked()
{
	if (!bSelected && IsValid(OwningCharacterSelectWidget.Get()))
	{
		OwningCharacterSelectWidget->SetSelectedCharacterSelectSlot(this);
	}
}

void URaveUserWidget_CharacterSelectSlot::PortraitAsyncLoadCompleted()
{
	if (PortraitStreamableHandle.IsValid())
	{
		UTexture2D* LoadedTextureAsset = Cast<UTexture2D>(PortraitStreamableHandle->GetLoadedAsset());
		if (LoadedTextureAsset)
		{
			CharacterImage->SetBrushFromTexture(LoadedTextureAsset);
		}

		PortraitStreamableHandle.Reset();
	}
}
