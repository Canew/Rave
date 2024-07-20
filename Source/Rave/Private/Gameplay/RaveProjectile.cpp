


#include "Gameplay/RaveProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GenericTeamAgentInterface.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveProjectile)

// Sets default values
ARaveProjectile::ARaveProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ProjectileCollision"));
	ProjectileCollision->SetCollisionProfileName(TEXT("RaveProjectile"));
	RootComponent = ProjectileCollision;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetGenerateOverlapEvents(false);
	ProjectileMesh->SetCollisionProfileName(TEXT("NoCollision"));
	ProjectileMesh->SetupAttachment(GetRootComponent());

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bInitialVelocityInLocalSpace = false;
}

void ARaveProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ProjectileCollision->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::HandleProjectileBeginOverlap);
}

// Called when the game starts or when spawned
void ARaveProjectile::BeginPlay()
{
	Super::BeginPlay();

}

void ARaveProjectile::AddImpactGameplayEffectSpec(FGameplayEffectSpecHandle ImpactGameplayEffectSpecHandle)
{
	ImpactGameplayEffectSpecHandles.Add(ImpactGameplayEffectSpecHandle);
}

void ARaveProjectile::AddImpactGameplayEvent(FGameplayTag EventTag)
{
	ImpactGameplayEventTags.AddTag(EventTag);
}

void ARaveProjectile::HandleProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == this || OtherActor == GetOwner())
	{
		return;
	}

	UAbilitySystemComponent* OwnerAbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	UAbilitySystemComponent* OtherAbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor);

	if (OwnerAbilitySystemComponent)
	{
		if (OtherAbilitySystemComponent)
		{
			for (FGameplayEffectSpecHandle ImpactGameplayEffectSpecHandle : ImpactGameplayEffectSpecHandles)
			{
				OwnerAbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*ImpactGameplayEffectSpecHandle.Data, OtherAbilitySystemComponent);
			}

			FGameplayEventData ImpactGameplayEventData;
			ImpactGameplayEventData.Instigator = GetInstigator();
			ImpactGameplayEventData.ContextHandle = OwnerAbilitySystemComponent->MakeEffectContext();
			ImpactGameplayEventData.ContextHandle.AddHitResult(SweepResult);
			ImpactGameplayEventData.ContextHandle.AddSourceObject(this);
			ImpactGameplayEventData.ContextHandle.AddOrigin(GetActorLocation());
			for (FGameplayTag ImpactGameplayEventTag : ImpactGameplayEventTags)
			{
				OtherAbilitySystemComponent->HandleGameplayEvent(ImpactGameplayEventTag, &ImpactGameplayEventData);
			}
		}

		if (ImpactGameplayCueTag.IsValid())
		{
			FGameplayCueParameters GameplayCueParameters;
			GameplayCueParameters.EffectContext = OwnerAbilitySystemComponent->MakeEffectContext();
			GameplayCueParameters.Location = (bFromSweep ? SweepResult.ImpactPoint : FVector_NetQuantize(GetActorLocation()));
			GameplayCueParameters.Normal = SweepResult.ImpactNormal;
			GameplayCueParameters.PhysicalMaterial = SweepResult.PhysMaterial;
			OwnerAbilitySystemComponent->ExecuteGameplayCue(ImpactGameplayCueTag, GameplayCueParameters);
		}
	}

	Destroy();
}
