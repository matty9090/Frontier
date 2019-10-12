// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FrontierPlayerController.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "AIController.h"
#include "UserWidget.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

#include "FrontierCharacter.h"
#include "FrontierPlayerState.h"
#include "Buildings/Building.h"
#include "Buildings/Barracks.h"
#include "Research.h"
#include "Frontier.h"
#include "Widgets/UI.h"
#include "RoamPawn.h"
#include "BaseResource.h"
#include "Widgets/BuildingBaseWidget.h"

AFrontierPlayerController::AFrontierPlayerController()
{
    bShowMouseCursor = true;
    bEnableMouseOverEvents = true;
    DefaultMouseCursor = EMouseCursor::Default;
}

void AFrontierPlayerController::BeginPlay()
{
    Super::BeginPlay();

    ControllerState = EControllerState::Idle;

    if (!IsRunningDedicatedServer())
    {
        ClientCreateUI();
    }
}

void AFrontierPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    InputComponent->BindAxis("Zoom", this, &AFrontierPlayerController::OnZoom);
    InputComponent->BindAxis("MoveUp", this, &AFrontierPlayerController::OnMoveUp);
    InputComponent->BindAxis("MoveRight", this, &AFrontierPlayerController::OnMoveRight);

    InputComponent->BindAction("Send", IE_Pressed, this, &AFrontierPlayerController::OnSend);
    InputComponent->BindAction("Select", IE_Pressed, this, &AFrontierPlayerController::OnSelect);
}

void AFrontierPlayerController::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);

    /* float MX, MY;
    
    if (GetMousePosition(MX, MY))
    {
        int32 VX, VY;
        GetViewportSize(VX, VY);

        if (MY <      MouseMovementPadding) OnMoveUp   ( 1.0f);
        if (MY > VY - MouseMovementPadding) OnMoveUp   (-1.0f);
        if (MX <      MouseMovementPadding) OnMoveRight(-1.0f);
        if (MX > VX - MouseMovementPadding) OnMoveRight( 1.0f);
    }*/

    if (ControllerState == EControllerState::PlacingBuilding)
    {
        if (HoveredBuilding)
        {
            FHitResult Hit;

            TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = {
                EObjectTypeQuery::ObjectTypeQuery1, // WorldStatic
            };

            if (GetHitResultUnderCursorForObjects(ObjectTypes, false, Hit))
            {
                auto BoxComponent = Cast<UBoxComponent>(HoveredBuilding->GetComponentByClass(UBoxComponent::StaticClass()));
                float Z = BoxComponent->GetScaledBoxExtent().Z;

                HoveredBuilding->SetActorLocation(Hit.Location + FVector(0.0f, 0.0f, Z));
            }
        }
    }
}

void AFrontierPlayerController::ClientCreateUI_Implementation()
{
    UI = CreateWidget<UUI>(this, UIClass, "UI");

    if (UI)
    {
        UI->AddToViewport();
    }

    AnimationFinishedEvent.BindUFunction(this, "BuildingUIAnimationFinished");
}

void AFrontierPlayerController::SetHoveredBuilding(TSubclassOf<ABuilding> BuildingType)
{
    if (HoveredBuilding)
        HoveredBuilding->Destroy();

    FTransform Transform;
    FActorSpawnParameters SpawnParams;

    HoveredBuilding = GetWorld()->SpawnActor<ABuilding>(BuildingType, Transform, SpawnParams);
    HoveredBuilding->SetActorEnableCollision(false);

    auto Mesh = HoveredBuilding->Mesh;
    
    if (Mesh)
    {
        auto NumMaterials = Mesh->GetMaterials().Num();

        for(int i = 0; i < NumMaterials; ++i)
            Mesh->SetMaterial(i, HoverMaterialGreen);
    }

    ControllerState = EControllerState::PlacingBuilding;
}

void AFrontierPlayerController::BuildingUIAnimationFinished()
{
    if(UI->bIsHidden)
        UI->ShowUI();

    SelectedBuilding = nullptr;
    SelectedBuildingUI = nullptr;
}

void AFrontierPlayerController::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    if (UI)
        UI->RemoveFromParent();

    ClientCreateUI();
}

void AFrontierPlayerController::OnRep_PlacedBuilding()
{
    if (HoveredBuilding)
    {
        HoveredBuilding->Destroy();
    }
}

