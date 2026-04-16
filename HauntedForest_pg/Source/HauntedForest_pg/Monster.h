#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Monster.generated.h"

// Forward declaration
class ATriggerVolume;

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
public:
    void SetEscapeVolume(class ATriggerVolume* Volume);

};