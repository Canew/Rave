


#include "UI/RaveUserWidget_HitCircle.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/Border.h"
#include "Kismet/KismetMathLibrary.h"
#include "Animation/WidgetAnimation.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveUserWidget_HitCircle)

void URaveUserWidget_HitCircle::NativeConstruct()
{
	Super::NativeConstruct();

	HitCircleMaterialInstance = HitCircleBorder->GetDynamicMaterial();

	DisplayDuration = 1.5f;
	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		PlayerController->GetWorldTimerManager().SetTimer(DisplayTimerHandle, this, &ThisClass::HandleDisplayDurationFinished, DisplayDuration, false);
	}
}

void URaveUserWidget_HitCircle::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (APawn* OwningPawn = GetOwningPlayerPawn())
	{
		FVector ControlVector = GetOwningPlayer()->GetControlRotation().Vector();
		FVector OwningPawnLocation = OwningPawn->GetActorLocation();

		FVector InstigatorToPawnVector = (OwningPawnLocation - HitLocation).GetSafeNormal();
		float ForwardDotInstigatorToPawn = FVector::DotProduct(ControlVector, InstigatorToPawnVector);

		float HitCircleDegree = UKismetMathLibrary::DegAtan2((ControlVector.X * InstigatorToPawnVector.Y) - (ControlVector.Y * InstigatorToPawnVector.X), ForwardDotInstigatorToPawn);
		float HitCircleAngleValue = UKismetMathLibrary::MapRangeClamped(HitCircleDegree, 180.0f, -180.0f, 0.0f, 1.0f);

		if (HitCircleMaterialInstance)
		{
			HitCircleMaterialInstance->SetScalarParameterValue(TEXT("Angle"), HitCircleAngleValue);
		}
	}
}

void URaveUserWidget_HitCircle::SetHitLocation(const FVector& InHitLocation)
{
	HitLocation = InHitLocation;
}

void URaveUserWidget_HitCircle::HandleDisplayDurationFinished()
{
	RemoveFromParent();
}
