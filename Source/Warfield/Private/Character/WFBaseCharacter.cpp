// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/WFBaseCharacter.h"

// Sets default values
AWFBaseCharacter::AWFBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWFBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWFBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AWFBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

