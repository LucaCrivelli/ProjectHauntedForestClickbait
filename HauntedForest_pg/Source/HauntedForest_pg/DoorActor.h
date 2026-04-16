// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DoorActor.generated.h"

UCLASS()
class HAUNTEDFOREST_PG_API ADoorActor : public AActor
{
    GENERATED_BODY()

public:
    ADoorActor();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // Componenti
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* DoorMesh;

    // Sound Door
    UPROPERTY(EditAnywhere, Category = "Audio")
    USoundBase* OpenSound;

    UPROPERTY(EditAnywhere, Category = "Audio")
    USoundBase* CloseSound;

    //blocco input
    UPROPERTY()
    bool bIsMoving;

    // Rotazione
    FRotator ClosedRotation;
    FRotator OpenRotation;

    bool bIsOpen;

    void ToggleDoor();
};