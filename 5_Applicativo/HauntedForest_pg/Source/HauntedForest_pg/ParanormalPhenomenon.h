#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ParanormalPhenomenon.generated.h"

UCLASS()
class HAUNTEDFOREST_PG_API AParanormalPhenomenon : public AActor
{
    GENERATED_BODY()

public:
    AParanormalPhenomenon();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // Mesh

    UPROPERTY(VisibleAnywhere)
    USceneComponent* Root;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* Mesh;

    // Floating

    UPROPERTY(EditAnywhere, Category = "Movement")
    bool bFloating = true;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float FloatAmplitude = 20.f;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float FloatSpeed = 2.f;

    // Rotation

    UPROPERTY(EditAnywhere, Category = "Movement")
    bool bRotate = false;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float RotationSpeed = 50.f;

    // Orbit

    UPROPERTY(EditAnywhere, Category = "Movement")
    bool bOrbit = false;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float OrbitRadius = 100.f;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float OrbitSpeed = 50.f;

    // Capture

    UPROPERTY(EditAnywhere, Category = "Phenomenon | Capture")
    float RequiredRecordingTime = 3.f;

    UPROPERTY(EditAnywhere, Category = "Phenomenon | Capture")
    float MaxDistance = 1200.f; // distanza minima alla quale il player deve stare per filmare

    bool bCaptured = false;

    void Capture();

private:

    FVector StartLocation;

    float RunningTime = 0.f;

    float OrbitAngle = 0.f;
};