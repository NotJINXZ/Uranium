#pragma once

#include "Unreal.hpp"
#include "Globals.hpp"
#include "Structs.hpp"

namespace Functions
{
	static inline void SwitchLevel(FString URL)
	{
		static auto fn = FindObject("Function /Script/Engine.PlayerController.SwitchLevel");
		ProcessEvent(Globals::PlayerController, fn, &URL);
	}
}