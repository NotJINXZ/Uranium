#pragma once

#include <Windows.h>
#include <cstdio>
#include <cstdint>
#include "Util.h"
#include "minhook/MinHook.h"
#include "skCryptor.h"

#pragma comment(lib, "minhook/minhook.lib")

typedef void* (__fastcall* fExitBypass)(__int64 arg1);
typedef void* (__fastcall* fCrashBypass)(__int64 arg1, __int64 arg2);
typedef void* (__fastcall* fRequestExit)(bool force);
typedef void* (__fastcall* fRequestExitWithStatus)(bool unknown, bool force);
typedef void* (__fastcall* fNotiBypass)(wchar_t** arg1, unsigned __int8 arg2, __int64 arg3, char arg4);
inline UObject* (*SpawnActorLong)(UObject* World, UObject* Class, FVector* Location, FRotator* Rotation, FActorSpawnParameters& SpawnParameters);
fExitBypass ExitBypass;
fCrashBypass CrashBypass;
fRequestExit RequestExitBypass;
fNotiBypass NotiBypass;
fRequestExitWithStatus RequestExitWithStatusBypass;
static void* RequestExitWithStatusAddress;


static UObject* SpawnActorHook(UObject* InWorld, UObject* Class, FVector* Location, FRotator* Rotation, FActorSpawnParameters& SpawnParameters)
{
    // Remember RBX Instruction
    //gSpawnParams = &SpawnParameters;
    //gSpawnActorWorld = InWorld;
    /*if (Class->GetFullName().find("DefaultPawn") != std::string::npos) {
        printf("Replaced Pawn!\n");
        Class = FindObject("BlueprintGeneratedClass /Game/Athena/PlayerPawn_Athena.PlayerPawn_Athena_C");
    }
    if (Class->GetFullName().find("FortPlayerControllerAthena") != std::string::npos && !((Class->GetFullName().find("Frontend") != std::string::npos) || (Class->GetFullName().find("Zone") != std::string::npos)))
    {
        printf("Replaced PC!\n");
        Class = FindObject("Class /Script/FortniteGame.FortPlayerControllerZone");
    }*/
    /*if (Class->GetFullName().find("FortGameState") != std::string::npos && !((Class->GetFullName().find("Frontend") != std::string::npos) || (Class->GetFullName().find("Zone") != std::string::npos)))
    {
        printf("Replaced GameState!\n");
        //MessageBox(NULL, L"Gamestate Swapped", L"Gamestate Swapped",0);
        Class = FindObject("Class /Script/FortniteGame.FortGameStateAthena");
    }
    if (Class->GetFullName().find("FortPlayerState") != std::string::npos && !((Class->GetFullName().find("Frontend") != std::string::npos) || (Class->GetFullName().find("Zone") != std::string::npos)))
    {
        printf("Replaced PlayerState!\n");
        Class = FindObject("Class /Script/FortniteGame.FortPlayerStateAthena");
    }*/
    return SpawnActorLong(InWorld, Class, Location, Rotation, SpawnParameters);
}


void* __fastcall ExitBypassHook(__int64 a1)
{
    return NULL;
}

void* __fastcall CrashHook(__int64 a1, __int64 a2)
{
    return NULL;
}

void* __fastcall RequestExitBypassHook(bool force)
{
    printf(crypt("RequestExit Call WAS CALLED: %i\n"), force);
    return NULL;
}

void* __fastcall NotificationHook(wchar_t** a1, unsigned __int8 a2, __int64 a3, char a4)
{
    return NULL;
}

