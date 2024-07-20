

#pragma once

#include "CoreMinimal.h"
#include "Character/RaveCharacter.h"
#include "RaveBotCharacter.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API ARaveBotCharacter : public ARaveCharacter
{
	GENERATED_BODY()
	
public:
	ARaveBotCharacter(const FObjectInitializer& ObjectInitializer);
};
