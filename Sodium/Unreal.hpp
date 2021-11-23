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
inline uintptr_t StaticConstructObject_InternalAddr;

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

static UObject* StaticConstructObjectInternal(
	void* Class,
	void* InOuter,
	void* Name,
	int SetFlags,
	unsigned int InternalSetFlags,
	void* Template,
	bool bCopyTransientsFromClassDefaults,
	void* InstanceGraph,
	bool bAssumeTemplateIsArchetype)
{
	auto fStaticConstructObject_Internal = reinterpret_cast<UObject * (__fastcall*)(void*, void*, void*, int, unsigned int, void*, bool, void*, bool)>(StaticConstructObject_InternalAddr);
	return fStaticConstructObject_Internal(Class, InOuter, Name, SetFlags, InternalSetFlags, Template, bCopyTransientsFromClassDefaults, InstanceGraph, bAssumeTemplateIsArchetype);
}