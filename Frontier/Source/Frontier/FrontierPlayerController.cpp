// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.

#include "FrontierPlayerController.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "EngineUtils.h"
#include "AIController.h"
#include "UserWidget.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Components/StaticMeshComponent.h"

#include "FrontierCharacter.h"
#include "FrontierPlayerState.h"
#include "FrontierGameInstance.h"
#include "FrontierHUD.h"
#include "Buildings/Building.h"
#include "Buildings/UnitQueueCommon.h"
#include "Buildings/DummyBuilding.h"
#include "Research.h"
#include "City.h"
#include "Frontier.h"
#include "Widgets/UI.h"
#include "RoamPawn.h"
#include "BaseResource.h"
#include "FogOfWar.h"
#include "Landscape.h"
#include "Engine/SceneCapture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Widgets/UnitSelected.h"
#include "Widgets/BuildingBaseWidget.h"
#include "Widgets/ResourcesContainerWidget.h"
#include "Widgets/BuildingPlacementErrorWidget.h"

AFrontierPlayerController::AFrontierPlayerController(const FObjectInitializer& ObjectInitializer)
{
    bShowMouseCursor = true;
    bEnableMouseOverEvents = true;
    bAllowTickBeforeBeginPlay = false;
    DefaultMouseCursor = EMouseCursor::Default;

    for (uint8_t i = 0; i < (uint8_t)ESound::_Max; ++i)
    {
        const TEnumAsByte<ESound> SoundEnum = (ESound)i;
        auto Comp = CreateDefaultSubobject<UAudioComponent>(*UEnum::GetValueAsString(SoundEnum.GetValue()));
        Comp->SetAutoActivate(false);
        Sounds.Add((ESound)i, Comp);
    }
}

void AFrontierPlayerController::BeginPlay()
{
    Super::BeginPlay();

    ControllerState = EControllerState::Idle;

    if (!IsRunningDedicatedServer())
    {
        for (auto Sound : UserSounds)
        {
            Sounds[Sound.Key]->SetSound(Sound.Value);
        }

        FTransform Transform(FVector(0.0f, 0.0f, 260.0f));
        FogOfWar = GetWorld()->SpawnActorDeferred<AFogOfWar>(FogOfWarClass, Transform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
        FogOfWar->Player = Cast<AFrontierPlayerState>(PlayerState);
        UGameplayStatics::FinishSpawningActor(FogOfWar, Transform);

        MinimapTarget = NewObject<UTextureRenderTarget2D>();
        MinimapTarget->InitAutoFormat(512, 512);

        auto Landscape = UGameplayStatics::GetActorOfClass(GetWorld(), ALandscape::StaticClass());

        FTransform CapTransform(FRotator(-90.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 1000.0f));
        ASceneCapture2D* SceneCapture = GetWorld()->SpawnActor<ASceneCapture2D>(ASceneCapture2D::StaticClass(), CapTransform);
        auto Capture = SceneCapture->GetCaptureComponent2D();
        Capture->TextureTarget = MinimapTarget;
        Capture->OrthoWidth =  FMath::Abs(Landscape->GetActorLocation().X);
        Capture->ProjectionType = ECameraProjectionMode::Orthographic;

        BoundsLimit = Capture->OrthoWidth / 2.0f - 3000.0f;
        
        ClientCreateUI();

        PlayerKilledEvent.BindLambda([&](AFrontierCharacter* C) {
            for (auto Selected : SelectedUnits)
            {
                if (Selected == C)
                {
                    SelectedUnits.Remove(C);

                    if (SelectedUnits.Num() <= 0)
                        ControllerState = EControllerState::Idle;

                    break;
                }
            }
        });

        BuildingConstructedEvent.BindLambda([&]() {
            Sounds[ESound::Built]->Play();
        });
    }
}

void AFrontierPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    InputComponent->BindAxis("Zoom", this, &AFrontierPlayerController::OnZoom);
    InputComponent->BindAxis("MoveUp", this, &AFrontierPlayerController::OnMoveUp);
    InputComponent->BindAxis("MoveRight", this, &AFrontierPlayerController::OnMoveRight);

    InputComponent->BindAction("Send", IE_Pressed, this, &AFrontierPlayerController::OnSend);
    InputComponent->BindAction("Select", IE_Pressed, this, &AFrontierPlayerController::OnSelectDown);
    InputComponent->BindAction("Select", IE_Released, this, &AFrontierPlayerController::OnSelectUp);
    InputComponent->BindAction("Rotate", IE_Pressed, this, &AFrontierPlayerController::OnRotate);
}

