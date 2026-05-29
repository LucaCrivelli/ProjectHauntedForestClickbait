// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "HauntedForest_pgCharacter.generated.h" // DEVE ESSERE SEMPRE L'ULTIMO DEGLI INCLUDE

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UFlashLightComponent;
class UInputMappingContext;
class ADoorActor;
class UUserWidget;
class AMapActor;
struct FInputActionValue;
class UPauseUserWidget; // menu di pausa
class UDeathUserWidget; // menu di morte
class AParanormalPhenomenon; // per progerssi di gioco
class URecordingUserWidget; // per barra di caricamento

class AMonsterSpawner;
class USoundBase;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  Personaggio giocabile
 */
UCLASS(abstract)
class AHauntedForest_pgCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: first person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* MouseLookAction;

public:
	AHauntedForest_pgCharacter();

	//per icona porta
	// Tick per line trace continuo
	virtual void Tick(float DeltaTime) override;

protected:

	/** Called from Input Actions for movement input */
	void MoveInput(const FInputActionValue& Value);

	/** Called from Input Actions for looking input */
	void LookInput(const FInputActionValue& Value);

	/** Handles aim inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoAim(float Yaw, float Pitch);

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles jump start inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoJumpStart();

	/** Handles jump end inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoJumpEnd();

	// INTERACT (porta)
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* InteractAction;

	// Porta attualmente guardata
	UPROPERTY()
	ADoorActor* CurrentDoor = nullptr;

	// UI
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> InteractWidgetClass;

	UPROPERTY()
	UUserWidget* InteractWidget;

	// per mappa
	UPROPERTY()
	AMapActor* CurrentMap = nullptr;

	// usare la  seguente variabile impedisce un BUG
	UPROPERTY()
	AMapActor* OpenedMap = nullptr;

	//per menu di pausa
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UPauseUserWidget> PauseWidgetClass;

	UPROPERTY()
	UPauseUserWidget* PauseWidget;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* PauseAction;

	void TogglePauseMenu();


	//per menu di morte
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UDeathUserWidget> DeathWidgetClass;

	UPROPERTY()
	UDeathUserWidget* DeathWidget;

	// Filmaggio

	UPROPERTY()
	AParanormalPhenomenon* CurrentPhenomenon;

	float RecordingTime = 0.f;

	float LostSightTime = 0.f;

	UPROPERTY(EditAnywhere, Category = "Recording")
	float MaxLostSightTime = 0.5f;

	// UI barra di caricamento
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<URecordingUserWidget> RecordingWidgetClass;

	UPROPERTY()
	URecordingUserWidget* RecordingWidget;

	// Per finale gioco
	UPROPERTY(EditAnywhere, Category = "Paranormal")
	int32 TotalPhenomena = 3;

	UPROPERTY(VisibleAnywhere, Category = "Paranormal")
	int32 CapturedPhenomena = 0;

	UPROPERTY(EditAnywhere, Category = "Paranormal")
	AParanormalPhenomenon* FinalPhenomenon;

	UPROPERTY(EditAnywhere, Category = "Paranormal")
	AActor* FinalEventPoint;

	bool bFinalEventStarted = false;

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* FinalEventSound;

	UPROPERTY(EditAnywhere, Category = "Paranormal")
	AMonsterSpawner* MonsterSpawner;

protected:

	/** Set up input action bindings */
	virtual void BeginPlay() override;  // <<< Serve per aver "BeginPlay" nel file .cpp
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	//aggiunto dopo
	void Interact();

public:

	/** Returns the first person mesh **/
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	/** Returns first person camera component **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }


	// [CODICE AGGIUNTO PER TORCIA]
private:
	// Componente torcia
	UPROPERTY(VisibleAnywhere)
	UFlashLightComponent* FlashlightComponent;

	// Input action Enhanced Input (tasto F)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UInputAction* FlashlightAction; // Collega asset di Unreal al codice C++

	// Mapping context per il player
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

public:
	// Metodo da chiamare la pressione del tasto "F"
	void ToggleFlashlight();

	//metodo che si collega al mostro
	void KillPlayer();

	// per finale gioco
	void StartFinalEvent();

	void TriggerEnding();


	// [ CODICE RELATIVO AL FLASH ]
private:
	// Tasto: Mouse DX
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UInputAction* FlashAction;

	bool bCanFlash = true;
	FTimerHandle FlashCooldownTimer;

	void UseFlash();
};