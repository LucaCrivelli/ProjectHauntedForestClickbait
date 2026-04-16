#include "Monster.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "NavigationSystem.h"
// Include necessario
#include "Engine/TriggerVolume.h"

// Per risolvere Bug (collegamento build.cs)
#include "NavigationSystem.h"

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
        Destroy();
        return;
    }

    // distanza player
    float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
    if (Distance < DestroyDistance)
    {
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

    // si puo aggiungere effetto dissolvenza qui
    Destroy();

    /*
    * versione migliore
    * void AMonster::OnFlashed()
{
    bIsActive = false;

    // Nasconde subito il mostro
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);

    // Delay leggerissimo (utile per effetti futuri)
    GetWorldTimerManager().SetTimerForNextTick([this]()
    {
        Destroy();
    });
}
    */
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