void AFrontierPlayerController::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);

    CursorState = ECursorState::Default;

    if (ResourcesContainerWidget)
    {
        ResourcesContainerWidget->SetVisibility(ESlateVisibility::Collapsed);
    }

    float MX, MY;
    
    if (GetMousePosition(MX, MY))
    {
        int32 VX, VY;
        GetViewportSize(VX, VY);

        if (MY <      MouseMovementPadding) OnMoveUp   ( 1.0f);
        if (MY > VY - MouseMovementPadding) OnMoveUp   (-1.0f);
        if (MX <      MouseMovementPadding) OnMoveRight(-1.0f);
        if (MX > VX - MouseMovementPadding) OnMoveRight( 1.0f);
    }

    if (ControllerState == EControllerState::PlacingBuilding)
    {
        if (HoveredBuilding)
        {
            FHitResult Hit;

            TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = {
                EObjectTypeQuery::ObjectTypeQuery7, // Terrain
            };

            if (GetHitResultUnderCursorForObjects(ObjectTypes, false, Hit))
            {
                auto BoxComponent = Cast<UBoxComponent>(HoveredBuilding->GetComponentByClass(UBoxComponent::StaticClass()));
                auto Extent = BoxComponent->GetScaledBoxExtent();
                bool bClear = FogOfWar->IsRevealedBox(HoveredBuilding->GetActorLocation(), Extent.X, Extent.Y);

                HoveredBuilding->SetActorLocation(Hit.Location + FVector(0.0f, 0.0f, Extent.Z));
                HoveredBuilding->Requirements[EBuildingPlacementConditions::NotInFog].Met = bClear;
                
                if (HoveredBuilding->IsAllRequirementsMet())
                {
                    BuildingPlacementErrorWidget->SetVisibility(ESlateVisibility::Collapsed);
                }
                else
                {
                    BuildingPlacementErrorWidget->SetPositionInViewport(FVector2D(MX, MY));
                    BuildingPlacementErrorWidget->SetVisibility(ESlateVisibility::Visible);
                    BuildingPlacementErrorWidget->ErrorMessage = HoveredBuilding->GetPlacementErrorString();
                }
            }
        }
    }
    else if (ControllerState == EControllerState::SelectedUnit)
    {
        FHitResult Hit;

        TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = {
            EObjectTypeQuery::ObjectTypeQuery1, // WorldStatic
            EObjectTypeQuery::ObjectTypeQuery2, // WorldDynamic
            EObjectTypeQuery::ObjectTypeQuery3, // Pawn
            EObjectTypeQuery::ObjectTypeQuery7  // Terrain
        };

        if (UI->UnitSelected->IsHovered())
        {
            CursorState = ECursorState::Default;
        }
        else if (GetHitResultUnderCursorForObjects(ObjectTypes, false, Hit))
        {
            auto PS = Cast<AFrontierPlayerState>(PlayerState);
            auto Char = Cast<AFrontierCharacter>(Hit.Actor);

            if (Cast<AFrontierCharacter>(Hit.Actor))
            {
				Char->Player->Team != PS->Team && Char->bRevealed ?	CursorState = ECursorState::Attack : CursorState = ECursorState::Send;
            }
            else if (Cast<ABuilding>(Hit.Actor))
            {
                auto Building = Cast<ABuilding>(Hit.Actor);

                if (Building->Player == PS)
                {
					if (SelectedUnits.ContainsByPredicate([](const AFrontierCharacter* C) { return C->bCanGather; }))
					{
						if (!Building->bBuilt)
						{
							CursorState = ECursorState::Build;
						}
						else if (Building->bBuilt && Building->IsDamaged())
						{
							CursorState = ECursorState::Build;
						}
					}
                }
                else if (Building->Player != PS && Building->bRevealed)
                {
                    CursorState = ECursorState::Attack;
                }
                else
                {
                    CursorState = ECursorState::Send;
                }
            }
            else if (Cast<ABaseResource>(Hit.Actor) && Cast<ABaseResource>(Hit.Actor)->bRevealed)
            {
                CursorState = ECursorState::Harvest;
                ShowResourceContainsWidget(Cast<ABaseResource>(Hit.Actor), MX, MY);
            }
            else
            {
                CursorState = ECursorState::Send;
            }
        }
    }
    else
    {
        FHitResult Hit;

        TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = {
            EObjectTypeQuery::ObjectTypeQuery1, // WorldStatic
            EObjectTypeQuery::ObjectTypeQuery2, // WorldDynamic
        };

        if (IsValid(UI) && !UI->IsHovered() && GetHitResultUnderCursorForObjects(ObjectTypes, false, Hit))
        {
            auto Res = Cast<ABaseResource>(Hit.Actor);

            if (Res && Res->bRevealed)
            {
                ShowResourceContainsWidget(Res, MX, MY);
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

    ResourcesContainerWidget = CreateWidget<UResourcesContainerWidget>(this, ResourcesContainerClass, "ResourcesContainer");

    if (ResourcesContainerWidget)
    {
        ResourcesContainerWidget->AddToViewport();
        ResourcesContainerWidget->SetDesiredSizeInViewport(ResourcesContainerWidget->GetDesiredSize());
        ResourcesContainerWidget->SetVisibility(ESlateVisibility::Collapsed);
    }

    BuildingPlacementErrorWidget = CreateWidget<UBuildingPlacementErrorWidget>(this, BuildingPlacementErrorClass, "BuildingPlacementError");

    if (BuildingPlacementErrorWidget)
    {
        BuildingPlacementErrorWidget->AddToViewport();
        BuildingPlacementErrorWidget->SetDesiredSizeInViewport(BuildingPlacementErrorWidget->GetDesiredSize());
        BuildingPlacementErrorWidget->SetVisibility(ESlateVisibility::Collapsed);
    }

    AnimationFinishedEvent.BindUFunction(this, "BuildingUIAnimationFinished");
}

void AFrontierPlayerController::SetHoveredBuilding(TSubclassOf<ABuilding> BuildingType)
{
    if (HoveredBuilding)
        HoveredBuilding->Destroy();

    HoveredBuildingType = BuildingType;

    auto BuildingDefaults = BuildingType.GetDefaultObject();
    auto BuildingMesh = BuildingDefaults->Mesh->GetStaticMesh();

    FTransform Transform(FRotator::ZeroRotator, FVector::ZeroVector, FVector::OneVector);

    HoveredBuilding = GetWorld()->SpawnActorDeferred<ADummyBuilding>(
        ADummyBuilding::StaticClass(),
        Transform,
        this,
        GetPawn(),
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn
    );

    HoveredBuilding->Box->SetRelativeTransform(BuildingDefaults->Box->GetRelativeTransform());
    HoveredBuilding->Box->SetBoxExtent(BuildingDefaults->Box->GetUnscaledBoxExtent());
    HoveredBuilding->Mesh->SetStaticMesh(BuildingDefaults->BuildingMesh);
    HoveredBuilding->Mesh->SetRelativeTransform(BuildingDefaults->Mesh->GetRelativeTransform());
    HoveredBuilding->HoverMaterialGreen = HoverMaterialGreen;
    HoveredBuilding->HoverMaterialRed = HoverMaterialRed;
    HoveredBuilding->BuildingType = BuildingType;
    HoveredBuilding->Player = GetPlayerState<AFrontierPlayerState>();

    UGameplayStatics::FinishSpawningActor(HoveredBuilding, Transform);

    auto PS = GetPlayerState<AFrontierPlayerState>();

    for (auto City : PS->Cities)
    {
        City->CityRadiusDecal->SetVisibility(true);
    }

    ControllerState = EControllerState::PlacingBuilding;
}

void AFrontierPlayerController::BuildingUIAnimationFinished()
{
    if(UI->bIsHidden)
        UI->ShowUI();

    if (SelectedBuilding)
    {
        SelectedBuilding->HideOutline();
        SelectedBuilding = nullptr;
    }
}

void AFrontierPlayerController::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    UE_LOG(LogFrontier, Display, TEXT("Controller: Received player state"));

    if (UI)
    {
        UI->RemoveFromParent();
    }

    ClientCreateUI();
}

void AFrontierPlayerController::OnRep_PlacedBuilding()
{
    if (HoveredBuilding)
    {
        HoveredBuilding->Destroy();
    }
}

void AFrontierPlayerController::ShowResourceContainsWidget(ABaseResource* Res, int MX, int MY)
{
    ResourcesContainerWidget->SetPositionInViewport(FVector2D(MX, MY));
    ResourcesContainerWidget->SetVisibility(ESlateVisibility::Visible);
    ResourcesContainerWidget->Amount->SetText(FText::FromString(FString::FromInt(Res->GetRemainingResources())));
    ResourcesContainerWidget->Label->SetText(FText::FromString(Res->ResourceName));
}

void AFrontierPlayerController::OnMoveUp(float Value)
{
    if (GetPawn())
    {
        auto RoamPawn = Cast<ARoamPawn>(GetPawn());
        auto Delta = Value * MoveSpeed * MovementCurve->GetFloatValue(RoamPawn->GetCameraHeight());
        auto Location = GetPawn()->GetActorLocation() + FVector(Delta, 0.0f, 0.0f);

        if (Location.X < -BoundsLimit) Location.X = -BoundsLimit;
        if (Location.X >  BoundsLimit) Location.X =  BoundsLimit;

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

        if (Location.Y < -BoundsLimit) Location.Y = -BoundsLimit;
        if (Location.Y >  BoundsLimit) Location.Y =  BoundsLimit;

        RoamPawn->SetActorLocation(Location);
    }
}

void AFrontierPlayerController::OnSelectDown()
{
    if (ControllerState != EControllerState::PlacingBuilding)
    {
        Cast<AFrontierHUD>(GetHUD())->BeginSelect();
    }
}

void AFrontierPlayerController::OnSelectUp()
{
    auto DeselectUnits = [&]() {
        for (auto Unit : SelectedUnits)
            Unit->HideOutline();

        SelectedUnits.Empty();
    };

    auto DeselectBuilding = [&] {
        if (!UI->bIsHidden)
            UI->HideUI();

        if (SelectedBuildingUI)
        {
            SelectedBuildingUI->PlayAnimationForward(SelectedBuildingUI->GetHideAnimation());
            SelectedBuildingUI->UnbindFromAnimationFinished(SelectedBuildingUI->GetHideAnimation(), AnimationFinishedEvent);
        }

        if (SelectedBuilding)
            SelectedBuilding->HideOutline();

        SelectedBuilding = nullptr;
        SelectedBuildingUI = nullptr;
    };

    auto HUD = Cast<AFrontierHUD>(GetHUD());

    DeselectUnits();
    HUD->EndSelect();

    SelectedUnits = HUD->GetCharactersInSelection();
    SelectedUnits.RemoveAll([&](AFrontierCharacter* Char) { return Char->Player != PlayerState; });

    if (SelectedUnits.Num() > 0)
    {
        DeselectBuilding();

        UI->UnitSelected->SetSelectedUnits(SelectedUnits);

        if (ControllerState != EControllerState::SelectedUnit)
            UI->ShowUnitUI();

        ControllerState = EControllerState::SelectedUnit;

        for (auto Char : SelectedUnits)
        {
            Char->ShowOutline();
        }

        Sounds[ESound::Select]->Play();

        return;
    }

    auto PS = Cast<AFrontierPlayerState>(PlayerState);

    if (ControllerState == EControllerState::PlacingBuilding)
    {
        if (HoveredBuilding->IsAllRequirementsMet())
        {
            ControllerState = EControllerState::Idle;
            ServerSpawnBuilding(HoveredBuildingType, HoveredBuilding->GetActorLocation(), HoveredBuilding->GetActorRotation());

            for (auto City : PS->Cities)
            {
                City->CityRadiusDecal->SetVisibility(false);
            }

            if (HasAuthority())
            {
                HoveredBuilding->Destroy();
                HoveredBuilding = nullptr;
            }

            Sounds[ESound::Place]->Play();
        }
        else
        {
            Sounds[ESound::Error]->Play();
        }
    }
    else
    {
        FHitResult Hit;

        TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = {
            EObjectTypeQuery::ObjectTypeQuery1, // WorldStatic
            EObjectTypeQuery::ObjectTypeQuery2, // WorldDynamic
            EObjectTypeQuery::ObjectTypeQuery3  // Pawn
        };

        bool Selected = false;

        if (GetHitResultUnderCursorForObjects(ObjectTypes, false, Hit))
        {
            auto Unit = Cast<AFrontierCharacter>(Hit.Actor);

            if (Unit && Unit->Player == PS)
            {
                DeselectUnits();
                DeselectBuilding();

                SelectedUnits.Add(Unit);
                Unit->ShowOutline();

                UI->UnitSelected->SetSelectedUnits(SelectedUnits);

                if (ControllerState != EControllerState::SelectedUnit)
                    UI->ShowUnitUI();

                Selected = true;
                ControllerState = EControllerState::SelectedUnit;

                Sounds[ESound::Select]->Play();
            }
            else
            {
                if (ControllerState == EControllerState::SelectedUnit)
                    UI->HideUnitUI();
            }

            auto Building = Cast<ABuilding>(Hit.Actor);

            if (!Selected && Building && Building->Player == PS && Building->bBuilt)
            {
                UE_LOG(LogFrontier, Display, TEXT("Selected building"));

                if (Building != SelectedBuilding)
                {
                    DeselectUnits();
                    DeselectBuilding();

                    SelectedBuilding = Building;
                    SelectedBuilding->ShowOutline();

                    SelectedBuildingUI = CreateWidget<UBuildingBaseWidget>(this, SelectedBuilding->Widget);
                    SelectedBuildingUI->BuildingActor = SelectedBuilding;
                    SelectedBuildingUI->AddToViewport();
                    SelectedBuildingUI->PlayAnimationForward(SelectedBuildingUI->GetShowAnimation());
                    SelectedBuildingUI->BindToAnimationFinished(SelectedBuildingUI->GetHideAnimation(), AnimationFinishedEvent);
                }

                ControllerState = EControllerState::SelectedBuilding;
                Selected = true;

                Sounds[ESound::ActionClickBuilding]->Play();
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
            if (UI && UI->bIsHidden)
                UI->ShowUI();

            DeselectUnits();

            if (SelectedBuilding)    SelectedBuilding->HideOutline();
            if (SelectedBuildingUI)  SelectedBuildingUI->PlayAnimationForward(SelectedBuildingUI->GetHideAnimation());

            if (ControllerState == EControllerState::SelectedUnit)
                UI->HideUnitUI();

            SelectedBuilding = nullptr, SelectedBuildingUI = nullptr;
            ControllerState = EControllerState::Idle;
        }
    }
}

void AFrontierPlayerController::OnSend()
{
    if (ControllerState == EControllerState::SelectedUnit)
    {
		ServerIncrementActions();
        FHitResult Hit;

        TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = {
            EObjectTypeQuery::ObjectTypeQuery1, // WorldStatic
            EObjectTypeQuery::ObjectTypeQuery2, // WorldDynamic
			EObjectTypeQuery::ObjectTypeQuery3, // Pawn
            EObjectTypeQuery::ObjectTypeQuery7, // Terrain
        };

        if (GetHitResultUnderCursorForObjects(ObjectTypes, false, Hit))
        {
            const auto Actor = Hit.Actor.Get();

            if (Actor->IsA<ABuilding>()) Sounds[ESound::ActionBuild]->Play();
            else if (Actor->IsA<ABaseResource>()) Sounds[ESound::ActionHarvest]->Play();
            else Sounds[ESound::ActionSent]->Play();

			UDecalComponent* Decal;
			FHitResult DecalHit;

			TArray<TEnumAsByte<EObjectTypeQuery>> DecalRayObjectTypes = {
				EObjectTypeQuery::ObjectTypeQuery7, // Terrain
			};
			
            float MX, MY;

            GetMousePosition(MX, MY);
			GetHitResultAtScreenPosition(FVector2D(MX + 20.0f, MY + 40.0f), DecalRayObjectTypes, false, DecalHit);

			if(CursorState == ECursorState::Attack)
				Decal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), MovementMaterialRed, FVector(50, 50, 50), DecalHit.Location, FRotator(-90, 0, 0), 0.4f);
			else
				Decal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), MovementMaterialGreen, FVector(50, 50, 50), DecalHit.Location, FRotator(-90, 0, 0), 0.4f);

			Decal->SetFadeIn(0.f, 0.1f);
			Decal->SetFadeOut(0.2f, 0.1f);

            ServerMoveAIToLocation(SelectedUnits, Hit.Location, Hit.Actor.Get());
        }
    }
    else if (ControllerState == EControllerState::PlacingBuilding)
    {
		ServerIncrementActions();
        ControllerState = EControllerState::Idle;
        BuildingPlacementErrorWidget->SetVisibility(ESlateVisibility::Collapsed);
        HoveredBuilding->Destroy();
        HoveredBuilding = nullptr;

        for (auto City : GetPlayerState<AFrontierPlayerState>()->Cities)
        {
            City->CityRadiusDecal->SetVisibility(false);
        }
    }
}

