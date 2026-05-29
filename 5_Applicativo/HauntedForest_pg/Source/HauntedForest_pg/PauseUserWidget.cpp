#include "PauseUserWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

bool UPauseUserWidget::Initialize()
{
    bool Success = Super::Initialize();

    if (!Success) return false;

    if (ResumeButton)
    {
        ResumeButton->OnClicked.AddDynamic(this, &UPauseUserWidget::OnResumeClicked);
    }

    if (MainMenuButton)
    {
        MainMenuButton->OnClicked.AddDynamic(this, &UPauseUserWidget::OnMainMenuClicked);
    }

    return true;
}

void UPauseUserWidget::OnResumeClicked()
{
    UGameplayStatics::SetGamePaused(GetWorld(), false);

    RemoveFromParent();

    APlayerController* PC = GetWorld()->GetFirstPlayerController();

    if (PC)
    {
        PC->SetInputMode(FInputModeGameOnly());
        PC->bShowMouseCursor = false;
    }
}

void UPauseUserWidget::OnMainMenuClicked()
{
    UGameplayStatics::SetGamePaused(GetWorld(), false);

    UGameplayStatics::OpenLevel(this, FName("MainMenu"));
}