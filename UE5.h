#pragma once

#include <cstdint>
#include <Windows.h>
#include <string>
#include <locale>
#include <iostream>
#include <fstream>
#include <Psapi.h>
#include <winscard.h>
#include "skCryptor.h"

struct UObject;


static UObject* StaticLoadObject(UObject* Class, UObject* InOuter, const TCHAR* Name, const TCHAR* FileName = nullptr, uint32_t LoadFlags = 0, void* Sandbox = nullptr, bool bAllowObjectReconciliation = false, void* InstancingContext = nullptr)
{
	auto staticloadobjectaddr = Util::FindPattern(crypt("48 8B C4 48 89 58 08 4C 89 48 20 4C 89 40 18 48 89 50 10 55 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC 70 33 FF 48 8D 05 ? ? ? ? 40 38 3D ? ? ? ? 4C 8B E1 49 8B D0 48 8D 4D E0 48 0F 45 C7 49 8B D8 48 89 45 D8 E8 ? ? ? ? 48 8D 15 ? ? ? ? 48 8B CB 8B F7 FF 15 ? ? ? ? 44 8B 7D 60 48 8D 55 E0 48 8B 7D 78 48 8D 4D 48 48 85 C0 48 89 7C 24 ? 41 8B C7 41 B1 01 41 0F 95 C5 45 8A C1 25 ? ? ? ? 89 44 24 20 E8 ? ? ? ? 48 8B 4D 48 4C 8D 35 ? ? ? ? 48 85 C9 0F 84 ? ? ? ? 33 DB 38 5D 70 0F 84 ? ? ? ? 45 84 ED 0F 84 ? ? ? ? 39 5D E8 44 8D 46 01 49 8B D6 48 8D 4D D0 48 0F 45 55 ? E8 ? ? ? ? 48 8B 55 48 45 33 C9 89 5C 24 30 49 8B CC 89 5C 24 28 4C 8B 00 88 5C 24 20"));
	auto fStaticLoadObject = reinterpret_cast<UObject * (__fastcall*)(UObject*, UObject*, const TCHAR*, const TCHAR*, uint32_t, void*, bool, void*)>(staticloadobjectaddr);
	return fStaticLoadObject(Class, InOuter, Name, FileName, LoadFlags, Sandbox, bAllowObjectReconciliation, InstancingContext);
}

template<class T>
struct TArray
{
	friend class FString;

public:
	inline TArray()
	{
		Data = NULL;
		Count = Max = 0;
	};

	inline INT Num() const
	{
		return Count;
	};

	inline T& operator[](int i)
	{
		return Data[i];
	};

	inline T& operator[](int i) const
	{
		return Data[i];
	};

	inline BOOL IsValidIndex(int i) const
	{
		return i < Num();
	}

	inline void Add(T InputData)
	{
		Data = (T*)realloc(Data, sizeof(T) * (Count + 1));
		Data[Count++] = InputData;
		Max = Count;
	};

	T* Data;
	INT32 Count;
	INT32 Max;
};


struct FString : private TArray<wchar_t>
{
	FString()
	{
	};

	FString(const wchar_t* other)
	{
		Max = Count = *other ? std::wcslen(other) + 1 : 0;

		if (Count)
		{
			Data = const_cast<wchar_t*>(other);
		}
	}

	bool IsValid() const
	{
		return Data != nullptr;
	}

	const wchar_t* c_str() const
	{
		return Data;
	}

	std::string ToString() const
	{
		auto length = std::wcslen(Data);

		std::string str(length, '\0');

		std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(Data, Data + length, '?', &str[0]);

		return str;
	}
};

template <class TEnum>
class TEnumAsByte
{
public:
	TEnumAsByte()
	{
	}

	TEnumAsByte(TEnum _value)
		: value(static_cast<uint8_t>(_value))
	{
	}

	explicit TEnumAsByte(int32_t _value)
		: value(static_cast<uint8_t>(_value))
	{
	}

	explicit TEnumAsByte(uint8_t _value)
		: value(_value)
	{
	}

	operator TEnum() const
	{
		return static_cast<TEnum>(value);
	}

