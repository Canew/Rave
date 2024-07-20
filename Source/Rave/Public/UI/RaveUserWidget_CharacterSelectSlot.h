

#pragma once

#include "CoreMinimal.h"
#include "UI/RaveUserWidget.h"
#include "RaveUserWidget_CharacterSelectSlot.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveUserWidget_CharacterSelectSlot : public URaveUserWidget
{
	GENERATED_BODY()
	
public:
	URaveUserWidget_CharacterSelectSlot(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeOnInitialized() override;

public:
	void InitializeCharacterSelectSlot(class URaveUserWidget_CharacterSelect* CharacterSelectWidget, class URaveCharacterSetting* CharacterSetting);
	class URaveCharacterSetting* GetSlotCharacterSetting() const;
	void SelectSlot();
	void UnSelectSlot();

protected:
	UFUNCTION()
	void CharacterSelectSlotButtonClicked();

	void PortraitAsyncLoadCompleted();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UBorder> CharacterSelectSlotBorder;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> CharacterSelectSlotButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> CharacterImage;

	TWeakObjectPtr<class URaveUserWidget_CharacterSelect> OwningCharacterSelectWidget;
	TWeakObjectPtr<class URaveCharacterSetting> SlotCharacterSetting;

	bool bSelected;
	FLinearColor SelectColor;
	FLinearColor UnselectColor;

	TSharedPtr<struct FStreamableHandle> PortraitStreamableHandle;
};
