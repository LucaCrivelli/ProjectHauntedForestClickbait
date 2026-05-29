#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseUserWidget.generated.h"

class UButton;

UCLASS()
class HAUNTEDFOREST_PG_API UPauseUserWidget : public UUserWidget
{
    GENERATED_BODY()

protected:

    virtual bool Initialize() override;

    // Bottone Resume
    UPROPERTY(meta = (BindWidget))
    UButton* ResumeButton;

    // Bottone Main Menu
    UPROPERTY(meta = (BindWidget))
    UButton* MainMenuButton;

    UFUNCTION()
    void OnResumeClicked();

    UFUNCTION()
    void OnMainMenuClicked();
};