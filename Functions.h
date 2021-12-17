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

static bool IsMatchingGuid(FGuid A, FGuid B)
{
	return A.A == B.A && A.B == B.B && A.C == B.C && A.D == B.D;
}

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
		auto FortEngine = FindObject(crypt("FortEngine /Engine/Transient.FortEngine"));
		auto GameInstance = *reinterpret_cast<UObject**>((uintptr_t)FortEngine + 0xd78);
		auto LocalPlayers = *reinterpret_cast<TArray<UObject*>*>((uintptr_t)GameInstance + 0x38);
		auto LocalPlayer = LocalPlayers[0];
		auto PlayerController = *reinterpret_cast<UObject**>((uintptr_t)LocalPlayer + 0x30);
		Controller = PlayerController;
		return PlayerController;
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

	static void SetInfiniteAmmo(UObject* InController)
	{
		RBitField* BitField = reinterpret_cast<RBitField*>(__int64(InController) + 0x218c);
		BitField->C = 1;
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
	}

	static void SpawnPlayer()
	{
		auto PawnClass = FindObject(crypt("BlueprintGeneratedClass /Game/Athena/PlayerPawn_Athena.PlayerPawn_Athena_C"));
		auto PawnSpawned = SpawnActor(PawnClass, FVector(0, 0, 0), FRotator(0,0,0));
		Pawn = PawnSpawned;
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
		UObject** AbilitySystemComponent = reinterpret_cast<UObject**>(__int64(TargetPawn) + 0xD48);
		static UObject* DefaultGameplayEffect = FindObject(crypt("GE_Athena_PurpleStuff_C /Game/Athena/Items/Consumables/PurpleStuff/GE_Athena_PurpleStuff.Default__GE_Athena_PurpleStuff_C"));
		if (!DefaultGameplayEffect)
		{
			DefaultGameplayEffect = FindObject(crypt("GE_Athena_PurpleStuff_Health_C /Game/Athena/Items/Consumables/PurpleStuff/GE_Athena_PurpleStuff_Health.Default__GE_Athena_PurpleStuff_Health_C"));
		}

		TArray<struct FGameplayAbilitySpecDef>* GrantedAbilities = reinterpret_cast<TArray<struct FGameplayAbilitySpecDef>*>(__int64(DefaultGameplayEffect) + 0x7F0);

		GrantedAbilities->operator[](0).Ability = GameplayAbilityClass;

		*reinterpret_cast<EGameplayEffectDurationType*>(__int64(DefaultGameplayEffect) + 0x30) = EGameplayEffectDurationType::Infinite;

		static auto GameplayEffectClass = FindObject(crypt("BlueprintGeneratedClass /Game/Athena/Items/Consumables/PurpleStuff/GE_Athena_PurpleStuff.GE_Athena_PurpleStuff_C"));
		if (!GameplayEffectClass)
		{
			GameplayEffectClass = FindObject(crypt("BlueprintGeneratedClass /Game/Athena/Items/Consumables/PurpleStuff/GE_Athena_PurpleStuff_Health.GE_Athena_PurpleStuff_Health_C"));
		}
		BP_ApplyGameplayEffectToSelf(*AbilitySystemComponent, GameplayEffectClass);

		std::cout << "Granted Ability: " << GameplayAbilityClass->GetFullName() << std::endl;
	}

	static UObject* FindAthenaGameMode()
	{
		auto GameplayStatics = FindObject("GameplayStatics /Script/Engine.Default__GameplayStatics");
		auto getgamemode = FindObject("Function /Script/Engine.GameplayStatics.GetGameMode");
		struct UGameplayStatics_GetGameMode_Params
		{
			class UObject* WorldContextObject;                                       // (ConstParm, Parm, ZeroConstructor, IsPlainOldData)
			class UObject* ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
		};

		UGameplayStatics_GetGameMode_Params ggmparams;
		ggmparams.WorldContextObject = Controller;

		ProcessEvent(GameplayStatics, getgamemode, &ggmparams);

		std::cout << "GameMode: " << ggmparams.ReturnValue->GetFullName() << std::endl;
		return ggmparams.ReturnValue;
	}

	static UObject* FindAthenaGameState()
	{
		auto dWorld = Util::FindPattern("48 8B 05 ? ? ? ? 4D 8B C1", true, 3);
		CHECKSIG(dWorld, "Failed to find UWorld address!");
		auto Worldd = *reinterpret_cast<UObject**>(dWorld);

		struct UGameplayStatics_GetGameState_Params
		{
			class UObject* WorldContextObject;
			class UObject* ReturnValue;
		};

		auto GameplayStatics = FindObject("GameplayStatics /Script/Engine.Default__GameplayStatics");
		auto GetGameState = FindObject("Function /Script/Engine.GameplayStatics.GetGameState");


		UGameplayStatics_GetGameState_Params ggsparams;
		ggsparams.WorldContextObject = Worldd;

		ProcessEvent(GameplayStatics, GetGameState, &ggsparams);

		std::cout << "GameState: " << ggsparams.ReturnValue->GetFullName() << std::endl;
		return ggsparams.ReturnValue;
	}

	static void SetPlaylist(UObject* Playlist)
	{
		auto Gamestate = FindAthenaGameState();
		auto BasePlaylist = reinterpret_cast<UObject**>((uintptr_t)Gamestate + 0x2068 + 0x120);
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
		ProcessEvent((UObject*)ReadPointer(ControllerFinder(), 0x340), fn, &ClassToSummon);
	}

	static void SetGamePhase(EAthenaGamePhase NewPhase, EAthenaGamePhase OldPhase)
	{
		EAthenaGamePhase* CurrentGamePhase = reinterpret_cast<EAthenaGamePhase*>(__int64(FindAthenaGameState()) + 0x2048);
		*CurrentGamePhase = NewPhase;

		static UObject* OnRep_GamePhase = FindObject(crypt("Function /Script/FortniteGame.FortGameStateAthena.OnRep_GamePhase"));
		ProcessEvent(FindAthenaGameState(), OnRep_GamePhase, &OldPhase);
	}

	static UObject* GetPickaxeDef()
	{
		auto CosmeticLoadoutPC = reinterpret_cast<FFortAthenaLoadout*>((uintptr_t)Controller + 0x1be8);
		auto Pickaxe = CosmeticLoadoutPC->Pickaxe;
		auto PickaxeDef = *reinterpret_cast<UObject**>((uintptr_t)Pickaxe + 0x708);
		return PickaxeDef;
	}

	static UObject* GetAnimInstance()
	{
		auto Pawn = *reinterpret_cast<UObject**>((uintptr_t)Controller + 0x2a8);
		auto Mesh = *reinterpret_cast<UObject**>((uintptr_t)Pawn + 0x280);
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

	static void ServerSetClientHasFinishedLoading()
	{
		static UObject* ServerSetClientHasFinishedLoading = FindObject(crypt("Function /Script/FortniteGame.FortPlayerController.ServerSetClientHasFinishedLoading"));
		bool HasFinishedLoading = true;
		ProcessEvent(Controller, ServerSetClientHasFinishedLoading, &HasFinishedLoading);
	}

	inline void DestroyActor(UObject* actor)
	{
		const auto fn = FindObject("Function /Script/Engine.Actor.K2_DestroyActor");

		ProcessEvent(actor, fn, nullptr);
	}

	inline void DestroyAll(UObject* Class)
	{
		auto dWorld = Util::FindPattern("48 8B 05 ? ? ? ? 4D 8B C1", true, 3);
		CHECKSIG(dWorld, "Failed to find UWorld address!");
		auto Worldd = *reinterpret_cast<UObject**>(dWorld);

		auto GameplayStatics = FindObject("GameplayStatics /Script/Engine.Default__GameplayStatics");
		auto GetAllActorsOfClass = FindObject("Function /Script/Engine.GameplayStatics.GetAllActorsOfClass");

		GetAllActorsOfClass_Params params;
		params.ActorClass = Class;
		params.WorldContextObject = Worldd;

		ProcessEvent(GameplayStatics, GetAllActorsOfClass, &params);

		auto Actors = params.OutActors;

		const auto K2_DestroyActor = FindObject("Function /Script/Engine.Actor.K2_DestroyActor");

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

		int* CurrentCount = reinterpret_cast<int*>(__int64(TemporaryItemInstance) + static_cast<__int64>(0xE0) + static_cast<__int64>(0xC));
		*CurrentCount = Count;

		return TemporaryItemInstance;
	}

	static void OnRep_QuickbarEquippedItems()
	{
		auto PlayerState = *reinterpret_cast<UObject**>((uintptr_t)Pawn + 0x238);
		auto Fn = FindObject("Function /Script/FortniteGame.FortPlayerStateZone.OnRep_QuickbarEquippedItems");
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
				auto ItemEntry = reinterpret_cast<FFortItemEntry*>(reinterpret_cast<uintptr_t>(ItemInstance) + 0xE0);
				reinterpret_cast<TArray<FFortItemEntry>*>(__int64(FortInventory) + static_cast<__int64>(0x228) + static_cast<__int64>(0x108))->Add(*ItemEntry);

				//return reinterpret_cast<FGuid*>((uintptr_t)ItemEntry + 0x68);
				//reinterpret_cast<TArray<UObject*>*>(__int64(Globals::FortInventory) + static_cast<__int64>(0x230) + static_cast<__int64>(0x168))->Add(ItemInstance);
			}

			if (bAddToQuickBars) {
				AddItemToQuickBars(ItemDef, QuickBarType, slot);
			}

			UpdateInventory();
			OnRep_QuickbarEquippedItems();
		}
	}

	static void StartPlay()
	{
		static auto fn = FindObject(crypt("Function /Script/Engine.GameModeBase.StartPlay"));
		ProcessEvent(GetGameMode(), fn, nullptr);
	}

	static void SetOwner(UObject* TargetActor, UObject* NewOwner)
	{
		static UObject* SetOwner = FindObject(crypt("Function /Script/Engine.Actor.SetOwner"));
		ProcessEvent(TargetActor, SetOwner, &NewOwner);
	}

	static void SetGodMode()
	{
		auto fn = FindObject("Function /Script/Engine.CheatManager.God");
		ProcessEvent(*reinterpret_cast<UObject**>(__int64(Controller) + __int64(0x340)), fn, nullptr);
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
}