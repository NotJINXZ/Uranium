#pragma once

#include "UE5.h"
#include "OffSetTable.hpp"
#include "skCryptor.h"

UObject* Controller;
UObject* World;
UObject* Pawn;
UObject* GameState;
PVOID LocalPawn;
UObject* QuickBar;
UObject* FortInventory;

namespace Functions
#define RELATIVE_ADDR(addr, size) ((PBYTE)((UINT_PTR)(addr) + *(PINT)((UINT_PTR)(addr) + ((size) - sizeof(INT))) + (size)))
#define ReadPointer(base, offset) (*(PVOID *)(((PBYTE)base + offset)))
{
	inline UObject* (*SpawnActorLong)(UObject* UWorld, UObject* Class, FTransform const* UserTransformPtr, const FActorSpawnParameters& SpawnParameters);


	PVOID ControllerFinder() {
		auto dWorld = Util::FindPattern("48 8B 05 ? ? ? ? 4D 8B C1", true, 3);
		CHECKSIG(dWorld, "Failed to find UWorld address!");
		auto Worldd = *reinterpret_cast<UObject**>(dWorld);
		auto GInstance = ReadPointer(Worldd, 0x190);
		if (!GInstance) printf("GameInstance");

		auto Players = ReadPointer(GInstance, 0x038);
		if (!Players) printf("Players");

		auto Player = ReadPointer(Players, 0x0); // Gets the first user in the array (LocalPlayers[0]).
		if (!Player) printf("Player");

		auto PlayerController = ReadPointer(Player, 0x30);
		if (!PlayerController) printf("PlayerController");
		return PlayerController;
	}
	PVOID PawnFinder() {LocalPawn = ReadPointer(ControllerFinder(), 0x2A8);if (!LocalPawn) printf("LocalPawn");return LocalPawn;}

	static FVector GetActorLocation(UObject* Actor)
	{
		static auto fn = FindObject(crypt("Function /Script/Engine.Actor.K2_GetActorLocation"));

		struct Params
		{
			FVector ReturnValue;
		};
		Params params;

		ProcessEvent(Actor, fn, &params);

		return params.ReturnValue;
	}


	static void ShowSkin() {

		struct UFortKismetLibrary_UpdatePlayerCustomCharacterPartsVisualization_Params
		{
			UObject* PlayerState;
		};
		auto PlayerState = ReadPointer(Functions::PawnFinder(), 0x238);
		auto KismetLib = FindObject(crypt("FortKismetLibrary /Script/FortniteGame.Default__FortKismetLibrary"));
		static auto fn = FindObject(crypt("Function /Script/FortniteGame.FortKismetLibrary.UpdatePlayerCustomCharacterPartsVisualization"));

		UFortKismetLibrary_UpdatePlayerCustomCharacterPartsVisualization_Params params;
		params.PlayerState = (UObject*)PlayerState;
		ProcessEvent(KismetLib, fn, &params);
	}



	static UObject* SpawnActorFromLong(UObject* Class, FTransform UserTransformPtr)
	{
		auto spawnParms = FActorSpawnParameters();
		SpawnActorLong = decltype(SpawnActorLong)(Util::FindPattern(crypt("48 8B C4 55 53 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 A8 0F 29 78 98 44 0F 29 40 ? 44 0F 29 88 ? ? ? ? 44 0F 29 90 ? ? ? ? 44 0F 29 98 ? ? ? ? 44 0F 29 A0 ? ? ? ? 44 0F 29 A8 ? ? ? ? 44 0F 29 B0 ? ? ? ? 44 0F 29 B8 ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 60 45 33 ED 48 89 4D 90 44 89 6D 80 48 8D 05 ? ? ? ? 44 38 2D ? ? ? ? 4C 8B F2 48 8B D9 48 8D 15 ? ? ? ? 49 0F 45 C5 48 8D 4D B8 48 89 45 B0 49 8B F1 4D 8B E0 E8 ? ? ? ? 4C 8B 7B 30 4C 89 7C 24 ? 4D 85 F6 0F 84 ? ? ? ? 41 8B 86 ? ? ? ? 0F BA E0 19 0F 82 ? ? ? ? A8 01 0F 85 ? ? ? ? E8 ? ? ? ? 48 8B D0 49 8B CE E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? 48 8B 46 08 48 8D 3D ? ? ? ? 48 85 C0 74 0A")));
		return SpawnActorLong(World, Class, &UserTransformPtr, spawnParms);
	}
	static UObject* UpdatePlayerController()
	{
		auto statics = FindObject(crypt("GameplayStatics /Script/Engine.Default__GameplayStatics"));
		auto fn = FindObject(crypt("Function /Script/Engine.GameplayStatics.GetPlayerController"));

		struct Params
		{
			UObject* WorldContextObject;
			int PlayerIndex;
			UObject* ReturnValue;
		};
		Params params;
		params.WorldContextObject = World;
		params.PlayerIndex = 0;

		ProcessEvent(statics, fn, &params);

		Controller = params.ReturnValue;
		std::cout << crypt("PlayerController: ") << Controller << std::endl;

		return params.ReturnValue;
	}

	void EquipWeapon(std::string Weapon) {
		auto WeaponToEquip = FindObject(Weapon);

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

		ProcessEvent((UObject*)PawnFinder(), EquiptWeaponFunc, &EquipWeaponDefinitionParams);

	}

	static void CustomSkin(std::string DefaultHeadPart ,std::string DefaultBodyPart) {
		TCHAR* DefaultHeadPartparam = new TCHAR[DefaultHeadPart.size() + 1];
		TCHAR* DefaultBodyPartparam = new TCHAR[DefaultBodyPart.size() + 1];


		StaticLoadObject(FindObject("Class /Script/FortniteGame.CustomCharacterPart"), nullptr, (DefaultHeadPartparam));
		StaticLoadObject(FindObject("Class /Script/FortniteGame.CustomCharacterPart"), nullptr, (DefaultBodyPartparam));


		auto PlayerState = ReadPointer(PawnFinder(), 0x238);
		UObject* DefaultHead = FindObject("CustomCharacterPart " + DefaultBodyPart);
		UObject* DefaultBody = FindObject("CustomCharacterPart " + DefaultBodyPart);

		DWORD CharacterDataOffset = 0x4F0;
		DWORD PartsOffset = 0x08;

		UObject** HeadPart = reinterpret_cast<UObject**>(__int64(PlayerState) + __int64(CharacterDataOffset) + __int64(PartsOffset));
		UObject** BodyPart = reinterpret_cast<UObject**>(__int64(PlayerState) + __int64(CharacterDataOffset) + __int64(PartsOffset) + __int64(8));
		*HeadPart = DefaultHead;
		*BodyPart = DefaultBody;

		UObject* OnRep_CharacterDataFunc = FindObject("Function /Script/FortniteGame.FortPlayerState.OnRep_CharacterData");

		ProcessEvent((UObject*)PlayerState, OnRep_CharacterDataFunc, nullptr);
	}
	static void SwitchLevel(FString URL)
	{
		auto fn = FindObject(crypt("Function /Script/Engine.PlayerController.SwitchLevel"));
		ProcessEvent(Controller, fn, &URL);
	}

	static UObject* GetGameState()
	{
		auto GameState = *reinterpret_cast<UObject**>((uintptr_t)World + Offsets::World::GameState);
		return GameState;
	}

	static UObject* GetGameMode()
	{
		auto GameMode = *reinterpret_cast<UObject**>((uintptr_t)World + Offsets::World::GameMode);
		return GameMode;
	}

		static void SetPlaylist(UObject* Playlist)
	{
		auto CurrentPlaylistInfo = reinterpret_cast<FPlaylistPropertyArray*>((uintptr_t)GetGameState() + 0x2068);
		CurrentPlaylistInfo->BasePlaylist = Playlist;
		CurrentPlaylistInfo->OverridePlaylist = Playlist;

		auto fn = FindObject(crypt("Function /Script/FortniteGame.FortGameStateAthena.OnRep_CurrentPlaylistInfo"));
		ProcessEvent(GetGameState(), fn, nullptr);
	}


	static UObject* SpawnActor(UObject* ClassToSpawn, FVector loc, FRotator rot)
	{
		FQuat SpawnQuat;
		SpawnQuat.W = 0;
		SpawnQuat.X = rot.Pitch;
		SpawnQuat.Y = rot.Roll;
		SpawnQuat.Z = rot.Yaw;

		FTransform SpawnTrans;
		SpawnTrans.Scale3D = FVector(1, 1, 1);
		SpawnTrans.Translation = loc;
		SpawnTrans.Rotation = SpawnQuat;

		return SpawnActorFromLong(ClassToSpawn, SpawnTrans);
	}

	static void SpawnPlayer()
	{
		auto PawnClass = FindObject(crypt("BlueprintGeneratedClass /Game/Athena/PlayerPawn_Athena.PlayerPawn_Athena_C"));
		//auto PawnSpawned = SpawnActor(PawnClass, FVector(0, 0, 0));
	//	Pawn = PawnSpawned;
	}

	static void StartFireWorks() {
		auto fn = FindObject(crypt("Function /NewYears/Blueprints/BP_NewYearTimer.BP_NewYearTimer_C.ReceiveBeginPlay"));
		auto Class = FindObject(crypt("BlueprintGeneratedClass /NewYears/Levels/Artemis_NYE_Celebration.Artemis_NYE_Celebration_C"));

		std::cout << crypt("Class: ") << Class << std::endl;

		ProcessEvent(Class,fn,nullptr);
	}

	static void ServerReadyToStartMatch()
	{
		auto fn = FindObject(crypt("Function /Script/FortniteGame.FortPlayerController.ServerReadyToStartMatch"));
		ProcessEvent(Controller, fn, nullptr);
	}

	static void StartMatch()
	{
		auto fn = FindObject(crypt("Function /Script/Engine.GameMode.StartMatch"));
		ProcessEvent(GetGameMode(), fn, nullptr);
	}

	static void Possess(UObject* Pawn)
	{
		auto fn = FindObject(crypt("Function /Script/Engine.Controller.Possess"));
		ProcessEvent(Controller, fn, &Pawn);
	}


	static void UnlockConsole()
	{
		auto FortGameViewportClient = FindObject(crypt("FortGameViewportClient /Engine/Transient.FortEngine_"));
		auto fn = FindObject(crypt("Function /Script/Engine.GameplayStatics.SpawnObject"));
		auto statics = FindObject(crypt("GameplayStatics /Script/Engine.Default__GameplayStatics"));
		auto ConsoleClass = FindObject(crypt("/Script/Engine.Console"));
		auto ViewportConsole = reinterpret_cast<UObject**>((uintptr_t)FortGameViewportClient + 0x40);

	//	std::cout << "FortGameViewportClient: " << FortGameViewportClient << std::endl;
	//	std::cout << "SpawnObject: " << fn << std::endl;
//std::cout << "Gameplay Statics: " << statics << std::endl;
	//	std::cout << "Console Class: " << ConsoleClass << std::endl;
	//	std::cout << "ViewportConsole: " << ViewportConsole << std::endl;

		SpawnObjectParams params;
		params.ObjectClass = ConsoleClass;
		params.Outer = FortGameViewportClient;

		ProcessEvent(statics, fn, &params);

		//std::cout << "Return Value: " << params.ReturnValue;
		*ViewportConsole = params.ReturnValue;

	}

	static void EnableCheatManager()
	{
		auto fn = FindObject(crypt("Function /Script/Engine.GameplayStatics.SpawnObject"));
		auto statics = FindObject(crypt("GameplayStatics /Script/Engine.Default__GameplayStatics"));
		auto CheatManager = reinterpret_cast<UObject**>((uintptr_t)ControllerFinder() + Offsets::PlayerController::CheatManager);
		auto CheatManagerClass = FindObject(crypt("/Script/Engine.CheatManager"));

		SpawnObjectParams params;
		params.ObjectClass = CheatManagerClass;
		params.Outer = (UObject*)ControllerFinder();

		ProcessEvent(statics, fn, &params);

		std::cout << crypt("CheatManager: ") << params.ReturnValue << std::endl;
		*CheatManager = params.ReturnValue;
	}
}