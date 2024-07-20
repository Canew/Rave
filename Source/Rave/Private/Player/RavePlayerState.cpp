


#include "Player/RavePlayerState.h"
#include "GameplayAbility/RaveAbilitySystemComponent.h"
#include "GameplayAbility/RaveAttributeSet.h"
#include "GameplayAbility/RaveAbilitySet.h"
#include "GameplayAbility/RaveGameplayAbility.h"
#include "Player/RavePlayerController.h"
#include "Character/RaveCharacter.h"
#include "Inventory/RaveInventorySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RavePlayerState)

ARavePlayerState::ARavePlayerState(const FObjectInitializer& ObjectInitializer)
{
	// AbilitySystemComponent needs to be updated at a high frequency.
	NetUpdateFrequency = 100.0f;

	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<URaveAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	CreateDefaultSubobject<URaveAttributeSet>(TEXT("AttributeSet"));

	InventorySystemComponent = ObjectInitializer.CreateDefaultSubobject<URaveInventorySystemComponent>(this, TEXT("InventorySystemComponent"));
	InventorySystemComponent->SetIsReplicated(true);
}

void ARavePlayerState::PreInitializeComponents()
{
	Super::InitializeComponents();
}

void ARavePlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);

	if (AActor* AvatarActor = GetPawn())
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, AvatarActor);
	}
}

ARavePlayerController* ARavePlayerState::GetRavePlayerController() const
{
	return Cast<ARavePlayerController>(GetOwner());
}

UAbilitySystemComponent* ARavePlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ARavePlayerState::SetCharacterSetting(const URaveCharacterSetting* InCharacterSetting)
{
	check(InCharacterSetting);
	
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	
	CharacterSetting = InCharacterSetting;

	ARaveCharacter* RaveCharacter = GetPawn<ARaveCharacter>();
	if (RaveCharacter)
	{
		RaveCharacter->SetCharacterSetting(InCharacterSetting);
	}

	AbilitySystemComponent->ClearAllAbilities();
	for (const URaveAbilitySet* AbilitySet : CharacterSetting->AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
		}
	}

	CharacterSettingChanged.Broadcast(InCharacterSetting);
}

URaveInventorySystemComponent* ARavePlayerState::GetInventorySystemComponent() const
{
	return InventorySystemComponent;
}
