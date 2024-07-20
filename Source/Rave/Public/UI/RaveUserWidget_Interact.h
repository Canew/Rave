

#pragma once

#include "CoreMinimal.h"
#include "UI/RaveUserWidget.h"
#include "RaveUserWidget_Interact.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveUserWidget_Interact : public URaveUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnInitialized();

public:
	virtual void SetInteractKeyText(FText InteractKeyText);
	virtual void SetInteractDescriptionText(FText InteractDescriptionText);

protected:
	virtual void HandleInteractionActorChanged(AActor* InteractionActor);

protected:
	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* InteractKeyTextBlock;

	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* InteractDescriptionTextBlock;

	TWeakObjectPtr<class URaveInteractionComponent> InteractionComponent;
};
