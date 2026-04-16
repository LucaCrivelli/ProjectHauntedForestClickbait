// Fill out your copyright notice in the Description page of Project Settings.


#include "BorderTrigger.h"

#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "TimerManager.h"

#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"

#include "GameFramework/CharacterMovementComponent.h"

ABorderTrigger::ABorderTrigger()
{
    // Ora il nome coincide con la classe definita nell'header
    OnActorBeginOverlap.AddDynamic(this, &ABorderTrigger::OnOverlapBegin);
}

void ABorderTrigger::BeginPlay()
{
    Super::BeginPlay();
}

void ABorderTrigger::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
    // Se stiamo gia processando un teletrasporto, esci subito
    if (bIsProcessingOverlap) return;

    ACharacter* PlayerCharacter = Cast<ACharacter>(OtherActor);
    if (PlayerCharacter && PlayerCharacter->IsPlayerControlled())
    {
        APlayerController* PC = Cast<APlayerController>(PlayerCharacter->GetController());
        if (PC && PC->PlayerCameraManager)
        {
            bIsProcessingOverlap = true; // Blocca il trigger

            // 1. Inizia il Fade Nero (1.0 secondi)
            PC->PlayerCameraManager->StartCameraFade(0.0f, 1.0f, 1.0f, FLinearColor::Black, true, true);

            // 2. Blocca input e AZZERA velocita (fondamentale per lo spostamento)
            PlayerCharacter->DisableInput(PC);
            PlayerCharacter->GetCharacterMovement()->StopMovementImmediately();

            FTimerHandle TimerHandle;
            GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, PlayerCharacter, PC]()
                {
                    if (PlayerCharacter && PC)
                    {
                        // 1. Ruota la visuale di 180 (come prima)
                        FRotator NewRot = PC->GetControlRotation();
                        NewRot.Yaw += 180.0f;
                        PC->SetControlRotation(NewRot);

                        // 2. RIPOSIZIONAMENTO PERPENDICOLARE
                        // Invece di usare la direzione del player, usiamo la rotazione del TRIGGER stesso
                        // GetActorForwardVector() del Trigger punta "fuori" o "dentro" la faccia del muro
                        FVector TriggerForward = GetActorForwardVector();

                        // Calcoliamo la nuova posizione: partiamo da dove siamo e ci spostiamo 
                        // lungo l'asse del Trigger, non quello del giocatore.
                        FVector NewLocation = PlayerCharacter->GetActorLocation() + (TriggerForward * 800.0f);

                        // 3. Applichiamo lo spostamento
                        PlayerCharacter->SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);

                        // 4. Reset Fade e Input
                        PC->PlayerCameraManager->StartCameraFade(1.0f, 0.0f, 1.0f, FLinearColor::Black, false);
                        PlayerCharacter->EnableInput(PC);

                        FTimerHandle ResetHandle;
                        GetWorld()->GetTimerManager().SetTimer(ResetHandle, [this]() { bIsProcessingOverlap = false; }, 1.0f, false);
                    }
                }, 1.2f, false); // Aspettiamo che il fade sia completato (1.0s + 0.2s di margine)
        }
    }
}