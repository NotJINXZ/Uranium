#pragma once

#include <Windows.h>

namespace Offsets
{
	namespace Engine
	{
		inline __int64 GameViewport = 0x780;
	}

	namespace GameViewportClient
	{
		inline __int64 GameInstance = 0x80;
		inline __int64 World = 0x78;
		inline __int64 ViewportConsole = 0x40;
	}

	namespace GameInstance
	{
		inline __int64 LocalPlayers = 0x38;
	}

	namespace LocalPlayer
	{
		inline __int64 PlayerController = 0x30;
	}

	namespace PlayerController
	{
		inline __int64 Pawn = 0x250;
		inline __int64 CheatManager = 0x338;
		inline __int64 PlayerState = 0x228;
	}

	namespace World
	{
		inline __int64 PersistentLevel = 0x30;
		inline __int64 GameState = 0x128;
		inline __int64 AuthorityGameMode = 0x120;
		inline __int64 OwningGameInstance = 0x188;
	}
}