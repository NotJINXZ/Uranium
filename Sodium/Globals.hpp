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
	}
}