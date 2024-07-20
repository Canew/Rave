


#include "Character/RaveCharacterMovementComponent.h"
#include "GameplayTagAssetInterface.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "RaveGameplayTags.h"
#include "AI/RaveAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameplayAbility/RaveAttributeSet.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveCharacterMovementComponent)

URaveCharacterMovementComponent::URaveCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SetIsReplicatedByDefault(true);
    bUseAccelerationForPaths = true;

    GuardSpeedMultiplier = 0.6f;
    PatrolSpeedMultiplier = 0.6f;

    bIsSprinting = false;
    SprintSpeedMultiplier = 1.3f;
}

float URaveCharacterMovementComponent::GetMaxSpeed() const
{
    float MaxSpeed = Super::GetMaxSpeed();

    if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()))
    {
        MaxSpeed = AbilitySystemComponent->GetNumericAttribute(URaveAttributeSet::GetMovementSpeedAttribute());

        UGameplayAbility* AnimatingAbility = AbilitySystemComponent->GetAnimatingAbility();
        if (AbilitySystemComponent->HasMatchingGameplayTag(RaveGameplayTags::Character_BlockMove) || (AnimatingAbility && AnimatingAbility->AbilityTags.HasTag(RaveGameplayTags::Ability_Property_BlockMove)))
        {
            MaxSpeed = 0.0f;
        }
    }

    if (IsValid(GetPawnOwner()))
    {
        ARaveAIController* AIController = Cast<ARaveAIController>(GetPawnOwner()->GetController());
        if (IsValid(AIController))
        {
            ERaveBotBehaviorState BehaviorState = AIController->GetBehaviorState();
            if (BehaviorState == ERaveBotBehaviorState::Guard)
            {
                MaxSpeed *= GuardSpeedMultiplier;
            }
            else if (BehaviorState == ERaveBotBehaviorState::Patrol)
            {
                MaxSpeed *= PatrolSpeedMultiplier;

            }
        }
    }

    if (IsSprinting())
    {
        MaxSpeed *= SprintSpeedMultiplier;
    }

    return MaxSpeed;
}

float URaveCharacterMovementComponent::GetMaxAcceleration() const
{
    if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()))
    {
        if (AbilitySystemComponent->HasMatchingGameplayTag(RaveGameplayTags::Character_ZeroAcceleration))
        {
            return 0.0f;
        }
    }

    return IsSprinting() ? 2.0f * Super::GetMaxAcceleration() : Super::GetMaxAcceleration();
}

bool URaveCharacterMovementComponent::HandlePendingLaunch()
{
    if (!PendingLaunchVelocity.IsZero() && HasValidData())
    {
        Velocity = PendingLaunchVelocity;
        PendingLaunchVelocity = FVector::ZeroVector;
        bForceNextFloorCheck = true;
        return true;
    }

    return false;
}

void URaveCharacterMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(URaveCharacterMovementComponent, bIsSprinting);
}

bool URaveCharacterMovementComponent::IsSprinting() const
{
    return bIsSprinting;
}

void URaveCharacterMovementComponent::Sprint()
{
    bIsSprinting = true;
}

void URaveCharacterMovementComponent::UnSprint()
{
    bIsSprinting = false;
}
