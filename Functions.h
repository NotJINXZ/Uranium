#pragma once

#include "UE5.h"
#include "OffSetTable.hpp"

UObject* Controller;
UObject* World;
UObject* Pawn;

namespace Functions
{
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
		auto CurrentPlaylistInfo = reinterpret_cast<FPlaylistPropertyArray*>((uintptr_t)GameState + Offsets::GameState::CurrentPlaylistInfo);
		CurrentPlaylistInfo->BasePlaylist = Playlist;
		CurrentPlaylistInfo->OverridePlaylist = Playlist;

		auto fn = FindObject("Function /Script/FortniteGame.FortGameStateAthena.OnRep_CurrentPlaylistInfo");
		ProcessEvent(GameState, fn, nullptr);
	}

	static UObject* SpawnActorStatics(UObject* ClassToSpawn, FVector loc, FRotator rot)
	{
		auto fn1 = FindObject("Function /Script/Engine.GameplayStatics.BeginDeferredActorSpawnFromClass");
		auto fn2 = FindObject("Function /Script/Engine.GameplayStatics.FinishSpawningActor");
		auto statics = FindObject("GameplayStatics /Script/Engine.Default__GameplayStatics");

		UGameplayStatics_BeginDeferredActorSpawnFromClass_Params params1;
		UGameplayStatics_FinishSpawningActor_Params params2;

		FQuat SpawnQuat;
		SpawnQuat.W = 0;
		SpawnQuat.X = rot.Pitch;
		SpawnQuat.Y = rot.Roll;
		SpawnQuat.Z = rot.Yaw;

		FTransform SpawnTrans;
		SpawnTrans.Scale3D = FVector(1, 1, 1);
		SpawnTrans.Translation = loc;
		SpawnTrans.Rotation = SpawnQuat;

		params1.ActorClass = ClassToSpawn;
		params1.WorldContextObject = World;
		params1.Owner = nullptr;
		params1.CollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		params1.SpawnTransform = SpawnTrans;
		
		ProcessEvent(statics, fn1, &params1);

		params2.Actor = params1.ReturnValue;
		params2.SpawnTransform = params1.SpawnTransform;

		ProcessEvent(statics, fn2, &params2);

		return params2.ReturnValue;
	}

	static void SpawnPlayer()
	{
		auto PawnClass = FindObject("BlueprintGeneratedClass /Game/Athena/PlayerPawn_Athena.PlayerPawn_Athena_C");
		auto PawnSpawned = SpawnActorStatics(PawnClass, FVector(0, 0, 0), FRotator(0, 0, 0));
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
		auto FortGameViewportClient = FindObject("/Engine/Transient.FortEngine.FortGameViewportClient");
		auto fn = FindObject("Function /Script/Engine.GameplayStatics.SpawnObject");
		auto statics = FindObject("GameplayStatics /Script/Engine.Default__GameplayStatics");
		auto ConsoleClass = FindObject("/Script/Engine.Console");
		auto ViewportConsole = reinterpret_cast<UObject**>((uintptr_t)FortGameViewportClient + Offsets::GameViewportClient::ViewportConsole);

		SpawnObjectParams params;
		params.ObjectClass = ConsoleClass;
		params.Outer = FortGameViewportClient;

		ProcessEvent(statics, fn, &params);

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