#include "ParanormalPhenomenon.h"

AParanormalPhenomenon::AParanormalPhenomenon()
{
    PrimaryActorTick.bCanEverTick = true;

    // Root
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    // Mesh
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(Root);
}

void AParanormalPhenomenon::BeginPlay()
{
    Super::BeginPlay();

    StartLocation = GetActorLocation();
}

void AParanormalPhenomenon::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    RunningTime += DeltaTime;

    FVector NewLocation = StartLocation;

    // Floating

    if (bFloating)
    {
        float ZOffset =
            FMath::Sin(RunningTime * FloatSpeed)
            * FloatAmplitude;

        NewLocation.Z += ZOffset;
    }

    // Orbit

    if (bOrbit)
    {
        OrbitAngle += OrbitSpeed * DeltaTime;

        float X =
            FMath::Cos(FMath::DegreesToRadians(OrbitAngle))
            * OrbitRadius;

        float Y =
            FMath::Sin(FMath::DegreesToRadians(OrbitAngle))
            * OrbitRadius;

        NewLocation.X += X;
        NewLocation.Y += Y;
    }

    SetActorLocation(NewLocation);

    // Rotation

    if (bRotate)
    {
        AddActorLocalRotation(
            FRotator(
                0.f,
                RotationSpeed * DeltaTime,
                0.f
            )
        );
    }
}

void AParanormalPhenomenon::Capture()
{
    if (bCaptured) return;

    bCaptured = true;

    Destroy();
}