	TEnum GetValue() const
	{
		return static_cast<TEnum>(value);
	}

private:
	uint8_t value;
};

struct FName;

void (*FNameToString)(FName* pThis, FString& out);
void (*FreeMemory)(__int64);

struct FName
{
	uint32_t ComparisonIndex;
	uint32_t DisplayIndex;

	FName() = default;

	explicit FName(int64_t name)
	{
		DisplayIndex = (name & 0xFFFFFFFF00000000LL) >> 32;
		ComparisonIndex = (name & 0xFFFFFFFFLL);
	};

	std::string ToString()
	{
		FString temp;
		FNameToString(this, temp);

		std::string ret(temp.ToString());
		FreeMemory((__int64)temp.c_str());

		return ret;
	}
};

struct FActorSpawnParameters
{
	unsigned char Unk00[0x40];
};

struct UObject
{
	void** VFT;
	int32_t ObjectFlags;
	int32_t InternalIndex;
	UObject* Class;
	FName Name;
	UObject* Outer;

	bool IsA(UObject* cmp) const
	{
		if (cmp == Class)
			return false;
		return false;
	}

	std::string GetName()
	{
		return Name.ToString();
	}

	std::string GetFullName()
	{
		std::string temp;

		for (auto outer = Outer; outer; outer = outer->Outer)
		{
			temp = outer->GetName() + "." + temp;
		}

		temp = reinterpret_cast<UObject*>(Class)->GetName() + " " + temp + this->GetName();
		return temp;
	}
};

struct FGuid
{
	int A;
	int B;
	int C;
	int D;
};

struct FUObjectItem
{
	UObject* Object;
	DWORD Flags;
	DWORD ClusterIndex;
	DWORD SerialNumber;
	DWORD SerialNumber2;
};

struct PreFUObjectItem
{
	FUObjectItem* FUObject[10];
};

struct FUObjectArray
{
	PreFUObjectItem* ObjectArray;
	BYTE unknown1[8];
	int32_t MaxElements;
	int32_t NumElements;

	void NumChunks(int* start, int* end) const
	{
		int cStart = 0, cEnd = 0;

		if (!cEnd)
		{
			while (true)
			{
				if (ObjectArray->FUObject[cStart] == nullptr)
				{
					cStart++;
				}
				else
				{
					break;
				}
			}

			cEnd = cStart;
			while (true)
			{
				if (ObjectArray->FUObject[cEnd] == nullptr)
				{
					break;
				}
				cEnd++;
			}
		}

		*start = cStart;
		*end = cEnd;
	}

	UObject* GetByIndex(int32_t index) const
	{
		int cStart = 0, cEnd = 0;
		int chunkIndex, chunkSize = 0xFFFF, chunkPos;
		FUObjectItem* Object;

		NumChunks(&cStart, &cEnd);

		chunkIndex = index / chunkSize;
		if (chunkSize * chunkIndex != 0 &&
			chunkSize * chunkIndex == index)
		{
			chunkIndex--;
		}

		chunkPos = cStart + chunkIndex;
		if (chunkPos < cEnd)
		{
			Object = ObjectArray->FUObject[chunkPos] + (index - chunkSize * chunkIndex);
			if (!Object) { return nullptr; }

			return Object->Object;
		}

		return nullptr;
	}
};

FUObjectArray* GObjects;

struct FVector
{
	float X;
	float Y;
	float Z;

	FVector()
		: X(0), Y(0), Z(0)
	{
	}

	FVector(float x, float y, float z)
		: X(x),
		Y(y),
		Z(z)
	{
	}

	FVector operator-(FVector v)
	{
		return FVector(X - v.X, Y - v.Y, Z - v.Z);
	}

	FVector operator+(FVector v)
	{
		return FVector(X + v.X, Y + v.Y, Z + v.Z);
	}

	float Distance(FVector v)
	{
		return ((X - v.X) * (X - v.X) +
			(Y - v.Y) * (Y - v.Y) +
			(Z - v.Z) * (Z - v.Z));
	}
};

struct FRotator
{
	float Pitch;
	float Yaw;
	float Roll;

	FRotator()
		: Pitch(0),
		Yaw(0),
		Roll(0)
	{
	}

