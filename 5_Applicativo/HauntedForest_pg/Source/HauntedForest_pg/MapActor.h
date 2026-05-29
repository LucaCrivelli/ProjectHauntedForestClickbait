#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapActor.generated.h"

class UUserWidget;
class APlayerController;
class APawn;
class ACharacter;
class UCharacterMovementComponent;

UCLASS()
class HAUNTEDFOREST_PG_API AMapActor : public AActor
{
    GENERATED_BODY()

public:
    AMapActor();

    void ToggleMap();

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(EditAnywhere)
    TSubclassOf<UUserWidget> MapWidgetClass;

    UPROPERTY()
    UUserWidget* MapWidget;

    bool bIsOpen = false;

public:
    bool IsOpen() const { return bIsOpen; }
};