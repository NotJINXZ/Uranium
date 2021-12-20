#pragma once

#include "UE5.h"
#include "OffSetTable.hpp"
#include "skCryptor.h"
#include "minhook/MinHook.h"

#pragma comment(lib, "minhook/minhook.lib")

UObject* Controller;
UObject* World;
UObject* Pawn;
UObject* GameState;
PVOID LocalPawn;
UObject* QuickBar;
UObject* FortInventory;

void* (*AGarbageInternal)(uint32_t, bool);
void* AGarbInteralHook(uint32_t KeepFlags, bool bPerformFullPurge)
{
	printf("[URANIUM] Garbage Collection Failed To Purge \n");
	return NULL;
}

static bool IsMatchingGuid(FGuid A, FGuid B)
{
	return A.A == B.A && A.B == B.B && A.C == B.C && A.D == B.D;
}

namespace Functions
#define RELATIVE_ADDR(addr, size) ((PBYTE)((UINT_PTR)(addr) + *(PINT)((UINT_PTR)(addr) + ((size) - sizeof(INT))) + (size)))
#define ReadPointer(base, offset) (*(PVOID *)(((PBYTE)base + offset)))
{
	inline UObject* (*SpawnActorLong)(UObject* UWorld, UObject* Class, FTransform const* UserTransformPtr,
		const FActorSpawnParameters& SpawnParameters);

	UObject* PawnFinder() {LocalPawn = ReadPointer((uintptr_t)Controller, __int64(FindOffset("Controller", "Pawn"))); return reinterpret_cast<UObject*>(LocalPawn); }

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
		auto PlayerState = ReadPointer(Functions::PawnFinder(), __int64(FindOffset("Pawn", "PlayerState")));
		auto KismetLib = FindObject(crypt("FortKismetLibrary /Script/FortniteGame.Default__FortKismetLibrary"));
		static auto fn = FindObject(crypt("Function /Script/FortniteGame.FortKismetLibrary.UpdatePlayerCustomCharacterPartsVisualization"));

		UFortKismetLibrary_UpdatePlayerCustomCharacterPartsVisualization_Params params;
		params.PlayerState = (UObject*)PlayerState;
		ProcessEvent(KismetLib, fn, &params);
	}

	static UObject* SpawnActorFromLong(UObject* Class, FTransform trans)
	{
		SpawnActorLong = decltype(SpawnActorLong)(Util::FindPattern(crypt("48 8b c4 55 53 56 57 41 54 41 55 41 56 41 57 48 8d a8 ? ? ? ? 48 81 ec ? ? ? ? 0f 29 70 ? 0f 29 78 ? 44 0f 29 40 ? 44 0f 29 88 ? ? ? ? 44 0f 29 90 ? ? ? ? 44 0f 29 98 ? ? ? ? 44 0f 29 a0 ? ? ? ? 44 0f 29 a8 ? ? ? ? 44 0f 29 b0 ? ? ? ? 44 0f 29 b8 ? ? ? ? 48 8b 05 ? ? ? ? 48 33 c4 48 89 45 ? 45 33 ed")));
		auto pWorld = reinterpret_cast<UObject**>(Util::FindPattern(crypt("48 8B 05 ? ? ? ? 4D 8B C1"), true, 3));

		auto parms = FActorSpawnParameters();
		return SpawnActorLong((*pWorld), Class, &trans, parms);
	}

	static UObject* UpdatePlayerController()
	{
		auto FortEngine = FindObject(crypt("FortEngine /Engine/Transient.FortEngine"));
		auto GameInstance = *reinterpret_cast<UObject**>((uintptr_t)FortEngine + __int64(FindOffset("GameEngine", "GameInstance")));
		auto LocalPlayers = *reinterpret_cast<TArray<UObject*>*>((uintptr_t)GameInstance + __int64(FindOffset("GameInstance", "LocalPlayers")));
		auto LocalPlayer = LocalPlayers[0];
		auto PlayerController = *reinterpret_cast<UObject**>((uintptr_t)LocalPlayer + __int64(FindOffset("Player", "PlayerController")));
		Controller = PlayerController;
		return PlayerController;
	}

	static void CustomSkin(std::string DefaultHeadPart ,std::string DefaultBodyPart)
	{
		auto PlayerState = *reinterpret_cast<UObject**>((uintptr_t)PawnFinder() + __int64(FindOffset("Pawn", "PlayerState")));
		auto Hero = FindObject(crypt("FortHero /Engine/Transient.FortHero"));
		auto CharacterParts = reinterpret_cast<TArray<UObject*>*>((uintptr_t)Hero + __int64(FindOffset("FortHero", "CharacterParts")));

		auto Head = FindObject(DefaultBodyPart);
		auto Body = FindObject(DefaultBodyPart);

		CharacterParts->operator[](1) = Head;
		CharacterParts->operator[](0) = Body;

		auto KismetLib = FindObject(crypt("FortKismetLibrary /Script/FortniteGame.Default__FortKismetLibrary"));
		auto fn = FindObject(crypt("Function /Script/FortniteGame.FortKismetLibrary.ApplyCharacterCosmetics"));

		struct {
			UObject* WorldContextObject;
			TArray<UObject*> CharacterParts;
			UObject* PlayerState;
			bool bSuccess;
		} params;

		params.WorldContextObject = World;
		params.CharacterParts = *CharacterParts;
		params.PlayerState = PlayerState;

		ProcessEvent(KismetLib, fn, &params);
	}

	static void SwitchLevel(FString URL)
	{
		auto fn = FindObject(crypt("Function /Script/Engine.PlayerController.SwitchLevel"));
		ProcessEvent(Controller, fn, &URL);
	}

	static void SetInfiniteAmmo(UObject* InController)
	{
		RBitField* BitField = reinterpret_cast<RBitField*>(__int64(InController) + __int64(FindOffset("FortPlayerController", "bInfiniteAmmo")));
		BitField->C = 1;
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

	static void StartFireWorks() {
		auto fn = FindObject(crypt("Function /NewYears/Blueprints/BP_NewYearTimer.BP_NewYearTimer_C.ReceiveBeginPlay"));
		auto Class = FindObject(crypt("BlueprintGeneratedClass /NewYears/Levels/Artemis_NYE_Celebration.Artemis_NYE_Celebration_C"));

		ProcessEvent(Class,fn,nullptr);
	}

	static void ServerReadyToStartMatch()
	{
		auto fn = FindObject(crypt("Function /Script/FortniteGame.FortPlayerController.ServerReadyToStartMatch"));
		ProcessEvent(Controller, fn, nullptr);
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
		auto CheatManager = reinterpret_cast<UObject**>(__int64(Controller) + __int64(FindOffset("PlayerController", "CheatManager")));
		auto CheatManagerClass = FindObject(crypt("/Script/Engine.CheatManager"));

		SpawnObjectParams params;
		params.ObjectClass = CheatManagerClass;
		params.Outer = Controller;

		ProcessEvent(statics, fn, &params);

		*CheatManager = params.ReturnValue;
	}

	static void BP_ApplyGameplayEffectToSelf(UObject* AbilitySystemComponent, UObject* GameplayEffectClass)
	{
		static UObject* BP_ApplyGameplayEffectToSelf = FindObject(crypt("Function /Script/GameplayAbilities.AbilitySystemComponent.BP_ApplyGameplayEffectToSelf"));

		struct
		{
			UObject* GameplayEffectClass;
			float Level;
			FGameplayEffectContextHandle EffectContext;
			FActiveGameplayEffectHandle ret;
		} Params;

		Params.EffectContext = FGameplayEffectContextHandle();
		Params.GameplayEffectClass = GameplayEffectClass;
		Params.Level = 1.0;

		ProcessEvent(AbilitySystemComponent, BP_ApplyGameplayEffectToSelf, &Params);
	}

	static void GrantGameplayAbility(UObject* TargetPawn, UObject* GameplayAbilityClass)
	{
		UObject** AbilitySystemComponent = reinterpret_cast<UObject**>(__int64(TargetPawn) + __int64(FindOffset("FortPawn", "AbilitySystemComponent")));
		static UObject* DefaultGameplayEffect = FindObject(crypt("GE_Athena_PurpleStuff_C /Game/Athena/Items/Consumables/PurpleStuff/GE_Athena_PurpleStuff.Default__GE_Athena_PurpleStuff_C"));
		if (!DefaultGameplayEffect)
		{
			DefaultGameplayEffect = FindObject(crypt("GE_Athena_PurpleStuff_Health_C /Game/Athena/Items/Consumables/PurpleStuff/GE_Athena_PurpleStuff_Health.Default__GE_Athena_PurpleStuff_Health_C"));
		}

		TArray<struct FGameplayAbilitySpecDef>* GrantedAbilities = reinterpret_cast<TArray<struct FGameplayAbilitySpecDef>*>(__int64(DefaultGameplayEffect) + __int64(FindOffset("GameplayEffect", "GrantedAbilities")));

		GrantedAbilities->operator[](0).Ability = GameplayAbilityClass;

		*reinterpret_cast<EGameplayEffectDurationType*>(__int64(DefaultGameplayEffect) + __int64(FindOffset("GameplayEffect", "DurationPolicy"))) = EGameplayEffectDurationType::Infinite;

		static auto GameplayEffectClass = FindObject(crypt("BlueprintGeneratedClass /Game/Athena/Items/Consumables/PurpleStuff/GE_Athena_PurpleStuff.GE_Athena_PurpleStuff_C"));
		if (!GameplayEffectClass)
		{
			GameplayEffectClass = FindObject(crypt("BlueprintGeneratedClass /Game/Athena/Items/Consumables/PurpleStuff/GE_Athena_PurpleStuff_Health.GE_Athena_PurpleStuff_Health_C"));
		}
		BP_ApplyGameplayEffectToSelf(*AbilitySystemComponent, GameplayEffectClass);
	}

	static UObject* FindAthenaGameMode()
	{
		auto GameplayStatics = FindObject(crypt("GameplayStatics /Script/Engine.Default__GameplayStatics"));
		auto getgamemode = FindObject(crypt("Function /Script/Engine.GameplayStatics.GetGameMode"));
		struct UGameplayStatics_GetGameMode_Params
		{
			class UObject* WorldContextObject;                                       // (ConstParm, Parm, ZeroConstructor, IsPlainOldData)
			class UObject* ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
		};

		UGameplayStatics_GetGameMode_Params ggmparams;
		ggmparams.WorldContextObject = Controller;

		ProcessEvent(GameplayStatics, getgamemode, &ggmparams);

		return ggmparams.ReturnValue;
	}

	static UObject* FindAthenaGameState()
	{
		auto dWorld = Util::FindPattern(crypt("48 8B 05 ? ? ? ? 4D 8B C1"), true, 3);
		CHECKSIG(dWorld, "Failed to find UWorld address!");
		auto Worldd = *reinterpret_cast<UObject**>(dWorld);

		struct UGameplayStatics_GetGameState_Params
		{
			class UObject* WorldContextObject;
			class UObject* ReturnValue;
		};

		auto GameplayStatics = FindObject(crypt("GameplayStatics /Script/Engine.Default__GameplayStatics"));
		auto GetGameState = FindObject(crypt("Function /Script/Engine.GameplayStatics.GetGameState"));


		UGameplayStatics_GetGameState_Params ggsparams;
		ggsparams.WorldContextObject = Worldd;

		ProcessEvent(GameplayStatics, GetGameState, &ggsparams);

		return ggsparams.ReturnValue;
	}

	static void SetPlaylist(UObject* Playlist)
	{
		auto Gamestate = FindAthenaGameState();
		auto BasePlaylist = reinterpret_cast<UObject**>((uintptr_t)Gamestate + __int64(FindOffset("FortGameStateAthena", "CurrentPlaylistInfo")) + __int64(FindOffset("PlaylistPropertyArray", "BasePlaylist")));
		*BasePlaylist = Playlist;

		auto fn = FindObject(crypt("Function /Script/FortniteGame.FortGameStateAthena.OnRep_CurrentPlaylistInfo"));
		ProcessEvent(Gamestate, fn, nullptr);
	}

	static void StartMatch()
	{
		auto fn = FindObject(crypt("Function /Script/Engine.GameMode.StartMatch"));
		ProcessEvent(FindAthenaGameMode(), fn, nullptr);
	}

	static void Summon(FString ClassToSummon)
	{
		auto fn = FindObject(crypt("Function /Script/Engine.CheatManager.Summon"));
		ProcessEvent((UObject*)ReadPointer((uintptr_t)Controller, __int64(FindOffset("PlayerController", "CheatManager"))), fn, &ClassToSummon);
	}

	static void SetGamePhase(EAthenaGamePhase NewPhase, EAthenaGamePhase OldPhase)
	{
		EAthenaGamePhase* CurrentGamePhase = reinterpret_cast<EAthenaGamePhase*>(__int64(FindAthenaGameState()) + __int64(FindOffset("FortGameStateAthena", "GamePhase")));
		*CurrentGamePhase = NewPhase;

		static UObject* OnRep_GamePhase = FindObject(crypt("Function /Script/FortniteGame.FortGameStateAthena.OnRep_GamePhase"));
		ProcessEvent(FindAthenaGameState(), OnRep_GamePhase, &OldPhase);
	}

	static UObject* GetPickaxeDef()
	{
		auto CosmeticLoadoutPC = reinterpret_cast<FFortAthenaLoadout*>((uintptr_t)Controller + __int64(FindOffset("FortPlayerController", "CosmeticLoadoutPC")));
		auto Pickaxe = CosmeticLoadoutPC->Pickaxe;
		auto PickaxeDef = *reinterpret_cast<UObject**>((uintptr_t)Pickaxe + __int64(FindOffset("AthenaPickaxeItemDefinition", "WeaponDefinition")));
		return PickaxeDef;
	}

	static UObject* GetAnimInstance()
	{
		auto Pawn = *reinterpret_cast<UObject**>((uintptr_t)Controller + __int64(FindOffset("Controller", "Pawn")));
		auto Mesh = *reinterpret_cast<UObject**>((uintptr_t)Pawn + __int64(FindOffset("Character", "Mesh")));
		static auto GetAnimInstanceFN = FindObject(crypt("Function /Script/Engine.SkeletalMeshComponent.GetAnimInstance"));

		struct GetAnimInstanceParams
		{
			UObject* ReturnValue;
		};
		GetAnimInstanceParams gaiparams;

		ProcessEvent(Mesh, GetAnimInstanceFN, &gaiparams);
		return gaiparams.ReturnValue;
	}

	static UObject* GetAnimationHardReference(UObject* FortMontageItemDefinition)
	{
		struct Params
		{
			TEnumAsByte<EFortCustomBodyType> BodyType;
			TEnumAsByte<EFortCustomGender> Gender;
			UObject* PawnContext;
			UObject* ReturnValue;
		};
		Params params;
		params.BodyType = EFortCustomBodyType::All;
		params.Gender = EFortCustomGender::Both;
		params.PawnContext = Pawn;

		auto fn = FindObject(crypt("Function /Script/FortniteGame.FortMontageItemDefinitionBase.GetAnimationHardReference"));

		ProcessEvent(FortMontageItemDefinition, fn, &params);

		return params.ReturnValue;
	}

	static void PlayMontage(UObject* Montage)
	{
		struct Params
		{
			UObject* MontageToPlay;
			float InPlayRate;
			EMontagePlayReturnType ReturnValueType;
			float InTimeToStartMontageAt;
			bool bStopAllMontages;
			float ReturnValue;
		};
		Params params;
		params.MontageToPlay = Montage;
		params.InPlayRate = 1.0;
		params.ReturnValueType = EMontagePlayReturnType::Duration;
		params.InTimeToStartMontageAt = 0;
		params.bStopAllMontages = true;

		auto fn = FindObject(crypt("Function /Script/Engine.AnimInstance.Montage_Play"));

		ProcessEvent(GetAnimInstance(), fn, &params);
	}

	inline void DestroyActor(UObject* actor)
	{
		const auto fn = FindObject(crypt("Function /Script/Engine.Actor.K2_DestroyActor"));

		ProcessEvent(actor, fn, nullptr);
	}

	inline void DestroyAll(UObject* Class)
	{
		auto dWorld = Util::FindPattern(crypt("48 8B 05 ? ? ? ? 4D 8B C1"), true, 3);
		CHECKSIG(dWorld, "Failed to find UWorld address!");
		auto Worldd = *reinterpret_cast<UObject**>(dWorld);

		auto GameplayStatics = FindObject(crypt("GameplayStatics /Script/Engine.Default__GameplayStatics"));
		auto GetAllActorsOfClass = FindObject(crypt("Function /Script/Engine.GameplayStatics.GetAllActorsOfClass"));

		GetAllActorsOfClass_Params params;
		params.ActorClass = Class;
		params.WorldContextObject = Worldd;

		ProcessEvent(GameplayStatics, GetAllActorsOfClass, &params);

		auto Actors = params.OutActors;

		const auto K2_DestroyActor = FindObject(crypt("Function /Script/Engine.Actor.K2_DestroyActor"));

		for (auto i = 0; i < Actors.Num(); i++)
		{
			DestroyActor(Actors[i]);
		}
	}

	static void UpdateInventory()
	{
		static auto HandleWorldInventoryLocalUpdate = FindObject(crypt("Function /Script/FortniteGame.FortPlayerController.HandleWorldInventoryLocalUpdate"));
		static auto HandleInventoryLocalUpdate = FindObject(crypt("Function /Script/FortniteGame.FortInventory.HandleInventoryLocalUpdate"));
		static auto ClientForceUpdateQuickbar = FindObject(crypt("Function /Script/FortniteGame.FortPlayerController.ClientForceUpdateQuickbar"));

		ProcessEvent(FortInventory, HandleInventoryLocalUpdate, nullptr);
		ProcessEvent(Controller, HandleWorldInventoryLocalUpdate, nullptr);

		auto PrimaryQuickbar = EFortQuickBars::Primary;
		auto SecondaryQuickbar = EFortQuickBars::Secondary;
		ProcessEvent(Controller, ClientForceUpdateQuickbar, &PrimaryQuickbar);
		ProcessEvent(Controller, ClientForceUpdateQuickbar, &SecondaryQuickbar);
	}

	static void SetOwningControllerForTemporaryItem(UObject* Item, UObject* Controller)
	{
		static UObject* SetOwningControllerForTemporaryItem = FindObject(crypt("Function /Script/FortniteGame.FortItem.SetOwningControllerForTemporaryItem"));
		ProcessEvent(Item, SetOwningControllerForTemporaryItem, &Controller);
	}

	static UObject* CreateTemporaryItemInstanceBP(UObject* ItemDefinition, int Count, int Level)
	{
		static UObject* CreateTemporaryItemInstanceBP = FindObject(crypt("Function /Script/FortniteGame.FortItemDefinition.CreateTemporaryItemInstanceBP"));

		struct
		{
			int Count;
			int Level;
			UObject* ReturnValue;
		} Params;

		Params.Count = Count;
		Params.Level = Level;

		ProcessEvent(ItemDefinition, CreateTemporaryItemInstanceBP, &Params);

		return Params.ReturnValue;
	}

	static UObject* CreateItem(UObject* ItemDefinition, int Count)
	{
		UObject* TemporaryItemInstance = CreateTemporaryItemInstanceBP(ItemDefinition, Count, 1);

		if (TemporaryItemInstance)
		{
			SetOwningControllerForTemporaryItem(TemporaryItemInstance, Controller);
		}

		int* CurrentCount = reinterpret_cast<int*>(__int64(TemporaryItemInstance) + static_cast<__int64>(FindOffset("FortWorldItem", "ItemEntry")) + static_cast<__int64>(FindOffset("FortItemEntry", "Count")));
		*CurrentCount = Count;

		return TemporaryItemInstance;
	}

	static void OnRep_QuickbarEquippedItems()
	{
		auto PlayerState = *reinterpret_cast<UObject**>((uintptr_t)Pawn + __int64(FindOffset("Pawn", "PlayerState")));
		auto Fn = FindObject(crypt("Function /Script/FortniteGame.FortPlayerStateZone.OnRep_QuickbarEquippedItems"));
		ProcessEvent(PlayerState, Fn, nullptr);
	}

	static void AddItemToQuickBars(UObject* ItemDef, EFortQuickBars QuickBarType, int32_t slot)
	{
		struct Params
		{
			UObject* ItemDefinition;
			EFortQuickBars QuickBarType;
			int32_t Slot;
		};
		Params params;
		params.ItemDefinition = ItemDef;
		params.QuickBarType = QuickBarType;
		params.Slot = slot;

		auto fn = FindObject(crypt("Function /Script/FortniteGame.FortPlayerController.AddItemToQuickBars"));
		ProcessEvent(Controller, fn, &params);
	}

	static void AddItemToInventory(UObject* ItemDef, int Count, bool bAddToQuickBars = false, EFortQuickBars QuickBarType = EFortQuickBars::Max_None, int32_t slot = 0)
	{
		if (ItemDef)
		{
			UObject* ItemInstance = CreateItem(ItemDef, Count);

			if (ItemInstance)
			{
				auto ItemEntry = reinterpret_cast<FFortItemEntry*>(reinterpret_cast<uintptr_t>(ItemInstance) + __int64(FindOffset("FortWorldItem", "ItemEntry")));
				reinterpret_cast<TArray<FFortItemEntry>*>(__int64(FortInventory) + static_cast<__int64>(__int64(FindOffset("FortInventory", "Inventory"))) + static_cast<__int64>(FindOffset("FortItemList", "ReplicatedEntries")))->Add(*ItemEntry);

				//return reinterpret_cast<FGuid*>((uintptr_t)ItemEntry + 0x68);
				//reinterpret_cast<TArray<UObject*>*>(__int64(Globals::FortInventory) + static_cast<__int64>(0x230) + static_cast<__int64>(0x168))->Add(ItemInstance);
			}

			if (bAddToQuickBars) {
				//AddItemToQuickBars(ItemDef, QuickBarType, slot);
			}

			UpdateInventory();
			OnRep_QuickbarEquippedItems();
		}
	}

	static void AddItemToInventoryWithEntry(FFortItemEntry Entry, int Count)
	{
		reinterpret_cast<TArray<FFortItemEntry>*>(__int64(FortInventory) + static_cast<__int64>(__int64(FindOffset("FortInventory", "Inventory"))) + static_cast<__int64>(FindOffset("FortItemList", "ReplicatedEntries")))->Add(Entry);

		UpdateInventory();
		OnRep_QuickbarEquippedItems();
	}

	static void SetOwner(UObject* TargetActor, UObject* NewOwner)
	{
		static UObject* SetOwner = FindObject(crypt("Function /Script/Engine.Actor.SetOwner"));
		ProcessEvent(TargetActor, SetOwner, &NewOwner);
	}

	static void SetGodMode()
	{
		auto fn = FindObject("Function /Script/Engine.CheatManager.God");
		ProcessEvent(*reinterpret_cast<UObject**>(__int64(Controller) + __int64(FindOffset("PlayerController", "CheatManager"))), fn, nullptr);
	}

	static void UeConsoleLog(FString message)
	{
		static auto fn = FindObject(crypt("Function /Script/Engine.GameMode.Say"));
		ProcessEvent(FindAthenaGameMode(), fn, &message);
	}

	static void TeleportToSkydive(float InHeight)
	{
		static auto fn = FindObject(crypt("Function /Script/FortniteGame.FortPlayerPawnAthena.TeleportToSkyDive"));
		ProcessEvent(Pawn, fn, &InHeight);
	}

	static void UnCrouch()
	{
		static auto fn = FindObject(crypt("Function /Script/Engine.Character.UnCrouch"));
		bool ClientSim = false;
		ProcessEvent(Pawn, fn, &ClientSim);
	}

	static UObject* GetCharacterMovementComponent()
	{
		return *reinterpret_cast<UObject**>((uintptr_t)Pawn + __int64(FindOffset("Character", "CharacterMovement")));
	}

	static bool IsFalling()
	{
		static auto fn = FindObject(crypt("Function /Script/Engine.NavMovementComponent.IsFalling"));
		bool ReturnValue;

		ProcessEvent(GetCharacterMovementComponent(), fn, &ReturnValue);

		return ReturnValue;
	}

	static void ServerSetClientHasFinishedLoading(UObject* Target)
	{
		static UObject* ServerSetClientHasFinishedLoading = FindObject(crypt("Function /Script/FortniteGame.FortPlayerController.ServerSetClientHasFinishedLoading"));

		bool HasFinishedLoading = true;

		ProcessEvent(Target, ServerSetClientHasFinishedLoading, &HasFinishedLoading);
	}

	static void InitializeBuildingActor(UObject* BuildingActor)
	{
		auto InitializeKismetSpawnedBuildingActor = FindObject(crypt("Function /Script/FortniteGame.BuildingActor.InitializeKismetSpawnedBuildingActor"));

		struct Params
		{
			UObject* BuildingOwner;
			UObject* SpawningController;
		};

		Params params;
		params.BuildingOwner = BuildingActor;
		params.SpawningController = Controller;

		ProcessEvent(BuildingActor, InitializeKismetSpawnedBuildingActor, &params);
	}

	static unsigned long __stdcall BuildAsync(void*)
	{
		auto CurrentBuildableClass = *reinterpret_cast<UObject**>((uintptr_t)Controller + __int64(FindOffset("FortPlayerController", "CurrentBuildableClass")));
		auto LastBuildPreviewGridSnapLoc = *reinterpret_cast<FVector*>((uintptr_t)Controller + __int64(FindOffset("FortPlayerController", "LastBuildPreviewGridSnapLoc")));
		auto LastBuildPreviewGridSnapRot = *reinterpret_cast<FRotator*>((uintptr_t)Controller + __int64(FindOffset("FortPlayerController", "LastBuildPreviewGridSnapRot")));

		auto Build = Functions::SpawnActor(CurrentBuildableClass, LastBuildPreviewGridSnapLoc, LastBuildPreviewGridSnapRot);
		Functions::InitializeBuildingActor(Build);

		return 0;
	}

	static void InitMatch()
	{
		Functions::UpdatePlayerController();
		Functions::EnableCheatManager();

		Functions::Summon(crypt(L"PlayerPawn_Athena_C"));

		for (int i = 0; i < GObjects->NumElements; i++)
		{
			auto object = GObjects->GetByIndex(i);

			if (object == nullptr)
				continue;

			if (object->GetFullName() == crypt("PlayerPawn_Athena_C /Game/Athena/PlayerPawn_Athena.Default__PlayerPawn_Athena_C"))
				continue;

			if (object->GetFullName().starts_with(crypt("PlayerPawn_Athena_C "))) {
				Pawn = object;
				break;
			}
		}

		if (Pawn) {
			Functions::SetPlaylist(FindObject(crypt("/Game/Athena/Playlists/BattleLab/Playlist_BattleLab.Playlist_BattleLab")));
			Functions::Possess(Pawn);
			Functions::StartMatch();
			Functions::ServerReadyToStartMatch();
			Functions::ShowSkin();
		}

		Functions::UpdatePlayerController();
		Functions::EnableCheatManager();

		Functions::Summon(crypt(L"PlayerPawn_Athena_C"));

		for (int i = 0; i < GObjects->NumElements; i++)
		{
			auto object = GObjects->GetByIndex(i);

			if (object == nullptr)
				continue;

			if (object->GetFullName() == crypt("PlayerPawn_Athena_C /Game/Athena/PlayerPawn_Athena.Default__PlayerPawn_Athena_C"))
				continue;

			if (object->GetFullName().starts_with(crypt("PlayerPawn_Athena_C "))) {
				Pawn = object;
				break;
			}
		}

		if (Pawn) {
			Functions::Possess(Pawn);
			Functions::SetGodMode();
			Functions::ShowSkin();
		}
	}

	static void SpawnPickup(UObject* ItemDef, int Count, EFortPickupSourceTypeFlag InPickupSourceTypeFlags, EFortPickupSpawnSource InPickupSpawnSource)
	{
		auto FortPickup = SpawnActor(FindObject(crypt("Class /Script/FortniteGame.FortPickupAthena")), Functions::GetActorLocation(Pawn), FRotator());
		/*Functions::Summon(L"FortPickupAthena_C");

		auto FortPickup = FindObjectWithSkip(FindObject("Class /Script/FortniteGame.FortPickupAthena"));*/

		auto entry = reinterpret_cast<FFortItemEntry*>((uintptr_t)FortPickup + __int64(FindOffset("FortPickup", "PrimaryPickupItemEntry")));
		*reinterpret_cast<UObject**>((uintptr_t)entry + __int64(FindOffset("FortItemEntry", "ItemDefinition"))) = ItemDef;
		*reinterpret_cast<int*>((uintptr_t)entry + __int64(FindOffset("FortItemEntry", "Count"))) = Count;

		auto Fn = FindObject(crypt("Function /Script/FortniteGame.FortPickup.TossPickup"));
		auto Fn2 = FindObject(crypt("Function /Script/FortniteGame.FortPickup.OnRep_PrimaryPickupItemEntry"));

		struct
		{
			FVector FinalLocation;
			UObject* Pawn;
			int32_t OverrideMaxStackCount;
			bool bToss;
			bool bShouldCombinePickupsWhenTossCompletes;
			EFortPickupSourceTypeFlag InPickupSourceTypeFlags;
			EFortPickupSpawnSource InPickupSpawnSource;
		}params;

		params.FinalLocation = GetActorLocation(Pawn);
		params.bToss = true;
		params.bShouldCombinePickupsWhenTossCompletes = true;
		params.Pawn = Pawn;
		params.OverrideMaxStackCount = 999;
		params.InPickupSourceTypeFlags = InPickupSourceTypeFlags;
		params.InPickupSpawnSource = InPickupSpawnSource;

		ProcessEvent(FortPickup, Fn2, nullptr);
		ProcessEvent(FortPickup, Fn, &params);
	}

	static void SetupNetDebugUI()
	{
		auto NDBGUI = FindObject("NetDebugUI_C /Engine/Transient.FortEngine_");


	}
}