	FRotator(float pitch, float yaw, float roll)
		: Pitch(pitch),
		Yaw(yaw),
		Roll(roll)
	{
	}
};

struct FQuat
{
	float W, X, Y, Z;

	FQuat()
		: W(0),
		X(0),
		Y(0),
		Z(0)
	{
	}

	FQuat(float w, float x, float y, float z)
		: W(w),
		X(x),
		Y(y),
		Z(z)
	{
	}
};

struct FTransform
{
	FQuat Rotation;
	FVector Translation;
	char UnknownData_1C[0x4];
	FVector Scale3D;
	char UnknownData_2C[0x4];
};
enum class EAthenaGamePhase
{
	None = 0,
	Setup = 1,
	Warmup = 2,
	Aircraft = 3,
	SafeZones = 4,
	EndGame = 5,
	Count = 6,
	EAthenaGamePhase_MAX = 7
};
struct GetAllActorsOfClass_Params
{
	UObject* WorldContextObject;
	UObject* ActorClass; //AActor
	TArray<UObject*> OutActors; //AActor
};
struct RBitField
{
	unsigned char A : 1;
	unsigned char B : 1;
	unsigned char C : 1;
	unsigned char D : 1;
	unsigned char E : 1;
	unsigned char F : 1;
	unsigned char G : 1;
	unsigned char H : 1;
};
struct FPlaylistPropertyArray
{
	unsigned char padding_180[0x68];//0x180 (0x68)
	UObject* BasePlaylist; //0x120 (0x8)
	UObject* OverridePlaylist; //0x128 (0x8)
};

enum class ESpawnActorCollisionHandlingMethod : uint8_t
{
	Undefined = 0,
	AlwaysSpawn = 1,
	AdjustIfPossibleButAlwaysSpawn = 2,
	AdjustIfPossibleButDontSpawnIfColliding = 3,
	DontSpawnIfColliding = 4,
	ESpawnActorCollisionHandlingMethod_MAX = 5
};

struct UGameplayStatics_BeginDeferredActorSpawnFromClass_Params
{
	UObject* WorldContextObject;
	UObject* ActorClass;
	FTransform SpawnTransform;
	ESpawnActorCollisionHandlingMethod CollisionHandlingOverride;
	UObject* Owner;
	UObject* ReturnValue;
};

struct UGameplayStatics_FinishSpawningActor_Params
{
	UObject* Actor;
	FTransform SpawnTransform;
	UObject* ReturnValue;
};

struct SpawnObjectParams
{
	UObject* ObjectClass;
	UObject* Outer;
	UObject* ReturnValue;
};

struct FFortGiftingInfo {};
struct FGameplayAbilitySpecHandle {};
struct FFastArraySerializer {};

struct FGameplayAbilitySpecDef
{
	UObject* Ability;
	unsigned char Unk00[0x90];
};

struct FActiveGameplayEffectHandle
{
	int Handle; // 0x00(0x04)
	bool bPassedFiltersAndWasExecuted; // 0x04(0x01)
	char UnknownData_5[0x3]; // 0x05(0x03)
};

struct FGameplayEffectContextHandle
{
	char UnknownData_0[0x30]; // 0x00(0x18)
};

//Inventory Structs Below
struct QuickbarSlot
{
	TArray<struct FGuid> Items;
	bool bEnabled;
	char Unk00[0x7];
};

struct InventoryPointer
{
	UObject* Inventory;
};

struct QuickBarPointer
{
	UObject* QuickBar;
};

static void DumpObjects()
{
	std::ofstream log(crypt("Dump.txt"));

	auto TotalObjects = GObjects->NumElements;

	for (int i = 0; i < TotalObjects; ++i)
	{
		auto CurrentObject = GObjects->GetByIndex(i);

		if (CurrentObject)
		{
			auto name = CurrentObject->GetFullName();
			std::string str(name.begin(), name.end());

			log << str + "\n";
		}
	}
	return;
}

