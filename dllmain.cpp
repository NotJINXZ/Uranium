#include <Windows.h>
#include "Util.h"
#include "UE5.h"
#include <iostream>
#include "Functions.h"
#include "minhook/MinHook.h"
#include "Hooks.h"
#include "StringUtils.h"

#pragma comment(lib, "minhook/minhook.lib")

bool bIsReady = false;
bool bHasSpawned = false;

DWORD WINAPI DumpObjectThread(LPVOID param)
{
    DumpObjects();
    system(("notepad \"Dump.txt\""));
    return NULL;
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
        if ((pFunction->GetFullName().find("BP_OnDeactivated") != std::string::npos && pObject->GetFullName().find("PickerOverlay_EmoteWheel") != std::string::npos))
        {
            auto LastEmotePlayed = *reinterpret_cast<UObject**>((uintptr_t)Functions::ControllerFinder() + 0x1e78);
            auto Mesh = *reinterpret_cast<UObject**>((uintptr_t)Functions::PawnFinder() + 0x188);
            static auto GetAnimInstanceFN = FindObject("Function /Script/Engine.SkeletalMeshComponent.GetAnimInstance");

            struct GetAnimInstanceParams
            {
                UObject* ReturnValue;
            };
            GetAnimInstanceParams gaiparams;

            ProcessEvent(Mesh, GetAnimInstanceFN, &gaiparams);
            auto AnimInstance = gaiparams.ReturnValue;

            if (LastEmotePlayed)
            {
                std::cout << "NewEmote: " << LastEmotePlayed->GetFullName() << std::endl;

                auto GetAnimationHardReferenceFN = FindObject("Function /Script/FortniteGame.FortMontageItemDefinitionBase.GetAnimationHardReference");
                auto MontagePlayFN = FindObject("Function /Script/Engine.AnimInstance.Montage_Play");
                enum class EFortCustomBodyType : uint8_t
                {
                    NONE = 0,
                    Small = 1,
                    Medium = 2,
                    MediumAndSmall = 3,
                    Large = 4,
                    LargeAndSmall = 5,
                    LargeAndMedium = 6,
                    All = 7,
                    Deprecated = 8,
                    EFortCustomBodyType_MAX = 9
                };
                enum class EFortCustomGender : uint8_t
                {
                    Invalid = 0,
                    Male = 1,
                    Female = 2,
                    Both = 3,
                    EFortCustomGender_MAX = 4
                };

                enum class EMontagePlayReturnType : uint8_t
                {
                    MontageLength = 0,
                    Duration = 1,
                    EMontagePlayReturnType_MAX = 2
                };

                struct UFortMontageItemDefinitionBase_GetAnimationHardReference_Params
                {
                    TEnumAsByte<EFortCustomBodyType> BodyType;
                    TEnumAsByte<EFortCustomGender> Gender;
                    UObject* PawnContext;
                    UObject* ReturnValue;
                };

                struct UAnimInstance_Montage_Play_Params
                {
                    UObject* MontageToPlay;
                    float InPlayRate;
                    EMontagePlayReturnType ReturnValueType;
                    float InTimeToStartMontageAt;
                    bool bStopAllMontages;
                    float ReturnValue;
                };

                UFortMontageItemDefinitionBase_GetAnimationHardReference_Params GetAnimationHardReference_Params;
                GetAnimationHardReference_Params.BodyType = EFortCustomBodyType::All;
                GetAnimationHardReference_Params.Gender = EFortCustomGender::Both;
                GetAnimationHardReference_Params.PawnContext = (UObject*)Functions::PawnFinder();

                ProcessEvent(LastEmotePlayed, GetAnimationHardReferenceFN, &GetAnimationHardReference_Params);

                auto Animation = GetAnimationHardReference_Params.ReturnValue;

                auto CurrentEmote = Animation;

                if (Animation == CurrentEmote)
                {
                    return NULL;
                }


                UAnimInstance_Montage_Play_Params params;
                params.MontageToPlay = Animation;
                params.InPlayRate = 1;
                params.ReturnValueType = EMontagePlayReturnType::Duration;
                params.InTimeToStartMontageAt = 0;
                params.bStopAllMontages = true;

                ProcessEvent(AnimInstance, MontagePlayFN, &params);

               auto LastEmoteLoc = Functions::GetActorLocation((UObject*)Functions::PawnFinder());
            }
        }


        if (pFunction->GetName().find("CheatScript") != std::string::npos) {

            struct CheatScriptParams { struct FString ScriptName; UObject* ReturnValue; };
            auto params = reinterpret_cast<CheatScriptParams*>(pParams);
            auto fstring = params->ScriptName;
            auto string = params->ScriptName.ToString();
            auto strings = String::StringUtils::Split(string, " ");

            if (strings[0] == "dump") {
                CreateThread(0, 0, DumpObjectThread, 0, 0, 0);
            }

            if (strings[0] == "weapon") {

                auto weapon = strings[1];
                Functions::EquipWeapon(weapon);
            }

            if (strings[0] == "weapon") {


                std::string converted(strings[1].begin(), strings[1].end());
                auto WeaponToEquip = FindObject(converted);
                struct
                {
                    UObject* WeaponData;
                    FGuid ItemEntryGuid;
                    UObject* ReturnValue;
                } EquipWeaponDefinitionParams;

                EquipWeaponDefinitionParams.WeaponData = WeaponToEquip;
                FGuid NewGUID;
                NewGUID.A = rand() % 1000;
                NewGUID.B = rand() % 1000;
                NewGUID.C = rand() % 1000;
                NewGUID.D = rand() % 1000;
                EquipWeaponDefinitionParams.ItemEntryGuid = NewGUID;
                auto EquiptWeaponFunc = FindObject("Function /Script/FortniteGame.FortPawn.EquipWeaponDefinition");
                ProcessEvent((UObject*)LocalPawn, EquiptWeaponFunc, &EquipWeaponDefinitionParams);
            }

            if (strings[0] == "loadbp") {
                auto BP = strings[1];
                StaticLoadObject(FindObject(crypt("Class /Script/Engine.BlueprintGeneratedClass")), nullptr, (std::wstring(BP.begin(), BP.end()).c_str()));
                //Functions::SpawnActorEasy(BP);
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

                if (strings[0] == ("granteffect"))
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

                if (strings[0] == "play")
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
               Functions::SwitchLevel(L"Artemis_terrain?game=/Script/FortniteGame.FortGameModeEmptyDedicated");
                bIsReady = true;
            }
            /*
            if (GetAsyncKeyState(VK_SPACE) & 0x01) {
                static auto fn = FindObject("Function /Script/Engine.Character.Jump");

                ProcessEvent((UObject*)Functions::PawnFinder(), fn, nullptr);
            }
            */
            if (GetAsyncKeyState(VK_F2) & 0x01) {
                auto Playlist = FindObject("FortPlaylistAthena /Game/Athena/Playlists/Omaha/Playlist_Omaha.Playlist_Omaha");
              //  Functions::SetPlaylist(Playlist);
               Functions::ShowSkin();
               // Functions::CustomSkin("/game/Characters/CharacterParts/Male/Medium/Heads/CP_Head_Med_Soldier_M_BananaWinter","/Game/Athena/Heroes/Meshes/Bodies/CP_Athena_Body_M_BananaWinter");
              // Functions::EnableCheatManager();

         //       Functions::SpawnPlayer();
                printf("1\n");
           //     Functions::Possess(Pawn);
                printf("2\n");
                Functions::EnableCheatManager();

             //   Functions::ServerReadyToStartMatch();
                printf("3\n");
             //   Functions::StartMatch();
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