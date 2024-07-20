

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RaveInteractionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class URaveInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RAVE_API IRaveInteractionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, Category = "Interact")
	void Interact(AActor* Interactor);

	UFUNCTION(BlueprintNativeEvent, Category = "Interact")
	FText GetInteractionDescription() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Interact")
	bool CanInteract() const;

	/** interaction time to complete, end instantly if less or equal than 0.0f */
	UFUNCTION(BlueprintNativeEvent, Category = "Interact")
	float GetInteractionDuration() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Interact")
	void InteractionRangeEnter(AActor* Interactor);

	UFUNCTION(BlueprintNativeEvent, Category = "Interact")
	void InteractionRangeLeave(AActor* Interactor);
};
