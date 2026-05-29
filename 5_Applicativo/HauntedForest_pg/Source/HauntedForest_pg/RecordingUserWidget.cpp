#include "RecordingUserWidget.h"
#include "Components/ProgressBar.h"

void URecordingUserWidget::SetRecordingProgress(float Percent)
{
    if (RecordingBar)
    {
        RecordingBar->SetPercent(Percent);
    }
}