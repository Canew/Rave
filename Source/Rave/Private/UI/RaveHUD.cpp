


#include "UI/RaveHUD.h"
#include "UI/RaveUserWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RaveHUD)

void ARaveHUD::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* OwningPlayerController = GetOwningPlayerController();
	if (!OwningPlayerController)
	{
		return;
	}

	for (TSubclassOf<URaveUserWidget> DefaultWidgetClass : DefaultWidgetClasses)
	{
		URaveUserWidget* DefaultWidget = CreateWidget<URaveUserWidget>(OwningPlayerController, DefaultWidgetClass);
		if (DefaultWidget)
		{
			DefaultWidget->AddToViewport();
			DefaultWidgets.Add(DefaultWidget);
		}
	}
}
