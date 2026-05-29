#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MonsterSpawner.generated.h"

class AMonster;
class ATargetPoint;
class ATriggerVolume;

UCLASS()
class HAUNTEDFOREST_PG_API AMonsterSpawner : public AActor
{
    GENERATED_BODY()

public:
    AMonsterSpawner();

protected:
    virtual void BeginPlay() override;

    void SpawnMonster();

    FTimerHandle SpawnTimer;

    AMonster* CurrentMonster;

public:
    UPROPERTY(EditAnywhere)
    TSubclassOf<AMonster> MonsterClass;

    UPROPERTY(EditAnywhere)
    float SpawnInterval = 15.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    TArray<ATargetPoint*> SpawnPoints;

    UPROPERTY(EditInstanceOnly, Category = "Spawning")
    class ATriggerVolume* EscapeVolume;

    UPROPERTY(EditAnywhere, Category = "Audio")
    USoundBase* SpawnSound;

    UPROPERTY(EditAnywhere, Category = "Spawning")
    class ADoorActor* SafeDoor;

    // per area che non fa apparire il mostro se il player è al suo interno
    UPROPERTY(EditInstanceOnly, Category = "Spawning")
    TArray<ATriggerVolume*> NoSpawnVolumes;

    //aggiunto per BUG riscontrato di spawn del mostro
    void NotifyMonsterDestroyed();

    // per finale gioco
    void StopSpawner();
};