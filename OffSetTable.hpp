#pragma once

#define PI (3.14159265358979323846264338327950288419716939937510)
#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct { unsigned char MAKE_PAD(offset); type name;}

namespace Globals{

	//static inline cUWorld* UWorld;

	class cActor
	{
	public:
		union
		{// TrackedNativeGlowFlags 0x01D8
			DEFINE_MEMBER_N(UObject*, PlayerState, 0x0238);
		};
	};


	class cPlayerController
	{
	public:
		union
		{
			DEFINE_MEMBER_N(cActor*, AcknowledgedPawn, 0x02A8);
			DEFINE_MEMBER_N(UObject*, CheatManager, 0x0338);
		};
	};

	class cConsole;

	class cGameViewportClient
	{
	public:
		union
		{
			DEFINE_MEMBER_N(UObject*, ViewportConsole, 0x0040);
		};
	};

	class cEngine
	{
	public:
		union
		{
			DEFINE_MEMBER_N(UObject*, ConsoleClass, 0x00F8);
			DEFINE_MEMBER_N(cGameViewportClient*, GameViewportClient, 0x0788);
		};
	};

	class cLocalPlayer
	{
	public:
		union
		{
			DEFINE_MEMBER_N(cPlayerController*, PlayerController, 0x0030);
		};
	};

	class cGameInstance
	{
	public:
		union
		{
			DEFINE_MEMBER_N(cLocalPlayer**, LocalPlayers, 0x0038);
		};
	};

	class cLevel
	{
	public:
		union
		{
			DEFINE_MEMBER_N(cActor**, Actors, 0x0098);
			DEFINE_MEMBER_N(int32_t, ActorsCount, 0x00A0);
		};
	};

	class cUWorld
	{
	public:
		union
		{
			DEFINE_MEMBER_N(cLevel**, Levels, 0x0148);
			DEFINE_MEMBER_N(int32_t, LevelCount, 0x150);
			DEFINE_MEMBER_N(cGameInstance*, GameInstance, 0x190);
		};
	};


}

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