void AFrontierPlayerController::OnRotate()
{
    if (HoveredBuilding)
    {
        HoveredBuilding->AddActorWorldRotation(FQuat::MakeFromEuler(FVector(0.0f, 0.0f, 90.0f)));
        Sounds[ESound::Rotate]->Play();
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
    return Cast<AFrontierPlayerState>(PlayerState)->CanCreateBuilding(Type, Location);
}

void AFrontierPlayerController::ServerSpawnBuilding_Implementation(TSubclassOf<ABuilding> Type, FVector Location, FRotator Rotation)
{
    // TODO: Move to PlayerState?
    if (Type)
    {
        auto PS = Cast<AFrontierPlayerState>(PlayerState);
        PS->Resources -= Type.GetDefaultObject()->Cost;

        FTransform Transform(Rotation, Location);

        PlacedBuilding = GetWorld()->SpawnActorDeferred<ABuilding>(
            Type,
            Transform,
            PS,
            GetPawn(),
            ESpawnActorCollisionHandlingMethod::AlwaysSpawn
        );

        auto Box = Cast<UBoxComponent>(Type.GetDefaultObject()->GetComponentByClass(UBoxComponent::StaticClass()));
        auto BoxExtent = Box->GetScaledBoxExtent();
        auto Extent = FMath::Max(BoxExtent.X, BoxExtent.Y);

        ACity* AvailableCity = nullptr;

        for (auto& City : PS->Cities)
        {
            if (City->CanPlaceBuilding(Type) && City->IsInCity(Location, Extent))
            {
                AvailableCity = City;
                break;
            }
        }

        PlacedBuilding->Player = PS;
        PlacedBuilding->City = AvailableCity;
        PlacedBuilding->Box->SetMobility(EComponentMobility::Static);
        UGameplayStatics::FinishSpawningActor(PlacedBuilding, Transform);

        AvailableCity->AddBuilding(PlacedBuilding);
    }
}

bool AFrontierPlayerController::ServerMoveAIToLocation_Validate(const TArray<AFrontierCharacter*>& AI, FVector Location, AActor* Resource)
{
    return true;
}

void AFrontierPlayerController::ServerMoveAIToLocation_Implementation(const TArray<AFrontierCharacter*>& AI, FVector Location, AActor* Object)
{
    if (IsValid(Object))
    {
        auto Cols = FMath::CeilToInt(FMath::Sqrt(AI.Num()));
        auto Rows = FMath::CeilToInt(StaticCast<float>(AI.Num()) / Cols);

        TArray<FVector> Positions;

        for (int Row = 0; Row < Rows; ++Row)
        {
            for (int Col = 0; Col < Cols; ++Col)
            {
                Positions.Add(FVector(FormationSeparation * Col, FormationSeparation * Row, 0.0f));
            }
        }

        auto Offset = FVector(-FormationSeparation * 0.5f * Cols, FormationSeparation * 0.5f * Rows, 0.0f);

        for(auto Unit : AI)
            Unit->MoveToLocation(Location + Offset + Positions.Pop(), Object);
    }
}

bool AFrontierPlayerController::ServerQueueUnit_Validate(TSubclassOf<AFrontierCharacter> Unit, ABuilding* Building)
{
    auto Barracks = Cast<AUnitQueueCommon>(Building);

    if (Barracks)
    {
        return Barracks->CanCreateUnit(Unit);
    }

    return false;
}

void AFrontierPlayerController::ServerQueueUnit_Implementation(TSubclassOf<AFrontierCharacter> Unit, ABuilding* Building)
{
    auto Barracks = Cast<AUnitQueueCommon>(Building);

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
    auto Barracks = Cast<AUnitQueueCommon>(Building);

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

bool AFrontierPlayerController::ServerIncrementActions_Validate()
{
    return true;
}

void AFrontierPlayerController::ServerIncrementActions_Implementation()
{
    GetPlayerState<AFrontierPlayerState>()->PlayerStats.Actions++;
}

void AFrontierPlayerController::OnGameOver_Implementation(const TArray<FPlayerStats>& Stats, FUniqueNetIdRepl WinningPlayer)
{
    auto GI = GetGameInstance<UFrontierGameInstance>();
    GI->Stats = Stats;
    GI->WinningPlayer = WinningPlayer;

    ClientTravel("GameOver", ETravelType::TRAVEL_Absolute);
}

void AFrontierPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AFrontierPlayerController, PlacedBuilding);
}