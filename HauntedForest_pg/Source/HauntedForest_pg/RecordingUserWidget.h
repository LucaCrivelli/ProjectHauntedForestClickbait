#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RecordingUserWidget.generated.h"

class UProgressBar;

UCLASS()
class HAUNTEDFOREST_PG_API URecordingUserWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    void SetRecordingProgress(float Percent);

protected:

    UPROPERTY(meta = (BindWidget))
    UProgressBar* RecordingBar;
};