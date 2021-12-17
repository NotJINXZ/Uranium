#include <Windows.h>
#include "Util.h"
#include "UE5.h"
#include <iostream>
#include "Functions.h"
#include "minhook/MinHook.h"

bool bIsReady = false;
bool bHasSpawned = false;

#include "Hooks.h"
#include "StringUtils.h"

#pragma comment(lib, "minhook/minhook.lib")

DWORD WINAPI DumpObjectThread(LPVOID param)
{
    DumpObjects();
    system(("notepad \"Dump.txt\""));
    return NULL;
}

FVector LastEmoteLoc;
bool bIsEmoting;
UObject* CurrentEmote;

DWORD WINAPI EmoteCheckThread(LPVOID)
{
    while (Pawn)
    {
        auto PawnLoc = Functions::GetActorLocation(Pawn);
        float Xdif = PawnLoc.X - LastEmoteLoc.X;
        float Ydif = PawnLoc.Y - LastEmoteLoc.Y;

        if (Xdif > 75 || Xdif < -75 || Ydif > 75 || Ydif < -75)
        {
            bIsEmoting = false;
            CurrentEmote = nullptr;
            
            auto emote = FindObject("AthenaEmojiItemDefinition /Game/Athena/Items/Cosmetics/Dances/Emoji/Emoji_S17_Believer.Emoji_S17_Believer");
            if (emote) {
                auto AnimRef = Functions::GetAnimationHardReference(emote);
                Functions::PlayMontage(AnimRef);
            }
        }

        /*if (Functions::IsFalling())
        {
            bIsEmoting = false;
            CurrentEmote = nullptr;

            auto emote = FindObject("AthenaEmojiItemDefinition /Game/Athena/Items/Cosmetics/Dances/Emoji/Emoji_S17_Believer.Emoji_S17_Believer");
            if (emote) {
                auto AnimRef = Functions::GetAnimationHardReference(emote);
                Functions::PlayMontage(AnimRef);
            }
        }*/

        Sleep(1000 / 60);
    }

    return 0;
}

