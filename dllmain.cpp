#include <Windows.h>
#include "pch.h"
#include "ext/MinHook.h"
#include <cstdio>
#include <cstdint>
#include <stdlib.h>
#pragma comment(lib, "libMinHook.x64.lib")

static uintptr_t baseAddress = reinterpret_cast<uintptr_t>(GetModuleHandleA(NULL));
static uintptr_t gameAssembly = reinterpret_cast<uintptr_t>(GetModuleHandleA("GameAssembly.dll"));
using isRobeUnlocked = bool(__fastcall*)(DWORD* _this, const char* robe, DWORD* methodinfo);
using isCharacterUnlocked = bool (__fastcall*)(DWORD* _this, const char* character, DWORD* methodinfo);
using playerHasItem = bool (__fastcall*)(DWORD* _this, uint32_t connectionId, int32_t steamDefId, DWORD* methodinfo);
using userHasItem = bool(__fastcall*)(DWORD* _this, int32_t steamDefId, DWORD* methodinfo);
using GrantPromoItems = void(__fastcall*)(DWORD* _this, DWORD* methodinfo);

void CreateConsole() {
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
}

static GrantPromoItems GrantPromoItems_o = nullptr;
static void __fastcall GrantPromoItems_hk(DWORD* _this, DWORD* methodinfo) {
	printf("GrantPromoItems called\n");
	GrantPromoItems_o(_this, methodinfo);
}

static userHasItem userHasItem_o = nullptr;
static bool __fastcall userHasItem_hk(DWORD* _this, int32_t steamDefId, DWORD* methodinfo) {
	printf("SteamDefId: %d\n", steamDefId);
	return true;
}

static playerHasItem playerHasItem_o = nullptr;
static bool __fastcall playerHasItem_hk(DWORD* _this, uint32_t connectionId, int32_t steamDefId, DWORD* methodinfo) {
	printf("SteamDefId: %d\n", steamDefId);
	return true;
}

static isCharacterUnlocked isCharacterUnlocked_o = nullptr;
static bool __fastcall isCharacterUnlocked_hk(DWORD* _this, const char* character, DWORD* methodinfo) {
	printf("Character address 0x%p\n", character);
	char buffer[256];
	wcstombs(buffer, (wchar_t*)character + 0xA, sizeof(buffer));
	printf("Character: %s\n", buffer);
	return true;
}

static isRobeUnlocked isRobeUnlocked_o = nullptr;
static bool __fastcall isRobeUnlocked_hk(DWORD* _this, const char* robe, DWORD* methodinfo) {
    printf("Robe address 0x%p\n", robe);
    char buffer[256];
    wcstombs(buffer, (wchar_t*)robe + 0xA, sizeof(buffer));
    printf("Robe: %s\n", buffer);
    return true;
}

void init() {
    MH_Initialize();
    CreateConsole();
}

void main() {
    init();
    MH_CreateHook(reinterpret_cast<LPVOID*>(gameAssembly + 0x575650), &isRobeUnlocked_hk, (LPVOID*)isRobeUnlocked_o);
    MH_CreateHook(reinterpret_cast<LPVOID*>(gameAssembly + 0x574240), &isCharacterUnlocked_hk, (LPVOID*)isCharacterUnlocked_o);
    MH_CreateHook(reinterpret_cast<LPVOID*>(gameAssembly + 0x4CE410), &playerHasItem_hk, (LPVOID*)playerHasItem_o);
    MH_CreateHook(reinterpret_cast<LPVOID*>(gameAssembly + 0x4CC490), &userHasItem_hk, (LPVOID*)userHasItem_o);
    MH_CreateHook(reinterpret_cast<LPVOID*>(gameAssembly + 0x4CC530), &GrantPromoItems_hk, (LPVOID*)GrantPromoItems_o);
    MH_EnableHook(reinterpret_cast<LPVOID*>(gameAssembly + 0x574240));
    MH_EnableHook(reinterpret_cast<LPVOID*>(gameAssembly + 0x575650));
    MH_EnableHook(reinterpret_cast<LPVOID*>(gameAssembly + 0x4CE410));
    MH_EnableHook(reinterpret_cast<LPVOID*>(gameAssembly + 0x4CC490));
    MH_EnableHook(reinterpret_cast<LPVOID*>(gameAssembly + 0x4CC530));
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)main, NULL, 0, NULL);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

