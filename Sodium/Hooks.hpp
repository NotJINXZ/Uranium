#pragma once

#include "Structs.hpp"
#include "Unreal.hpp"
#include <fstream>
#include <iostream>

void* ProcessEventHook(UObject* object, UObject* function, void* params)
{
	if (object && function) {
		if (function->GetName().find("ServerLoadingScreenDropped") != std::string::npos)
		{

		}
	}

	return ProcessEvent(object, function, params);
}