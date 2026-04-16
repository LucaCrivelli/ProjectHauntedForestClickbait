#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SpotLightComponent.h" // Serve per il fascio di luce
#include "FlashLightComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class HAUNTEDFOREST_PG_API UFlashLightComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFlashLightComponent();

	// Toggle accensione/spegnimento della torcia
	void ToggleFlashlight();
	// Attacco per il flash
	void DoFlash();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	// Componente della luce
	UPROPERTY(VisibleAnywhere) // >> Lo mostra all'interno della BluePrint visuale
		USpotLightComponent* Flashlight;

	// Stato acceso/spento
	bool bLightIsOn;

private:
	// Parametri flash
	float FlashIntensity = 2000000.f;
	float NormalIntensity = 20000.f; // Riporta la luce al valore iniziale
	float FlashDuration = 0.3;

	FTimerHandle FlashTimer;

public:
	bool IsLightOn() const { // const non applica modifiche, controlla e basta
		return bLightIsOn;
	}

	//public:	
	//	// Called every frame
	//	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};