

#pragma once

#include "CoreMinimal.h"
#include "UI/RaveUserWidget.h"
#include "RaveUserWidget_StatisticsToolTip.generated.h"

/**
 * 
 */
UCLASS()
class RAVE_API URaveUserWidget_StatisticsToolTip : public URaveUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> StatisticsToolTipNameTextBlock;
};
