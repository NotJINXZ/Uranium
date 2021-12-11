#pragma once

#include "UE5.h"
#include "OffSetTable.hpp"

UObject* Controller;
UObject* World;
UObject* Pawn;

namespace Functions
{
	inline UObject* (*SpawnActorLong)(UObject* UWorld, UObject* Class, FTransform const* UserTransformPtr, const FActorSpawnParameters& SpawnParameters);

	static UObject* SpawnActorFromLong(UObject* Class, FTransform UserTransformPtr)
	{
		auto spawnParms = FActorSpawnParameters();
		SpawnActorLong = decltype(SpawnActorLong)(Util::FindPattern("48 8B C4 55 53 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 A8 0F 29 78 98 44 0F 29 40 ? 44 0F 29 88 ? ? ? ? 44 0F 29 90 ? ? ? ? 44 0F 29 98 ? ? ? ? 44 0F 29 A0 ? ? ? ? 44 0F 29 A8 ? ? ? ? 44 0F 29 B0 ? ? ? ? 44 0F 29 B8 ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 60 45 33 ED 48 89 4D 90 44 89 6D 80 48 8D 05 ? ? ? ? 44 38 2D ? ? ? ? 4C 8B F2 48 8B D9 48 8D 15 ? ? ? ? 49 0F 45 C5 48 8D 4D B8 48 89 45 B0 49 8B F1 4D 8B E0 E8 ? ? ? ? 4C 8B 7B 30 4C 89 7C 24 ? 4D 85 F6 0F 84 ? ? ? ? 41 8B 86 ? ? ? ? 0F BA E0 19 0F 82 ? ? ? ? A8 01 0F 85 ? ? ? ? E8 ? ? ? ? 48 8B D0 49 8B CE E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? 48 8B 46 08 48 8D 3D ? ? ? ? 48 85 C0 74 0A"));
		return SpawnActorLong(World, Class, &UserTransformPtr, spawnParms);
	}

	static UObject* UpdatePlayerController()
	{
		auto statics = FindObject("GameplayStatics /Script/Engine.Default__GameplayStatics");
		auto fn = FindObject("Function /Script/Engine.GameplayStatics.GetPlayerController");

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
		std::cout << "PlayerController: " << Controller << std::endl;

		return params.ReturnValue;
	}

	static void SwitchLevel(FString URL)
	{
		auto fn = FindObject("Function /Script/Engine.PlayerController.SwitchLevel");
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
		auto GameState = GetGameState();
		auto BasePlaylist = reinterpret_cast<UObject**>((uintptr_t)GameState + Offsets::GameState::CurrentPlaylistInfo + Offsets::CurrentPlaylistInfo::BasePlaylist);
		*BasePlaylist = Playlist;

		auto fn = FindObject("Function /Script/FortniteGame.FortGameStateAthena.OnRep_CurrentPlaylistInfo");
		ProcessEvent(GameState, fn, nullptr);
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
		auto PawnClass = FindObject("BlueprintGeneratedClass /Game/Athena/PlayerPawn_Athena.PlayerPawn_Athena_C");
		auto PawnSpawned = SpawnActor(PawnClass, FVector(0, 0, 0), FRotator(0, 0, 0));
		Pawn = PawnSpawned;
	}

	static void ServerReadyToStartMatch()
	{
		auto fn = FindObject("Function /Script/FortniteGame.FortPlayerController.ServerReadyToStartMatch");
		ProcessEvent(Controller, fn, nullptr);
	}

	static void StartMatch()
	{
		auto fn = FindObject("Function /Script/Engine.GameMode.StartMatch");
		ProcessEvent(GetGameMode(), fn, nullptr);
	}

	static void Possess(UObject* Pawn)
	{
		auto fn = FindObject("Function /Script/Engine.Controller.Possess");
		ProcessEvent(Controller, fn, &Pawn);
	}


	static void UnlockConsole()
	{
		auto FortGameViewportClient = FindObject("FortGameViewportClient /Engine/Transient.FortEngine_2147482585.FortGameViewportClient_2147482425");
		auto fn = FindObject("Function /Script/Engine.GameplayStatics.SpawnObject");
		auto statics = FindObject("GameplayStatics /Script/Engine.Default__GameplayStatics");
		auto ConsoleClass = FindObject("/Script/Engine.Console");
		auto ViewportConsole = reinterpret_cast<UObject**>((uintptr_t)FortGameViewportClient + 0x40);

		std::cout << "FortGameViewportClient: " << FortGameViewportClient << std::endl;
		std::cout << "SpawnObject: " << fn << std::endl;
		std::cout << "Gameplay Statics: " << statics << std::endl;
		std::cout << "Console Class: " << ConsoleClass << std::endl;
		std::cout << "ViewportConsole: " << ViewportConsole << std::endl;

		SpawnObjectParams params;
		params.ObjectClass = ConsoleClass;
		params.Outer = FortGameViewportClient;

		std::cout << "Before PE" << std::endl;
		ProcessEvent(statics, fn, &params);
		std::cout << "After PE" << std::endl;

		//std::cout << "Return Value: " << params.ReturnValue;
		*ViewportConsole = params.ReturnValue;

	}

	static void EnableCheatManager()
	{
		auto fn = FindObject("Function /Script/Engine.GameplayStatics.SpawnObject");
		auto statics = FindObject("GameplayStatics /Script/Engine.Default__GameplayStatics");
		auto CheatManager = reinterpret_cast<UObject**>((uintptr_t)Controller + Offsets::PlayerController::CheatManager);
		auto CheatManagerClass = FindObject("/Script/Engine.CheatManager");

		SpawnObjectParams params;
		params.ObjectClass = CheatManagerClass;
		params.Outer = Controller;

		ProcessEvent(statics, fn, &params);

		*CheatManager = params.ReturnValue;
	}
}