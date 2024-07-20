


#include "GameplayAbility/Tasks/RaveAbilityTask_WaitCollisionEvent.h"
#include "Components/CapsuleComponent.h"

URaveAbilityTask_WaitCollisionEvent::URaveAbilityTask_WaitCollisionEvent()
{
	bTickingTask = true;
	HalfHeight = 0.0f;
	Radius = 0.0f;
	Duration = 0.0f;
	AccumulatedTime = 0.0f;

	bOnlyTriggerOnce = false;
	bDebug = false;
}

void URaveAbilityTask_WaitCollisionEvent::Activate()
{
	Super::Activate();

	UCapsuleComponent* CapsuleComponent = NewObject<UCapsuleComponent>(GetAvatarActor());
	if (!CapsuleComponent)
	{
		EndTask();
		return;
	}

	CollisionComponent = CapsuleComponent;
	CollisionComponent->RegisterComponent();
	CollisionComponent->SetCollisionProfileName(TEXT("RaveCollider"));
	CollisionComponent->MoveIgnoreActors = IgnoreActors;
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::HandleComponentBeginOverlap);
	CollisionComponent->SetGenerateOverlapEvents(true);

	FTransform CollisionTransform;
	if (AttachComponent.IsValid())
	{
		CollisionTransform = AttachComponent->DoesSocketExist(AttachSocketName) ? AttachComponent->GetSocketTransform(AttachSocketName) : AttachComponent->GetComponentTransform();
		CollisionTransform.SetLocation(CollisionTransform.TransformPosition(Location));
		CollisionTransform.SetRotation(CollisionTransform.TransformRotation(FQuat(Rotation)));
		
	}
	else
	{
		CollisionTransform = FTransform(Rotation, Location);
	}

	CollisionComponent->SetCapsuleSize(Radius, HalfHeight);
	CollisionComponent->SetWorldTransform(CollisionTransform);

#if WITH_EDITOR
	if (bDebug)
	{
		CollisionComponent->SetVisibility(true);
		CollisionComponent->SetHiddenInGame(false);
	}
#endif
}

void URaveAbilityTask_WaitCollisionEvent::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	AccumulatedTime += DeltaTime;
	if (Duration > 0.0f && AccumulatedTime >= Duration)
	{
		EndTask();
		return;
	}

	FTransform CollisionTransform;
	if (AttachComponent.IsValid())
	{
		CollisionTransform = AttachComponent->DoesSocketExist(AttachSocketName) ? AttachComponent->GetSocketTransform(AttachSocketName) : AttachComponent->GetComponentTransform();
		CollisionTransform.SetLocation(CollisionTransform.TransformPosition(Location));
		CollisionTransform.SetRotation(CollisionTransform.GetRotation() * FQuat(Rotation));
	}
	else
	{
		CollisionTransform = FTransform(Rotation, Location);
	}

	CollisionComponent->SetWorldTransform(CollisionTransform, true);
}

void URaveAbilityTask_WaitCollisionEvent::OnDestroy(bool bAbilityEnded)
{
	if (IsValid(CollisionComponent))
	{
		CollisionComponent->OnComponentBeginOverlap.RemoveAll(this);
		CollisionComponent->DestroyComponent();
	}

	Super::OnDestroy(bAbilityEnded);
}

URaveAbilityTask_WaitCollisionEvent* URaveAbilityTask_WaitCollisionEvent::WaitCollisionEventAtLocation(UGameplayAbility* OwningAbility, float HalfHeight, float Radius, float Duration, FVector Location, FRotator Rotation, TArray<AActor*> IgnoreActors, bool bOnlyTriggerOnce, bool bDebug)
{
	URaveAbilityTask_WaitCollisionEvent* MyObj = NewAbilityTask<URaveAbilityTask_WaitCollisionEvent>(OwningAbility);
	MyObj->HalfHeight = HalfHeight;
	MyObj->Radius = Radius;
	MyObj->Duration = Duration;
	MyObj->Location = Location;
	MyObj->Rotation = Rotation;
	MyObj->IgnoreActors = IgnoreActors;
	MyObj->bOnlyTriggerOnce = bOnlyTriggerOnce;
	MyObj->bDebug = bDebug;

	return MyObj;
}

URaveAbilityTask_WaitCollisionEvent* URaveAbilityTask_WaitCollisionEvent::WaitCollisionEventAttached(UGameplayAbility* OwningAbility, float HalfHeight, float Radius, float Duration, USceneComponent* AttachComponent, FName AttachSocketName, FVector LocationOffset, FRotator RotationOffset, TArray<AActor*> IgnoreActors, bool bOnlyTriggerOnce,  bool bDebug)
{
	URaveAbilityTask_WaitCollisionEvent* MyObj = NewAbilityTask<URaveAbilityTask_WaitCollisionEvent>(OwningAbility);
	MyObj->HalfHeight = HalfHeight;
	MyObj->Radius = Radius;
	MyObj->Duration = Duration;
	MyObj->AttachComponent = AttachComponent;
	MyObj->AttachSocketName = AttachSocketName;
	MyObj->Location = LocationOffset;
	MyObj->Rotation = RotationOffset;
	MyObj->IgnoreActors = IgnoreActors;
	MyObj->bOnlyTriggerOnce = bOnlyTriggerOnce;
	MyObj->bDebug = bDebug;

	return MyObj;
}

void URaveAbilityTask_WaitCollisionEvent::HandleComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(OtherActor) || !IsValid(OverlappedComponent) || IgnoreActors.Find(OtherActor) != INDEX_NONE || CollideActors.Find(OtherActor) != INDEX_NONE)
	{
		return;
	}

	FHitResult OutHitResult = SweepResult;

	if (!bFromSweep)
	{
		OutHitResult.Location = OtherActor->GetActorLocation();
		OutHitResult.ImpactPoint = OtherActor->GetActorLocation();
		OutHitResult.Normal = CollisionComponent->GetComponentLocation() - OtherActor->GetActorLocation();
		OutHitResult.ImpactNormal = CollisionComponent->GetComponentLocation() - OtherActor->GetActorLocation();
	}

	OutHitResult.bBlockingHit = true;
	OnCollisionDetected.Broadcast(OutHitResult);

	if (bOnlyTriggerOnce)
	{
		EndTask();
	}

	CollideActors.Add(OtherActor);
}
