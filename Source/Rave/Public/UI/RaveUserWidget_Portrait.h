

#pragma once

#include "CoreMinimal.h"
#include "UI/RaveUserWidget.h"
#include "Styling/SlateBrush.h"
#include "RaveUserWidget_Portrait.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveUserWidget_Portrait : public URaveUserWidget
{
	GENERATED_BODY()
	
public:
	URaveUserWidget_Portrait(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void CharacterSettingChanged(const class URaveCharacterSetting* CharacterSetting);

	void PortraitAsyncLoadCompleted();

protected:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class UImage> PortraitImage;

	UPROPERTY(EditAnywhere)
	FName PortraitTextureParameterName;

	TSharedPtr<struct FStreamableHandle> PortraitStreamableHandle;
};
