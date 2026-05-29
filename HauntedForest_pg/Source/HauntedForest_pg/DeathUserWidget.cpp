#include "DeathUserWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

//per audio morte
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"

bool UDeathUserWidget::Initialize()
{
    bool Success = Super::Initialize();

    if (!Success) return false;

    if (RetryButton)
    {
        RetryButton->OnClicked.AddDynamic(this, &UDeathUserWidget::OnRetryClicked);
    }

    if (MainMenuButton)
    {
        MainMenuButton->OnClicked.AddDynamic(this, &UDeathUserWidget::OnMainMenuClicked);
    }

    return true;
}

void UDeathUserWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Audio di morte

    if (DeathSound)
    {
        DeathAudioComponent = UGameplayStatics::SpawnSound2D(
            this,
            DeathSound
        );

        if (DeathAudioComponent)
        {
            // continua anche in pausa
            DeathAudioComponent->SetUISound(true);
        }
    }
}

void UDeathUserWidget::OnRetryClicked()
{

    // Fade out audio
    if (DeathAudioComponent)
    {
        DeathAudioComponent->FadeOut(0.2f, 0.0f);
    }

    UGameplayStatics::SetGamePaused(GetWorld(), false);

    APlayerController* PC = GetWorld()->GetFirstPlayerController();

    if (PC)
    {
        PC->SetInputMode(FInputModeGameOnly());
        PC->bShowMouseCursor = false;
    }

    UGameplayStatics::OpenLevel(
        this,
        FName(*GetWorld()->GetName())
    );
}

void UDeathUserWidget::OnMainMenuClicked()
{
    // Fade out audio
    if (DeathAudioComponent)
    {
        DeathAudioComponent->FadeOut(0.2f, 0.0f);
    }

    UGameplayStatics::SetGamePaused(GetWorld(), false);

    UGameplayStatics::OpenLevel(this, FName("MainMenu"));
}