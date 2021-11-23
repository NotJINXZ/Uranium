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

bool bHasSpawned = false;

void* ProcessEventHook(UObject* object, UObject* function, void* params)
{
	if (object && function) {
        /*if (function->GetName().find("ReadyToStartMatch") != std::string::npos)
        {
            if (!bHasSpawned)
            {
                printf("Called ReadyToStartMatch!\n");
                Globals::SetupGlobals();

                auto SpawnActorParms = FActorSpawnParameters();
                auto SpawnLoc = FVector{ 0,0,5000 };
                auto SpawnRot = FRotator();
                //Globals::Pawn = SpawnActorLong(Globals::World, (UClass*)FindObject("BlueprintGeneratedClass /Game/Athena/PlayerPawn_Athena.PlayerPawn_Athena_C"), &SpawnLoc, &SpawnRot, SpawnActorParms);

                //Functions::Possess(Globals::Pawn);
                Functions::ServerReadyToStartMatch();
                Functions::StartMatch();

                bHasSpawned = true;
            }
        }*/
	}

	return ProcessEvent(object, function, params);
}

//Anti-Crash???
void RequestExitWithStatusHook(bool unknown, bool force)
{
}

void RequestExitHook(bool force)
{
}

void UnsafeEnvironmentPopupHook(wchar_t** unknown1,
    unsigned __int8 _case,
    __int64 unknown2,
    char unknown3)
{
}

void* __fastcall CrashHook(__int64 a1, __int64 a2) { return NULL; }

static inline void SetupAntiClose()
{
    auto UnsafeEnvironmentPopupAddr = Util::FindPattern("\x4C\x8B\xDC\x55\x49\x8D\xAB\x00\x00\x00\x00\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x85\x00\x00\x00\x00\x49\x89\x73\xF0\x49\x89\x7B\xE8\x48\x8B\xF9\x4D\x89\x63\xE0\x4D\x8B\xE0\x4D\x89\x6B\xD8", "xxxxxxx????xxx????xxx????xxxxxx????xxxxxxxxxxxxxxxxxxxxxx");
    auto RequestExitWithStatusAddr = Util::FindPattern("\x48\x89\x5C\x24\x00\x57\x48\x83\xEC\x40\x41\xB9\x00\x00\x00\x00\x0F\xB6\xF9\x44\x38\x0D\x00\x00\x00\x00\x0F\xB6\xDA\x72\x24\x89\x5C\x24\x30\x48\x8D\x05\x00\x00\x00\x00\x89\x7C\x24\x28\x4C\x8D\x05\x00\x00\x00\x00\x33\xD2\x48\x89\x44\x24\x00\x33\xC9\xE8\x00\x00\x00\x00", "xxxx?xxxxxxx????xxxxxx????xxxxxxxxxxxx????xxxxxxx????xxxxxx?xxx????");
    auto RequestExitAddr = Util::FindPattern("\x40\x53\x48\x83\xEC\x30\x41\xB9\x00\x00\x00\x00\x0F\xB6\xD9\x44\x38\x0D\x00\x00\x00\x00\x72\x20\x48\x8D\x05\x00\x00\x00\x00\x89\x5C\x24\x28\x4C\x8D\x05\x00\x00\x00\x00\x48\x89\x44\x24\x00\x33\xD2\x33\xC9\xE8\x00\x00\x00\x00", "xxxxxxxx????xxxxxx????xxxxx????xxxxxxx????xxxx?xxxxx????");
    auto FNCrashAddr = Util::FindPattern("\x48\x8B\xC4\x48\x89\x58\x08\x48\x89\x70\x10\x48\x89\x78\x18\x4C\x89\x60\x20\x55\x41\x56\x41\x57\x48\x8B\xEC\x48\x83\xEC\x40\x45\x33\xFF\x4C\x8D\x35\x00\x00\x00\x00", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx????");
    
    if (!UnsafeEnvironmentPopupAddr)
    {
        std::cout << "[Sodium]: Failed to find UnsafeEnvironmentPopup address\n";
        return;
    }

    if (!RequestExitWithStatusAddr)
    {
        std::cout << "[Sodium]: Failed to find RequestExitWithStatus address!\n";
        return;
    }

    if (!RequestExitAddr)
    {
        std::cout << "[Sodium]: Failed to find RequestExit address!\n";
        return;
    }

    if (!FNCrashAddr)
    {
        std::cout << "[Sodium]: Failed to find FNCrash address!\n";
        return;
    }

    MH_CreateHook((void*)UnsafeEnvironmentPopupAddr, UnsafeEnvironmentPopupHook, nullptr);
    MH_EnableHook((void*)UnsafeEnvironmentPopupAddr);
    
    MH_CreateHook((void*)RequestExitWithStatusAddr, RequestExitWithStatusHook, nullptr);
    MH_EnableHook((void*)RequestExitWithStatusAddr);

    MH_CreateHook((void*)RequestExitAddr, RequestExitHook, nullptr);
    MH_EnableHook((void*)RequestExitAddr);

    MH_CreateHook((void*)FNCrashAddr, CrashHook, nullptr);
    MH_EnableHook((void*)FNCrashAddr);
}