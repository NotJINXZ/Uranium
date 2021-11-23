#pragma once

#include "Structs.hpp"
#include <iostream>
#include <fstream>

inline FUObjectArray* GObjects;
inline void* (*ProcessEvent)(void*, void*, void*);
inline UObject* FortEngine;
inline uintptr_t FNameToStringAddr;
inline uintptr_t GObjectsAddr;
inline uintptr_t FreeMemoryAddr;

static inline UObject* FindObject(std::string name, bool bEqual = false)
{
	for (int32_t i = 0; i < GObjects->NumElements; i++)
	{
		auto object = GObjects->GetByIndex(i);

		if (object == nullptr)
			continue;

		if (bEqual) {
			if (object->GetFullName() == name)
				return object;
		} else {
			if (object->GetFullName().find(name) != std::string::npos)
				return object;
		}
	}

	return nullptr;
}

static inline void DumpObjects()
{
	std::ofstream file("GObjects.log");
	file << "Fortnite GObjects Dump\n\n";

	int CurrentObjectNum = 0;

	for (int32_t i = 0; i < GObjects->NumElements; i++)
	{
		auto object = GObjects->GetByIndex(i);

		if (object == nullptr)
			continue;

		file << "[" << CurrentObjectNum << "] " << object->GetFullName() << "\n";
	}

	CurrentObjectNum = 0;
	file.close();
}

static inline UObject* StaticLoadObject(UObject* Class, UObject* InOuter, const TCHAR* Name, const TCHAR* FileName = nullptr, uint32_t LoadFlags = 0, void* Sandbox = nullptr, bool bAllowObjectReconciliation = false, void* InstancingContext = nullptr)
{
	auto fStaticLoadObject = reinterpret_cast<UObject * (__fastcall*)(UObject*, UObject*, const TCHAR*, const TCHAR*, uint32_t, void*, bool, void*)>(Util::FindPattern("\x4C\x89\x4C\x24\x00\x48\x89\x54\x24\x00\x48\x89\x4C\x24\x00\x55\x53\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8B\xEC", "xxxx? xxxx?xxxx?xxxxxxxxxxxxxxx"));
	return fStaticLoadObject(Class, InOuter, Name, FileName, LoadFlags, Sandbox, bAllowObjectReconciliation, InstancingContext);
}