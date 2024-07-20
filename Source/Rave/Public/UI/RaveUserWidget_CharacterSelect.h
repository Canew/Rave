

#pragma once

#include "CoreMinimal.h"
#include "UI/RaveUserWidget.h"
#include "RaveUserWidget_CharacterSelect.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveUserWidget_CharacterSelect : public URaveUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	void SetSelectedCharacterSelectSlot(class URaveUserWidget_CharacterSelectSlot* CharacterSelectSlot);

protected:
	UFUNCTION()
	void CharacterSelectButtonClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UUniformGridPanel> CharactersUniformGrid;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> CharacterSelectButton;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class URaveUserWidget_CharacterSelectSlot> CharacterSelectSlotClass;

	TObjectPtr<class URaveUserWidget_CharacterSelectSlot> SelectedCharacterSelectSlot;
};
