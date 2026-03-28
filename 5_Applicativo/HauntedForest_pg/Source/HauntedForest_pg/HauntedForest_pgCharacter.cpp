// Copyright Epic Games, Inc. All Rights Reserved.

#include "HauntedForest_pgCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HauntedForest_pg.h"

#include "EnhancedInputSubsystems.h" // Input
#include "FlashLightComponent.h"



AHauntedForest_pgCharacter::AHauntedForest_pgCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	// Create the first person mesh that will be viewed only by this character's owner
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	// Create the Camera Component	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	// FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	// Telecamera NON attaccata allo scheletro non esistente
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(0.f, 0.f, 64.f)); // Altezza degli occhi

	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;

	// [ TORCIA ]
	FlashlightComponent = CreateDefaultSubobject<UFlashLightComponent>(TEXT("FlashLightComponent"));
	FlashlightComponent->SetupAttachment(FirstPersonCameraComponent); // attacca componente torcia alla camera
}


void AHauntedForest_pgCharacter::BeginPlay()
{
	Super::BeginPlay(); // Chiama BeginPlay della classe "[...].h"

	// Associa il Mapping Context al Character
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			// Aggiungi solo il mapping che vuoi usare
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AHauntedForest_pgCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{

		UE_LOG(LogTemp, Warning, TEXT("Azioni caricate."));

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AHauntedForest_pgCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AHauntedForest_pgCharacter::DoJumpEnd);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AHauntedForest_pgCharacter::MoveInput);

		// Looking/Aiming
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AHauntedForest_pgCharacter::LookInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AHauntedForest_pgCharacter::LookInput);

		// TORCIA - Tasto F
		if (FlashlightAction) 
		{
			UE_LOG(LogTemp, Warning, TEXT("Flash salvato!"));
			EnhancedInputComponent->BindAction(FlashlightAction, ETriggerEvent::Started, this, &AHauntedForest_pgCharacter::ToggleFlashlight);
		}

		// FLASH - Tasto DX del Mouse
		if (FlashAction)
		{
			EnhancedInputComponent->BindAction(FlashAction, ETriggerEvent::Started, this, &AHauntedForest_pgCharacter::UseFlash);
		}
	}
	else
	{
		UE_LOG(LogHauntedForest_pg, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AHauntedForest_pgCharacter::MoveInput(const FInputActionValue& Value)
{
	// get the Vector2D move axis
	FVector2D MovementVector = Value.Get<FVector2D>();

	// pass the axis values to the move input
	DoMove(MovementVector.X, MovementVector.Y);

}

void AHauntedForest_pgCharacter::LookInput(const FInputActionValue& Value)
{
	// get the Vector2D look axis
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// pass the axis values to the aim input
	DoAim(LookAxisVector.X, LookAxisVector.Y);

}

void AHauntedForest_pgCharacter::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		// pass the rotation inputs
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AHauntedForest_pgCharacter::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		// pass the move inputs
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void AHauntedForest_pgCharacter::DoJumpStart()
{
	// pass Jump to the character
	Jump();
}

void AHauntedForest_pgCharacter::DoJumpEnd()
{
	// pass StopJumping to the character
	StopJumping();
}

// [ Implementazione del metodo "ToggleFlashlight()" ]
void AHauntedForest_pgCharacter::ToggleFlashlight()
{

	UE_LOG(LogTemp, Warning, TEXT("ToggleFlashlight called!"));


	if (FlashlightComponent)
	{
		FlashlightComponent->ToggleFlashlight();
	}
}

void AHauntedForest_pgCharacter::UseFlash()
{
	// La funzione "IsLightOn()" è in "FlashLightComponent.h"
	if (!bCanFlash || !FlashlightComponent->IsLightOn()) {
		return;
	} 

	if (FlashlightComponent) {
		FlashlightComponent->DoFlash();
	}

	// Variabile esterna, se il componente torcia non esiste, il cooldown può funzionare.
	bCanFlash = false;

	GetWorld()->GetTimerManager().SetTimer(
		FlashCooldownTimer,
		[this]()
		{
			bCanFlash = true;
			UE_LOG(LogTemp, Warning, TEXT("Flash disponibile"));
		},
		30.0f, 	// Tempo di cooldown (30 secondi)
		false
	);
}