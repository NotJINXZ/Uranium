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
bool bIsPickingUp = false;
UObject* OldCheat;

void* (*PEOG)(void*, void*, void*);
void* ProcessEventDetour(UObject* pObject, UObject* pFunction, void* pParams)
{
    if (pObject && pFunction) {

        auto FuncName = pFunction->GetName();
        auto FullFuncName = pFunction->GetFullName();

        if (FullFuncName.find(crypt("BP_OnDeactivated")) != std::string::npos && pObject->GetFullName().find(crypt("PickerOverlay_EmoteWheel")) != std::string::npos)
        {
            if (Pawn) {
                Functions::UnCrouch();
                
                if (Functions::IsFalling()) {
                    return NULL;
                }

                UObject* LastEmotePlayed = *reinterpret_cast<UObject**>(__int64(Controller) + __int64(Offsets::LastEmotePlayedOffset));

                if (LastEmotePlayed) {
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

        if (pObject->GetName().find(crypt("RiftPortal_Item_Athena")) != std::string::npos && FuncName.find(crypt("ComponentBeginOverlapSignature")) != std::string::npos)
        {
            Functions::TeleportToSkydive(25000);
        }

        if (FullFuncName.find(crypt("ServerExecuteInventoryItem")) != std::string::npos && FortInventory)
        {
            FGuid* guid = reinterpret_cast<FGuid*>(pParams);

            auto entries = reinterpret_cast<TArray<FFortItemEntry>*>(__int64(FortInventory) + __int64(Offsets::InventoryOffset) + __int64(Offsets::EntriesOffset));

            for (int i = 0; i < entries->Num(); i++)
            {
                auto entry = entries->operator[](i);
                auto entryGuid = reinterpret_cast<FGuid*>((uintptr_t)&entry + __int64(Offsets::ItemGuidOffset));

                if (IsMatchingGuid(*entryGuid, *guid))
                {
                    struct
                    {
                        UObject* WeaponData;
                        FGuid ItemEntryGuid;
                        UObject* ReturnValue;
                    } EquipWeaponDefinitionParams;

                    EquipWeaponDefinitionParams.WeaponData = *reinterpret_cast<UObject**>((uintptr_t)&entry + __int64(Offsets::ItemDefinitionOffset));
                    EquipWeaponDefinitionParams.ItemEntryGuid = *guid;

                    auto EquiptWeaponFunc = FindObject(crypt("Function /Script/FortniteGame.FortPawn.EquipWeaponDefinition"));
                    ProcessEvent((UObject*)Pawn, EquiptWeaponFunc, &EquipWeaponDefinitionParams);
                }
            }
        }

        if (FuncName.find(crypt("ServerCreateBuilding")) != std::string::npos)
        {
            CreateThread(0, 0, Functions::BuildAsync, 0, 0, 0);
        }

        if (FuncName.find(crypt("ServerHandlePickup")) != std::string::npos && FortInventory)
        {
            if (!bIsPickingUp)
                bIsPickingUp = true;

            struct Params
            {
                UObject* PickUp;
                float InFlyTime;
                FVector InStartDirection;
                bool bPlayPickupSound;
            };
            auto params = (Params*)(pParams);

            if (params->PickUp == nullptr) {
                printf(crypt("Invalid pickup!\n"));
                return NULL;
            }

            auto primQuickbar = reinterpret_cast<FQuickBar*>((uintptr_t)QuickBar + __int64(Offsets::PrimQuickBarOffset));
            auto entries = reinterpret_cast<TArray<FFortItemEntry>*>(__int64(FortInventory) + static_cast<__int64>(Offsets::InventoryOffset) + static_cast<__int64>(Offsets::EntriesOffset));

            auto PickupEntry = reinterpret_cast<FFortItemEntry*>((uintptr_t)params->PickUp + __int64(Offsets::PickupEntryOffset));
            Functions::AddItemToInventoryWithEntry(*PickupEntry, *(int*)((uintptr_t)PickupEntry + __int64(Offsets::EntryCountOffset)));

            Functions::DestroyActor(params->PickUp);
        }

        if (FuncName.find(crypt("ServerAttemptInventoryDrop")) != std::string::npos && FortInventory)
        {
            if (bIsPickingUp) {
                bIsPickingUp = false;
                return NULL;
            }

            printf(crypt("Called remove item!\n"));

            struct Params
            {
                FGuid ItemGuid;
                int32_t Count;
                bool bTrash;
            };
            auto params = (Params*)(pParams);

            auto entries = reinterpret_cast<TArray<FFortItemEntry>*>(__int64(FortInventory) + static_cast<__int64>(Offsets::InventoryOffset) + static_cast<__int64>(Offsets::EntriesOffset));

            for (int i = 0; i < entries->Num(); i++)
            {
                auto entry = entries->operator[](i);
                auto entryGuid = reinterpret_cast<FGuid*>((uintptr_t)&entry + __int64(Offsets::ItemGuidOffset));
                auto entryItemDef = *reinterpret_cast<UObject**>((uintptr_t)&entry + __int64(Offsets::ItemDefinitionOffset));
                auto entryCount = *reinterpret_cast<int*>((uintptr_t)&entry + 0xC);

                if (IsMatchingGuid(params->ItemGuid, *entryGuid)) {
                    entries->Remove(i);
                    Functions::SpawnPickup(entryItemDef, params->Count, EFortPickupSourceTypeFlag::Tossed, EFortPickupSpawnSource::TossedByPlayer);

                    if (params->Count != entryCount)
                    {
                        Functions::AddItemToInventory(entryItemDef, entryCount - params->Count);
                    }

                    Functions::OnRep_QuickbarEquippedItems();
                    Functions::OnRep_AccumulatedItems();
                    Functions::UpdateInventory();
                }
            }
        }

        if (FuncName.find(crypt("ServerReturnToMainMenu")) != std::string::npos)
        {
            auto CheatManager = reinterpret_cast<UObject**>((uintptr_t)Controller + __int64(FindOffset("PlayerController", "CheatManager")));
            *CheatManager = nullptr;
            Sleep(500);
            Functions::SwitchLevel(L"Frontend?Game=/Script/FortniteGame.FortGameModeFrontEnd");
        }

        if (FuncName.find(crypt("CheatScript")) != std::string::npos) {

            struct CheatScriptParams { struct FString ScriptName; UObject* ReturnValue; };
            auto params = reinterpret_cast<CheatScriptParams*>(pParams);
            auto fstring = params->ScriptName;
            auto string = params->ScriptName.ToString();
            auto strings = String::StringUtils::Split(string, " ");
            strings[0] = String::StringUtils::ToLower(strings[0]);

            if (strings[0] == crypt("dump")) {
                CreateThread(0, 0, DumpObjectThread, 0, 0, 0);
            }

            if (strings[0] == crypt("weapon")) {
                auto weapon = FindObject(strings[1] + "." + strings[1]);
                if (weapon == nullptr) {
                    Functions::UeConsoleLog(crypt(L"Failed to find weapon!\n"));
                    return NULL;
                }

                Functions::AddItemToInventory(weapon, 1);
            }

            if (strings[0] == crypt("pickup")) {
                auto weapon = FindObject(strings[1] + "." + strings[1]);
                if (weapon == nullptr) {
                    Functions::UeConsoleLog(crypt(L"Failed to find pickup!\n"));
                    return NULL;
                }

                Functions::SpawnPickup(weapon, 1, EFortPickupSourceTypeFlag::Other, EFortPickupSpawnSource::Unset);
            }

            if (strings[0] == crypt("loadbp")) {
                auto BP = strings[1];
                StaticLoadObject(FindObject(crypt("Class /Script/Engine.BlueprintGeneratedClass")), nullptr, (std::wstring(BP.begin(), BP.end()).c_str()));
            }

            if (strings[0] == crypt("jonl")) {

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

            if (strings[0] == crypt("setskin"))
            {
                Functions::CustomSkin(strings[1], strings[2]);
            }

            if (strings[0] == crypt("stopemote")) {
                auto emote = FindObject(crypt("AthenaEmojiItemDefinition /Game/Athena/Items/Cosmetics/Dances/Emoji/Emoji_S17_Believer.Emoji_S17_Believer"));
                if (emote) {
                    auto AnimRef = Functions::GetAnimationHardReference(emote);
                    Functions::PlayMontage(AnimRef);
                }
            }

            if (strings[0] == "play")
            {
                auto func = FindObject(crypt("Function /Script/MovieScene.MovieSceneSequencePlayer.Play"));
                auto obj = FindObject(std::string(strings[1].begin(), strings[1].end()));

                if (!obj)
                {
                    std::cout << crypt("Failed To Find Sequence") << std::endl;
                    return NULL;
                }

                ProcessEvent(obj, func, nullptr);
            }
        }

        if (FuncName.find("Tick") != std::string::npos)
        {
            if (GetAsyncKeyState(VK_F1) & 0x01) {
                Functions::SwitchLevel(crypt(L"Artemis_Terrain?game=/Game/Athena/Athena_GameMode.Athena_GameMode_C"));
                bIsReady = true;
            }

            if (GetAsyncKeyState(VK_F4) & 0x01) {
                auto emote = FindObject(crypt("AthenaEmojiItemDefinition /Game/Athena/Items/Cosmetics/Dances/Emoji/Emoji_S17_Believer.Emoji_S17_Believer"));
                if (emote) {
                    auto AnimRef = Functions::GetAnimationHardReference(emote);
                    Functions::PlayMontage(AnimRef);
                }
            }
 
            if (GetAsyncKeyState(VK_F5) & 0x01 && bIsReady) {
                //CreateThread(0, 0, DumpObjectThread, 0, 0, 0);
                Functions::InitMatch();
            }

            if (GetAsyncKeyState(VK_F3) & 0x01) {
                Functions::TeleportToSkydive(50000);
            }
        }

        if (FuncName.find(crypt("ServerLoadingScreenDropped")) != std::string::npos)
        {
            Functions::UpdatePlayerController();
            Functions::ShowSkin();
            Functions::EnableCheatManager();
            Functions::DestroyAll(FindObject(crypt("Class /Script/FortniteGame.FortHLODSMActor")));

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

            FortInventory = reinterpret_cast<InventoryPointer*>((uintptr_t)Controller + __int64(FindOffset("FortPlayerController", "WorldInventory")))->Inventory;
            QuickBar = reinterpret_cast<QuickBarPointer*>((uintptr_t)Controller + __int64(FindOffset("FortPlayerController", "ClientQuickBars")))->QuickBar;

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
            Functions::AddItemToInventory(FindObject(crypt("FortAmmoItemDefinition /Game/Athena/Items/Ammo/AmmoInfinite_NoIcon.AmmoInfinite_NoIcon")), 999);
            Functions::SetInfiniteAmmo(Controller);
            Functions::SetGamePhase(EAthenaGamePhase::None, EAthenaGamePhase::Warmup);
            Functions::TeleportToSkydive(60000);

            Functions::ServerSetClientHasFinishedLoading(Controller);

            auto bHasServerFinishedLoading = reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(Controller) + __int64(FindOffset("FortPlayerController", "bHasServerFinishedLoading")));
            *bHasServerFinishedLoading = true;

            Offsets::InitPreDefinedOffsets();
        }
    }

    return PEOG(pObject, pFunction, pParams);
}

DWORD WINAPI MainThread(LPVOID)
{
    Util::InitConsole();

    std::cout << "Finding Patterns!\n";

    //17 - 19
    auto pGObjects = Util::FindPattern(crypt("48 8B 05 ? ? ? ? 48 8B 0C C8 48 8B 04 D1"), true, 3);
    CHECKSIG(pGObjects, "Failed to find GObjects address!");
    GObjects = decltype(GObjects)(pGObjects);

    //19
    auto pFNameToString = Util::FindPattern(crypt("48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 56 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 48 8B F2 4C 8B F1 E8 ? ? ? ? 45 8B 06 33 ED"));
    if (!pFNameToString) {
        //17 - 18
        pFNameToString = Util::FindPattern(crypt("48 89 5C 24 ? 48 89 74 24 ? 55 57 41 56 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 45 33 F6 48 8B F2 44 39 71 04 0F 85 ? ? ? ? 8B 19 0F B7 FB E8 ? ? ? ? 8B CB 48 8D 54 24"));
    }
    CHECKSIG(pFNameToString, "Failed to find FNameToString address!");
    FNameToString = decltype(FNameToString)(pFNameToString);

    //17 - 19
    auto pFreeMemory = Util::FindPattern(crypt("48 85 C9 0F 84 ? ? ? ? 53 48 83 EC 20 48 89 7C 24 30 48 8B D9 48 8B 3D ? ? ? ? 48 85 FF 0F 84 ? ? ? ? 48 8B 07 4C 8B 40 30 48 8D 05 ? ? ? ? 4C 3B C0"));
    CHECKSIG(pFreeMemory, "Failed to find FreeMemory address!");
    FreeMemory = decltype(FreeMemory)(pFreeMemory);
    
    //19
    auto pWorld = Util::FindPattern(crypt("48 8B 05 ? ? ? ? 4D 8B C1"), true, 3);
    if (!pWorld) {
        //17 - 18
        pWorld = Util::FindPattern(crypt("48 8B 05 ? ? ? ? 4D 8B C2"), true, 3);
    }
    CHECKSIG(pWorld, "Failed to find UWorld address!");
    World = reinterpret_cast<UObject**>(pWorld);

    auto FortEngine = FindObject(crypt("FortEngine /Engine/Transient.FortEngine"));
    auto FEVFT = *reinterpret_cast<void***>(FortEngine);
    auto PEAddr = FEVFT[0x4B];

    MH_Initialize();
    MH_CreateHook((void*)PEAddr, ProcessEventDetour, (void**)(&PEOG));
    MH_EnableHook((void*)PEAddr);

    InitHooks();

    Functions::UnlockConsole();
    Functions::UpdatePlayerController();

    std::cout << "Setup!\n";

    return NULL;
}

BOOL APIENTRY DllMain(HMODULE mod, DWORD reason, LPVOID res)
{
    if (reason == DLL_PROCESS_ATTACH)
        CreateThread(0, 0, MainThread, mod, 0, 0);

    return TRUE;
}