void* (*PEOG)(void*, void*, void*);
void* ProcessEventDetour(UObject* pObject, UObject* pFunction, void* pParams)
{
    if (pObject && pFunction) {
        if (pFunction->GetName().find("SendClientHello") != std::string::npos ||
            pFunction->GetName().find("SendPacketToServer") != std::string::npos ||
            pFunction->GetName().find("SendPacketToClient") != std::string::npos)
        {
            return NULL;
        }

        if (pFunction->GetFullName().find("BP_OnDeactivated") != std::string::npos && pObject->GetFullName().find("PickerOverlay_EmoteWheel") != std::string::npos)
        {
            if (Pawn) {
                Functions::UnCrouch();
                
                if (Functions::IsFalling()) {
                    return NULL;
                }

                UObject* LastEmotePlayed = *reinterpret_cast<UObject**>(__int64(Controller) + __int64(0x1e78));

                if (LastEmotePlayed) {
                    std::cout << "NewEmote: " << LastEmotePlayed->GetFullName() << std::endl;

                    auto AnimRef = Functions::GetAnimationHardReference(LastEmotePlayed);

                    if (CurrentEmote == AnimRef) {
                        return NULL;
                    }

                    Functions::PlayMontage(AnimRef);

                    CurrentEmote = AnimRef;
                    LastEmoteLoc = Functions::GetActorLocation(Pawn);
                }
            }
        }

        if (pFunction->GetFullName().find(crypt("ServerExecuteInventoryItem")) != std::string::npos && FortInventory)
        {
            FGuid* guid = reinterpret_cast<FGuid*>(pParams);

            auto entries = reinterpret_cast<TArray<FFortItemEntry>*>(__int64(FortInventory) + 0x228 + 0x108);

            for (int i = 0; i < entries->Num(); i++)
            {
                auto entry = entries->operator[](i);
                auto entryGuid = reinterpret_cast<FGuid*>((uintptr_t)&entry + 0x68);

                if (IsMatchingGuid(*entryGuid, *guid))
                {
                    struct
                    {
                        UObject* WeaponData;
                        FGuid ItemEntryGuid;
                        UObject* ReturnValue;
                    } EquipWeaponDefinitionParams;

                    EquipWeaponDefinitionParams.WeaponData = *reinterpret_cast<UObject**>((uintptr_t)&entry + 0x18);
                    EquipWeaponDefinitionParams.ItemEntryGuid = *guid;

                    auto EquiptWeaponFunc = FindObject(crypt("Function /Script/FortniteGame.FortPawn.EquipWeaponDefinition"));
                    ProcessEvent((UObject*)Pawn, EquiptWeaponFunc, &EquipWeaponDefinitionParams);
                }
            }
        }

        if (pFunction->GetName().find("ServerCreateBuilding") != std::string::npos)
        {
            //auto CurrentBuildableClass = *reinterpret_cast<UObject**>((uintptr_t)Controller + 0x1638);
            //auto LastBuildPreviewGridSnapLoc = *reinterpret_cast<FVector*>((uintptr_t)Controller + 0x174c);
            //auto LastBuildPreviewGridSnapRot = *reinterpret_cast<FRotator*>((uintptr_t)Controller + 0x1758);
            //auto BuildingActor = Functions::SpawnActor(CurrentBuildableClass, LastBuildPreviewGridSnapLoc, LastBuildPreviewGridSnapRot);
            //Functions::K2_SetActorLocation(BuildingActor, LastBuildPreviewGridSnapLoc);
            //Functions::InitializeBuildingActor(BuildingActor);
            //Functions::K2_SetActorRotation(BuildingActor, LastBuildPreviewGridSnapRot);
        }

        if (pFunction->GetName().find("CheatScript") != std::string::npos) {

            struct CheatScriptParams { struct FString ScriptName; UObject* ReturnValue; };
            auto params = reinterpret_cast<CheatScriptParams*>(pParams);
            auto fstring = params->ScriptName;
            auto string = params->ScriptName.ToString();
            auto strings = String::StringUtils::Split(string, " ");

            if (strings[0] == "Dump") {
                CreateThread(0, 0, DumpObjectThread, 0, 0, 0);
            }

            if (strings[0] == "Weapon") {
                auto weapon = FindObject(strings[1] + "." + strings[1]);
                if (weapon == nullptr) {
                    Functions::UeConsoleLog(L"Failed to find weapon!\n");
                    return NULL;
                }

                Functions::AddItemToInventory(weapon, 1);
            }

            if (strings[0] == "Loadbp") {
                auto BP = strings[1];
                StaticLoadObject(FindObject(crypt("Class /Script/Engine.BlueprintGeneratedClass")), nullptr, (std::wstring(BP.begin(), BP.end()).c_str()));
            }

            if (strings[0] == crypt("Jonl")) {

                struct JonLHack_GetAllObjectsOfClassFromPathParams
                {
                    struct FString Path;
                    class UClass* Class;
                    TArray<class UObject*> ReturnValue;
                };
                auto JonLHack = FindObject(crypt("Function /Script/FortniteGame.FortKismetLibrary.JonLHack_GetAllObjectsOfClassFromPath"));
                auto path = strings[1]; // folder path
                auto classPath = strings[2]; // class path
                auto kismet = FindObject(crypt("FortKismetLibrary /Script/FortniteGame.Default__FortKismetLibrary")); // find kismet
                UClass* classInstance = reinterpret_cast<UClass*>(FindObject(std::string(classPath.begin(), classPath.end()))); // find the class from clasPath parameter

                JonLHack_GetAllObjectsOfClassFromPathParams Params{ std::wstring(path.begin(), path.end()).c_str(), classInstance }; // set up parameters
                ProcessEvent(kismet, JonLHack, &Params);
            }

            if (strings[0] == ("GrantEffect"))
            {
                auto Effect = strings[1];

                UObject** AbilitySystemComponent = reinterpret_cast<UObject**>(__int64(reinterpret_cast<UObject**>((uintptr_t)Controller + Offsets::PlayerController::AcknowledgedPawn)) + 0x3c0);

                auto EffectObject = FindObject("BlueprintGeneratedClass " + std::string(Effect.begin(), Effect.end()));
                if (EffectObject == nullptr)
                {
                    std::cout << "Could Not Find Effect \n";
                    return NULL;
                }
                //  Functions::BP_ApplyGameplayEffectToSelf(*AbilitySystemComponent, EffectObject);
            }

            if (strings[0] == "StopEmote") {
                auto emote = FindObject("AthenaEmojiItemDefinition /Game/Athena/Items/Cosmetics/Dances/Emoji/Emoji_S17_Believer.Emoji_S17_Believer");
                if (emote) {
                    auto AnimRef = Functions::GetAnimationHardReference(emote);
                    Functions::PlayMontage(AnimRef);
                }
            }

            if (strings[0] == "Play")
            {
                auto func = FindObject("Function /Script/MovieScene.MovieSceneSequencePlayer.Play");
                auto obj = FindObject(std::string(strings[1].begin(), strings[1].end()));

                if (!obj)
                {
                    std::cout << "Failed To Find Sequence" << std::endl;
                    return NULL;
                }

                ProcessEvent(obj, func, nullptr);
            }
        }

        if (pFunction->GetName().find("Tick") != std::string::npos)
        {
            if (GetAsyncKeyState(VK_F1) & 0x01) {
                Functions::SwitchLevel(L"Artemis_Terrain?Game=/Game/Athena/Athena_GameMode.Athena_GameMode_C");
                bIsReady = true;
            }

            if (GetAsyncKeyState(VK_F4) & 0x01) {
                auto emote = FindObject("AthenaEmojiItemDefinition /Game/Athena/Items/Cosmetics/Dances/Emoji/Emoji_S17_Believer.Emoji_S17_Believer");
                if (emote) {
                    auto AnimRef = Functions::GetAnimationHardReference(emote);
                    Functions::PlayMontage(AnimRef);
                }
            }
 
            if (GetAsyncKeyState(VK_F5) & 0x01 && bIsReady) {
                //CreateThread(0, 0, DumpObjectThread, 0, 0, 0);

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

            if (GetAsyncKeyState(VK_F3) & 0x01) {
                Functions::TeleportToSkydive(50000);
            }
        }

        if (pFunction->GetName().find("ServerLoadingScreenDropped") != std::string::npos)
        {
            Functions::UpdatePlayerController();
            Functions::ShowSkin();
            Functions::ShowSkin();
            Functions::EnableCheatManager();
            Functions::DestroyAll(FindObject("Class /Script/FortniteGame.FortHLODSMActor"));

            Functions::GrantGameplayAbility(Pawn, FindObject(crypt("Class /Script/FortniteGame.FortGameplayAbility_Sprint")));
            Functions::GrantGameplayAbility(Pawn, FindObject(crypt("Class /Script/FortniteGame.FortGameplayAbility_Jump")));
            Functions::GrantGameplayAbility(Pawn, FindObject(crypt("Class /Script/FortniteGame.FortGameplayAbility_Crouch")));
            Functions::GrantGameplayAbility(Pawn, FindObject(crypt("Class /Script/FortniteGame.FortGameplayAbility_RangedWeapon")));
            Functions::GrantGameplayAbility(Pawn, FindObject(crypt("Class /Script/FortniteGame.FortGameplayAbility_Reload")));
            Functions::GrantGameplayAbility(Pawn, FindObject(crypt("BlueprintGeneratedClass /Game/Abilities/Player/Sliding/GA_Athena_Player_Slide.GA_Athena_Player_Slide_C")));
            Functions::GrantGameplayAbility(Pawn, FindObject(crypt("BlueprintGeneratedClass /Game/Abilities/Player/Generic/Traits/DefaultPlayer/GA_DefaultPlayer_InteractSearch.GA_DefaultPlayer_InteractSearch_C")));
            Functions::GrantGameplayAbility(Pawn, FindObject(crypt("BlueprintGeneratedClass /Game/Abilities/Player/Generic/Traits/DefaultPlayer/GA_DefaultPlayer_InteractUse.GA_DefaultPlayer_InteractUse_C")));
            Functions::GrantGameplayAbility(Pawn, FindObject(crypt("BlueprintGeneratedClass /Game/Athena/DrivableVehicles/GA_AthenaEnterVehicle.GA_AthenaEnterVehicle_C")));
            Functions::GrantGameplayAbility(Pawn, FindObject(crypt("BlueprintGeneratedClass /Game/Athena/DrivableVehicles/GA_AthenaExitVehicle.GA_AthenaExitVehicle_C")));
            Functions::GrantGameplayAbility(Pawn, FindObject(crypt("BlueprintGeneratedClass /Game/Athena/DrivableVehicles/GA_AthenaInVehicle.GA_AthenaInVehicle_C")));
            Functions::GrantGameplayAbility(Pawn, FindObject(crypt("BlueprintGeneratedClass /Game/Athena/Items/EnvironmentalItems/HidingProps/GA_Athena_HidingProp_JumpOut.GA_Athena_HidingProp_JumpOut_C")));
            Functions::GrantGameplayAbility(Pawn, FindObject(crypt("BlueprintGeneratedClass /Game/Athena/Items/EnvironmentalItems/HidingProps/GA_Athena_HidingProp_Hide.GA_Athena_HidingProp_Hide_C")));
            Functions::GrantGameplayAbility(Pawn, FindObject(crypt("BlueprintGeneratedClass /Game/Abilities/Player/Generic/Traits/DefaultPlayer/GA_DefaultPlayer_InteractUse.GA_DefaultPlayer_InteractUse_C")));

            //auto Controller = *reinterpret_cast<UObject**>((uintptr_t)Functions::ControllerFinder());
            FortInventory = reinterpret_cast<InventoryPointer*>((uintptr_t)Controller + 0x1ab0)->Inventory;
            QuickBar = *reinterpret_cast<UObject**>((uintptr_t)Controller + 0x17f8);

            //std::cout << "FortInventory: " << FortInventory->GetFullName() << std::endl;
            //std::cout << "QuickBar: " << QuickBar << std::endl;

            //Functions::SetOwner(Controller, QuickBar);

            Functions::AddItemToInventory(Functions::GetPickaxeDef(), 1, true, EFortQuickBars::Primary, 0);
            Functions::AddItemToInventory(FindObject(crypt("FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_Wall.BuildingItemData_Wall")), 1);
            Functions::AddItemToInventory(FindObject(crypt("FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_Floor.BuildingItemData_Floor")), 1);
            Functions::AddItemToInventory(FindObject(crypt("FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_Stair_W.BuildingItemData_Stair_W")), 1);
            Functions::AddItemToInventory(FindObject(crypt("FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_RoofS.BuildingItemData_RoofS")), 1);
            Functions::AddItemToInventory(FindObject(crypt("FortResourceItemDefinition /Game/Items/ResourcePickups/WoodItemData.WoodItemData")), 999);
            Functions::AddItemToInventory(FindObject(crypt("FortResourceItemDefinition /Game/Items/ResourcePickups/StoneItemData.StoneItemData")), 999);
            Functions::AddItemToInventory(FindObject(crypt("FortResourceItemDefinition /Game/Items/ResourcePickups/MetalItemData.MetalItemData")), 999);
            Functions::AddItemToInventory(FindObject(crypt("FortAmmoItemDefinition /Game/Items/Ammo/AmmoDataEnergyCell.AmmoDataEnergyCell")), 999);
            Functions::AddItemToInventory(FindObject(crypt("FortAmmoItemDefinition /Game/Items/Ammo/AmmoDataExplosive.AmmoDataExplosive")), 999);
            Functions::AddItemToInventory(FindObject(crypt("FortAmmoItemDefinition /Game/Items/Ammo/AmmoDataShells.AmmoDataShells")), 999);
            Functions::AddItemToInventory(FindObject(crypt("FortAmmoItemDefinition /Game/Items/Ammo/AmmoDataBulletsMedium.AmmoDataBulletsMedium")), 999);
            Functions::AddItemToInventory(FindObject(crypt("FortAmmoItemDefinition /Game/Items/Ammo/AmmoDataBulletsLight.AmmoDataBulletsLight")), 999);
            Functions::AddItemToInventory(FindObject(crypt("FortAmmoItemDefinition /Game/Items/Ammo/AmmoDataBulletsHeavy.AmmoDataBulletsHeavy")), 999);
            Functions::AddItemToInventory(FindObject(crypt("FortAmmoItemDefinition /Game/Athena/Items/Ammo/AmmoInfinite_NoIcon.AmmoInfinite_NoIcon")), 80);
            Functions::SetInfiniteAmmo(Controller);
            Functions::SetGamePhase(EAthenaGamePhase::None, EAthenaGamePhase::Warmup);
            Functions::TeleportToSkydive(50000);

            Functions::ServerSetClientHasFinishedLoading(Controller);

            auto bHasServerFinishedLoading = reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(Controller) + 0x959);
            *bHasServerFinishedLoading = true;
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

    auto pWorld = Util::FindPattern("48 8B 05 ? ? ? ? 4D 8B C1", true, 3);
    CHECKSIG(pWorld, "Failed to find UWorld address!");
    World = *reinterpret_cast<UObject**>(pWorld);

    auto FortEngine = FindObject("FortEngine /Engine/Transient.FortEngine");
    auto FEVFT = *reinterpret_cast<void***>(FortEngine);
    auto PEAddr = FEVFT[0x4B];

    MH_Initialize();
    MH_CreateHook((void*)PEAddr, ProcessEventDetour, (void**)(&PEOG));
    MH_EnableHook((void*)PEAddr);

    InitHooks();

    Functions::UnlockConsole();
    Functions::UpdatePlayerController();
  //  Functions::EnableCheatManager();
   // StaticLoadObject(FindObject("Class /Script/Engine.BlueprintGeneratedClass"), nullptr, (L"/Caretaker/Pawns/NPC_Pawn_Irwin_Monster_Caretaker"));

    std::cout << "Setup!\n";

    return NULL;
}

BOOL APIENTRY DllMain(HMODULE mod, DWORD reason, LPVOID res)
{
    if (reason == DLL_PROCESS_ATTACH)
        CreateThread(0, 0, MainThread, mod, 0, 0);

    return TRUE;
}