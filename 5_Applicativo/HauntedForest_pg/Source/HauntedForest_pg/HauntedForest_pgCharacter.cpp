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

// per porta
#include "DoorActor.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

#include "Monster.h"

#include "MapActor.h"

// per menu di pausa
#include "PauseUserWidget.h"

//per morte
#include "DeathUserWidget.h" 

// per progressi
#include "ParanormalPhenomenon.h"

#include "RecordingUserWidget.h"

// per finale gioco
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "TimerManager.h"
#include "MonsterSpawner.h" 



AHauntedForest_pgCharacter::AHauntedForest_pgCharacter()
{
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

	// UI porta
	if (InteractWidgetClass)
	{
		InteractWidget = CreateWidget<UUserWidget>(GetWorld(), InteractWidgetClass);
		if (InteractWidget)
		{
			InteractWidget->AddToViewport();
			InteractWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	// UI barra di caricamento
	if (RecordingWidgetClass)
	{
		RecordingWidget =
			CreateWidget<URecordingUserWidget>(
				GetWorld(),
				RecordingWidgetClass
			);

		if (RecordingWidget)
		{
			RecordingWidget->AddToViewport();

			RecordingWidget->SetVisibility(
				ESlateVisibility::Hidden
			);
		}
	}

	// Cerca automaticamente il Fenomeno Finale nella mappa tramite il Tag
	TArray<AActor*> FoundPhenomena;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("FenomenoFinale"), FoundPhenomena);
	if (FoundPhenomena.Num() > 0)
	{
		// Castiamo l'actor trovato alla classe corretta
		FinalPhenomenon = Cast<AParanormalPhenomenon>(FoundPhenomena[0]);
	}

	// Cerca automaticamente il Target Point nella mappa tramite il Tag
	TArray<AActor*> FoundPoints;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("PuntoFinale"), FoundPoints);
	if (FoundPoints.Num() > 0)
	{
		FinalEventPoint = FoundPoints[0];
	}

	// Cerca automaticamente il Monster Spawner nella mappa
	TArray<AActor*> FoundSpawners;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMonsterSpawner::StaticClass(), FoundSpawners);
	if (FoundSpawners.Num() > 0)
	{
		MonsterSpawner = Cast<AMonsterSpawner>(FoundSpawners[0]);
	}

	// Risetta i controlli di gioco al ritorno dal Main Menu
	APlayerController* PC = Cast<APlayerController>(GetController());

	if (PC)
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->bShowMouseCursor = false;

		// forza il focus sul gioco
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
	}
}


