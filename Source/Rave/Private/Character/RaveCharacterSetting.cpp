


#include "Character/RaveCharacterSetting.h"
#include "GameplayAbility/RaveAbilitySet.h"
#include "Input/RaveInputConfig.h"
#include "Equipments/RaveEquipmentDefinition.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveCharacterSetting)

URaveCharacterSetting::URaveCharacterSetting(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InputConfig = nullptr;
}
