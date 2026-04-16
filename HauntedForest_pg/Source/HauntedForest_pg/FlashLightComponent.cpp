#include "FlashLightComponent.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h" // "Visuale" del giocatore

// Sets default values for this component's properties
UFlashLightComponent::UFlashLightComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bLightIsOn = false;
}


// Called when the game starts
void UFlashLightComponent::BeginPlay()
{
	Super::BeginPlay();

	Flashlight = NewObject<USpotLightComponent>(GetOwner());
	if (!Flashlight) return;

	Flashlight->RegisterComponent();

	UCameraComponent* Camera = GetOwner()->FindComponentByClass<UCameraComponent>();
	if (Camera)
	{
		Flashlight->AttachToComponent(Camera, FAttachmentTransformRules::KeepRelativeTransform);
	}

	Flashlight->SetIntensity(20000.f);
	Flashlight->SetInnerConeAngle(10.f);
	Flashlight->SetOuterConeAngle(40.f);
	Flashlight->SetAttenuationRadius(3000.f);
	Flashlight->SetVisibility(false);

	// Nuovo per correggere bug linea nera
	Flashlight->ShadowBias = 0.5f;                // riduce artefatti su landscape
	Flashlight->ContactShadowLength = 50.f;       // migliora la transizione delle ombre
	Flashlight->SetCastShadows(true);       // assicurati che la luce generi ombre dinamiche
	//
}


void UFlashLightComponent::ToggleFlashlight()
{
	if (!Flashlight) {
		return;
	}
	// Inverto lo stato della torcia e lo applico
	bLightIsOn = !bLightIsOn;
	Flashlight->SetVisibility(bLightIsOn);
}

void UFlashLightComponent::DoFlash()
{
	// Il flash non si attiva se la torcia è spenta
	if (!Flashlight) {
		return;
	}

	Flashlight->SetIntensity(FlashIntensity);
	Flashlight->SetVisibility(true);

	GetWorld()->GetTimerManager().SetTimer(
		FlashTimer,
		[this]()
		{
			if (Flashlight)
			{
				Flashlight->SetIntensity(NormalIntensity);
				Flashlight->SetVisibility(bLightIsOn);
			}
		},
		FlashDuration, // Durata flash
		false
	);
}

// Called every frame
//void UFlashLightComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}