// Per gestione lineTrace e filming fenomeni paranormali
void AHauntedForest_pgCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Start = FirstPersonCameraComponent->GetComponentLocation();
	FVector End = Start + (FirstPersonCameraComponent->GetForwardVector() * 15000.f);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	CurrentDoor = nullptr;
	CurrentMap = nullptr;
	CurrentPhenomenon = nullptr;

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		AActor* HitActor = Hit.GetActor();

		ADoorActor* Door = Cast<ADoorActor>(HitActor);
		if (Door && Hit.Distance <= 300.f)
		{
			CurrentDoor = Door;
		}

		AMapActor* Map = Cast<AMapActor>(HitActor);
		if (Map && Hit.Distance <= 300.f)
		{
			CurrentMap = Map;
		}

		AParanormalPhenomenon* Phenomenon = Cast<AParanormalPhenomenon>(HitActor);
		if (Phenomenon)
		{
			if (Hit.Distance <= Phenomenon->MaxDistance)
			{
				CurrentPhenomenon = Phenomenon;
			}
		}
	}

	// Recording

	if (CurrentPhenomenon)
	{
		LostSightTime = 0.f;

		RecordingTime += DeltaTime;

		// UI

		if (RecordingWidget)
		{
			RecordingWidget->SetVisibility(ESlateVisibility::Visible);

			float Percent = RecordingTime / CurrentPhenomenon->RequiredRecordingTime;

			RecordingWidget->SetRecordingProgress(Percent);
		}

		// Cattura

		if (RecordingTime >= CurrentPhenomenon->RequiredRecordingTime)
		{
			AParanormalPhenomenon* Captured = CurrentPhenomenon;

			CurrentPhenomenon = nullptr;
			RecordingTime = 0.f;
			LostSightTime = 0.f;

			// Nasconde UI
			if (RecordingWidget)
			{
				RecordingWidget->SetVisibility(ESlateVisibility::Hidden);

				RecordingWidget->SetRecordingProgress(0.f);
			}

			if (Captured == FinalPhenomenon)
			{
				TriggerEnding();
				Captured->Capture();
				return;
			}

			CapturedPhenomena++;
			Captured->Capture();

			if (CapturedPhenomena >= TotalPhenomena && !bFinalEventStarted)
			{
				StartFinalEvent();
			}
		}
	}
	else
	{
		// Non stai guardando il fenomeno
		LostSightTime += DeltaTime;

		// aspetta un po prima del reset UI
		if (LostSightTime >= MaxLostSightTime)
		{
			RecordingTime = 0.f;

			if (RecordingWidget)
			{
				RecordingWidget->SetVisibility(ESlateVisibility::Hidden);

				RecordingWidget->SetRecordingProgress(0.f);
			}
		}
	}

	// UI
	if (InteractWidget)
	{
		bool bMapOpen = (CurrentMap && CurrentMap->IsOpen());

		bool bCanInteract = ((CurrentDoor != nullptr) || (CurrentMap != nullptr)) && !bMapOpen;

		InteractWidget->SetVisibility(
			bCanInteract ? ESlateVisibility::Visible : ESlateVisibility::Hidden
		);
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

		// per menu di pausa
		if (PauseAction)
		{
			EnhancedInputComponent->BindAction(
				PauseAction,
				ETriggerEvent::Started,
				this,
				&AHauntedForest_pgCharacter::TogglePauseMenu
			);
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
	// se la mappa è aperta la chiude SEMPRE
	if (OpenedMap && OpenedMap->IsOpen())
	{
		OpenedMap->ToggleMap();
		OpenedMap = nullptr; // pulisce dopo la chiusura
		return;
	}

	if (CurrentDoor)
	{
		CurrentDoor->ToggleDoor();
	}
	else if (CurrentMap)
	{
		CurrentMap->ToggleMap();
		OpenedMap = CurrentMap; // salva riferimento quando la apri
	}
}

// per menu di pausa
void AHauntedForest_pgCharacter::TogglePauseMenu()
{
	APlayerController* PC = Cast<APlayerController>(GetController());

	if (!PC) return;

	bool bPaused = UGameplayStatics::IsGamePaused(GetWorld());

	if (bPaused)
	{
		UGameplayStatics::SetGamePaused(GetWorld(), false);

		if (PauseWidget)
		{
			PauseWidget->RemoveFromParent();
			PauseWidget = nullptr;
		}

		PC->SetInputMode(FInputModeGameOnly());
		PC->bShowMouseCursor = false;
	}
	else
	{
		UGameplayStatics::SetGamePaused(GetWorld(), true);

		if (PauseWidgetClass)
		{
			PauseWidget = CreateWidget<UPauseUserWidget>(PC, PauseWidgetClass);

			if (PauseWidget)
			{
				PauseWidget->AddToViewport();
			}
		}

		PC->SetInputMode(FInputModeUIOnly());
		PC->bShowMouseCursor = true;
	}
}

// per uccidere il player

void AHauntedForest_pgCharacter::KillPlayer()
{
	APlayerController* PC = Cast<APlayerController>(GetController());

	if (!PC) return;

	DisableInput(PC);

	//mette il gioco in pausa
	UGameplayStatics::SetGamePaused(GetWorld(), true);

	if (DeathWidgetClass)
	{
		DeathWidget = CreateWidget<UDeathUserWidget>(
			PC,
			DeathWidgetClass
		);

		if (DeathWidget)
		{
			DeathWidget->AddToViewport();
		}
	}

	PC->SetInputMode(FInputModeUIOnly());
	PC->bShowMouseCursor = true;
}

// finale gioco
void AHauntedForest_pgCharacter::StartFinalEvent()
{
	bFinalEventStarted = true;

	// blocca spawner
	if (MonsterSpawner)
	{
		MonsterSpawner->StopSpawner();
	}

	// fade nero
	APlayerController* PC = GetWorld()->GetFirstPlayerController();

	if (PC && PC->PlayerCameraManager)
	{
		PC->PlayerCameraManager->StartCameraFade(0.f, 1.f, 2.f, FLinearColor::Black, false, true);
	}

	// 4. delay
	FTimerHandle Handle;

	GetWorldTimerManager().SetTimer(
		Handle,
		[this, PC]()
		{
			// TELETRASPORTO
			if (FinalEventPoint)
			{
				SetActorLocation(
					FinalEventPoint->GetActorLocation()
				);
			}

			// mostra fenomeno finale
			/*if (FinalPhenomenon)
			{
				FinalPhenomenon->SetActorHiddenInGame(false);
				FinalPhenomenon->SetActorEnableCollision(true);
			} */

			// fade ritorna
			if (PC && PC->PlayerCameraManager)
			{
				PC->PlayerCameraManager->StartCameraFade(1.f, 0.f, 2.f, FLinearColor::Black, false, false);
			}

		},
		2.5f,
		false
	);
}

void AHauntedForest_pgCharacter::TriggerEnding()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();

	// suono inquietante
	if (FinalEventSound)
	{
		UGameplayStatics::PlaySound2D(this, FinalEventSound);
	}

	if (PC && PC->PlayerCameraManager)
	{
		PC->PlayerCameraManager->StartCameraFade(0.f, 1.f, 2.f, FLinearColor::Black, false, true);
	}

	FTimerHandle Handle;

	GetWorldTimerManager().SetTimer(
		Handle,
		[this]()
		{
			UGameplayStatics::OpenLevel(this, FName("MainMenu"));
		},
		3.f,
		false
	);
}