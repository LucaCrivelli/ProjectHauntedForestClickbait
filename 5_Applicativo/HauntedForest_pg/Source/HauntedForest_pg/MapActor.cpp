#include "MapActor.h"
#include "Blueprint/UserWidget.h"

// per disabilitare solo i movimenti del player
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


AMapActor::AMapActor()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AMapActor::BeginPlay()
{
    Super::BeginPlay();

    if (MapWidgetClass)
    {
        MapWidget = CreateWidget<UUserWidget>(GetWorld(), MapWidgetClass);

        if (MapWidget)
        {
            MapWidget->AddToViewport();
            MapWidget->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}

void AMapActor::ToggleMap()
{
    if (!MapWidget) return;

    bIsOpen = !bIsOpen;

    MapWidget->SetVisibility(
        bIsOpen ? ESlateVisibility::Visible : ESlateVisibility::Hidden
    );

    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    APawn* PlayerPawn = PC ? PC->GetPawn() : nullptr;

    if (PC && PlayerPawn)
    {
        ACharacter* Character = Cast<ACharacter>(PlayerPawn);
        if (bIsOpen)
        {
            PC->SetInputMode(FInputModeGameAndUI());
            PC->bShowMouseCursor = true;

            // blocca movimento del player
            if (Character)
            {
                Character->GetCharacterMovement()->DisableMovement();
            }
        }
        else
        {
            PC->SetInputMode(FInputModeGameOnly());
            PC->bShowMouseCursor = false;

            // riattiva il movimento del player
            if (Character)
            {
                Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
            }
        }
    }
}