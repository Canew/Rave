


#include "Input/RaveInputConfig.h"
#include "InputMappingContext.h"
#include "InputAction.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveInputConfig)

URaveInputConfig::URaveInputConfig(const FObjectInitializer& ObjectInitializer)
{
}

const UInputAction* URaveInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag) const
{
	for (const FRaveInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	return nullptr;
}

const UInputAction* URaveInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag) const
{
	for (const FRaveInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	return nullptr;
}