void AFrontierPlayerController::OnMoveUp(float Value)
{
    if (GetPawn())
    {
        auto RoamPawn = Cast<ARoamPawn>(GetPawn());
        auto Delta = Value * MoveSpeed * MovementCurve->GetFloatValue(RoamPawn->GetCameraHeight());
        auto Location = GetPawn()->GetActorLocation() + FVector(Delta, 0.0f, 0.0f);

        RoamPawn->SetActorLocation(Location);
    }
}

void AFrontierPlayerController::OnMoveRight(float Value)
{
    if (GetPawn())
    {
        auto RoamPawn = Cast<ARoamPawn>(GetPawn());
        auto Delta = Value * MoveSpeed * MovementCurve->GetFloatValue(RoamPawn->GetCameraHeight());
        auto Location = GetPawn()->GetActorLocation() + FVector(0.0f, Delta, 0.0f);

        RoamPawn->SetActorLocation(Location);
    }
}

void AFrontierPlayerController::OnSelect()
{
    FHitResult Hit;

    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = {
        EObjectTypeQuery::ObjectTypeQuery1, // WorldStatic
        EObjectTypeQuery::ObjectTypeQuery2, // WorldDynamic
        EObjectTypeQuery::ObjectTypeQuery3  // Pawn
    };

    auto DeselectUnit = [&]() {
        if (SelectedUnit) SelectedUnit->HideOutline();
        SelectedUnit = nullptr;
    };

    auto DeselectBuilding = [&] {
        if (!UI->bIsHidden)
            UI->HideUI();

        if (SelectedBuildingUI)
        {
            SelectedBuildingUI->PlayAnimationForward(SelectedBuildingUI->GetHideAnimation());
            SelectedBuildingUI->UnbindFromAnimationFinished(SelectedBuildingUI->GetHideAnimation(), AnimationFinishedEvent);
        }

        if(SelectedBuilding)
            SelectedBuilding->HideOutline();

        SelectedBuilding = nullptr;
        SelectedBuildingUI = nullptr;
    };

    auto PS = Cast<AFrontierPlayerState>(PlayerState);

    if (ControllerState == EControllerState::PlacingBuilding)
    {
        if (PS->CanCreateBuilding(HoveredBuilding->GetClass()))
        {
            ServerSpawnBuilding(HoveredBuilding->GetClass(), HoveredBuilding->GetActorLocation(), FRotator::ZeroRotator);
            ControllerState = EControllerState::Idle;

            if (HasAuthority())
            {
                HoveredBuilding->Destroy();
                HoveredBuilding = nullptr;
            }
        }
    }
    else
    {
        bool Selected = false;

        if (GetHitResultUnderCursorForObjects(ObjectTypes, false, Hit))
        {
            auto Unit = Cast<AFrontierCharacter>(Hit.Actor);

            if (Unit && Unit->Player == PS)
            {
                UE_LOG(LogFrontier, Display, TEXT("Selected unit"));

                if (Unit != SelectedUnit)
                {
                    DeselectUnit();
                    DeselectBuilding();

                    SelectedUnit = Unit;
                    SelectedUnit->ShowOutline();
                }

                Selected = true;
            }

            auto Building = Cast<ABuilding>(Hit.Actor);

            if (!Selected && Building && Building->Player == PS)
            {
                UE_LOG(LogFrontier, Display, TEXT("Selected building"));

                if (Building != SelectedBuilding)
                {
                    DeselectUnit();
                    DeselectBuilding();

                    SelectedBuilding = Building;
                    SelectedBuilding->ShowOutline();

                    SelectedBuildingUI = CreateWidget<UBuildingBaseWidget>(this, SelectedBuilding->Widget);
                    SelectedBuildingUI->BuildingActor = SelectedBuilding;
                    SelectedBuildingUI->AddToViewport();
                    SelectedBuildingUI->PlayAnimationForward(SelectedBuildingUI->GetShowAnimation());
                    SelectedBuildingUI->BindToAnimationFinished(SelectedBuildingUI->GetHideAnimation(), AnimationFinishedEvent);
                }
                
                Selected = true;
            }

            auto Resource = Cast<ABaseResource>(Hit.Actor);

            if (!Selected && Resource)
            {
                UE_LOG(LogFrontier, Display, TEXT("Selected resource"));
                Selected = true;
            }
        }

        if (!Selected)
        {
            if (UI->bIsHidden)
                UI->ShowUI();

            DeselectUnit();

            if (SelectedBuilding)
            {
                SelectedBuilding->HideOutline();
                SelectedBuildingUI->PlayAnimationForward(SelectedBuildingUI->GetHideAnimation());
            }

            SelectedBuilding = nullptr, SelectedBuildingUI = nullptr;
        }
    }
}

