#include "MonsterSpawner.h"
#include "Monster.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/TargetPoint.h" //per spawnpoint

// Include necessario
#include "Engine/TriggerVolume.h"

// per audio quando l'entita appare
#include "Sound/SoundBase.h"

AMonsterSpawner::AMonsterSpawner()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AMonsterSpawner::BeginPlay()
{
    Super::BeginPlay();

    GetWorld()->GetTimerManager().SetTimer(
        SpawnTimer,
        this,
        &AMonsterSpawner::SpawnMonster,
        SpawnInterval,
        true
    );
}

void AMonsterSpawner::SpawnMonster()
{
    APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

    // safe zone check
    if (EscapeVolume && EscapeVolume->IsOverlappingActor(Player))
    {
        return;
    }

    if (CurrentMonster && IsValid(CurrentMonster))
    {
        return;
    }

    if (SpawnPoints.Num() > 0)
    {
        int32 Index = FMath::RandRange(0, SpawnPoints.Num() - 1);

        // Controllo sicurezza (evita crash se null)
        if (!SpawnPoints[Index]) return;

        FVector SpawnLocation = SpawnPoints[Index]->GetActorLocation();
        FRotator SpawnRotation = SpawnPoints[Index]->GetActorRotation(); // meglio della ZeroRotator

        AMonster* Monster = GetWorld()->SpawnActor<AMonster>(MonsterClass, SpawnLocation, SpawnRotation);

        if (Monster)
        {
            // Target player
            Monster->SetTarget(Player);

            // PASSAGGIO DEL VOLUME (QUESTA È LA PARTE IMPORTANTE)
            Monster->SetEscapeVolume(EscapeVolume);

            CurrentMonster = Monster;

            // SUONO DI SPAWN
            if (SpawnSound)
            {
                UGameplayStatics::PlaySoundAtLocation(
                    this,
                    SpawnSound,
                    SpawnLocation
                );
            }
        }
    }
}