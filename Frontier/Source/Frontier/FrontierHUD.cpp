// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#include "FrontierHUD.h"
#include "GameFramework/PlayerController.h"
#include "EngineUtils.h"
#include "FrontierCharacter.h"
#include "Frontier.h"

AFrontierHUD::AFrontierHUD()
{

}

void AFrontierHUD::BeginSelect()
{
    bSelecting = true;
    GetOwningPlayerController()->GetMousePosition(SelStartX, SelStartY);
}

void AFrontierHUD::EndSelect()
{
    bSelecting = false;
}

TArray<AFrontierCharacter*> AFrontierHUD::GetCharactersInSelection()
{
    return SelectedCharacters;
}

void AFrontierHUD::DrawHUD()
{
    if (bSelecting)
    {
        GetOwningPlayerController()->GetMousePosition(SelEndX, SelEndY);
        DrawSelectionRectangle(SelStartX, SelStartY, SelEndX, SelEndY);

        GetCharactersInSelectionRectangle(
            FVector2D(SelStartX, SelStartY),
            FVector2D(SelEndX, SelEndY),
            SelectedCharacters
        );
    }
}

void AFrontierHUD::DrawSelectionRectangle(float X1, float Y1, float X2, float Y2)
{
    if (X1 > X2) Swap(X1, X2);
    if (Y1 > Y2) Swap(Y1, Y2);

    auto DrawXLine = [&](int32 Y) {
        bool Odd = false;

        for (float X = X1; X < X2; X += DashLength)
        {
            Odd = !Odd;
            float EndX = (X + DashLength > X2) ? X2 : X + DashLength;

            if (Odd) DrawLine(X, Y, EndX, Y, SelectionBoxColour, SelectionThickness);
        }
    };

    auto DrawYLine = [&](int32 X) {
        bool Odd = false;

        for (float Y = Y1; Y < Y2; Y += DashLength)
        {
            Odd = !Odd;
            float EndY = (Y + DashLength > Y2) ? Y2 : Y + DashLength;

            if (Odd) DrawLine(X, Y, X, EndY, SelectionBoxColour, SelectionThickness);
        }
    };

    DrawYLine(X1);
    DrawXLine(Y1);
    DrawYLine(X2);
    DrawXLine(Y2);
}

void AFrontierHUD::GetCharactersInSelectionRectangle(const FVector2D& FirstPoint, const FVector2D& SecondPoint, TArray<AFrontierCharacter*>& OutActors)
{
    OutActors.Reset();

    FBox2D SelectionRectangle(ForceInit);

    SelectionRectangle += FirstPoint;
    SelectionRectangle += SecondPoint;

    const FVector BoundsPointMapping[8] =
    {
        FVector(1.f, 1.f, 0.0f),
        FVector(1.f, 1.f, 0.0f),
        FVector(1.f, -1.f, 0.0f),
        FVector(1.f, -1.f, 0.0f),
        FVector(-1.f, 1.f, 0.0f),
        FVector(-1.f, 1.f, 0.0f),
        FVector(-1.f, -1.f, 0.0f),
        FVector(-1.f, -1.f, 0.0f)
    };

    for (TActorIterator<AFrontierCharacter> Itr(GetWorld(), AFrontierCharacter::StaticClass()); Itr; ++Itr)
    {
        AFrontierCharacter* EachActor = *Itr;

        const FBox EachActorBounds = EachActor->GetRootComponent()->Bounds.GetBox();
        const FVector BoxCenter = EachActorBounds.GetCenter();
        const FVector BoxExtents = EachActorBounds.GetExtent();

        FBox2D ActorBox2D(ForceInit);

        for (uint8 BoundsPointItr = 0; BoundsPointItr < 8; BoundsPointItr++)
        {
            const FVector ProjectedWorldLocation = Project(BoxCenter + (BoundsPointMapping[BoundsPointItr] * BoxExtents));
            ActorBox2D += FVector2D(ProjectedWorldLocation.X, ProjectedWorldLocation.Y);
        }

        /*Draw2DLine(ActorBox2D.Min.X, ActorBox2D.Min.Y, ActorBox2D.Max.X, ActorBox2D.Min.Y, FColor::Red);
        Draw2DLine(ActorBox2D.Max.X, ActorBox2D.Min.Y, ActorBox2D.Max.X, ActorBox2D.Max.Y, FColor::Red);
        Draw2DLine(ActorBox2D.Min.X, ActorBox2D.Min.Y, ActorBox2D.Min.X, ActorBox2D.Max.Y, FColor::Red);
        Draw2DLine(ActorBox2D.Max.X, ActorBox2D.Max.Y, ActorBox2D.Min.X, ActorBox2D.Max.Y, FColor::Red);*/

        if (SelectionRectangle.IsInside(ActorBox2D))
        {
            OutActors.Add(EachActor);
        }
    }
}