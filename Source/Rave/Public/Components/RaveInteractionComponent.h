

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RaveInteractionComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnInteractionActorChanged, AActor*);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RAVE_API URaveInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URaveInteractionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Interact")
	virtual AActor* GetInteractionActor() const;

	UFUNCTION(BlueprintCallable, Category = "Interact")
	virtual bool CanInteract() const;

	UFUNCTION(BlueprintCallable, Category = "Interact")
	virtual void Interact();

protected:
	virtual TWeakObjectPtr<AActor> FindInteractableActor();

	virtual void InitializeInteractWidget();
	virtual void ShowInteractionWidget(TWeakObjectPtr<AActor> Actor, FName SocketName = NAME_None);
	virtual void HideInteractionWidget();

	UFUNCTION()
	void InteractionWidgetFadeOutCompletedCallback();

public:
	FOnInteractionActorChanged OnInteractionActorChanged;

protected:
	UPROPERTY()
	TWeakObjectPtr<AActor> InteractionActor;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionRange;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Widget")
	TSubclassOf<class UUserWidget> InteractionWidgetClass;

	UPROPERTY()
	class UWidgetComponent* InteractionWidgetComponent;

	TSet<AActor*> LastInteractionCandidateActors;
};
