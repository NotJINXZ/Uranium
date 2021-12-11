#pragma once

#include <Windows.h>
#include <cstdio>
#include <cstdint>
#include "Util.h"
#include "minhook/MinHook.h"

#pragma comment(lib, "minhook/minhook.lib")

typedef void* (__fastcall* fExitBypass)(__int64 arg1);
typedef void* (__fastcall* fCrashBypass)(__int64 arg1, __int64 arg2);
typedef void* (__fastcall* fRequestExit)(bool force);
typedef void* (__fastcall* fRequestExitWithStatus)(bool unknown, bool force);
typedef void* (__fastcall* fNotiBypass)(wchar_t** arg1, unsigned __int8 arg2, __int64 arg3, char arg4);
fExitBypass ExitBypass;
fCrashBypass CrashBypass;
fRequestExit RequestExitBypass;
fNotiBypass NotiBypass;
fRequestExitWithStatus RequestExitWithStatusBypass;
static void* RequestExitWithStatusAddress;

void* __fastcall ExitBypassHook(__int64 a1)
{
    return NULL;
}

void* __fastcall CrashHook(__int64 a1, __int64 a2)
{
    return NULL;
}

void* __fastcall RequestExitBypassHook(bool force)
{
    printf("RequestExit Call WAS CALLED: %i\n", force);
    return NULL;
}

void* __fastcall NotificationHook(wchar_t** a1, unsigned __int8 a2, __int64 a3, char a4)
{
    return NULL;
}

void RequestExitWithStatusHook(bool Force, uint8_t ReturnCode)
{
    printf("BigFatCock Check: %i ReturnCode: %u\n", Force, ReturnCode);
}

static void InitHooks()
{
    auto NotificationHookAddress = Util::FindPattern("4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 5B 20");
    auto FNCrashHookAddress = Util::FindPattern("48 8B C4 48 89 58 08 48 89 70 10 48 89 78 18 4C 89 60 20 55 41 56 41 57 48 8B EC 48 83 EC 40 45 33 FF 4C 8D 35 ? ? ? ?");
    RequestExitWithStatusAddress = Util::FindPattern("\x48\x8B\xC4\x48\x89\x58\x18\x88\x50\x10\x88\x48\x08\x57\x48\x83\xEC\x30", "xxxxxxxxxxxxxxxxxx");
    CHECKSIG(NotificationHookAddress, "Failed to find Notification address!");
    CHECKSIG(FNCrashHookAddress, "Failed to find FNCrash address!");
    CHECKSIG(RequestExitWithStatusAddress, "Failed to find RequestExitWithStatus address");

    MH_CreateHook(static_cast<LPVOID>((LPVOID)RequestExitWithStatusAddress), RequestExitWithStatusHook, reinterpret_cast<LPVOID*>(&RequestExitWithStatusBypass));
    MH_EnableHook(static_cast<LPVOID>((LPVOID)RequestExitWithStatusAddress));
    MH_CreateHook(static_cast<LPVOID>((LPVOID)FNCrashHookAddress), CrashHook, reinterpret_cast<LPVOID*>(&CrashBypass));
    MH_CreateHook(static_cast<LPVOID>((LPVOID)NotificationHookAddress), ExitBypassHook, reinterpret_cast<LPVOID*>(&NotiBypass));
    MH_EnableHook(static_cast<LPVOID>((LPVOID)NotificationHookAddress));
}