#pragma once

#include "Structs.hpp"
#include "Unreal.hpp"
#include "Offsets.hpp"

namespace Globals
{
    UObject* GameViewport;
    UObject* GameInstance;
    TArray<UObject*> LocalPlayers;
    UObject* LocalPlayer;
    UObject* PlayerController;
    UObject* CheatMananger;
    UObject* World;
    UObject* GameMode;
    UObject* GameState;
    UObject* Pawn;
    UObject* PlayerState;

	inline static void SetupGlobals()
	{
        GameViewport = *reinterpret_cast<UObject**>(__int64(FortEngine) + Offsets::Engine::GameViewport);
        std::cout << GameViewport << std::endl;
        GameInstance = *reinterpret_cast<UObject**>(__int64(GameViewport) + Offsets::GameViewportClient::GameInstance);
        std::cout << GameInstance << std::endl;
        LocalPlayers = *reinterpret_cast<TArray<UObject*>*>(__int64(GameInstance) + Offsets::GameInstance::LocalPlayers);
        LocalPlayer = LocalPlayers[0];
        std::cout << LocalPlayer << std::endl;
        PlayerController = *reinterpret_cast<UObject**>(__int64(LocalPlayer) + Offsets::LocalPlayer::PlayerController);
        std::cout << PlayerController << std::endl;
        World = *reinterpret_cast<UObject**>(__int64(GameViewport) + Offsets::GameViewportClient::World);
        std::cout << World << std::endl;
        GameMode = *reinterpret_cast<UObject**>(__int64(World) + Offsets::World::AuthorityGameMode);
        std::cout << GameMode << std::endl;
        GameState = *reinterpret_cast<UObject**>(__int64(World) + Offsets::World::GameState);
        std::cout << GameState << std::endl;
        PlayerState = *reinterpret_cast<UObject**>(__int64(PlayerController) + Offsets::PlayerController::AcknowledgedPawn + Offsets::Pawn::PlayerState);
        std::cout << PlayerState << std::endl;
	}
}