#include "Monster.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "NavigationSystem.h"
// Include necessario
#include "Engine/TriggerVolume.h"

#include "DoorActor.h"

#include "MonsterSpawner.h"

#include "HauntedForest_pgCharacter.h"

AMonster::AMonster()
{
    PrimaryActorTick.bCanEverTick = true;

    //per movimento
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    AIControllerClass = AAIController::StaticClass();
}

void AMonster::BeginPlay()
{
    Super::BeginPlay();

    GetWorldTimerManager().SetTimer(
        MoveTimer,
        this,
        &AMonster::UpdateMovement,
        0.5f, // ogni mezzo secondo
        true
    );
}

void AMonster::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!Target || !bIsActive) return;

    // Guarda il player
    FRotator LookAt = (Target->GetActorLocation() - GetActorLocation()).Rotation();
    SetActorRotation(FRotator(0.f, LookAt.Yaw, 0.f));

    // se il player è nell area sicura l entita scompare
    if (EscapeVolume && EscapeVolume->IsOverlappingActor(Target))
    {
        if (SafeDoor && !SafeDoor->IsOpen() && !SafeDoor->bIsMoving)
        {
            //controllo aggiunto dopo bug SafeDoor
            if (Spawner) {
                Spawner->NotifyMonsterDestroyed();
            }
            UE_LOG(LogTemp, Warning, TEXT("Distrutto per porta"));
            Destroy();
            return;
        }
    }

    // distanza player
    float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
    if (Distance < DestroyDistance)
    {
        if (Spawner) {
            Spawner->NotifyMonsterDestroyed();
        }

        AHauntedForest_pgCharacter* Player =
            Cast<AHauntedForest_pgCharacter>(Target);

        if (Player)
        {
            Player->KillPlayer();
        }

        Destroy();
    }
}

void AMonster::SetTarget(APawn* NewTarget)
{
    Target = NewTarget;
}

void AMonster::OnFlashed()
{
    bIsActive = false;

    // aggiunto per bug dello spawn del mostro
    if (Spawner) {
        Spawner->NotifyMonsterDestroyed();
    }

    // si puo aggiungere effetto dissolvenza qui
    Destroy();
}

//Nuova per Tick
void AMonster::UpdateMovement()
{
    if (!Target || !bIsActive) return;

    AAIController* AI = Cast<AAIController>(GetController());
    if (!AI) return;

    //Modificato con aggiunte per risolvere bug
    // Ottieni Nav System
    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    if (!NavSys) return;

    FVector TargetLocation = Target->GetActorLocation();
    FNavLocation NavLocation;

    // Proietta il punto del player sulla NavMesh
    bool bFound = NavSys->ProjectPointToNavigation(
        TargetLocation,
        NavLocation,
        FVector(1000.f, 1000.f, 1000.f) // raggio di ricerca
    );

    if (bFound)
    {
        // Muovi verso il punto valido sulla NavMesh
        AI->MoveToLocation(NavLocation.Location, 5.0f);
    }
    else
    {
        // il mostro si ferma e guarda il player
        FVector Direction = (Target->GetActorLocation() - GetActorLocation()).Rotation().Vector();
        SetActorRotation(Direction.Rotation());

        // fallback: prova comunque verso il player
        //AI->MoveToActor(Target, 5.0f);
    }

    /*if (AI)
    {
        AI->MoveToActor(Target, 5.0f);
    }*/
}

void AMonster::SetEscapeVolume(ATriggerVolume* Volume)
{
    EscapeVolume = Volume;
}

void AMonster::SetSafeDoor(ADoorActor* Door)
{
    SafeDoor = Door;
}

// Aggiunto dopo BUG safeDoor
void AMonster::SetSpawner(AMonsterSpawner* InSpawner)
{
    Spawner = InSpawner;
}