


#include "UI/RaveUserWidget_Interact.h"
#include "Components/TextBlock.h"
#include "Components/RaveInteractionComponent.h"
#include "Interfaces/RaveInteractionInterface.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Character/RaveCharacter.h"

void URaveUserWidget_Interact::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ARaveCharacter* Character = Cast<ARaveCharacter>(GetOwningPlayerPawn()))
	{
		InteractionComponent = Character->FindComponentByClass<URaveInteractionComponent>();
		if (InteractionComponent.IsValid())
		{
			InteractionComponent->OnInteractionActorChanged.AddUObject(this, &ThisClass::HandleInteractionActorChanged);
		}
	}
}

void URaveUserWidget_Interact::SetInteractKeyText(FText InteractKeyText)
{
	if (InteractKeyTextBlock)
	{
		InteractKeyTextBlock->SetText(InteractKeyText);
	}
}

void URaveUserWidget_Interact::SetInteractDescriptionText(FText InteractDescriptionText)
{
	if (InteractDescriptionTextBlock)
	{
		InteractDescriptionTextBlock->SetText(InteractDescriptionText);
	}
}

void URaveUserWidget_Interact::HandleInteractionActorChanged(AActor* InteractionActor)
{
	if (InteractionActor->GetClass()->ImplementsInterface(URaveInteractionInterface::StaticClass()))
	{
		SetInteractDescriptionText(IRaveInteractionInterface::Execute_GetInteractionDescription(InteractionActor));
	}
}
