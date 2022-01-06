// Fill out your copyright notice in the Description page of Project Settings.


#include "WFGameModeBase.h"
#include "Character/WFBaseCharacter.h"

AWFGameModeBase::AWFGameModeBase()
{
    DefaultPawnClass = AWFBaseCharacter::StaticClass();
}