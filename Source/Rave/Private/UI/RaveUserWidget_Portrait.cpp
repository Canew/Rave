


#include "UI/RaveUserWidget_Portrait.h"
#include "RaveGameInstance.h"
#include "Components/Image.h"
#include "Character/RaveCharacter.h"
#include "Character/RaveCharacterSetting.h"
#include "Player/RavePlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveUserWidget_Portrait)

URaveUserWidget_Portrait::URaveUserWidget_Portrait(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PortraitTextureParameterName = TEXT("PortraitTexture");
}

void URaveUserWidget_Portrait::NativeConstruct()
{
	Super::NativeConstruct();

	if (ARavePlayerState* RavePlayerState = GetOwningPlayerState<ARavePlayerState>())
	{
		const URaveCharacterSetting* RaveCharacterSetting = RavePlayerState->GetCharacterSetting();
		if (RaveCharacterSetting)
		{
			if (RaveCharacterSetting->Portrait.Get())
			{
				UMaterialInstanceDynamic* MaterialInstanceDynamic = PortraitImage->GetDynamicMaterial();
				if (MaterialInstanceDynamic)
				{
					MaterialInstanceDynamic->SetTextureParameterValue(PortraitTextureParameterName, RaveCharacterSetting->Portrait.Get());
				}
			}
			else
			{
				URaveGameInstance* RaveGameInstance = GetGameInstance<URaveGameInstance>();
				if (RaveGameInstance)
				{
					PortraitStreamableHandle = RaveGameInstance->StreamableManager.RequestAsyncLoad(
						RaveCharacterSetting->Portrait.ToSoftObjectPath(),
						FStreamableDelegate::CreateUObject(this, &ThisClass::PortraitAsyncLoadCompleted));
				}
			}
		}

		RavePlayerState->CharacterSettingChanged.AddDynamic(this, &ThisClass::CharacterSettingChanged);
	}
}

void URaveUserWidget_Portrait::CharacterSettingChanged(const URaveCharacterSetting* CharacterSetting)
{
	if (CharacterSetting)
	{
		if (CharacterSetting->Portrait.Get())
		{
			UMaterialInstanceDynamic* MaterialInstanceDynamic = PortraitImage->GetDynamicMaterial();
			if (MaterialInstanceDynamic)
			{
				MaterialInstanceDynamic->SetTextureParameterValue(PortraitTextureParameterName, CharacterSetting->Portrait.Get());
			}
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

void URaveUserWidget_Portrait::PortraitAsyncLoadCompleted()
{
	if (PortraitStreamableHandle.IsValid())
	{
		UTexture2D* LoadedTextureAsset = Cast<UTexture2D>(PortraitStreamableHandle->GetLoadedAsset());
		if (LoadedTextureAsset)
		{
			UMaterialInstanceDynamic* MaterialInstanceDynamic = PortraitImage->GetDynamicMaterial();
			if (MaterialInstanceDynamic)
			{
				MaterialInstanceDynamic->SetTextureParameterValue(PortraitTextureParameterName, LoadedTextureAsset);
			}
		}

		PortraitStreamableHandle.Reset();
	}
}
