#include "DoorActor.h"
#include "Kismet/GameplayStatics.h"

ADoorActor::ADoorActor()
{
    PrimaryActorTick.bCanEverTick = true;

    DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
    RootComponent = DoorMesh;

    bIsOpen = false;
    bIsMoving = false;
}

void ADoorActor::BeginPlay()
{
    Super::BeginPlay();

    ClosedRotation = GetActorRotation();
    OpenRotation = ClosedRotation + FRotator(0.f, 90.f, 0.f); // sempre stessa direzione
}

void ADoorActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FRotator Target = bIsOpen ? OpenRotation : ClosedRotation;

    FRotator NewRotation = FMath::RInterpTo(
        GetActorRotation(),
        Target,
        DeltaTime,
        2.0f
    );

    SetActorRotation(NewRotation);

    // Controllo fine animazione
    if (bIsMoving)
    {
        float Difference = FMath::Abs((GetActorRotation() - Target).Yaw);

        if (Difference < 1.0f) // soglia
        {
            bIsMoving = false;

            // Suono di CHIUSURA qui
            if (!bIsOpen && CloseSound)
            {
                UGameplayStatics::PlaySoundAtLocation(this, CloseSound, GetActorLocation());
            }
        }
    }
}

void ADoorActor::ToggleDoor()
{
    if (bIsMoving) return;

    bIsMoving = true;
    bIsOpen = !bIsOpen;

    if (bIsOpen)
    {
        if (OpenSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, OpenSound, GetActorLocation());
        }
    }
}