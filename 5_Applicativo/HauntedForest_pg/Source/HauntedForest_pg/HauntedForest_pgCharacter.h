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
class UFlashLightComponent;        // AGGIUNTO
class UInputMappingContext;        // AGGIUNTO
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  Personaggio giocabile
 */
UCLASS(abstract)
class AHauntedForest_pgCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: first person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* MouseLookAction;
	
public:
	AHauntedForest_pgCharacter();

protected:

	/** Called from Input Actions for movement input */
	void MoveInput(const FInputActionValue& Value);

	/** Called from Input Actions for looking input */
	void LookInput(const FInputActionValue& Value);

	/** Handles aim inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoAim(float Yaw, float Pitch);

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles jump start inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump end inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

protected:

	/** Set up input action bindings */
	virtual void BeginPlay() override;  // <<< Serve per aver "BeginPlay" nel file .cpp
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	
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


// [ CODICE RELATIVO AL FLASH ]
private:
	// Tasto: Mouse DX
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UInputAction* FlashAction;

	bool bCanFlash = true;
	FTimerHandle FlashCooldownTimer;

	void UseFlash();
};