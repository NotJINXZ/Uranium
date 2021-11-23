#pragma once

#include "Structs.hpp"
#include "Unreal.hpp"
#include <fstream>
#include <iostream>
#include "Functions.hpp"

static UObject* SpawnActorHook(UObject* InWorld, UClass* Class, FVector* Location, FRotator* Rotation, FActorSpawnParameters& SpawnParameters)
{
    // Scuff Way To get ingame this is a NEED TO FIX!!!
    //Globals::gSpawnParams = &SpawnParameters;
    //Globals::gSpawnActorWorld = InWorld;
    if (Class->GetName().find("DefaultPawn") != std::string::npos) {
        Class = (UClass*)FindObject("BlueprintGeneratedClass /Game/Abilities/Player/Pawns/PlayerPawn_Generic.PlayerPawn_Generic_C");
    }
    if (Class->GetName().find("FortPlayerController") != std::string::npos && !((Class->GetName().find("Frontend") != std::string::npos) || (Class->GetName().find("Zone") != std::string::npos)))
    {
        Class = (UClass*)FindObject("Class /Script/FortniteGame.FortPlayerControllerZone");
    }
    if (Class->GetName().find("FortGameState") != std::string::npos && !((Class->GetName().find("Frontend") != std::string::npos) || (Class->GetName().find("Zone") != std::string::npos)))
    {
        //MessageBox(NULL, L"Gamestate Swapped", L"Gamestate Swapped",0);
        Class = (UClass*)FindObject("Class /Script/FortniteGame.FortGameStateZone");
    }
    if (Class->GetName().find("FortPlayerState") != std::string::npos && !(Class->GetName().find("Frontend") != std::string::npos) || (Class->GetName().find("Zone") != std::string::npos))
    {
        Class = (UClass*)FindObject("Class /Script/FortniteGame.FortPlayerStateZone");
    }
    return SpawnActorLong(InWorld, Class, Location, Rotation, SpawnParameters);
}

void* ProcessEventHook(UObject* object, UObject* function, void* params)
{
	if (object && function) {
		if (function->GetName().find("ServerLoadingScreenDropped") != std::string::npos)
		{
            Functions::InitCheatManager();
            Functions::DestroyAll("FortHLODSMActor");
		}
	}

	return ProcessEvent(object, function, params);
}