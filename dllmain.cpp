#include <Windows.h>
#include "Util.h"
#include "UE5.h"
#include <iostream>
#include "Functions.h"
#include "minhook/MinHook.h"
#include "Hooks.h"

#pragma comment(lib, "minhook/minhook.lib")

bool bIsReady = false;
bool bHasSpawned = false;

void* (*PEOG)(void*, void*, void*);
void* ProcessEventDetour(UObject* pObject, UObject* pFunction, void* pParams)
{
    if (pObject && pFunction) {
        if (pFunction->GetName().find("Tick") != std::string::npos)
        {
            if (GetAsyncKeyState(VK_F1) & 0x01) {
                Functions::SwitchLevel(L"Apollo_Papaya");
                bIsReady = true;
            }

            if (GetAsyncKeyState(VK_F2) & 0x01) {
                //auto Playlist = FindObject("FortPlaylistAthena /Game/Athena/Playlists/Omaha/Playlist_Omaha.Playlist_Omaha");
                //Functions::SetPlaylist(Playlist);

                //Functions::SpawnPlayer();
                printf("1\n");
                //Functions::Possess(Pawn);
                printf("2\n");

                //Functions::EnableCheatManager();

                Functions::ServerReadyToStartMatch();
                printf("3\n");
                Functions::StartMatch();
                printf("4\n");
            }
        }
    }

    return PEOG(pObject, pFunction, pParams);
}

DWORD WINAPI MainThread(LPVOID)
{
    Util::InitConsole();

    std::cout << "Finding Patterns!\n";
    
    auto pGObjects = Util::FindPattern("48 8B 05 ? ? ? ? 48 8B 0C C8 48 8B 04 D1", true, 3);
    CHECKSIG(pGObjects, "Failed to find GObjects address!");
    GObjects = decltype(GObjects)(pGObjects);

    auto pFNameToString = Util::FindPattern("48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 56 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 48 8B F2 4C 8B F1 E8 ? ? ? ? 45 8B 06 33 ED");
    CHECKSIG(pFNameToString, "Failed to find FNameToString address!");
    FNameToString = decltype(FNameToString)(pFNameToString);

    auto pFreeMemory = Util::FindPattern("48 85 C9 0F 84 ? ? ? ? 53 48 83 EC 20 48 89 7C 24 30 48 8B D9 48 8B 3D ? ? ? ? 48 85 FF 0F 84 ? ? ? ? 48 8B 07 4C 8B 40 30 48 8D 05 ? ? ? ? 4C 3B C0");
    CHECKSIG(pFreeMemory, "Failed to find FreeMemory address!");
    FreeMemory = decltype(FreeMemory)(pFreeMemory);

    auto pStaticConstructObject = Util::FindPattern("48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 54 41 56 41 57 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 48 8B 29 48 8D 05 ? ? ? ? 4C 8B 79 08 45 33 E4 44 38 25 ? ? ? ? 48 8B F9 44 8B 71 18 48 8B 59 28 49 0F 45 C4 48 89 44 24 ? F7 85 ? ? ? ? ? ? ? ? 0F 84 ? ? ? ? 41 8D 74 24 ? 48 85 DB 74 23 44 39 61 10 0F 95 C0 44 39 67 14 0F B6 C8 0F 45 CE 84 C9");
    CHECKSIG(pStaticConstructObject, "Failed to find StaticConstructObject address!");
    StaticConstructObject = decltype(StaticConstructObject)(pStaticConstructObject);

    auto pWorld = Util::FindPattern("48 8B 05 ? ? ? ? 4D 8B C1", true, 3);
    CHECKSIG(pWorld, "Failed to find UWorld address!");
    World = *reinterpret_cast<UObject**>(pWorld);

    Functions::UpdatePlayerController();

    auto FortEngine = FindObject("FortEngine /Engine/Transient.FortEngine");
    auto FEVFT = *reinterpret_cast<void***>(FortEngine);
    auto PEAddr = FEVFT[0x4B];

    MH_Initialize();
    MH_CreateHook((void*)PEAddr, ProcessEventDetour, (void**)(&PEOG));
    MH_EnableHook((void*)PEAddr);

    InitHooks();

    Functions::UnlockConsole();

    // DumpObjects();

    auto FortGameViewportClient = FindObject("FortGameViewportClient /Engine/Transient.FortEngine_2147482585.FortGameViewportClient_2147482425");
    std::cout << "FortGameViewportClient: " << FortGameViewportClient->GetFullName() << std::endl;

    std::cout << "Setup!\n";

    return NULL;
}

BOOL APIENTRY DllMain(HMODULE mod, DWORD reason, LPVOID res)
{
    if (reason == DLL_PROCESS_ATTACH)
        CreateThread(0, 0, MainThread, mod, 0, 0);

    return TRUE;
}