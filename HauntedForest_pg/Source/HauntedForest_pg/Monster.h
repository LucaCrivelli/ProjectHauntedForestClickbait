#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Monster.generated.h"

// Forward declaration
class ATriggerVolume;
class ADoorActor;
// aggiunto dopo Bug SafeDoor
class AMonsterSpawner;

// aggiunto per morte player
class HauntedForest_pgCharacter;

UCLASS()
class HAUNTEDFOREST_PG_API AMonster : public ACharacter
{
    GENERATED_BODY()

public:
    AMonster();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

public:
    void SetTarget(APawn* NewTarget);
    void OnFlashed();

private:
    APawn* Target;

    UPROPERTY(EditAnywhere)
    float MoveSpeed = 400.f;

    UPROPERTY(EditAnywhere)
    float DestroyDistance = 200.f;

    // Puntatore al volume dove il mostro deve rimanere
    UPROPERTY(EditAnywhere)
    ATriggerVolume* EscapeVolume;

    bool bIsActive = true;

    FTimerHandle MoveTimer;

    void UpdateMovement();

    //per assegnare porta da controllare se chiusa
    UPROPERTY(EditAnywhere)
    ADoorActor* SafeDoor;

    // aggiunto dopo Bug SafeDoor

public:
    void SetEscapeVolume(class ATriggerVolume* Volume);

    void SetSafeDoor(ADoorActor* Door);

    // aggiunto dopo Bug SafeDoor
    AMonsterSpawner* Spawner;
    void SetSpawner(AMonsterSpawner* InSpawner);
};