


#include "Components/RaveInteractionComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Animation/AnimInstance.h"
#include "Interfaces/RaveInteractionInterface.h"
#include "UI/RaveUserWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveInteractionComponent)

URaveInteractionComponent::URaveInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f;

	InteractionRange = 150.0f;
}

void URaveInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeInteractWidget();
}

void URaveInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* LastInteractionActor = InteractionActor.Get();
	if (CanInteract())
	{
		InteractionActor = FindInteractableActor();
		if (IsValid(InteractionActor.Get()))
		{
			if (LastInteractionActor != InteractionActor)
			{
				ShowInteractionWidget(InteractionActor);

				OnInteractionActorChanged.Broadcast(InteractionActor.Get());
			}
		}
		else if (IsValid(LastInteractionActor))
		{
			HideInteractionWidget();
		}
	}
	else if (IsValid(LastInteractionActor))
	{
		InteractionActor = nullptr;
		HideInteractionWidget();

		for (AActor* LastInteractionCandidateActor : LastInteractionCandidateActors)
		{
			IRaveInteractionInterface::Execute_InteractionRangeLeave(LastInteractionCandidateActor, GetOwner());
		}
		LastInteractionCandidateActors.Empty();
	}
}

AActor* URaveInteractionComponent::GetInteractionActor() const
{
	return InteractionActor.Get();
}

bool URaveInteractionComponent::CanInteract() const
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter || !OwnerCharacter->GetMesh()->GetAnimInstance())
	{
		return false;
	}

	if (OwnerCharacter->GetCharacterMovement()->IsFalling() || OwnerCharacter->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
	{
		return false;
	}

	return true;
}

void URaveInteractionComponent::Interact()
{
	if (IsValid(InteractionActor.Get()) && InteractionActor->GetClass()->ImplementsInterface(URaveInteractionInterface::StaticClass()))
	{
		IRaveInteractionInterface::Execute_Interact(InteractionActor.Get(), GetOwner());
	}
}

TWeakObjectPtr<AActor> URaveInteractionComponent::FindInteractableActor()
{
	UWorld* World = GetOwner()->GetWorld();
	if (!World)
	{
		return nullptr;
	}

	TArray<FOverlapResult> OverlapResults;
	World->OverlapMultiByProfile(OverlapResults, GetOwner()->GetActorLocation(), FQuat::Identity, TEXT("OverlapAll"), FCollisionShape::MakeSphere(InteractionRange));

	AActor* ClosestInteractableActor = nullptr;
	TSet<AActor*> LeaveInteractionCandidateActors = LastInteractionCandidateActors;
	for (const FOverlapResult& OverlapResult : OverlapResults)
	{
		AActor* OverlapActor = OverlapResult.GetActor();
		if (!OverlapActor)
		{
			continue;
		}

		if (OverlapActor->GetClass()->ImplementsInterface(URaveInteractionInterface::StaticClass()))
		{
			if (!IRaveInteractionInterface::Execute_CanInteract(OverlapActor))
			{
				continue;
			}

			LeaveInteractionCandidateActors.Remove(OverlapActor);
			if (!LastInteractionCandidateActors.Contains(OverlapActor))
			{
				IRaveInteractionInterface::Execute_InteractionRangeEnter(OverlapActor, GetOwner());
				LastInteractionCandidateActors.Add(OverlapActor);
			}

			if (!IsValid(InteractionActor.Get()) || !IsValid(ClosestInteractableActor) || GetOwner()->GetDistanceTo(OverlapActor) < GetOwner()->GetDistanceTo(ClosestInteractableActor))
			{
				ClosestInteractableActor = OverlapActor;
			}
		}
	}

	for (AActor* LeaveInteractionCandidateActor : LeaveInteractionCandidateActors)
	{
		IRaveInteractionInterface::Execute_InteractionRangeLeave(LeaveInteractionCandidateActor, GetOwner());
		LastInteractionCandidateActors.Remove(LeaveInteractionCandidateActor);
	}

	return ClosestInteractableActor;
}

void URaveInteractionComponent::InitializeInteractWidget()
{
	if (APawn* Owner = GetOwner<APawn>())
	{
		AController* Controller = Owner->GetController();
		if (Controller && Controller->IsLocalPlayerController())
		{
			InteractionWidgetComponent = NewObject<UWidgetComponent>(this, FName(TEXT("InteractionWidget")));
			if (InteractionWidgetComponent)
			{
				InteractionWidgetComponent->RegisterComponent();
				InteractionWidgetComponent->SetWidgetClass(InteractionWidgetClass);
				InteractionWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
				InteractionWidgetComponent->SetDrawAtDesiredSize(true);
				InteractionWidgetComponent->InitWidget();
				if (URaveUserWidget* InteractionWidget = Cast<URaveUserWidget>(InteractionWidgetComponent->GetUserWidgetObject()))
				{
					InteractionWidget->Initialize();
					InteractionWidget->OnFadeOutAnimationFinished.AddDynamic(this, &ThisClass::InteractionWidgetFadeOutCompletedCallback);
				}

				InteractionWidgetComponent->SetVisibility(false);
			}
		}
	}
}

void URaveInteractionComponent::ShowInteractionWidget(TWeakObjectPtr<AActor> Actor, FName SocketName)
{
	if (!IsValid(InteractionWidgetComponent))
	{
		InitializeInteractWidget();
	}

	if (IsValid(InteractionWidgetComponent))
	{
		if (Actor.IsValid())
		{
			InteractionWidgetComponent->SetVisibility(true);
			InteractionWidgetComponent->AttachToComponent(Actor->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
		}

		URaveUserWidget* InteractionWidget = Cast<URaveUserWidget>(InteractionWidgetComponent->GetUserWidgetObject());
		if (InteractionWidget)
		{
			InteractionWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
			InteractionWidget->PlayFadeInAnimation();
		}
	}
}

void URaveInteractionComponent::HideInteractionWidget()
{
	if (IsValid(InteractionWidgetComponent))
	{
		URaveUserWidget* InteractionWidget = Cast<URaveUserWidget>(InteractionWidgetComponent->GetUserWidgetObject());
		if (InteractionWidget)
		{
			InteractionWidget->PlayFadeOutAnimation();
		}
	}
}

void URaveInteractionComponent::InteractionWidgetFadeOutCompletedCallback()
{
	if (IsValid(InteractionWidgetComponent) && !IsValid(InteractionActor.Get()))
	{
		InteractionWidgetComponent->SetVisibility(false);
		InteractionWidgetComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	}
}
