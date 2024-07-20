


#include "Character/RaveBotCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/RaveInteractionComponent.h"

ARaveBotCharacter::ARaveBotCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	if (SpringArmComponent)
	{
		SpringArmComponent->DestroyComponent();
		SpringArmComponent = nullptr;
	}

	if (CameraComponent)
	{
		CameraComponent->DestroyComponent();
		CameraComponent = nullptr;
	}

	if (InteractionComponent)
	{
		InteractionComponent->DestroyComponent();
		InteractionComponent = nullptr;
	}
}
