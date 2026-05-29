// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "BorderTrigger.generated.h"

/**
 * 
 */
UCLASS()
class HAUNTEDFOREST_PG_API ABorderTrigger : public ATriggerBox
{
	GENERATED_BODY()
	
	public:
		ABorderTrigger();

	private:
		bool bIsProcessingOverlap = false; // Impedisce attivazioni multiple

	protected:
		virtual void BeginPlay() override;

		UFUNCTION()
		void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);
};
