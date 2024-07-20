


#include "GameplayAbility/RaveExecutionCalculation_Damage.h"
#include "GameplayAbility/RaveAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GenericTeamAgentInterface.h"
#include "RaveGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveExecutionCalculation_Damage)

struct FDamageStatics
{
	FGameplayEffectAttributeCaptureDefinition BaseDamageDef;

	FDamageStatics()
	{
		BaseDamageDef = FGameplayEffectAttributeCaptureDefinition(URaveAttributeSet::GetAttackPowerAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FDamageStatics& DamageStatics()
{
	static FDamageStatics Statics;
	return Statics;
}

URaveExecutionCalculation_Damage::URaveExecutionCalculation_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().BaseDamageDef);
}

void URaveExecutionCalculation_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
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

	float BaseDamage = Spec.GetSetByCallerMagnitude(RaveGameplayTags::Data_Damage, false, 0.0f);
	//ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BaseDamageDef, EvaluateParameters, BaseDamage);

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

	// Clamping is done when damage is converted to -health
	const float DamageDone = FMath::Max(BaseDamage * DistanceAttenuation * DamageInteractionAllowedMultiplier, 0.0f);

	if (DamageDone > 0.0f)
	{
		// Apply a damage modifier, this gets turned into - health on the target
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(URaveAttributeSet::GetDamageAttribute(), EGameplayModOp::Additive, DamageDone));
	}
#endif
}