void AFrontierPlayerController::OnSend()
{
    if (SelectedUnit)
    {
        FHitResult Hit;

        TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = {
            EObjectTypeQuery::ObjectTypeQuery1, // WorldStatic
            EObjectTypeQuery::ObjectTypeQuery2, // WorldDynamic
        };

        if (GetHitResultUnderCursorForObjects(ObjectTypes, false, Hit))
        {
            ServerMoveAIToLocation(SelectedUnit, Hit.Location, Hit.Actor.Get());
        }
    }
    else if (HoveredBuilding)
    {
        HoveredBuilding->Destroy();
        HoveredBuilding = nullptr;
    }
}

void AFrontierPlayerController::OnZoom(float Value)
{
    if (GetPawn())
    {
        auto RoamPawn = Cast<ARoamPawn>(GetPawn());
        auto Zoom = ZoomCurve->GetFloatValue(RoamPawn->GetCameraHeight()) * ZoomSpeed * Value * -1.0f;

        Zoom = FMath::Clamp(Zoom + RoamPawn->GetCameraHeight(), MinZoom, MaxZoom);
        RoamPawn->SetCameraHeight(Zoom);
    }
}

bool AFrontierPlayerController::ServerSpawnBuilding_Validate(TSubclassOf<ABuilding> Type, FVector Location, FRotator Rotation)
{
    return Cast<AFrontierPlayerState>(PlayerState)->CanCreateBuilding(Type);
}

void AFrontierPlayerController::ServerSpawnBuilding_Implementation(TSubclassOf<ABuilding> Type, FVector Location, FRotator Rotation)
{
    // TODO: Move to PlayerState?
    if (Type)
    {
        auto PS = Cast<AFrontierPlayerState>(PlayerState);
        PS->Resources -= Type.GetDefaultObject()->Cost;

        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        PlacedBuilding = GetWorld()->SpawnActor<ABuilding>(
            Type,
            Location,
            Rotation,
            SpawnParams
        );

        PlacedBuilding->Player = PS;
    }
}

bool AFrontierPlayerController::ServerMoveAIToLocation_Validate(AFrontierCharacter* AI, FVector Location, AActor* Resource)
{
    return true;
}

void AFrontierPlayerController::ServerMoveAIToLocation_Implementation(AFrontierCharacter* AI, FVector Location, AActor* Object)
{
    AI->MoveToLocation(Location, Object);
}

bool AFrontierPlayerController::ServerQueueUnit_Validate(TSubclassOf<AFrontierCharacter> Unit, ABuilding* Building)
{
    auto Barracks = Cast<ABarracks>(Building);

    if (Barracks)
    {
        return Barracks->CanCreateUnit(Unit);
    }

    return false;
}

void AFrontierPlayerController::ServerQueueUnit_Implementation(TSubclassOf<AFrontierCharacter> Unit, ABuilding* Building)
{
    auto Barracks = Cast<ABarracks>(Building);

    if (Barracks)
    {
        Barracks->QueueUnit(Unit);
    }
}

bool AFrontierPlayerController::ServerRemoveQueuedUnit_Validate(int Index, ABuilding* Building)
{
    return true;
}

void AFrontierPlayerController::ServerRemoveQueuedUnit_Implementation(int Index, ABuilding* Building)
{
    auto Barracks = Cast<ABarracks>(Building);

    if (Barracks)
    {
        Barracks->RemoveQueuedUnit(Index);
    }
}

bool AFrontierPlayerController::ServerResearch_Validate(UResearchNode* Node)
{
    return Cast<AFrontierPlayerState>(PlayerState)->CanResearchNode(Node);
}

void AFrontierPlayerController::ServerResearch_Implementation(UResearchNode* Node)
{
    Cast<AFrontierPlayerState>(PlayerState)->Research(Node);
}

void AFrontierPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AFrontierPlayerController, PlacedBuilding);
}