#pragma once

#include "Unreal.hpp"
#include "Globals.hpp"
#include "Structs.hpp"
#include "Offsets.hpp"
#include "enums.hpp"

namespace Functions
{
	static void InitConsole() {
		SpawnObjectParams params;

		params.ObjectClass = FindObject("Class /Script/FortniteGame.FortConsole");
		params.Outer = Globals::GameViewport;

		auto GameplayStatics = FindObject("Default__GameplayStatics");
		static auto fn = FindObject("Function /Script/Engine.GameplayStatics.SpawnObject");
		ProcessEvent(GameplayStatics, fn, &params);

		UObject** ViewportConsole = reinterpret_cast<UObject**>(__int64(Globals::GameViewport) + Offsets::GameViewportClient::ViewportConsole);
		*ViewportConsole = params.ReturnValue;
	}

	static void InitCheatManager() {
		SpawnObjectParams params;

		params.ObjectClass = FindObject("Class /Script/Engine.CheatManager");
		params.Outer = Globals::PlayerController;

		auto GameplayStatics = FindObject("Default__GameplayStatics");
		static auto fn = FindObject("Function /Script/Engine.GameplayStatics.SpawnObject");
		ProcessEvent(GameplayStatics, fn, &params);

		UObject** CheatManager = reinterpret_cast<UObject**>(__int64(Globals::PlayerController) + Offsets::PlayerController::CheatManager);
		*CheatManager = params.ReturnValue;
		Globals::CheatMananger = params.ReturnValue;
	}

	static inline void SwitchLevel(FString URL)
	{
		static auto fn = FindObject("Function /Script/Engine.PlayerController.SwitchLevel");
		ProcessEvent(Globals::PlayerController, fn, &URL);
	}

	static void Possess(UObject* Inpawn) {
		auto PossessFunc = FindObject("Function /Script/Engine.Controller.Possess");
		struct
		{
			class UObject* InPawn;                                                   // (Parm, ZeroConstructor, IsPlainOldData)
		} possesparams;
		possesparams.InPawn = Inpawn;
		ProcessEvent(Globals::PlayerController, PossessFunc, &possesparams);
	}

	static inline UObject* SpawnActor(UObject* ActorClass, FVector loc) {
		auto spawnfunction1 = FindObject("Function /Script/Engine.GameplayStatics.BeginDeferredActorSpawnFromClass");
		auto spawnfunction2 = FindObject("Function /Script/Engine.GameplayStatics.FinishSpawningActor");

		UGameplayStatics_BeginDeferredActorSpawnFromClass_Params bdasfc;
		UGameplayStatics_FinishSpawningActor_Params fsap;

		bdasfc.ActorClass = (UClass*)ActorClass;
		bdasfc.CollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		bdasfc.SpawnTransform.Rotation = FQuat{ 0,0,0,0 };
		bdasfc.SpawnTransform.Scale3D = FVector{ 1,1,1 };
		bdasfc.SpawnTransform.Translation = loc;
		bdasfc.WorldContextObject = Globals::World;
		bdasfc.Owner = nullptr;

		auto GameplayStatics = FindObject("Default__GameplayStatics");

		std::cout << GameplayStatics->GetFullName();
		std::cout << spawnfunction1->GetFullName();

		ProcessEvent(GameplayStatics, spawnfunction1, &bdasfc);

		fsap.Actor = (UObject*)bdasfc.ReturnValue;
		fsap.SpawnTransform = bdasfc.SpawnTransform;

		ProcessEvent(GameplayStatics, spawnfunction2, &fsap);

		return (UObject*)fsap.ReturnValue;
	}

	static inline void DestroyAll(const char* ClassToDestroy)
	{
		UObject* LocatedClass = FindObject(ClassToDestroy);
		ProcessEvent(Globals::CheatMananger, FindObject("Function /Script/Engine.CheatManager.DestroyAll"), &LocatedClass);
	}

	static inline void StartMatch()
	{
		static auto fn = FindObject("Function /Script/Engine.GameMode.StartMatch");
		ProcessEvent(Globals::GameMode, fn, nullptr);
	}

	static inline void ServerReadyToStartMatch()
	{
		static auto fn = FindObject("Function /Script/FortniteGame.FortPlayerController.ServerReadyToStartMatch");
		ProcessEvent(Globals::PlayerController, fn, nullptr);
	}
}