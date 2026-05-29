#include "MonsterSpawner.h"
#include "Monster.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/TargetPoint.h" //per spawnpoint

// per area sicura
#include "Engine/TriggerVolume.h"

// per audio quando l'entita appare
#include "Kismet/GameplayStatics.h"
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

    // no spawn zone
    for (ATriggerVolume* Volume : NoSpawnVolumes)
    {
        if (Volume && Volume->IsOverlappingActor(Player))
        {
            return; // blocca spawn
        }
    }

    // safe zone check
    if (EscapeVolume && EscapeVolume->IsOverlappingActor(Player))
    {
        UE_LOG(LogTemp, Warning, TEXT("player in safezone"));
        return;
    }

    if (CurrentMonster && IsValid(CurrentMonster))
    {
        UE_LOG(LogTemp, Warning, TEXT("Mostro esistente"));
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

            // aggiunto dopo BUG dello psawn del mostro
            Monster->SetSpawner(this);

            // Passaggio del Volume
            Monster->SetEscapeVolume(EscapeVolume);

            Monster->SetSafeDoor(SafeDoor);

            CurrentMonster = Monster;

            // suono di spawn
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

// aggiunto dopo BUG safeDoor
void AMonsterSpawner::NotifyMonsterDestroyed()
{
    CurrentMonster = nullptr;
}

// per finale gioco
void AMonsterSpawner::StopSpawner()
{
    GetWorldTimerManager().ClearTimer(SpawnTimer);

    if (CurrentMonster)
    {
        CurrentMonster->Destroy();
    }
}