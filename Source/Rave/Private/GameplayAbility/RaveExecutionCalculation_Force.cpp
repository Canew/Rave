


#include "GameplayAbility/RaveExecutionCalculation_Force.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GenericTeamAgentInterface.h"
#include "AbilitySystemComponent.h"
#include "RaveGameplayTags.h"
#include "Animations/RaveAnimInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveExecutionCalculation_Force)

URaveExecutionCalculation_Force::URaveExecutionCalculation_Force()
{
}

void URaveExecutionCalculation_Force::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContext* Context = Spec.GetContext().Get();
	check(Context);

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BaseForce = Spec.GetSetByCallerMagnitude(RaveGameplayTags::Data_Force, false, 0.0f);
	float HitStunDuration = Spec.GetSetByCallerMagnitude(RaveGameplayTags::Data_Duration, false, 0.0f);

	const AActor* EffectCauser = Context->GetEffectCauser();
	const FHitResult* HitActorResult = Context->GetHitResult();

	AActor* HitActor = nullptr;
	FVector ImpactLocation = FVector::ZeroVector;
	FVector ImpactNormal = FVector::ZeroVector;
	FVector StartTrace = FVector::ZeroVector;
	FVector EndTrace = FVector::ZeroVector;

	// Calculation of hit actor, surface, zone, and distance all rely on whether the calculation has a hit result or not.
	// Effects just being added directly w/o having been targeted will always come in without a hit result, which must default
	// to some fallback information.
	if (HitActorResult)
	{
		const FHitResult& CurHitResult = *HitActorResult;
		HitActor = CurHitResult.HitObjectHandle.FetchActor();
		if (HitActor)
		{
			ImpactLocation = CurHitResult.ImpactPoint;
			ImpactNormal = CurHitResult.ImpactNormal;
			StartTrace = CurHitResult.TraceStart;
			EndTrace = CurHitResult.TraceEnd;
		}
	}

	// Handle case of no hit result or hit result not actually returning an actor
	UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();
	UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	if (!HitActor)
	{
		HitActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->GetAvatarActor_Direct() : nullptr;
		if (HitActor)
		{
			ImpactLocation = HitActor->GetActorLocation();
		}
	}

	// Apply rules for team damage/self damage/etc...
	float DamageInteractionAllowedMultiplier = 1.0f;
	if (HitActor)
	{
		const IGenericTeamAgentInterface* GenericTeamAgentInterface = Cast<IGenericTeamAgentInterface>(EffectCauser);
		if (ensure(GenericTeamAgentInterface))
		{
			ETeamAttitude::Type TeamAttitude = GenericTeamAgentInterface->GetTeamAttitudeTowards(*HitActor);
			switch (TeamAttitude)
			{
			case ETeamAttitude::Friendly:
				DamageInteractionAllowedMultiplier = 0.0f;
				break;

			case ETeamAttitude::Hostile:
			case ETeamAttitude::Neutral:
				DamageInteractionAllowedMultiplier = 1.0f;
				break;
			}
		}
	}

	// Determine distance
	double Distance = WORLD_MAX;

	if (Context->HasOrigin())
	{
		Distance = FVector::Dist(Context->GetOrigin(), ImpactLocation);
	}
	else if (EffectCauser)
	{
		Distance = FVector::Dist(EffectCauser->GetActorLocation(), ImpactLocation);
	}
	else
	{
		ensureMsgf(false, TEXT("Damage Calculation cannot deduce a source location for damage coming from %s; Falling back to WORLD_MAX dist!"), *GetPathNameSafe(Spec.Def));
	}

	// Apply ability source modifiers
	float DistanceAttenuation = 1.0f;
	//DistanceAttenuation = FMath::Max(DistanceAttenuation, 0.0f);

	// Clamping is done when damage is converted to -force
	const float ForceDone = FMath::Max(BaseForce * DistanceAttenuation * DamageInteractionAllowedMultiplier, 0.0f);

	FVector LaunchDirection = (HitActor->GetActorLocation() - EffectCauser->GetActorLocation());
	LaunchDirection.Z = 0.0f;
	LaunchDirection.Normalize();

	if (ForceDone > 0.0f)
	{
		if (SourceAbilitySystemComponent && TargetAbilitySystemComponent)
		{
			if (HitStunEffectClass && HitStunDuration > 0.0f)
			{
				FGameplayEffectSpecHandle EffectSpecHandle = SourceAbilitySystemComponent->MakeOutgoingSpec(HitStunEffectClass, 1.0f, SourceAbilitySystemComponent->MakeEffectContext());
				if (EffectSpecHandle.IsValid())
				{
					EffectSpecHandle.Data->SetDuration(HitStunDuration, true);
					SourceAbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*EffectSpecHandle.Data.Get(), TargetAbilitySystemComponent);

					TargetAbilitySystemComponent->CurrentMontageStop(0.0f);
				}
			}

			HitStunDuration = HitStunDuration == 0.0f ? KINDA_SMALL_NUMBER : HitStunDuration;
			if (ACharacter* HitCharacter = Cast<ACharacter>(HitActor))
			{
				if (UCharacterMovementComponent* CharacterMovementComponent = HitCharacter->GetCharacterMovement())
				{
					if (URaveAnimInstance* RaveAnimInstance = Cast<URaveAnimInstance>(HitCharacter->GetMesh()->GetAnimInstance()))
					{
						RaveAnimInstance->PushDisableRootMotion(TargetAbilitySystemComponent->GetCurrentMontage(), HitStunDuration);
					}

					CharacterMovementComponent->StopMovementImmediately();
					CharacterMovementComponent->AddImpulse(ForceDone * LaunchDirection, true);
				}
			}
		}
	}
#endif
}