void RequestExitWithStatusHook(bool Force, uint8_t ReturnCode)
{
    printf(crypt("BigFatCock Check: %i ReturnCode: %u\n"), Force, ReturnCode);

    if (bIsReady && !bHasSpawned)
    {
        bHasSpawned = true;
        Functions::UpdatePlayerController();
        Functions::EnableCheatManager();

        Functions::Summon(L"PlayerPawn_Athena_C");

        for (int i = 0; i < GObjects->NumElements; i++)
        {
            auto object = GObjects->GetByIndex(i);

            if (object == nullptr)
                continue;

            if (object->GetFullName() == "PlayerPawn_Athena_C /Game/Athena/PlayerPawn_Athena.Default__PlayerPawn_Athena_C")
                continue;

            if (object->GetFullName().starts_with("PlayerPawn_Athena_C ")) {
                Pawn = object;
                break;
            }
        }

        if (Pawn) {
            std::cout << "Pawn: " << Pawn->GetFullName() << std::endl;
            Functions::SetPlaylist(FindObject("FortPlaylistAthena /Game/Athena/Playlists/BattleLab/Playlist_BattleLab.Playlist_BattleLab"));
            Functions::Possess(Pawn);
            //Functions::SetGodMode();
            Functions::StartMatch();
            Functions::ServerReadyToStartMatch();
            Functions::ShowSkin();
        }

        Functions::UpdatePlayerController();
        Functions::EnableCheatManager();

        Functions::Summon(L"PlayerPawn_Athena_C");

        for (int i = 0; i < GObjects->NumElements; i++)
        {
            auto object = GObjects->GetByIndex(i);

            if (object == nullptr)
                continue;

            if (object->GetFullName() == "PlayerPawn_Athena_C /Game/Athena/PlayerPawn_Athena.Default__PlayerPawn_Athena_C")
                continue;

            if (object->GetFullName().starts_with("PlayerPawn_Athena_C ")) {
                Pawn = object;
                break;
            }
        }

        if (Pawn) {
            std::cout << "Pawn: " << Pawn->GetFullName() << std::endl;
            //Functions::SetPlaylist(FindObject("FortPlaylistAthena /Game/Athena/Playlists/BattleLab/Playlist_BattleLab.Playlist_BattleLab"));
            Functions::Possess(Pawn);
            Functions::SetGodMode();
            //Functions::StartMatch();
            //Functions::ServerReadyToStartMatch();
            Functions::ShowSkin();
        }
    }
}

static void InitHooks()
{
    auto NotificationHookAddress = Util::FindPattern(crypt("4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 5B 20"));
    auto FNCrashHookAddress = Util::FindPattern(crypt("48 8B C4 48 89 58 08 48 89 70 10 48 89 78 18 4C 89 60 20 55 41 56 41 57 48 8B EC 48 83 EC 40 45 33 FF 4C 8D 35 ? ? ? ?"));
    auto Spawnactoraddress = Util::FindPattern(crypt("48 8B C4 55 53 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 A8 0F 29 78 98 44 0F 29 40 ? 44 0F 29 88 ? ? ? ? 44 0F 29 90 ? ? ? ? 44 0F 29 98 ? ? ? ? 44 0F 29 A0 ? ? ? ? 44 0F 29 A8 ? ? ? ? 44 0F 29 B0 ? ? ? ? 44 0F 29 B8 ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 60 45 33 ED 48 89 4D 90 44 89 6D 80 48 8D 05 ? ? ? ? 44 38 2D ? ? ? ? 4C 8B F2 48 8B D9 48 8D 15 ? ? ? ? 49 0F 45 C5 48 8D 4D B8 48 89 45 B0 49 8B F1 4D 8B E0 E8 ? ? ? ? 4C 8B 7B 30 4C 89 7C 24 ? 4D 85 F6 0F 84 ? ? ? ? 41 8B 86 ? ? ? ? 0F BA E0 19 0F 82 ? ? ? ? A8 01 0F 85 ? ? ? ? E8 ? ? ? ? 48 8B D0 49 8B CE E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? 48 8B 46 08 48 8D 3D ? ? ? ? 48 85 C0 74 0A"));
    RequestExitWithStatusAddress = Util::FindPattern(crypt("\x48\x8B\xC4\x48\x89\x58\x18\x88\x50\x10\x88\x48\x08\x57\x48\x83\xEC\x30"), crypt("xxxxxxxxxxxxxxxxxx"));
    CHECKSIG(NotificationHookAddress, crypt("Failed to find Notification address!"));
    CHECKSIG(FNCrashHookAddress, crypt("Failed to find FNCrash address!"));
    CHECKSIG(RequestExitWithStatusAddress, crypt("Failed to find RequestExitWithStatus address"));

    MH_CreateHook(static_cast<LPVOID>((LPVOID)RequestExitWithStatusAddress), RequestExitWithStatusHook, reinterpret_cast<LPVOID*>(&RequestExitWithStatusBypass));
    MH_EnableHook(static_cast<LPVOID>((LPVOID)RequestExitWithStatusAddress));
    MH_CreateHook(static_cast<LPVOID>((LPVOID)FNCrashHookAddress), CrashHook, reinterpret_cast<LPVOID*>(&CrashBypass));
    MH_CreateHook(static_cast<LPVOID>((LPVOID)NotificationHookAddress), ExitBypassHook, reinterpret_cast<LPVOID*>(&NotiBypass));
    MH_EnableHook(static_cast<LPVOID>((LPVOID)NotificationHookAddress));
  //  MH_CreateHook(static_cast<LPVOID>((LPVOID)Spawnactoraddress), SpawnActorHook, reinterpret_cast<LPVOID*>(&SpawnActorLong));
    //MH_EnableHook(static_cast<LPVOID>((LPVOID)Spawnactoraddress));
}