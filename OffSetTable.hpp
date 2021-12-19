#pragma once

namespace Offsets
{
	namespace GameState
	{
		inline __int64 CurrentPlaylistInfo = 0x2068;
	}

	namespace World
	{
		inline __int64 GameState = 0x130;
		inline __int64 GameMode = 0x128;
	}

	namespace GameViewportClient
	{
		inline __int64 ViewportConsole = 0x40;
	}

	namespace PlayerController
	{
		inline __int64 CheatManager = 0x340;
		inline __int64 AcknowledgedPawn = 0x2a8;
	}

	namespace CurrentPlaylistInfo
	{
		inline __int64 BasePlaylist = 0x120;
	}
}