// Copyright Nathan Williams & Matthew Lowe 2019. All Rights Reserved.


#include "RoamPawn.h"

// Sets default values
ARoamPawn::ARoamPawn()
{
     // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARoamPawn::BeginPlay()
{
    Super::BeginPlay();
    
}

// Called every frame
void ARoamPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ARoamPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

}