static UObject* FindObject(std::string name)
{
	for (int32_t i = 0; i < GObjects->NumElements; i++)
	{
		auto object = GObjects->GetByIndex(i);

		if (object == nullptr)
			continue;

		if (object->GetFullName().find(name) != std::string::npos) {
			//std::cout << "Found Object: " << object->GetFullName() << std::endl;
			return object;
		}
	}

	return nullptr;
}

std::vector<UObject*> FoundObjects;

static UObject* FindObjectWithSkip(UObject* Class, int Skip = 1)
{
	for (int32_t i = 0; i < GObjects->NumElements; i++)
	{
		auto object = GObjects->GetByIndex(i);

		if (object == nullptr)
			continue;

		if (object->Class == Class) 
		{
			if (Skip > 0)
			{
				Skip--;
			}
			else
			{
				bool bFoundObject = false;
				for (auto Obj : FoundObjects)
				{
					if (Obj == object)
					{
						bFoundObject = true;
					}
				}
				if (!bFoundObject)
				{
					std::cout << "Name: " << object->GetFullName() << std::endl;
					FoundObjects.push_back(object);
					return object;
				}
			}
		}
	}

	return nullptr;
}

static DWORD FindOffset(std::string OffsetToFind)
{
	auto Object = FindObject(OffsetToFind);

	if (Object)
	{
		return *(uint32_t*)(__int64(Object) + 0x4C);
	}

	return 0;
}

static UObject* FindObjectStart(std::string name)
{
	for (int32_t i = 0; i < GObjects->NumElements; i++)
	{
		auto object = GObjects->GetByIndex(i);
		
		if (object == nullptr)
			continue;

		if (object->GetFullName().starts_with(name))
			return object;
	}

	return nullptr;
}

inline bool ProcessEvent(UObject* pObject, UObject* pFunction, void* pParams) {
	auto vtable = *reinterpret_cast<void***>(pObject);
	auto ProcesseventVtable = static_cast<void(*)(void*, void*, void*)>(vtable[0x4B]); if (!ProcesseventVtable) return false;
	ProcesseventVtable(pObject, pFunction, pParams);
	return true;
}

enum class EGameplayEffectDurationType : uint8_t
{
	Instant,
	Infinite,
	HasDuration,
	EGameplayEffectDurationType_MAX
};

struct FFortItemEntry
{
	unsigned char Unk00[0x1A0];
};

enum class EFortQuickBars : uint8_t
{
	Primary,
	Secondary,
	Max_None,
	EFortQuickBars_MAX
};

struct FFortAthenaLoadout
{
	FString BannerIconId;
	FString BannerColorId;
	UObject* SkyDiveContrail;
	UObject* Glider;
	UObject* Pickaxe;
	bool bIsDefaultCharacter;
	unsigned char UnknownData00[0x7];
	UObject* Character;
	TArray<UObject*> CharacterVariantChannels;
	bool bForceUpdateVariants;
	unsigned char UnknownData01[0x7];
	UObject* Hat;
	UObject* Backpack;
	UObject* LoadingScreen;
	UObject* BattleBus;
	UObject* VehicleDecoration;
	UObject* CallingCard;
	UObject* MapMarker;
	TArray<UObject*> Dances;
	UObject* VictoryPose;
	UObject* MusicPack;
	UObject* ItemWrapOverride;
	TArray<UObject*> ItemWraps;
	UObject* CharmOverride;
	TArray<UObject*> Charms;
	UObject* PetSkin;
};

enum class EFortCustomBodyType : uint8_t
{
	NONE = 0,
	Small = 1,
	Medium = 2,
	MediumAndSmall = 3,
	Large = 4,
	LargeAndSmall = 5,
	LargeAndMedium = 6,
	All = 7,
	Deprecated = 8,
	EFortCustomBodyType_MAX = 9
};

enum class EFortCustomGender : uint8_t
{
	Invalid = 0,
	Male = 1,
	Female = 2,
	Both = 3,
	EFortCustomGender_MAX = 4
};

enum class EMontagePlayReturnType : uint8_t
{
	MontageLength = 0,
	Duration = 1,
	EMontagePlayReturnType_MAX = 2
};

struct FSlateBrush
{
	unsigned char Unk00[0x48];
	UObject* ObjectResource; // 0x08
};