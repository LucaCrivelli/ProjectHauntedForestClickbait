#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeathUserWidget.generated.h"

class UButton;
class UAudioComponent;
class USoundBase;

UCLASS()
class HAUNTEDFOREST_PG_API UDeathUserWidget : public UUserWidget
{
    GENERATED_BODY()

protected:

    virtual bool Initialize() override;

    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    UButton* RetryButton;

    UPROPERTY(meta = (BindWidget))
    UButton* MainMenuButton;

    // Per audio morte

    UPROPERTY(EditAnywhere, Category = "Audio")
    USoundBase* DeathSound;

    UPROPERTY()
    UAudioComponent* DeathAudioComponent;

    // buttons

    UFUNCTION()
    void OnRetryClicked();

    UFUNCTION()
    void OnMainMenuClicked();
};
