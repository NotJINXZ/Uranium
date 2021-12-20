#pragma once

namespace Offsets
{
	//If there is an offset that is being used for core gameplay put it here if not just find it
	//where the function is.
	//TODO: Move all of the offsets here
	DWORD InventoryOffset;
	DWORD ItemGuidOffset;
	DWORD ItemDefinitionOffset;
	DWORD PrimQuickBarOffset;
	DWORD EntriesOffset;
	DWORD PickupEntryOffset;
	DWORD EntryCountOffset;

	static void InitPreDefinedOffsets()
	{
		InventoryOffset = FindOffset("FortInventory", "Inventory");
		EntriesOffset = FindOffset("FortItemList", "ReplicatedEntries");
		ItemGuidOffset = FindOffset("FortItemEntry", "ItemGuid");
		ItemDefinitionOffset = FindOffset("FortItemEntry", "ItemDefinition");
		PrimQuickBarOffset = FindOffset("FortQuickBars", "PrimaryQuickBar");
		PickupEntryOffset = FindOffset("FortPickup", "PrimaryPickupItemEntry");
		EntryCountOffset = FindOffset("FortItemEntry", "Count");
	}
}