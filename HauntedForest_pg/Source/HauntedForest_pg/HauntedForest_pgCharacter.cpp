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

//Aggiunti dopo per porta
#include "DoorActor.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

#include "Monster.h"



AHauntedForest_pgCharacter::AHauntedForest_pgCharacter()
{
	//aggiunto dopo
	PrimaryActorTick.bCanEverTick = true;

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

	// UI porta aggiunto dopo
	if (InteractWidgetClass)
	{
		InteractWidget = CreateWidget<UUserWidget>(GetWorld(), InteractWidgetClass);
		if (InteractWidget)
		{
			InteractWidget->AddToViewport();
			InteractWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}


//Aggiunta dopo per porta
void AHauntedForest_pgCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Start = FirstPersonCameraComponent->GetComponentLocation();
	FVector End = Start + (FirstPersonCameraComponent->GetForwardVector() * 300.f);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	CurrentDoor = nullptr;

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		ADoorActor* Door = Cast<ADoorActor>(Hit.GetActor());
		if (Door)
		{
			CurrentDoor = Door;
		}
	}

	// UI (opzionale)
	if (InteractWidget)
	{
		InteractWidget->SetVisibility(CurrentDoor ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
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

		//Aggiunto dopo per porta
		if (InteractAction)
		{
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AHauntedForest_pgCharacter::Interact);
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
	if (!bCanFlash || !FlashlightComponent->IsLightOn()) {
		return;
	}

	// Effetto visivo
	FlashlightComponent->DoFlash();

	// ===== LOGICA GAMEPLAY =====
	FVector PlayerLocation = GetActorLocation();
	FVector Forward = FirstPersonCameraComponent->GetForwardVector();

	TArray<AActor*> Monsters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMonster::StaticClass(), Monsters);

	for (AActor* Actor : Monsters)
	{
		AMonster* Monster = Cast<AMonster>(Actor);
		if (!Monster) continue;

		FVector ToMonster = (Monster->GetActorLocation() - PlayerLocation).GetSafeNormal();

		float Dot = FVector::DotProduct(Forward, ToMonster);

		float Distance = FVector::Dist(PlayerLocation, Monster->GetActorLocation());

		// PARAMETRI (puoi tweakarli)
		float MaxDistance = 1200.f;
		float MinDot = 0.7f; // più alto = cono più stretto

		//per flash contro mostro
		if (Dot > MinDot && Distance < MaxDistance)
		{
			// LINE TRACE (controlla se c'è un muro in mezzo)
			FHitResult Hit;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(this);

			bool bHit = GetWorld()->LineTraceSingleByChannel(
				Hit,
				PlayerLocation,
				Monster->GetActorLocation(),
				ECC_Visibility,
				Params
			);

			// Se colpisce qualcosa che NON è il mostro → bloccato
			if (bHit && Hit.GetActor() != Monster)
			{
				continue;
			}

			// Flash valido
			Monster->OnFlashed();
		}
	}

	// ===== COOLDOWN =====
	bCanFlash = false;

	GetWorld()->GetTimerManager().SetTimer(
		FlashCooldownTimer,
		[this]()
		{
			bCanFlash = true;
			UE_LOG(LogTemp, Warning, TEXT("Flash disponibile"));
		},
		30.0f,
		false
	);
}

//Aggiunta dopo per porta
void AHauntedForest_pgCharacter::Interact()
{
	if (CurrentDoor)
	{
		CurrentDoor->ToggleDoor();
	}
}