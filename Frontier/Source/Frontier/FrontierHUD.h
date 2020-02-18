// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "FrontierHUD.generated.h"

class AFrontierCharacter;

/**
 * 
 */
UCLASS()
class FRONTIER_API AFrontierHUD : public AHUD
{
	GENERATED_BODY()
	
public:
    AFrontierHUD();

    void BeginSelect();
    void EndSelect();
    TArray<AFrontierCharacter*> GetCharactersInSelection();

    void DrawHUD() override;

    UPROPERTY(EditAnywhere)
    float SelectionThickness = 2.0f;

    UPROPERTY(EditAnywhere)
    int32 DashLength = 6;

    UPROPERTY(EditAnywhere)
    FLinearColor SelectionBoxColour = FColor::White;

private:
    void DrawSelectionRectangle(float X1, float Y1, float X2, float Y2);
    void GetCharactersInSelectionRectangle(const FVector2D& FirstPoint, const FVector2D& SecondPoint, TArray<AFrontierCharacter*>& OutActors);

    bool bSelecting = false;

    float SelStartX, SelStartY;
    float SelEndX, SelEndY;

    UPROPERTY()
    TArray<AFrontierCharacter*> SelectedCharacters;
};
