/*	EQEMu: Everquest Server Emulator
	Copyright (C) 2001-2002 EQEMu Development Team (http://eqemu.org)

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; version 2 of the License.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY except by those people which sell it, which
	are required to give you total support for your newly bought product;
	without even the implied warranty of MERCHANTABILITY or FITNESS FOR
	A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "../common/global_define.h"
#include "../common/loottable.h"
#include "../common/misc_functions.h"
#include "../common/data_verification.h"

#include "client.h"
#include "entity.h"
#include "mob.h"
#include "npc.h"
#include "zonedb.h"
#include "../common/zone_store.h"
#include "global_loot_manager.h"
#include "../common/repositories/criteria/content_filter_criteria.h"
#include "../common/say_link.h"

#include <iostream>
#include <stdlib.h>

#ifdef _WINDOWS
#define snprintf	_snprintf
#endif

#include "data_bucket.h"
#include "raids.h"
#include "groups.h"

// Queries the loottable: adds item & coin to the npc
void ZoneDatabase::AddLootTableToNPC(NPC* npc, uint32 loottable_id, ItemList* itemlist, uint32* copper, uint32* silver, uint32* gold, uint32* plat) {
	const LootTable_Struct* lts = nullptr;
	// global loot passes nullptr for these
	bool bGlobal = copper == nullptr && silver == nullptr && gold == nullptr && plat == nullptr;
	if (!bGlobal) {
		*copper = 0;
		*silver = 0;
		*gold = 0;
		*plat = 0;
	}

	lts = database.GetLootTable(loottable_id);
	if (!lts) {
		return;
	}

	if (!content_service.DoesPassContentFiltering(lts->content_flags)) {
		return;
	}

	uint32 min_cash = lts->mincash;
	uint32 max_cash = lts->maxcash;
	if(min_cash > max_cash) {
		uint32 t = min_cash;
		min_cash = max_cash;
		max_cash = t;
	}

	uint32 cash = 0;
	if (!bGlobal) {
		if(max_cash > 0 && lts->avgcoin > 0 && EQ::ValueWithin(lts->avgcoin, min_cash, max_cash)) {
			float upper_chance = (float)(lts->avgcoin - min_cash) / (float)(max_cash - min_cash);
			float avg_cash_roll = (float)zone->random.Real(0.0, 1.0);

			if(avg_cash_roll < upper_chance) {
				cash = zone->random.Int(lts->avgcoin, max_cash);
			} else {
				cash = zone->random.Int(min_cash, lts->avgcoin);
			}
		} else {
			cash = zone->random.Int(min_cash, max_cash);
		}
	}

	if(cash != 0) {
		if (RuleB(Vegas, EnableVegasDrops)) {
			cash *= RuleR(Vegas, CashMultiplier);
		}
		*plat = cash / 1000;
		cash -= *plat * 1000;

		*gold = cash / 100;
		cash -= *gold * 100;

		*silver = cash / 10;
		cash -= *silver * 10;

		*copper = cash;

		/*
		//Discord send
		if (top_client->CastToClient()->Admin() < RuleI(Vegas, MinStatusToBypassVegasDiscordLogging)) {
			std::ostringstream output;
			output <<
				"Cash: Added " << *plat << "p " << *gold << "g " << *silver << "s " << *copper << "c "
				" to [" << npc->GetCleanName() << "](http://vegaseq.com/Allaclone/?a=npc&id=" << npc->GetNPCTypeID() << ") [ID: " << npc->GetNPCTypeID() << "] [Level: " << int(GetLevel()) << "]"
				" in [" << zone->GetLongName() << "](http://vegaseq.com/Allaclone/?a=zone&name=" << zone->GetShortName() << ") [Instance: " << zone->GetInstanceID() << "]"
				;
			std::string out_final = output.str();
			zone->SendDiscordMessage("rngcash", out_final);
		}
		//Database Logging NEEDS WORK
		if (top_client->CastToClient()->Admin() < RuleI(Vegas, MinStatusToBypassVegasLootLogging)) {
			raid_target && !NPCBypassesVegasRaidLoot() ? vegas_loot_type = "Raid-Bonus" : rare_spawn ? vegas_loot_type = "Rare-Bonus" : vegas_loot_type = "Common-Bonus";
			item_name = chosen_item->Name;
			//std::replace(item_name.begin(), item_name.end(), "\`", "\'");
			query = StringFormat(
				"INSERT INTO rng_cash "
				"SET `npc_id` = '%i', `npc_name` = '%s', `npc_level` = '%i', `type` = '%s', `npc_diff` = '%f', `item_id` = '%i', `raid_item` = '%i', `item` = '%s', `item_diff` = '%f', `zone_id` = '%i', `zone` = '%s', `zone_instance_id` = '%i', `time` = NOW(), `charid` = '%i', `char` = '%s', `char_level` = '%i' ",
				GetNPCTypeID(), Strings::Escape(GetCleanName()).c_str(), GetLevel(), vegas_loot_type, double(difficulty), chosen_item->ID, raid_only, Strings::Escape(chosen_item->Name).c_str(), double(chosen_item->difficulty), GetZoneID(), zone->GetShortName(), zone->GetInstanceID(), top_client->CastToClient()->CharacterID(), top_client->CastToClient()->GetCleanName(), top_client->CastToClient()->GetLevel());
			QueryVegasLoot(query, top_client->CastToClient()->GetCleanName(), GetCleanName());
		}
		*/
	}

	uint32 global_loot_multiplier = RuleI(Zone, GlobalLootMultiplier);

	// Do items
	for (uint32 i=0; i<lts->NumEntries; i++) {
		for (uint32 k = 1; k <= (lts->Entries[i].multiplier * global_loot_multiplier); k++) {
			uint8 droplimit = lts->Entries[i].droplimit;
			uint8 mindrop = lts->Entries[i].mindrop;

			//LootTable Entry probability
			float ltchance = 0.0f;
			ltchance = lts->Entries[i].probability;

			float drop_chance = 0.0f;
			if(ltchance > 0.0 && ltchance < 100.0) {
				drop_chance = (float)zone->random.Real(0.0, 100.0);
			}

			if (ltchance != 0.0 && (ltchance == 100.0 || drop_chance <= ltchance)) {
				AddLootDropToNPC(npc, lts->Entries[i].lootdrop_id, itemlist, droplimit, mindrop);
			}
		}
	}
}

// Called by AddLootTableToNPC
// maxdrops = size of the array npcd
void ZoneDatabase::AddLootDropToNPC(NPC *npc, uint32 lootdrop_id, ItemList *item_list, uint8 droplimit, uint8 mindrop)
{
	const LootDrop_Struct *loot_drop = GetLootDrop(lootdrop_id);
	if (!loot_drop) {
		return;
	}

	if (loot_drop->NumEntries == 0) {
		return;
	}

	if (!content_service.DoesPassContentFiltering(loot_drop->content_flags)) {
		return;
	}

	// if this lootdrop is droplimit=0 and mindrop 0, scan list once and return
	if (droplimit == 0 && mindrop == 0) {
		for (uint32 i = 0; i < loot_drop->NumEntries; ++i) {
			int      charges = loot_drop->Entries[i].multiplier;
			for (int j       = 0; j < charges; ++j) {
				if (zone->random.Real(0.0, 100.0) <= loot_drop->Entries[i].chance &&
					npc->MeetsLootDropLevelRequirements(loot_drop->Entries[i], true)) {
					const EQ::ItemData *database_item = GetItem(loot_drop->Entries[i].item_id);
					npc->AddLootDrop(
						database_item,
						item_list,
						loot_drop->Entries[i]
					);
				}
			}
		}
		return;
	}

	if (loot_drop->NumEntries > 100 && droplimit == 0) {
		droplimit = 10;
	}

	if (droplimit < mindrop) {
		droplimit = mindrop;
	}

	float roll_t                   = 0.0f;
	float no_loot_prob             = 1.0f;
	bool  roll_table_chance_bypass = false;
	bool  active_item_list         = false;

	for (uint32 i = 0; i < loot_drop->NumEntries; ++i) {
		const EQ::ItemData *db_item = GetItem(loot_drop->Entries[i].item_id);
		if (db_item && npc->MeetsLootDropLevelRequirements(loot_drop->Entries[i])) {
			roll_t += loot_drop->Entries[i].chance;
			if (loot_drop->Entries[i].chance >= 100) {
				roll_table_chance_bypass = true;
			}
			else {
				no_loot_prob *= (100 - loot_drop->Entries[i].chance) / 100.0f;
			}
			active_item_list = true;
		}
	}

	if (!active_item_list) {
		return;
	}

	// This will pick one item per iteration until mindrop.
	// Don't let the compare against chance fool you.
	// The roll isn't 0-100, its 0-total and it picks the item, we're just
	// looping to find the lucky item, descremening otherwise. This is ok,
	// items with chance 60 are 6 times more likely than items chance 10.
	int drops = 0;

	for (int i = 0; i < droplimit; ++i) {
		if (drops < mindrop || roll_table_chance_bypass || (float) zone->random.Real(0.0, 1.0) >= no_loot_prob) {
			float       roll = (float) zone->random.Real(0.0, roll_t);
			for (uint32 j    = 0; j < loot_drop->NumEntries; ++j) {
				const EQ::ItemData *db_item = GetItem(loot_drop->Entries[j].item_id);
				if (db_item) {
					// if it doesn't meet the requirements do nothing
					if (!npc->MeetsLootDropLevelRequirements(loot_drop->Entries[j])) {
						continue;
					}

					if (roll < loot_drop->Entries[j].chance) {
						npc->AddLootDrop(
							db_item,
							item_list,
							loot_drop->Entries[j]
						);
						drops++;

						int charges = (int) loot_drop->Entries[i].multiplier;
						charges = EQ::ClampLower(charges, 1);

						for (int k = 1; k < charges; ++k) {
							float c_roll = (float) zone->random.Real(0.0, 100.0);
							if (c_roll <= loot_drop->Entries[i].chance) {
								npc->AddLootDrop(
									db_item,
									item_list,
									loot_drop->Entries[i]
								);
							}
						}

						j = loot_drop->NumEntries;
						break;
					}
					else {
						roll -= loot_drop->Entries[j].chance;
					}
				}
			}
		}
	}

	npc->UpdateEquipmentLight();
	// no wearchange associated with this function..so, this should not be needed
	//if (npc->UpdateActiveLightValue())
	//	npc->SendAppearancePacket(AT_Light, npc->GetActiveLightValue());
}

bool NPC::MeetsLootDropLevelRequirements(LootDropEntries_Struct loot_drop, bool verbose)
{
	if (loot_drop.npc_min_level > 0 && GetLevel() < loot_drop.npc_min_level) {
		if (verbose) {
			LogLootDetail(
				"NPC [{}] does not meet loot_drop level requirements (min_level) level [{}] current [{}] for item [{}]",
				GetCleanName(),
				loot_drop.npc_min_level,
				GetLevel(),
				database.CreateItemLink(loot_drop.item_id)
			);
		}
		return false;
	}

	if (loot_drop.npc_max_level > 0 && GetLevel() > loot_drop.npc_max_level) {
		if (verbose) {
			LogLootDetail(
				"NPC [{}] does not meet loot_drop level requirements (max_level) level [{}] current [{}] for item [{}]",
				GetCleanName(),
				loot_drop.npc_max_level,
				GetLevel(),
				database.CreateItemLink(loot_drop.item_id)
			);
		}
		return false;
	}

	return true;
}

LootDropEntries_Struct NPC::NewLootDropEntry()
{
	LootDropEntries_Struct loot_drop{};
	loot_drop.item_id           = 0;
	loot_drop.item_charges      = 1;
	loot_drop.equip_item        = 1;
	loot_drop.chance            = 0;
	loot_drop.trivial_min_level = 0;
	loot_drop.trivial_max_level = 0;
	loot_drop.npc_min_level     = 0;
	loot_drop.npc_max_level     = 0;
	loot_drop.multiplier        = 0;

	return loot_drop;
}

//if itemlist is null, just send wear changes
void NPC::AddLootDrop(
	const EQ::ItemData *item2,
	ItemList *itemlist,
	LootDropEntries_Struct loot_drop,
	bool wear_change,
	uint32 aug1,
	uint32 aug2,
	uint32 aug3,
	uint32 aug4,
	uint32 aug5,
	uint32 aug6
)
{
	if (item2 == nullptr) {
		return;
	}

	//make sure we are doing something...
	if (!itemlist && !wear_change) {
		return;
	}

	auto item = new ServerLootItem_Struct;

	if (LogSys.log_settings[Logs::Loot].is_category_enabled == 1) {
		EQ::SayLinkEngine linker;
		linker.SetLinkType(EQ::saylink::SayLinkItemData);
		linker.SetItemData(item2);

		LogLoot(
			"NPC [{}] Item ({}) [{}] charges [{}] chance [{}] trivial min/max [{}/{}] npc min/max [{}/{}]",
			GetName(),
			item2->ID,
			linker.GenerateLink(),
			loot_drop.item_charges,
			loot_drop.chance,
			loot_drop.trivial_min_level,
			loot_drop.trivial_max_level,
			loot_drop.npc_min_level,
			loot_drop.npc_max_level
		);
	}

	EQApplicationPacket *outapp               = nullptr;
	WearChange_Struct   *p_wear_change_struct = nullptr;
	if (wear_change) {
		outapp               = new EQApplicationPacket(OP_WearChange, sizeof(WearChange_Struct));
		p_wear_change_struct = (WearChange_Struct *) outapp->pBuffer;
		p_wear_change_struct->spawn_id = GetID();
		p_wear_change_struct->material = 0;
	}

	item->item_id           = item2->ID;
	item->charges           = loot_drop.item_charges;
	item->aug_1             = aug1;
	item->aug_2             = aug2;
	item->aug_3             = aug3;
	item->aug_4             = aug4;
	item->aug_5             = aug5;
	item->aug_6             = aug6;
	item->attuned           = 0;
	item->trivial_min_level = loot_drop.trivial_min_level;
	item->trivial_max_level = loot_drop.trivial_max_level;
	item->equip_slot        = EQ::invslot::SLOT_INVALID;


	// unsure if required to equip, YOLO for now
	if (item2->ItemType == EQ::item::ItemTypeBow) {
		SetBowEquipped(true);
	}

	if (item2->ItemType == EQ::item::ItemTypeArrow) {
		SetArrowEquipped(true);
	}

	if (loot_drop.equip_item > 0) {
		uint8 eslot = 0xFF;
		char newid[20];
		const EQ::ItemData* compitem = nullptr;
		bool found = false; // track if we found an empty slot we fit into
		int32 foundslot = -1; // for multi-slot items

		// Equip rules are as follows:
		// If the item has the NoPet flag set it will not be equipped.
		// An empty slot takes priority. The first empty one that an item can
		// fit into will be the one picked for the item.
		// AC is the primary choice for which item gets picked for a slot.
		// If AC is identical HP is considered next.
		// If an item can fit into multiple slots we'll pick the last one where
		// it is an improvement.

		if (!item2->NoPet) {
			for (int i = EQ::invslot::EQUIPMENT_BEGIN; !found && i <= EQ::invslot::EQUIPMENT_END; i++) {
				uint32 slots = (1 << i);
				if (item2->Slots & slots) {
					if(equipment[i])
					{
						compitem = database.GetItem(equipment[i]);
						if (item2->AC > compitem->AC ||
							(item2->AC == compitem->AC && item2->HP > compitem->HP))
						{
							// item would be an upgrade
							// check if we're multi-slot, if yes then we have to keep
							// looking in case any of the other slots we can fit into are empty.
							if (item2->Slots != slots) {
								foundslot = i;
							}
							else {
								// Unequip old item
								auto* olditem = GetItem(i);

								olditem->equip_slot = EQ::invslot::SLOT_INVALID;

								equipment[i] = item2->ID;

								foundslot = i;
								found     = true;
							}
						} // end if ac
					}
					else
					{
						equipment[i] = item2->ID;
						foundslot = i;
						found = true;
					}
				} // end if (slots)
			} // end for
		} // end if NoPet

		// Possible slot was found but not selected. Pick it now.
		if (!found && foundslot >= 0) {
			equipment[foundslot] = item2->ID;
			found = true;
		}

		// @merth: IDFile size has been increased, this needs to change
		uint16 emat;
		if(item2->Material <= 0
			|| (item2->Slots & ((1 << EQ::invslot::slotPrimary) | (1 << EQ::invslot::slotSecondary)))) {
			memset(newid, 0, sizeof(newid));
			for(int i=0;i<7;i++){
				if (!isalpha(item2->IDFile[i])){
					strn0cpy(newid, &item2->IDFile[i],6);
					i=8;
				}
			}

			emat = Strings::ToInt(newid);
		} else {
			emat = item2->Material;
		}

		if (foundslot == EQ::invslot::slotPrimary) {

			eslot = EQ::textures::weaponPrimary;
			if (item2->Damage > 0) {
				SendAddPlayerState(PlayerState::PrimaryWeaponEquipped);
				if (!RuleB(Combat, ClassicNPCBackstab))
					SetFacestab(true);
			}
			if (item2->IsType2HWeapon())
				SetTwoHanderEquipped(true);
		}
		else if (foundslot == EQ::invslot::slotSecondary
			&& (GetOwner() != nullptr || (CanThisClassDualWield() && zone->random.Roll(NPC_DW_CHANCE)) || (item2->Damage==0)) &&
			(item2->IsType1HWeapon() || item2->ItemType == EQ::item::ItemTypeShield || item2->ItemType ==  EQ::item::ItemTypeLight))
		{

			eslot = EQ::textures::weaponSecondary;
			if (item2->Damage > 0)
				SendAddPlayerState(PlayerState::SecondaryWeaponEquipped);
		}
		else if (foundslot == EQ::invslot::slotHead) {
			eslot = EQ::textures::armorHead;
		}
		else if (foundslot == EQ::invslot::slotChest) {
			eslot = EQ::textures::armorChest;
		}
		else if (foundslot == EQ::invslot::slotArms) {
			eslot = EQ::textures::armorArms;
		}
		else if (foundslot == EQ::invslot::slotWrist1 || foundslot == EQ::invslot::slotWrist2) {
			eslot = EQ::textures::armorWrist;
		}
		else if (foundslot == EQ::invslot::slotHands) {
			eslot = EQ::textures::armorHands;
		}
		else if (foundslot == EQ::invslot::slotLegs) {
			eslot = EQ::textures::armorLegs;
		}
		else if (foundslot == EQ::invslot::slotFeet) {
			eslot = EQ::textures::armorFeet;
		}

		/*
		what was this about???

		if (((npc->GetRace()==127) && (npc->CastToMob()->GetOwnerID()!=0)) && (item2->Slots==24576) || (item2->Slots==8192) || (item2->Slots==16384)){
			npc->d_melee_texture2=Strings::ToInt(newid);
			wc->wear_slot_id=8;
			if (item2->Material >0)
				wc->material=item2->Material;
			else
				wc->material=Strings::ToInt(newid);
			npc->AC+=item2->AC;
			npc->STR+=item2->STR;
			npc->INT+=item2->INT;
		}
		*/

		//if we found an open slot it goes in...
		if(eslot != 0xFF) {
			if(wear_change) {
				p_wear_change_struct->wear_slot_id = eslot;
				p_wear_change_struct->material     = emat;
			}

		}
		if (found) {
			CalcBonuses(); // This is less than ideal for bulk adding of items
			item->equip_slot = foundslot;
		}
	}

	if (itemlist != nullptr) {
		itemlist->push_back(item);
	}
	else safe_delete(item);

	if (IsRecordLootStats()) {
		m_rolled_items.emplace_back(item->item_id);
	}

	if (wear_change && outapp) {
		entity_list.QueueClients(this, outapp);
		safe_delete(outapp);
	}

	UpdateEquipmentLight();
	if (UpdateActiveLight()) {
		SendAppearancePacket(AT_Light, GetActiveLightType());
	}
}

void NPC::AddItem(const EQ::ItemData *item, uint16 charges, bool equipitem)
{
	//slot isnt needed, its determined from the item.
	auto loot_drop_entry = NPC::NewLootDropEntry();
	loot_drop_entry.equip_item   = static_cast<uint8>(equipitem ? 1 : 0);
	loot_drop_entry.item_charges = charges;

	AddLootDrop(item, &itemlist, loot_drop_entry, true);
}

void NPC::AddItem(
	uint32 itemid,
	uint16 charges,
	bool equipitem,
	uint32 aug1,
	uint32 aug2,
	uint32 aug3,
	uint32 aug4,
	uint32 aug5,
	uint32 aug6
)
{
	//slot isnt needed, its determined from the item.
	const EQ::ItemData *i = database.GetItem(itemid);
	if (i == nullptr) {
		return;
	}

	auto loot_drop_entry = NPC::NewLootDropEntry();
	loot_drop_entry.equip_item   = static_cast<uint8>(equipitem ? 1 : 0);
	loot_drop_entry.item_charges = charges;

	AddLootDrop(i, &itemlist, loot_drop_entry, true, aug1, aug2, aug3, aug4, aug5, aug6);
}

void NPC::AddLootTable() {
	if (npctype_id != 0) { // check if it's a GM spawn
		database.AddLootTableToNPC(this,loottable_id, &itemlist, &copper, &silver, &gold, &platinum);
	}
}

void NPC::AddLootTable(uint32 ldid) {
	if (npctype_id != 0) { // check if it's a GM spawn
	  database.AddLootTableToNPC(this,ldid, &itemlist, &copper, &silver, &gold, &platinum);
	}
}

void NPC::CheckGlobalLootTables()
{
	auto tables = zone->GetGlobalLootTables(this);

	for (auto &id : tables)
		database.AddLootTableToNPC(this, id, &itemlist, nullptr, nullptr, nullptr, nullptr);
}

void ZoneDatabase::LoadGlobalLoot()
{
	auto query = fmt::format(
		SQL
		(
			SELECT
			  id,
			  loottable_id,
			  description,
			  min_level,
			  max_level,
			  rare,
			  raid,
			  race,
			  class,
			  bodytype,
			  zone,
			  hot_zone
			FROM
			  global_loot
			WHERE
			  enabled = 1
			  {}
		),
		ContentFilterCriteria::apply()
	);

	auto results = QueryDatabase(query);
	if (!results.Success() || results.RowCount() == 0) {
		return;
	}

	LogInfo("Loaded [{}] global loot entries", Strings::Commify(results.RowCount()));

	// we might need this, lets not keep doing it in a loop
	auto      zoneid = std::to_string(zone->GetZoneID());
	for (auto row    = results.begin(); row != results.end(); ++row) {
		// checking zone limits
		if (row[10]) {
			auto zones = Strings::Split(row[10], '|');

			auto it = std::find(zones.begin(), zones.end(), zoneid);
			if (it == zones.end()) {  // not in here, skip
				continue;
			}
		}

		GlobalLootEntry e(Strings::ToInt(row[0]), Strings::ToInt(row[1]), row[2] ? row[2] : "");

		auto min_level = Strings::ToInt(row[3]);
		if (min_level) {
			e.AddRule(GlobalLoot::RuleTypes::LevelMin, min_level);
		}

		auto max_level = Strings::ToInt(row[4]);
		if (max_level) {
			e.AddRule(GlobalLoot::RuleTypes::LevelMax, max_level);
		}

		// null is not used
		if (row[5]) {
			e.AddRule(GlobalLoot::RuleTypes::Rare, Strings::ToInt(row[5]));
		}

		// null is not used
		if (row[6]) {
			e.AddRule(GlobalLoot::RuleTypes::Raid, Strings::ToInt(row[6]));
		}

		if (row[7]) {
			auto races = Strings::Split(row[7], '|');

			for (auto &r : races)
				e.AddRule(GlobalLoot::RuleTypes::Race, Strings::ToInt(r));
		}

		if (row[8]) {
			auto classes = Strings::Split(row[8], '|');

			for (auto &c : classes)
				e.AddRule(GlobalLoot::RuleTypes::Class, Strings::ToInt(c));
		}

		if (row[9]) {
			auto bodytypes = Strings::Split(row[9], '|');

			for (auto &b : bodytypes)
				e.AddRule(GlobalLoot::RuleTypes::BodyType, Strings::ToInt(b));
		}

		// null is not used
		if (row[11]) {
			e.AddRule(GlobalLoot::RuleTypes::HotZone, Strings::ToInt(row[11]));
		}

		zone->AddGlobalLootEntry(e);
	}
}

void NPC::AddVegasItemLoot(Mob* top_client) {

	BenchTimer benchmark;

	uint16 zoneid = zone->GetZoneID();
	uint8 zone_era; // 0 = Classic, 1 = Kunark, 2 = Velious, 3 = Luclin, 4 = PoP
	uint32 id_min = RuleI(Vegas, MinVegasItemID);
	uint32 id_max = RuleI(Vegas, MaxVegasItemID);
	bool raid_only = (IsRaidTarget() && !NPCBypassesVegasRaidLoot()) ? true : false;
	uint32 zone_range_min, zone_range_max;
	GetVegasZoneRange(zoneid, &zone_era, &zone_range_min, &zone_range_max);
	float difficulty_min, difficulty_max, difficulty_bonus_min, difficulty_bonus_max;
	GetVegasDifficultyRange(zoneid, &difficulty_min, &difficulty_max, &difficulty_bonus_min, &difficulty_bonus_max);
	auto random_drop_multiplier_scale = 0;
	bool guaranteed_shard_drop = false;
	float shard_rolled;
	float shard_multiplier = 1.00;

	if (top_client->CastToClient()->GetLevel() < RuleI(Vegas, RandomDropScaleLevelLimit)) {
		random_drop_multiplier_scale = RuleR(Vegas, RandomDropMultiplierScaling) - (top_client->CastToClient()->GetLevel() / (RuleI(Vegas, RandomDropScaleLevelLimit) / RuleR(Vegas, RandomDropMultiplierScaling)));
	}

	if (random_drop_multiplier_scale < 0) {
		random_drop_multiplier_scale = 0;
	}

	auto random_drop_multiplier = RuleR(Vegas, RandomDropMultiplier) + random_drop_multiplier_scale; //1.428571428571 multiplier for drop rate - 10 ||| Normally set to 1.75
	auto roll_count = GetRollCount(difficulty);
	auto raid_roll_count = GetRaidRollCount(difficulty);
	auto bonus_roll_count = GetBonusRollCount(difficulty);
	auto roll_max = GetRollMax();
	auto roll_to_hit = GetRollToHit(roll_max, random_drop_multiplier);
	auto bonus_roll_max = GetBonusRollMax();
	auto bonus_roll_to_hit = GetBonusRollToHit(bonus_roll_max, random_drop_multiplier);
	auto raid_roll_max = GetRaidRollMax();
	auto raid_roll_to_hit = GetRaidRollToHit(raid_roll_max, random_drop_multiplier);
	int rolled;
	std::string query;
	std::string vegas_loot_type;
	std::string npc_name = GetCleanName();
	//std::replace(npc_name.begin(), npc_name.end(), "\`", "\'");
	std::string item_name;
	
	VegasLoot("Attempting Vegas Loot for [{}] on [{}] - [{}].", top_client->GetCleanName(), GetCleanName(), IsRaidTarget() && NPCBypassesVegasRaidLoot() ? "Raid-Bypass" : IsRaidTarget() ? "Raid" : IsRareSpawn() ? "Rare" : "Normal"); //deleteme
	VegasLootDetail(
		"We are looking for item IDs in ZoneID [{}] between [{}] and [{}], with a difficulty between [{}] and [{}], a bonus difficulty between [{}] and [{}], lowest_drop_ids between [{}] and [{}], with a loot drop modifier of [{}] and a scale modifier of [{}] - that [{}] raid_only."
		, zoneid, id_min, id_max, difficulty_min, difficulty_max, difficulty_bonus_min, difficulty_bonus_max, zone_range_min, zone_range_max, random_drop_multiplier, random_drop_multiplier_scale, raid_only ? "are" : "are not"
	); //deleteme
	VegasLootDetail("Roll count is [{}], bonus roll count is [{}], raid roll count is [{}], roll max is [{}], bonus roll max is [{}], raid roll max is [{}], roll to hit is [{}], bonus roll to hit is [{}] and raid roll to hit is [{}]"
		, roll_count, bonus_roll_count, raid_roll_count, roll_max, bonus_roll_max, raid_roll_max, roll_to_hit, bonus_roll_to_hit, raid_roll_to_hit
	); //deleteme

	while (bonus_roll_count >= 1) {
		rolled = zone->random.Int(1, std::min(bonus_roll_max, bonus_roll_to_hit));
		VegasLootDetail("Rolled a [{}]", rolled); //deleteme
		if (rolled >= std::min(bonus_roll_max, bonus_roll_to_hit) || GuaranteedBonusDrop()) {
			VegasLoot("Bonus Roll hit on [{}] for [{}].", GetCleanName(), top_client->GetCleanName()); //deleteme
			shard_multiplier = RuleR(Vegas, ShardBonusLootMultiplier);
			guaranteed_shard_drop = true;
			const EQ::ItemData* chosen_item = GetVegasItems(id_min, id_max, difficulty_bonus_min, difficulty_bonus_max, zone_range_min, zone_range_max, raid_only);
			if (chosen_item) {
				AddItem(chosen_item, chosen_item->MaxCharges, RuleB(Vegas, EquipVegasDrops));
				VegasLoot("Added [{}] to [{}] for [{}]", database.CreateItemLink(chosen_item->ID), GetCleanName(), top_client->GetCleanName()); //deleteme

				if (top_client->CastToClient()->Admin() < RuleI(Vegas, MinStatusToBypassVegasLootLogging)) {
					std::string key = "TotalRandomDrops";
					std::string bucket_value = DataBucket::GetData(key);
					if (Strings::IsNumber(bucket_value) && Strings::ToUnsignedInt(bucket_value) > 0) {
						DataBucket::SetData(key, std::to_string(Strings::ToUnsignedInt(bucket_value) + 1));
					}
					else {
						DataBucket::SetData(key, std::to_string(1));
					}
				}

				//Discord send
				if (top_client->CastToClient()->Admin() < RuleI(Vegas, MinStatusToBypassVegasDiscordLogging)) {
					std::ostringstream output;
					output <<
						"Added [Bonus] [" << chosen_item->Name << "](http://vegaseq.com/Allaclone/?a=item&id=" << chosen_item->ID << ") [ID: " << chosen_item->ID << "] [GS:" << int(chosen_item->GearScore) << "] [" << (GetNPCTypeID(), IsRaidTarget() ? "Raid-Bonus" : IsRareSpawn() ? "Rare-Bonus" : "Common-Bonus") << "]"
						" to [" << GetCleanName() << "](http://vegaseq.com/Allaclone/?a=npc&id=" << GetNPCTypeID() << ") [ID: " << GetNPCTypeID() << "] [Level: " << int(GetLevel()) << "] [MobDiff: " << int(difficulty) << "]"
						" For [" << top_client->GetCleanName() << "](http://vegaseq.com/charbrowser/index.php?page=character&char=" << top_client->GetCleanName() << ") [ID: " << top_client->CastToClient()->CharacterID() << "] [Level: " << int(top_client->CastToClient()->GetLevel()) << "]"
						" in [" << zone->GetLongName() << "](http://vegaseq.com/Allaclone/?a=zone&name=" << zone->GetShortName() << ") [Instance: " << zone->GetInstanceID() << "]"
						;
					std::string out_final = output.str();
					zone->SendDiscordMessage("rngitems", out_final);
				}
				//Database Logging
				if (top_client->CastToClient()->Admin() < RuleI(Vegas, MinStatusToBypassVegasLootLogging)) {
					raid_target && !NPCBypassesVegasRaidLoot() ? vegas_loot_type = "Raid-Bonus" : rare_spawn ? vegas_loot_type = "Rare-Bonus" : vegas_loot_type = "Common-Bonus";
					item_name = chosen_item->Name;
					//std::replace(item_name.begin(), item_name.end(), "\`", "\'");
					query = StringFormat(
						"INSERT INTO rng_items "
						"SET `npc_id` = '%i', `npc_name` = '%s', `npc_level` = '%i', `type` = '%s', `npc_diff` = '%f', `item_id` = '%i', `raid_item` = '%i', `item` = '%s', `item_diff` = '%f', `item_gearscore` = '%f', `zone_id` = '%i', `zone` = '%s', `zone_instance_id` = '%i', `time` = NOW(), `charid` = '%i', `char` = '%s', `char_level` = '%i' ",
						GetNPCTypeID(), Strings::Escape(GetCleanName()).c_str(), GetLevel(), vegas_loot_type, double(difficulty), chosen_item->ID, raid_only, Strings::Escape(chosen_item->Name).c_str(), double(chosen_item->difficulty), double(chosen_item->GearScore), GetZoneID(), zone->GetShortName(), zone->GetInstanceID(), top_client->CastToClient()->CharacterID(), top_client->CastToClient()->GetCleanName(), top_client->CastToClient()->GetLevel());
					QueryVegasLoot(query, top_client->CastToClient()->GetCleanName(), GetCleanName());
				}
			}
		}
		--bonus_roll_count;
	}
	while (roll_count >= 1) {
		raid_only = false;
		rolled = zone->random.Int(1, std::min(roll_max, roll_to_hit));
		VegasLootDetail("Rolled a [{}]", rolled); //deleteme
		if (rolled >= std::min(roll_max, roll_to_hit) || GuaranteedNormalDrop()) {
			VegasLoot("Normal Roll hit on [{}] for [{}].", GetCleanName(), top_client->GetCleanName()); //deleteme
			const EQ::ItemData* chosen_item = GetVegasItems(id_min, id_max, difficulty_min, difficulty_max, zone_range_min, zone_range_max, raid_only);
			if (chosen_item) {
				AddItem(chosen_item, chosen_item->MaxCharges, RuleB(Vegas, EquipVegasDrops));
				VegasLoot("Added [{}] to [{}] for [{}]", database.CreateItemLink(chosen_item->ID), GetCleanName(), top_client->GetCleanName()); //deleteme

				if (top_client->CastToClient()->Admin() < RuleI(Vegas, MinStatusToBypassVegasLootLogging)) {
					std::string key = "TotalRandomDrops";
					std::string bucket_value = DataBucket::GetData(key);
					if (Strings::IsNumber(bucket_value) && Strings::ToUnsignedInt(bucket_value) > 0) {
						DataBucket::SetData(key, std::to_string(Strings::ToUnsignedInt(bucket_value) + 1));
					}
					else {
						DataBucket::SetData(key, std::to_string(1));
					}
				}

				//Discord send
				if (top_client->CastToClient()->Admin() < RuleI(Vegas, MinStatusToBypassVegasDiscordLogging)) {
					std::ostringstream output;
					output <<
						"Added [" << chosen_item->Name << "](http://vegaseq.com/Allaclone/?a=item&id=" << chosen_item->ID << ") [ID: " << chosen_item->ID << "] [GS:" << int(chosen_item->GearScore) << "] [" << (GetNPCTypeID(), IsRaidTarget() ? "Raid" : IsRareSpawn() ? "Rare" : "Common") << "]"
						" to [" << GetCleanName() << "](http://vegaseq.com/Allaclone/?a=npc&id=" << GetNPCTypeID() << ") [ID: " << GetNPCTypeID() << "] [Level: " << int(GetLevel()) << "] [MobDiff: " << int(difficulty) << "]"
						" For [" << top_client->GetCleanName() << "](http://vegaseq.com/charbrowser/index.php?page=character&char=" << top_client->GetCleanName() << ") [ID: " << top_client->CastToClient()->CharacterID() << "] [Level: " << int(top_client->CastToClient()->GetLevel()) << "]"
						" in [" << zone->GetLongName() << "](http://vegaseq.com/Allaclone/?a=zone&name=" << zone->GetShortName() << ") [Instance: " << zone->GetInstanceID() << "]"
						;
					std::string out_final = output.str();
					zone->SendDiscordMessage("rngitems", out_final);
				}
				//Database Logging
				if (top_client->CastToClient()->Admin() < RuleI(Vegas, MinStatusToBypassVegasLootLogging)) {
					raid_target && !NPCBypassesVegasRaidLoot() ? vegas_loot_type = "Raid" : rare_spawn ? vegas_loot_type = "Rare" : vegas_loot_type = "Common";
					item_name = chosen_item->Name;
					//std::replace(item_name.begin(), item_name.end(), "\`", "\'");
					query = StringFormat(
						"INSERT INTO rng_items "
						"SET `npc_id` = '%i', `npc_name` = '%s', `npc_level` = '%i', `type` = '%s', `npc_diff` = '%f', `item_id` = '%i', `raid_item` = '%i', `item` = '%s', `item_diff` = '%f', `item_gearscore` = '%f', `zone_id` = '%i', `zone` = '%s', `zone_instance_id` = '%i', `time` = NOW(), `charid` = '%i', `char` = '%s', `char_level` = '%i' ",
						GetNPCTypeID(), Strings::Escape(GetCleanName()).c_str(), GetLevel(), vegas_loot_type, double(difficulty), chosen_item->ID, raid_only, Strings::Escape(chosen_item->Name).c_str(), double(chosen_item->difficulty), double(chosen_item->GearScore), GetZoneID(), zone->GetShortName(), zone->GetInstanceID(), top_client->CastToClient()->CharacterID(), top_client->CastToClient()->GetCleanName(), top_client->CastToClient()->GetLevel());
					QueryVegasLoot(query, top_client->CastToClient()->GetCleanName(), GetCleanName());
				}
			}
		}
		--roll_count;
	}
	while (raid_roll_count >= 1) {
		raid_only = true;
		rolled = zone->random.Int(1, std::min(raid_roll_max, raid_roll_to_hit));
		VegasLootDetail("Rolled a [{}]", rolled); //deleteme
		if (rolled >= std::min(raid_roll_max, raid_roll_to_hit) || GuaranteedRaidDrop()) {
			VegasLoot("Raid Roll hit on [{}] for [{}].", GetCleanName(), top_client->GetCleanName()); //deleteme
			const EQ::ItemData* chosen_item = GetVegasItems(id_min, id_max, difficulty_min, difficulty_max, zone_range_min, zone_range_max, raid_only);
			if (chosen_item) {
				AddItem(chosen_item, chosen_item->MaxCharges, RuleB(Vegas, EquipVegasDrops));
				VegasLoot("Added [{}] to [{}] for [{}]", database.CreateItemLink(chosen_item->ID), GetCleanName(), top_client->GetCleanName()); //deleteme

				if (top_client->CastToClient()->Admin() < RuleI(Vegas, MinStatusToBypassVegasLootLogging)) {
					std::string key = "TotalRandomDrops";
					std::string bucket_value = DataBucket::GetData(key);
					if (Strings::IsNumber(bucket_value) && Strings::ToUnsignedInt(bucket_value) > 0) {
						DataBucket::SetData(key, std::to_string(Strings::ToUnsignedInt(bucket_value) + 1));
					}
					else {
						DataBucket::SetData(key, std::to_string(1));
					}
				}

				//Discord send
				if (top_client->CastToClient()->Admin() < RuleI(Vegas, MinStatusToBypassVegasDiscordLogging)) {
					std::ostringstream output;
					output <<
						"Added [Raid Item] [" << chosen_item->Name << "](http://vegaseq.com/Allaclone/?a=item&id=" << chosen_item->ID << ") [ID: " << chosen_item->ID << "] [GS:" << int(chosen_item->GearScore) << "] [" << (GetNPCTypeID(), IsRaidTarget() ? "Raid" : IsRareSpawn() ? "Rare" : "Common") << "]"
						" to [" << GetCleanName() << "](http://vegaseq.com/Allaclone/?a=npc&id=" << GetNPCTypeID() << ") [ID: " << GetNPCTypeID() << "] [Level: " << int(GetLevel()) << "] [MobDiff: " << int(difficulty) << "]"
						" For [" << top_client->GetCleanName() << "](http://vegaseq.com/charbrowser/index.php?page=character&char=" << top_client->GetCleanName() << ") [ID: " << top_client->CastToClient()->CharacterID() << "] [Level: " << int(top_client->CastToClient()->GetLevel()) << "]"
						" in [" << zone->GetLongName() << "](http://vegaseq.com/Allaclone/?a=zone&name=" << zone->GetShortName() << ") [Instance: " << zone->GetInstanceID() << "]"
						;
					std::string out_final = output.str();
					zone->SendDiscordMessage("rngitems", out_final);
				}
				//Database Logging
				if (top_client->CastToClient()->Admin() < RuleI(Vegas, MinStatusToBypassVegasLootLogging)) {
					vegas_loot_type = "Raid";
					item_name = chosen_item->Name;
					//std::replace(item_name.begin(), item_name.end(), "\`", "\'");
					query = StringFormat(
						"INSERT INTO rng_items "
						"SET `npc_id` = '%i', `npc_name` = '%s', `npc_level` = '%i', `type` = '%s', `npc_diff` = '%f', `item_id` = '%i', `raid_item` = '%i', `item` = '%s', `item_diff` = '%f', `item_gearscore` = '%f', `zone_id` = '%i', `zone` = '%s', `zone_instance_id` = '%i', `time` = NOW(), `charid` = '%i', `char` = '%s', `char_level` = '%i' ",
						GetNPCTypeID(), Strings::Escape(GetCleanName()).c_str(), GetLevel(), vegas_loot_type, double(difficulty), chosen_item->ID, raid_only, Strings::Escape(chosen_item->Name).c_str(), double(chosen_item->difficulty), double(chosen_item->GearScore), GetZoneID(), zone->GetShortName(), zone->GetInstanceID(), top_client->CastToClient()->CharacterID(), top_client->CastToClient()->GetCleanName(), top_client->CastToClient()->GetLevel());
					QueryVegasLoot(query, top_client->CastToClient()->GetCleanName(), GetCleanName());
				}
			}
		}
		--raid_roll_count;
	}

	AddVegasShardLoot(top_client, shard_multiplier, guaranteed_shard_drop);
	AddVegasSpellLoot(top_client);
	AddVegasBagLoot(top_client);
	AddVegasBridleLoot(top_client);

	if (GetLevel() >= RuleI(Vegas, MinLevelForEpicToken)) {
		AddVegasEpicTokenLoot(top_client);
	}

	VegasLootDetail("Vegas Loot Operation took [{}]", benchmark.elapsed());
}

const EQ::ItemData* NPC::GetVegasItems(uint32 id_min, uint32 id_max, float difficulty_min, float difficulty_max, uint32 zone_range_min, uint32 zone_range_max, bool raid_only) {
	std::vector<VegasItem> vitems;
	VegasItem vitem;
	const EQ::ItemData* item;

	for (int i = id_min; i <= id_max; i++) {
		item = database.GetItem(i);
		if (!item) {
			continue;
		}
		if (
			item->ID >= id_min && item->ID <= id_max
			&& item->difficulty >= difficulty_min && item->difficulty <= difficulty_max
			&& item->lowest_drop_npc_id >= zone_range_min && item->lowest_drop_npc_id <= zone_range_max
			&& (raid_only ? item->raid_only : !item->raid_only)
			) {
			vitem.id = item->ID;
			vitem.name = item->Name;
			vitem.difficulty = item->difficulty;
			vitem.maxcharges = item->MaxCharges;
			vitems.push_back(vitem);
		}
	}

	if (vitems.size() > 0) {
		VegasLootDetail("Found [{}] items matching the criteria.", vitems.size()); //deleteme
		const EQ::ItemData* chosen_item = database.GetItem(vitems[std::rand() % vitems.size()].id);
		VegasLootDetail("Item [{}]-[{}] was chosen.", chosen_item->Name, chosen_item->ID); //deleteme
		return chosen_item;
	}
	else {
		return 0;
		VegasLootDetail("Didn't find any items matching that criteria"); //deleteme
	}
}

void NPC::GetVegasZoneRange(uint16 zoneid, uint8* zone_era, uint32* zone_range_min, uint32* zone_range_max) {
	if ((zoneid < 78 || zoneid == 187 || zoneid == 407) && (zoneid != 28 && zoneid != 43 && zoneid != 53)) { //Classic
		*zone_era = 0;
		*zone_range_min = RuleI(Vegas, ClassicZoneNPCIDMin);
		*zone_range_max = RuleI(Vegas, ClassicZoneNPCIDMax);
	}
	else if (zoneid >= 78 && zoneid < 109) { //Kunark
		*zone_era = 1;
		*zone_range_min = RuleI(Vegas, KunarkZoneNPCIDMin);
		*zone_range_max = RuleI(Vegas, KunarkZoneNPCIDMax);
	}
	else if (zoneid >= 110 && zoneid < 130) { //Velious
		*zone_era = 2;
		*zone_range_min = RuleI(Vegas, VeliousZoneNPCIDMin);
		*zone_range_max = RuleI(Vegas, VeliousZoneNPCIDMax);
	}
	else if (zoneid >= 150 && zoneid <= 181) { //Luclin
		*zone_era = 3;
		*zone_range_min = RuleI(Vegas, LuclinZoneNPCIDMin);
		*zone_range_max = RuleI(Vegas, LuclinZoneNPCIDMax);
	}
	else if (zoneid >= 200 && zoneid < 224) { //PoP
		*zone_era = 4;
		*zone_range_min = RuleI(Vegas, PoPZoneNPCIDMin);
		*zone_range_max = RuleI(Vegas, PoPZoneNPCIDMax);
	}
	else if (SpecialZonePassesVegasLoot()) { //Special Zones
		if (zoneid == RuleI(Vegas, SpecialEventZoneOne)) {
			*zone_range_min = RuleI(Vegas, SpecialEventZoneOneIDMin);
			*zone_range_max = RuleI(Vegas, SpecialEventZoneOneIDMax);
		}
		else if (zoneid == RuleI(Vegas, SpecialEventZoneTwo)) {
			*zone_range_min = RuleI(Vegas, SpecialEventZoneTwoIDMin);
			*zone_range_max = RuleI(Vegas, SpecialEventZoneTwoIDMax);
		}
		else if (zoneid == RuleI(Vegas, SpecialEventZoneThree)) {
			*zone_range_min = RuleI(Vegas, SpecialEventZoneThreeIDMin);
			*zone_range_max = RuleI(Vegas, SpecialEventZoneThreeIDMax);
		}
	}
}

void NPC::GetVegasDifficultyRange(uint16 zoneid, float* difficulty_min, float* difficulty_max, float* difficulty_bonus_min, float* difficulty_bonus_max) {
	float npc_diff = difficulty;
	float diff_min;
	float diff_max;
	float diff_bonus_min;
	float diff_bonus_max;

	if (npc_diff < RuleR(Vegas, DifficultyFloor)) {
		diff_min = npc_diff * RuleR(Vegas, DifficultyFloorBonusMinMultiplier);
		diff_max = RuleR(Vegas, DifficultyFloorMax);
		diff_bonus_min = RuleR(Vegas, DifficultyFloorBonusMin);
		diff_bonus_max = RuleR(Vegas, DifficultyFloorBonusMax);
	}
	else if(IsRaidTarget() && !NPCBypassesVegasRaidLoot()) {
		diff_min = npc_diff * RuleR(Vegas, RaidMinDifficultyMultiplier);
		diff_max = npc_diff * RuleR(Vegas, RaidMaxDifficultyMultiplier);
		diff_bonus_min = npc_diff * RuleR(Vegas, RaidMinDifficultyBonusMultiplier);
		diff_bonus_max = npc_diff * RuleR(Vegas, RaidMaxDifficultyBonusMultiplier);
	}
	else if(IsRareSpawn()) {
		diff_min = npc_diff * RuleR(Vegas, RareMinDifficultyMultiplier);
		diff_max = npc_diff * RuleR(Vegas, RareMaxDifficultyMultiplier);
		diff_bonus_min = npc_diff * RuleR(Vegas, RareMinDifficultyBonusMultiplier);
		diff_bonus_max = npc_diff * RuleR(Vegas, RareMaxDifficultyBonusMultiplier);
	}
	else {
		diff_min = npc_diff * RuleR(Vegas, NormalMinDifficultyMultiplier);
		diff_max = npc_diff * RuleR(Vegas, NormalMaxDifficultyMultiplier);
		diff_bonus_min = npc_diff * RuleR(Vegas, NormalMinDifficultyBonusMultiplier);
		diff_bonus_max = npc_diff * RuleR(Vegas, NormalMaxDifficultyBonusMultiplier);
	}
	diff_min > RuleR(Vegas, MinDifficultyCap) ? *difficulty_min = RuleR(Vegas, MinDifficultyCap) : *difficulty_min = diff_min;
	diff_max > RuleR(Vegas, MaxDifficultyCap) ? *difficulty_max = RuleR(Vegas, MaxDifficultyCap) : *difficulty_max = diff_max;
	diff_bonus_min > RuleR(Vegas, MinBonusDifficultyCap) ? *difficulty_bonus_min = RuleR(Vegas, MinBonusDifficultyCap) : *difficulty_bonus_min = diff_bonus_min;
	diff_bonus_max > RuleR(Vegas, MaxBonusDifficultyCap) ? *difficulty_bonus_max = RuleR(Vegas, MaxBonusDifficultyCap) : *difficulty_bonus_max = diff_bonus_max;
}

bool NPC::IsVegasLootEligible(Mob* top_client) {
	bool check = false;

	if (!IsOnVegasCoolDown(top_client)) {
		int top_hate_char_id = top_client->CastToClient()->CharacterID();
		Raid* raid = entity_list.GetRaidByClient(top_client->CastToClient());
		Group* group = top_client->GetGroup();
		int level_diff = 999;

		if (top_hate_char_id) {
			raid ? level_diff = raid->GetHighestLevel() - GetLevel() : group ? level_diff = group->GetHighestLevel() - GetLevel() : level_diff = top_client->CastToClient()->GetLevel() - GetLevel();
			if (level_diff <= RuleI(Vegas, LevelDifferenceForVegasDrops)) {
				check = true;
			}
			else {
				VegasLootDetail("[{}] on [{}]:::Failed level checks [{}] - [{}] - [{} - {} = {}]", top_client->GetCleanName(), GetCleanName(), raid ? "has raid" : "no raid", group ? "has group" : "no group", raid ? raid->GetHighestLevel() : group ? group->GetHighestLevel() : top_client->CastToClient()->GetLevel(), GetLevel(), raid ? raid->GetHighestLevel() - GetLevel() : group ? group->GetHighestLevel() - GetLevel() : top_client->CastToClient()->GetLevel() - GetLevel());
				Emote("'s fortunes decay away.");
				return false;
			}
		}
	}

	if (SpecialNPCPassesVegasLoot()) {
		return true;
	}

	if (SpecialZonePassesVegasLoot()) {
		return true;
	}

	if (NPCBypassesVegasLoot()) {
		return false;
	}

	if (NPCBypassesVegasRaidLoot()) {
		return false;
	}

	if (difficulty <= RuleR(Vegas, MinDifficultyToTriggerVegasLoot)) {
		return false;
	}

	if (GetNPCTypeID() < RuleI(Vegas, MinNPCIDToTriggerVegasLoot) || GetNPCTypeID() > RuleI(Vegas, MaxNPCIDToTriggerVegasLoot)) {
		return false;
	}

	if (RuleB(Vegas, RequireLoottableForVegasLoot) && GetLoottableID() < 1) {
		return false;
	}

	return check;
}

bool NPC::IsOnVegasCoolDown(Mob* top_client) {
	if (top_client->CastToClient()->Admin() >= RuleI(Vegas, MinStatusToBypassVegasLootCooldown)) {
		return false;
	}

	uint32 nid = GetNPCTypeID();
	uint32 s2 = GetSpawnPointID();

	uint32 top_hate_character_id = top_client->CastToClient()->CharacterID();
	uint32 npc_instance = zone->GetInstanceID();
	uint64 respawn_time = GetRespawnTime();
	std::string key;
	std::string bucket_value;

	//if (respawn_time < RuleI(Vegas, QuickRespawnMaxToTriggerLockout)) {
		key = "" + std::to_string(top_hate_character_id) + "_" + std::to_string(s2) + "_" + std::to_string(nid) + "_" + std::to_string(npc_instance) + "_respawnrnghold";
		VegasLootDetail("Respawn Key Check [{}]", key); //deleteme
		bucket_value = DataBucket::GetData(key);
		if (Strings::IsNumber(bucket_value) && Strings::ToUnsignedInt(bucket_value) == 2) {
			VegasLoot("Failed respawnrnghold check for [{}]", GetCleanName()); //deleteme
			Emote("'s fortunes poof.");
			return true;
		}
		//else {
		//	DataBucket::SetData(key, "1", std::to_string(RuleI(Vegas, QuickRespawnVegasLootLockoutPeriod)));
		//}
	//}

	std::string zone_name = zone->GetShortName();
	key = "" + std::to_string(top_hate_character_id) + "_" + (zone_name) + "_" + std::to_string(nid) + "_" + std::to_string(npc_instance);
	VegasLootDetail("Respawn Key Check [{}]", key); //deleteme
	bucket_value = DataBucket::GetData(key);
	if (!bucket_value.empty()) {
		VegasLoot("Failed lockout check for [{}]", GetCleanName()); //deleteme
		Emote("'s fortunes poof.");
		return true;
	}
	return false;
}

bool NPC::NPCBypassesVegasLoot() {
	uint32 NPCIDChecks[] = { RuleI(Vegas, NPCNotCountAsVegasLootOne), RuleI(Vegas, NPCNotCountAsVegasLootTwo), RuleI(Vegas, NPCNotCountAsVegasLootThree), RuleI(Vegas, NPCNotCountAsVegasLootFour), RuleI(Vegas, NPCNotCountAsVegasLootFive) };
	for (int i : NPCIDChecks) {
		if (i == GetNPCTypeID()) {
			return true;
		}
	}
	return false;
}

bool NPC::GuaranteedNormalDrop() {
	uint32 NPCIDChecks[] = { RuleI(Vegas, GuaranteedNormalDropOne), RuleI(Vegas, GuaranteedNormalDropTwo), RuleI(Vegas, GuaranteedNormalDropThree), RuleI(Vegas, GuaranteedNormalDropFour), RuleI(Vegas, GuaranteedNormalDropFive) };
	for (int i : NPCIDChecks) {
		if (i == GetNPCTypeID()) {
			return true;
		}
	}
	return false;
}

bool NPC::GuaranteedBonusDrop() {
	uint32 NPCIDChecks[] = { RuleI(Vegas, GuaranteedBonusDropOne), RuleI(Vegas, GuaranteedBonusDropTwo), RuleI(Vegas, GuaranteedBonusDropThree), RuleI(Vegas, GuaranteedBonusDropFour), RuleI(Vegas, GuaranteedBonusDropFive) };
	for (int i : NPCIDChecks) {
		if (i == GetNPCTypeID()) {
			return true;
		}
	}
	return false;
}

bool NPC::GuaranteedRaidDrop() {
	uint32 NPCIDChecks[] = { RuleI(Vegas, GuaranteedRaidDropOne), RuleI(Vegas, GuaranteedRaidDropTwo), RuleI(Vegas, GuaranteedRaidDropThree), RuleI(Vegas, GuaranteedRaidDropFour), RuleI(Vegas, GuaranteedRaidDropFive) };
	for (int i : NPCIDChecks) {
		if (i == GetNPCTypeID()) {
			return true;
		}
	}
	return false;
}

bool NPC::NPCBypassesVegasRaidLoot() {
	uint32 NPCIDChecks[] = { RuleI(Vegas, RaidTargetNotCountAsRaidOne), RuleI(Vegas, RaidTargetNotCountAsRaidTwo), RuleI(Vegas, RaidTargetNotCountAsRaidThree), RuleI(Vegas, RaidTargetNotCountAsRaidFour), RuleI(Vegas, RaidTargetNotCountAsRaidFive) };
	for (int i : NPCIDChecks) {
		if (i == GetNPCTypeID()) {
			return true;
		}
	}
	return false;
}

bool NPC::SpecialNPCPassesVegasLoot() {
	uint32 NPCIDChecks[] = { RuleI(Vegas, NPCSpecialIDPassesLootCheckOne), RuleI(Vegas, NPCSpecialIDPassesLootCheckTwo), RuleI(Vegas, NPCSpecialIDPassesLootCheckThree), RuleI(Vegas, NPCSpecialIDPassesLootCheckFour), RuleI(Vegas, NPCSpecialIDPassesLootCheckFive) };
	for (int i : NPCIDChecks) {
		if (i == GetNPCTypeID()) {
			return true;
		}
	}
	return false;
}

bool NPC::SpecialZonePassesVegasLoot() {
	uint32 ZoneIDChecks[] = { RuleI(Vegas, SpecialEventZoneOne), RuleI(Vegas, SpecialEventZoneTwo), RuleI(Vegas, SpecialEventZoneThree) };
	for (int i : ZoneIDChecks) {
		if (i == zone->GetZoneID()) {
			return true;
		}
	}
	return false;
}

int NPC::GetRollCount(float npc_diff) {
	if (IsRaidTarget() && !NPCBypassesVegasRaidLoot()) {
		if (npc_diff < RuleR(Vegas, RaidTargetNormalRollMaxDifficulty)) {
			return RuleI(Vegas, RaidTargetNormalRollsCount);
		}
		else {
			return 0;
		}
	}
	else if (IsRareSpawn()) {
		return RuleI(Vegas, RareTargetRollsCount);
	}
	else {
		return RuleI(Vegas, NormalTargetRollsCount);
	}
	return 0;
}

int NPC::GetRaidRollCount(float npc_diff) {
	if (IsRaidTarget() && !NPCBypassesVegasRaidLoot()) {
		if (npc_diff < RuleR(Vegas, RaidTargetTierOneThreshold)) {
			return RuleI(Vegas, RaidTargetBaseRaidRollsCount);
		}
		else if (npc_diff >= RuleR(Vegas, RaidTargetTierThreeThreshold)) {
			return RuleI(Vegas, RaidTargetTierThreeRaidRollsCount);
		}
		else if (npc_diff >= RuleR(Vegas, RaidTargetTierTwoThreshold)) {
			return RuleI(Vegas, RaidTargetTierTwoRaidRollsCount);
		}
		else if (npc_diff >= RuleR(Vegas, RaidTargetTierOneThreshold)) {
			return RuleI(Vegas, RaidTargetTierOneRaidRollsCount);
		}
	}
	return 0;
}

int NPC::GetRollMax() {
	if (IsRaidTarget() && !NPCBypassesVegasRaidLoot()) {
		return RuleI(Vegas, RaidVegasRollUpperLimit);
	}
	else if (IsRareSpawn()) {
		return RuleI(Vegas, RareVegasRollUpperLimit);
	}
	else {
		return RuleI(Vegas, NormalVegasRollUpperLimit);
	}
	return 0;
}

int NPC::GetRollToHit(auto roll_max, auto random_drop_multiplier) {
	if (IsRaidTarget() && !NPCBypassesVegasRaidLoot()) {
		return int((roll_max * RuleR(Vegas, RaidTargetRollMultiplier) / random_drop_multiplier));
	}
	else if (IsRareSpawn()) {
		return int((roll_max * RuleR(Vegas, RareTargetRollMultiplier) / random_drop_multiplier));
	}
	else {
		return int((roll_max * RuleR(Vegas, NormalTargetRollMultiplier) / random_drop_multiplier));
	}
	return 0;
}

int NPC::GetBonusRollCount(float npc_diff) {
	if (IsRaidTarget() && !NPCBypassesVegasRaidLoot()) {
		return RuleI(Vegas, RaidBonusVegasRollCount);
	}
	else if (IsRareSpawn()) {
		return RuleI(Vegas, RareBonusVegasRollCount);
	}
	else {
		return RuleI(Vegas, NormalBonusVegasRollCount);
	}
	return 0;
}

int NPC::GetBonusRollMax() {
	if (IsRaidTarget() && !NPCBypassesVegasRaidLoot()) {
		return RuleI(Vegas, RaidBonusVegasRollUpperLimit);
	}
	else if (IsRareSpawn()) {
		return RuleI(Vegas, RareBonusVegasRollUpperLimit);
	}
	else {
		return RuleI(Vegas, NormalBonusVegasRollUpperLimit);
	}

	return 0;
}

int NPC::GetBonusRollToHit(auto roll_max, auto random_drop_multiplier) {
	if (IsRaidTarget() && !NPCBypassesVegasRaidLoot()) {
		return int((roll_max / RuleR(Vegas, RaidTargetBonusRollMultiplier)) / random_drop_multiplier);
	}
	else if (IsRareSpawn()) {
		return int((roll_max / RuleR(Vegas, RareTargetBonusRollMultiplier)) / random_drop_multiplier);
	}
	else {
		return int((roll_max / RuleR(Vegas, NormalTargetBonusRollMultiplier)) / random_drop_multiplier);
	}
	return 0;
}

int NPC::GetRaidRollMax() {
	if (IsRaidTarget() && !NPCBypassesVegasRaidLoot()) {
		return RuleI(Vegas, RaidTargetRaidVegasRollUpperLimit);
	}
	else if (IsRareSpawn()) {
		return RuleI(Vegas, RareTargetRaidVegasRollUpperLimit);
	}
	else {
		return RuleI(Vegas, NormalTargetRaidVegasRollUpperLimit);
	}
	return 0;
}

int NPC::GetRaidRollToHit(auto roll_max, auto random_drop_multiplier) {
	if (IsRaidTarget() && !NPCBypassesVegasRaidLoot()) {
		return int((roll_max / RuleR(Vegas, RaidTargetRaidRollMultiplier)) / random_drop_multiplier);
	}
	else if (IsRareSpawn()) {
		return int((roll_max / RuleR(Vegas, RareTargetRaidRollMultiplier)) / random_drop_multiplier);
	}
	else {
		return int((roll_max / RuleR(Vegas, NormalTargetRaidRollMultiplier)) / random_drop_multiplier);
	}
	return 0;
}

void NPC::AddVegasShardLoot(Mob* top_client, float shard_multiplier, bool guaranteed_shard_drop) {
	if (guaranteed_shard_drop || (zone->random.Int(1, RuleI(Vegas, ShardRollUpperLimit)) >= RuleI(Vegas, ShardRollUpperLimit))) {
		uint32 npc_id = GetNPCTypeID();
		int shard_amount = GetShardAmount();
		uint8 shard_add_counter = 0;
		float npc_diff = GetDifficulty();
		std::string query;
		std::string npc_name = GetCleanName();
		std::string vegas_loot_type;

		shard_amount *= shard_multiplier;

		if (!shard_amount) {
			return;
		}

		if (zone->IsHotzone()) {
			shard_amount *= RuleR(Vegas, ShardHotZoneMultiplier);
		}

		if (guaranteed_shard_drop) {
			raid_target && !NPCBypassesVegasRaidLoot() ? vegas_loot_type = "Raid-Bonus" : rare_spawn ? vegas_loot_type = "Rare-Bonus" : vegas_loot_type = "Common-Bonus";
			if (zone->IsHotzone()) {
				raid_target && !NPCBypassesVegasRaidLoot() ? shard_amount = EQ::Clamp(shard_amount, 1, int((RuleI(Vegas, RaidTargetBonusShardDropCap) * RuleR(Vegas, ShardHotZoneCapMultiplier)))) : rare_spawn ? shard_amount = EQ::Clamp(shard_amount, 1, int((RuleI(Vegas, RareTargetBonusShardDropCap) * RuleR(Vegas, ShardHotZoneCapMultiplier)))) : shard_amount = EQ::Clamp(shard_amount, 1, int((RuleI(Vegas, NormalTargetBonusShardDropCap) * RuleR(Vegas, ShardHotZoneCapMultiplier))));
			}
			else {
				raid_target && !NPCBypassesVegasRaidLoot() ? shard_amount = EQ::Clamp(shard_amount, 1, RuleI(Vegas, RaidTargetBonusShardDropCap)) : rare_spawn ? shard_amount = EQ::Clamp(shard_amount, 1, RuleI(Vegas, RareTargetBonusShardDropCap)) : shard_amount = EQ::Clamp(shard_amount, 1, RuleI(Vegas, NormalTargetBonusShardDropCap));
			}
		}
		else {
			raid_target && !NPCBypassesVegasRaidLoot() ? vegas_loot_type = "Raid" : rare_spawn ? vegas_loot_type = "Rare" : vegas_loot_type = "Common";
			if (zone->IsHotzone()) {
				raid_target && !NPCBypassesVegasRaidLoot() ? shard_amount = EQ::Clamp(shard_amount, 1, int((RuleI(Vegas, RaidTargetShardDropCap) * RuleR(Vegas, ShardHotZoneCapMultiplier)))) : rare_spawn ? shard_amount = EQ::Clamp(shard_amount, 1, int((RuleI(Vegas, RareTargetShardDropCap) * RuleR(Vegas, ShardHotZoneCapMultiplier)))) : shard_amount = EQ::Clamp(shard_amount, 1, int((RuleI(Vegas, NormalTargetShardDropCap) * RuleR(Vegas, ShardHotZoneCapMultiplier))));
			}
			else {
				raid_target && !NPCBypassesVegasRaidLoot() ? shard_amount = EQ::Clamp(shard_amount, 1, RuleI(Vegas, RaidTargetShardDropCap)) : rare_spawn ? shard_amount = EQ::Clamp(shard_amount, 1, RuleI(Vegas, RareTargetShardDropCap)) : shard_amount = EQ::Clamp(shard_amount, 1, RuleI(Vegas, NormalTargetShardDropCap));
			}
		}

		shard_amount = int(shard_amount);
		
		if (shard_amount >= 1000) {
			shard_add_counter = int(shard_amount / 1000);
		}

		if (top_client->CastToClient()->Admin() < RuleI(Vegas, MinStatusToBypassVegasLootLogging)) {
			std::string key = "ShardsDropped";
			std::string bucket_value = DataBucket::GetData(key);
			if (Strings::IsNumber(bucket_value) && Strings::ToUnsignedInt(bucket_value) > 0) {
				DataBucket::SetData(key, std::to_string(Strings::ToUnsignedInt(bucket_value) + shard_amount));
			}
			else {
				DataBucket::SetData(key, std::to_string(1));
			}
		}
		//Discord send
		if (top_client->CastToClient()->Admin() < RuleI(Vegas, MinStatusToBypassVegasDiscordLogging)) {
			std::ostringstream output;
			output <<
				"Added " << shard_amount << " [Shard](http://vegaseq.com/allaclone/?a=item&id=" << RuleI(Vegas, ShardItemID) << ")"
				" to [" << GetCleanName() << "](http://vegaseq.com/Allaclone/?a=npc&id=" << GetNPCTypeID() << ") [ID: " << GetNPCTypeID() << "] [Level: " << int(GetLevel()) << "] [" << vegas_loot_type << "] [MobDiff: " << int(difficulty) << "]"
				" For [" << top_client->GetCleanName() << "](http://vegaseq.com/charbrowser/index.php?page=character&char=" << top_client->GetCleanName() << ") [ID: " << top_client->CastToClient()->CharacterID() << "] [Level: " << int(top_client->CastToClient()->GetLevel()) << "]"
				" in [" << zone->GetLongName() << "](http://vegaseq.com/Allaclone/?a=zone&name=" << zone->GetShortName() << ") [Instance: " << zone->GetInstanceID() << "]"
				;
			std::string out_final = output.str();
			zone->SendDiscordMessage("rngshards", out_final);
		}
		//Database Logging
		if (top_client->CastToClient()->Admin() < RuleI(Vegas, MinStatusToBypassVegasLootLogging)) {
			query = StringFormat(
				"INSERT INTO rng_shards "
				"SET `npc_id` = '%i', `npc_name` = '%s', `npc_level` = '%i', `type` = '%s', `npc_diff` = '%f', `shard_amount` = '%i', `zone_id` = '%i', `zone` = '%s', `zone_instance_id` = '%i', `time` = NOW(), `charid` = '%i', `char` = '%s', `char_level` = '%i' ",
				GetNPCTypeID(), Strings::Escape(GetCleanName()).c_str(), GetLevel(), vegas_loot_type, double(difficulty), shard_amount, zone->GetZoneID(), zone->GetShortName(), zone->GetInstanceID(), top_client->CastToClient()->CharacterID(), top_client->CastToClient()->GetCleanName(), top_client->CastToClient()->GetLevel());
			QueryVegasLoot(query, top_client->CastToClient()->GetCleanName(), GetCleanName());
		}

		if (shard_amount < 1000) {
			AddItem(RuleI(Vegas, ShardItemID), shard_amount, RuleB(Vegas, EquipVegasDrops));
			VegasLoot("Added [{}x] [{}] to [{}] for [{}]", shard_amount, database.CreateItemLink(RuleI(Vegas, ShardItemID)), GetCleanName(), top_client->GetCleanName()); //deleteme
		}
		else {
			while (shard_add_counter >= 1) {
				if (shard_amount >= 1000) {
					AddItem(RuleI(Vegas, ShardItemID), 1000, RuleB(Vegas, EquipVegasDrops));
					VegasLoot("Added [1000]x [{}] to [{}] for [{}]", database.CreateItemLink(RuleI(Vegas, ShardItemID)), GetCleanName(), top_client->GetCleanName()); //deleteme
					shard_amount = shard_amount - 1000;
				}
				if (shard_amount < 1000) {
					AddItem(RuleI(Vegas, ShardItemID), shard_amount, RuleB(Vegas, EquipVegasDrops));
					VegasLoot("Added [{}x] [{}] to [{}] for [{}]", shard_amount, database.CreateItemLink(RuleI(Vegas, ShardItemID)), GetCleanName(), top_client->GetCleanName()); //deleteme
				}
				--shard_add_counter;
			}
		}
	}
}

int NPC::GetShardAmount() {
	int shard_amount;
	uint8 shard_add_counter = 1;
	float npc_diff = GetDifficulty();

	if ((npc_diff / sqrt(npc_diff)) <= 75) {
		shard_amount = npc_diff / sqrt(npc_diff);
	}
	else {
		shard_amount = (((npc_diff / sqrt(npc_diff)) / 10) + 75);
	}
	shard_amount *= RuleR(Vegas, ShardMultiplier);

	return shard_amount;
}

void NPC::AddVegasEpicTokenLoot(Mob* top_client) {
	int roll_count;
	int roll_max = RuleI(Vegas, EpicTokenRollMax);
	int roll_to_hit = int(roll_max / RuleR(Vegas, TokenMultiplier));
	std::string query;
	std::string vegas_loot_type;
	std::string npc_name = GetCleanName();
	//std::replace(npc_name.begin(), npc_name.end(), "\`", "\'");

	if (IsRaidTarget() && !NPCBypassesVegasRaidLoot()) {
		roll_to_hit = int(roll_to_hit - (GetShardAmount() * RuleR(Vegas, TokenMultiplier)));
		roll_count = RuleI(Vegas, RaidTargetEpicTokenRollCount);
	}
	else {
		if (IsRareSpawn()) {
			roll_count = RuleI(Vegas, RareTargetEpicTokenRollCount);
		}
		else {
			roll_count = RuleI(Vegas, NormalTargetEpicTokenRollCount);
		}
	}

	int rolled = zone->random.Int(1, roll_max);
	int to_hit = std::min(roll_max, roll_to_hit);
	while (roll_count >= 1) {
		VegasLoot("[Tries left: {}] Roll was [{}] To Hit [{}] on [{}] [ID: {}] For [{}] [ID: {}]", (roll_count - 1), rolled, to_hit, GetCleanName(), GetNPCTypeID(), top_client->GetCleanName(), top_client->CastToClient()->CharacterID()); //deleteme
		if (rolled >= to_hit) {
			AddItem(RuleI(Vegas, EpicTokenItemID), 1, false);
			VegasLoot("Epic Token hit! Added an [{}] to [{}] [ID: {}] For [{}] [ID: {}] Roll was [{}] To Hit [{}]", database.CreateItemLink(RuleI(Vegas, EpicTokenItemID)), GetCleanName(), GetNPCTypeID(), top_client->GetCleanName(), top_client->CastToClient()->CharacterID(), rolled, to_hit); //deleteme

			if (top_client->CastToClient()->Admin() < RuleI(Vegas, MinStatusToBypassVegasLootLogging)) {
				std::string key = "EpicTokenCount";
				std::string bucket_value = DataBucket::GetData(key);
				if (Strings::IsNumber(bucket_value) && Strings::ToUnsignedInt(bucket_value) > 0) {
					DataBucket::SetData(key, std::to_string(Strings::ToUnsignedInt(bucket_value) + 1));
				}
				else {
					DataBucket::SetData(key, std::to_string(1));
				}
			}

			//Discord send
			if (top_client->CastToClient()->Admin() < RuleI(Vegas, MinStatusToBypassVegasDiscordLogging)) {
				std::ostringstream output;
				output <<
					"[" << (IsRaidTarget() ? "Raid Target" : "Non-Raid") << "] Epic Token WIN on [" << GetCleanName() << "](http://vegaseq.com/Allaclone/?a=npc&id=" << GetNPCTypeID() << ") [ID: " << GetNPCTypeID() << "] [Level: " << int(GetLevel()) << "] MobDiff: [" << int(difficulty) << "]"
					" For [" << top_client->GetCleanName() << "](http://vegaseq.com/charbrowser/index.php?page=character&char=" << top_client->GetCleanName() << ") [ID: " << top_client->CastToClient()->CharacterID() << "] [Level: " << int(top_client->CastToClient()->GetLevel()) << "]"
					" in [" << zone->GetLongName() << "](http://vegaseq.com/Allaclone/?a=zone&name=" << zone->GetShortName() << ") [Instance: " << zone->GetInstanceID() << "]"
					" Roll was [" << rolled << "] To Hit [" << to_hit << "] [Tries left : " << (roll_count - 1) << "]"
					;
				std::string out_final = output.str();
				zone->SendDiscordMessage("rngtokens", out_final);
			}
			//Database Logging
			if (top_client->CastToClient()->Admin() < RuleI(Vegas, MinStatusToBypassVegasLootLogging)) {
				raid_target && !NPCBypassesVegasRaidLoot() ? vegas_loot_type = "Raid" : vegas_loot_type = "Non-Raid";
				query = StringFormat(
					"INSERT INTO rng_tokens "
					"SET `npc_id` = '%i', `npc_name` = '%s', `npc_level` = '%i', `type` = '%s', `npc_diff` = '%f', `roll` = '%i', `tohit` = '%i', `zone_id` = '%i', `zone` = '%s', `zone_instance_id` = '%i', `time` = NOW(), `charid` = '%i', `char` = '%s', `char_level` = '%i' ",
					GetNPCTypeID(), Strings::Escape(GetCleanName()).c_str(), GetLevel(), vegas_loot_type, double(difficulty), rolled, to_hit, zone->GetZoneID(), zone->GetShortName(), zone->GetInstanceID(), top_client->CastToClient()->CharacterID(), top_client->CastToClient()->GetCleanName(), top_client->CastToClient()->GetLevel());
				QueryVegasLoot(query, top_client->CastToClient()->GetCleanName(), GetCleanName());
			}
		}
		else {
			if (IsRaidTarget() && !NPCBypassesVegasRaidLoot()) {
				//Discord send
				if (top_client->CastToClient()->Admin() < RuleI(Vegas, MinStatusToBypassVegasDiscordLogging)) {
					std::ostringstream output;
					output <<
						"[Tries left: " << (roll_count - 1) << "] Roll was [" << rolled << "] To Hit [" << to_hit << "]"
						" on [" << GetCleanName() << "](http://vegaseq.com/Allaclone/?a=npc&id=" << GetNPCTypeID() << ") [ID: " << GetNPCTypeID() << "] [Level: " << int(GetLevel()) << "] MobDiff: [" << int(difficulty) << "]"
						" For [" << top_client->GetCleanName() << "](http://vegaseq.com/charbrowser/index.php?page=character&char=" << top_client->GetCleanName() << ") [ID: " << top_client->CastToClient()->CharacterID() << "] [Level: " << int(top_client->CastToClient()->GetLevel()) << "]"
						" in [" << zone->GetLongName() << "](http://vegaseq.com/Allaclone/?a=zone&name=" << zone->GetShortName() << ") [Instance: " << zone->GetInstanceID() << "]"
						;
					std::string out_final = output.str();
					zone->SendDiscordMessage("rngtokens", out_final);
				}
			}
		}
		--roll_count;
	}
}

void NPC::AddVegasSpellLoot(Mob* top_client) {

	int LootdropID[] = { RuleI(Vegas, VegasSpellTierOneLootDropID), RuleI(Vegas, VegasSpellTierTwoLootDropID), RuleI(Vegas, VegasSpellTierThreeLootDropID), RuleI(Vegas, VegasSpellTierFourLootDropID), RuleI(Vegas, VegasSpellTierFiveLootDropID), RuleI(Vegas, VegasSpellTierSixLootDropID), RuleI(Vegas, VegasSpellTierSevenLootDropID), RuleI(Vegas, VegasSpellTierEightLootDropID) };
	int MinLevel[] = { RuleI(Vegas, VegasSpellTierOneMinLevel), RuleI(Vegas, VegasSpellTierTwoMinLevel), RuleI(Vegas, VegasSpellTierThreeMinLevel), RuleI(Vegas, VegasSpellTierFourMinLevel), RuleI(Vegas, VegasSpellTierFiveMinLevel), RuleI(Vegas, VegasSpellTierSixMinLevel), RuleI(Vegas, VegasSpellTierSevenMinLevel), RuleI(Vegas, VegasSpellTierEightMinLevel) };
	int MaxLevel[] = { RuleI(Vegas, VegasSpellTierOneMaxLevel), RuleI(Vegas, VegasSpellTierTwoMaxLevel), RuleI(Vegas, VegasSpellTierThreeMaxLevel), RuleI(Vegas, VegasSpellTierFourMaxLevel), RuleI(Vegas, VegasSpellTierFiveMaxLevel), RuleI(Vegas, VegasSpellTierSixMaxLevel), RuleI(Vegas, VegasSpellTierSevenMaxLevel), RuleI(Vegas, VegasSpellTierEightMaxLevel) };
	int BonusLowLootdropID[] = { RuleI(Vegas, VegasSpellTierOneBonusLowLootDropID), RuleI(Vegas, VegasSpellTierTwoBonusLowLootDropID), RuleI(Vegas, VegasSpellTierThreeBonusLowLootDropID), RuleI(Vegas, VegasSpellTierFourBonusLowLootDropID), RuleI(Vegas, VegasSpellTierFiveBonusLowLootDropID), RuleI(Vegas, VegasSpellTierSixBonusLowLootDropID), RuleI(Vegas, VegasSpellTierSevenBonusLowLootDropID), RuleI(Vegas, VegasSpellTierEightBonusLowLootDropID) };
	int BonusHighLootdropID[] = { RuleI(Vegas, VegasSpellTierOneBonusHighLootDropID), RuleI(Vegas, VegasSpellTierTwoBonusHighLootDropID), RuleI(Vegas, VegasSpellTierThreeBonusHighLootDropID), RuleI(Vegas, VegasSpellTierFourBonusHighLootDropID), RuleI(Vegas, VegasSpellTierFiveBonusHighLootDropID), RuleI(Vegas, VegasSpellTierSixBonusHighLootDropID), RuleI(Vegas, VegasSpellTierSevenBonusHighLootDropID), RuleI(Vegas, VegasSpellTierEightBonusHighLootDropID) };
	int roll_count = RuleI(Vegas, VegasSpellRollCount);
	int roll_max = int(RuleI(Vegas, VegasSpellRollMax) * RuleR(Vegas, SpellMultiplier));
	int rolled;
	int chosen_type[sizeof(LootdropID) / sizeof(int)];
	std::string vegas_loot_type;

	while (roll_count >= 1) {
		rolled = zone->random.Int(1, roll_max);
		if (rolled >= RuleI(Vegas, VegasSpellRollMaxHit)) {
			VegasLoot("Max Spell Roll hit on [{}] for [{}]. Roll [{}] | To Hit [{}].", GetCleanName(), top_client->GetCleanName(), rolled, RuleI(Vegas, VegasSpellRollMaxHit)); //deleteme
			raid_target && !NPCBypassesVegasRaidLoot() ? vegas_loot_type = "Raid-Max" : rare_spawn ? vegas_loot_type = "Rare-Max" : vegas_loot_type = "Common-Max";
			std::copy(std::begin(BonusHighLootdropID), std::end(BonusHighLootdropID), std::begin(chosen_type));
		}
		else if (rolled >= RuleI(Vegas, VegasSpellRollNormalHit)) {
			VegasLoot("Normal Spell Roll hit on [{}] for [{}]. Roll [{}] | To Hit [{}].", GetCleanName(), top_client->GetCleanName(), rolled, RuleI(Vegas, VegasSpellRollNormalHit)); //deleteme
			raid_target && !NPCBypassesVegasRaidLoot() ? vegas_loot_type = "Raid-Normal" : rare_spawn ? vegas_loot_type = "Rare-Normal" : vegas_loot_type = "Common-Normal";
			std::copy(std::begin(LootdropID), std::end(LootdropID), std::begin(chosen_type));
		}
		else if (rolled >= RuleI(Vegas, VegasSpellRollMinHit)) {
			VegasLoot("Min Spell Roll hit on [{}] for [{}]. Roll [{}] | To Hit [{}].", GetCleanName(), top_client->GetCleanName(), rolled, RuleI(Vegas, VegasSpellRollMinHit)); //deleteme
			raid_target && !NPCBypassesVegasRaidLoot() ? vegas_loot_type = "Raid-Min" : rare_spawn ? vegas_loot_type = "Rare-Min" : vegas_loot_type = "Common-Min";
			std::copy(std::begin(BonusLowLootdropID), std::end(BonusLowLootdropID), std::begin(chosen_type));
		}
		else {
			--roll_count;
			continue;
		}
		for (int i = 0; i < sizeof(chosen_type) / sizeof(int); i++) {
			if (chosen_type[i] > 0) {
				if (MinLevel[i] > 0 && MaxLevel[i] > 0 && GetLevel() >= MinLevel[i] && GetLevel() <= MaxLevel[i]) {
					const LootDrop_Struct* chosen_loot_drop = nullptr;
					const EQ::ItemData* chosen_item = nullptr;
					chosen_loot_drop = database.GetLootDrop(chosen_type[i]);
					if (!chosen_loot_drop) {
						VegasLoot("No spell loot drop found on [{}] for [{}].", GetCleanName(), top_client->GetCleanName()); //deleteme
						break;
					}
					chosen_item = database.GetItem(chosen_loot_drop->Entries[zone->random.Int(0, chosen_loot_drop->NumEntries)].item_id);
					if (!chosen_item) {
						VegasLoot("No spell item found on [{}] for [{}].", GetCleanName(), top_client->GetCleanName()); //deleteme
						break;
					}
					AddItem(chosen_item->ID, 1, RuleB(Vegas, EquipVegasDrops));
					VegasLoot("Added spell [{}] to [{}] for [{}].", chosen_item->Name, GetCleanName(), top_client->GetCleanName()); //deleteme
					//Discord send
					if (top_client->CastToClient()->Admin() < RuleI(Vegas, MinStatusToBypassVegasDiscordLogging)) {
						std::ostringstream output;
						output <<
							"Added [" << vegas_loot_type  << "] [" << chosen_item->Name << "](http://vegaseq.com/Allaclone/?a=item&id=" << chosen_item->ID << ") [ID: " << chosen_item->ID << "] [" << (GetNPCTypeID(), IsRaidTarget() ? "Raid" : IsRareSpawn() ? "Rare" : "Common") << "]"
							" to [" << GetCleanName() << "](http://vegaseq.com/Allaclone/?a=npc&id=" << GetNPCTypeID() << ") [ID: " << GetNPCTypeID() << "] [Level: " << int(GetLevel()) << "] [MobDiff: " << int(difficulty) << "]"
							" For [" << top_client->GetCleanName() << "](http://vegaseq.com/charbrowser/index.php?page=character&char=" << top_client->GetCleanName() << ") [ID: " << top_client->CastToClient()->CharacterID() << "] [Level: " << int(top_client->CastToClient()->GetLevel()) << "]"
							" in [" << zone->GetLongName() << "](http://vegaseq.com/Allaclone/?a=zone&name=" << zone->GetShortName() << ") [Instance: " << zone->GetInstanceID() << "]"
							;
						std::string out_final = output.str();
						zone->SendDiscordMessage("rngspells", out_final);
					}
					//Database Logging
					if (top_client->CastToClient()->Admin() < RuleI(Vegas, MinStatusToBypassVegasLootLogging)) {
						std::string query;
						std::string item_name;
						item_name = chosen_item->Name;
						//std::replace(item_name.begin(), item_name.end(), "\`", "\'");
						query = StringFormat(
							"INSERT INTO rng_spells "
							"SET `npc_id` = '%i', `npc_name` = '%s', `npc_level` = '%i', `type` = '%s', `npc_diff` = '%f', `item_id` = '%i', `raid_item` = '%i', `item` = '%s', `item_diff` = '%f', `zone_id` = '%i', `zone` = '%s', `zone_instance_id` = '%i', `time` = NOW(), `charid` = '%i', `char` = '%s', `char_level` = '%i' ",
							GetNPCTypeID(), Strings::Escape(GetCleanName()).c_str(), GetLevel(), vegas_loot_type, double(difficulty), chosen_item->ID, 0, Strings::Escape(chosen_item->Name).c_str(), double(chosen_item->difficulty), GetZoneID(), zone->GetShortName(), zone->GetInstanceID(), top_client->CastToClient()->CharacterID(), top_client->CastToClient()->GetCleanName(), top_client->CastToClient()->GetLevel());
						QueryVegasLoot(query, top_client->CastToClient()->GetCleanName(), GetCleanName());
					}
					break;
				}
			}
		}
		--roll_count;
	}
}

void NPC::AddVegasBagLoot(Mob* top_client) {

	int LootdropID[] = { RuleI(Vegas, VegasBagTierOneLootDropID), RuleI(Vegas, VegasBagTierTwoLootDropID), RuleI(Vegas, VegasBagTierThreeLootDropID), RuleI(Vegas, VegasBagTierFourLootDropID), RuleI(Vegas, VegasBagTierFiveLootDropID), RuleI(Vegas, VegasBagTierSixLootDropID), RuleI(Vegas, VegasBagTierSevenLootDropID) };
	int MinLevel[] = { RuleI(Vegas, VegasBagTierOneMinLevel), RuleI(Vegas, VegasBagTierTwoMinLevel), RuleI(Vegas, VegasBagTierThreeMinLevel), RuleI(Vegas, VegasBagTierFourMinLevel), RuleI(Vegas, VegasBagTierFiveMinLevel), RuleI(Vegas, VegasBagTierSixMinLevel), RuleI(Vegas, VegasBagTierSevenMinLevel) };
	int MaxLevel[] = { RuleI(Vegas, VegasBagTierOneMaxLevel), RuleI(Vegas, VegasBagTierTwoMaxLevel), RuleI(Vegas, VegasBagTierThreeMaxLevel), RuleI(Vegas, VegasBagTierFourMaxLevel), RuleI(Vegas, VegasBagTierFiveMaxLevel), RuleI(Vegas, VegasBagTierSixMaxLevel), RuleI(Vegas, VegasBagTierSevenMaxLevel) };
	int roll_count = RuleI(Vegas, VegasBagRollCount);
	int roll_max = int(RuleI(Vegas, VegasBagRollMax) * RuleR(Vegas, BagMultiplier));
	int rolled;
	std::string vegas_loot_type;
	raid_target && !NPCBypassesVegasRaidLoot() ? vegas_loot_type = "Raid-Bag" : rare_spawn ? vegas_loot_type = "Rare-Bag" : vegas_loot_type = "Common-Bag";

	while (roll_count >= 1) {
		rolled = zone->random.Int(1, roll_max);
		if (rolled >= roll_max) {
			for (int i = 0; i < sizeof(LootdropID) / sizeof(int); i++) {
				if (LootdropID[i] > 0) {
					if (MinLevel[i] > 0 && MaxLevel[i] > 0 && GetLevel() >= MinLevel[i] && GetLevel() <= MaxLevel[i]) {
						const LootDrop_Struct* chosen_loot_drop = nullptr;
						const EQ::ItemData* chosen_item = nullptr;
						VegasLoot("Bag Roll hit on [{}] for [{}]. Roll [{}] | To Hit [{}].", GetCleanName(), top_client->GetCleanName(), rolled, roll_max); //deleteme
						chosen_loot_drop = database.GetLootDrop(LootdropID[i]);
						if (!chosen_loot_drop) {
							VegasLoot("No bag loot drop found on [{}] for [{}].", GetCleanName(), top_client->GetCleanName()); //deleteme
							continue;
						}
						VegasLoot("Bag loot using lootdrop [{}] on [{}] for [{}].", LootdropID[i], GetCleanName(), top_client->GetCleanName()); //deleteme
						chosen_item = database.GetItem(chosen_loot_drop->Entries[zone->random.Int(0, chosen_loot_drop->NumEntries)].item_id);
						if (!chosen_item) {
							VegasLoot("No bag item found on [{}] for [{}].", GetCleanName(), top_client->GetCleanName()); //deleteme
							continue;
						}
						AddItem(chosen_item->ID, 1, RuleB(Vegas, EquipVegasDrops));
						VegasLoot("Added bag [{}] to [{}] for [{}].", chosen_item->Name, GetCleanName(), top_client->GetCleanName()); //deleteme
						//Discord send
						if (top_client->CastToClient()->Admin() < RuleI(Vegas, MinStatusToBypassVegasDiscordLogging)) {
							std::ostringstream output;
							output <<
								"Added [" << chosen_item->Name << "](http://vegaseq.com/Allaclone/?a=item&id=" << chosen_item->ID << ") [ID: " << chosen_item->ID << "] [" << (GetNPCTypeID(), IsRaidTarget() ? "Raid" : IsRareSpawn() ? "Rare" : "Common") << "]"
								" to [" << GetCleanName() << "](http://vegaseq.com/Allaclone/?a=npc&id=" << GetNPCTypeID() << ") [ID: " << GetNPCTypeID() << "] [Level: " << int(GetLevel()) << "] [MobDiff: " << int(difficulty) << "]"
								" For [" << top_client->GetCleanName() << "](http://vegaseq.com/charbrowser/index.php?page=character&char=" << top_client->GetCleanName() << ") [ID: " << top_client->CastToClient()->CharacterID() << "] [Level: " << int(top_client->CastToClient()->GetLevel()) << "]"
								" in [" << zone->GetLongName() << "](http://vegaseq.com/Allaclone/?a=zone&name=" << zone->GetShortName() << ") [Instance: " << zone->GetInstanceID() << "]"
								;
							std::string out_final = output.str();
							zone->SendDiscordMessage("rngbags", out_final);
						}
						//Database Logging
						if (top_client->CastToClient()->Admin() < RuleI(Vegas, MinStatusToBypassVegasLootLogging)) {
							std::string query;
							std::string item_name;
							item_name = chosen_item->Name;
							//std::replace(item_name.begin(), item_name.end(), "\`", "\'");
							query = StringFormat(
								"INSERT INTO rng_bags "
								"SET `npc_id` = '%i', `npc_name` = '%s', `npc_level` = '%i', `type` = '%s', `npc_diff` = '%f', `item_id` = '%i', `raid_item` = '%i', `item` = '%s', `item_diff` = '%f', `zone_id` = '%i', `zone` = '%s', `zone_instance_id` = '%i', `time` = NOW(), `charid` = '%i', `char` = '%s', `char_level` = '%i' ",
								GetNPCTypeID(), Strings::Escape(GetCleanName()).c_str(), GetLevel(), vegas_loot_type, double(difficulty), chosen_item->ID, 0, Strings::Escape(chosen_item->Name).c_str(), double(chosen_item->difficulty), GetZoneID(), zone->GetShortName(), zone->GetInstanceID(), top_client->CastToClient()->CharacterID(), top_client->CastToClient()->GetCleanName(), top_client->CastToClient()->GetLevel());
							QueryVegasLoot(query, top_client->CastToClient()->GetCleanName(), GetCleanName());
						}
						break;
					}
				}
			}
		}
		--roll_count;
	}
}

void NPC::AddVegasBridleLoot(Mob* top_client) {

	int LootdropID[] = { RuleI(Vegas, VegasBridleTierOneLootDropID), RuleI(Vegas, VegasBridleTierTwoLootDropID), RuleI(Vegas, VegasBridleTierThreeLootDropID), RuleI(Vegas, VegasBridleTierFourLootDropID) };
	int MinLevel[] = { RuleI(Vegas, VegasBridleTierOneMinLevel), RuleI(Vegas, VegasBridleTierTwoMinLevel), RuleI(Vegas, VegasBridleTierThreeMinLevel), RuleI(Vegas, VegasBridleTierFourMinLevel) };
	int MaxLevel[] = { RuleI(Vegas, VegasBridleTierOneMaxLevel), RuleI(Vegas, VegasBridleTierTwoMaxLevel), RuleI(Vegas, VegasBridleTierThreeMaxLevel), RuleI(Vegas, VegasBridleTierFourMaxLevel) };
	int roll_count = RuleI(Vegas, VegasBridleRollCount);
	int roll_max = int(RuleI(Vegas, VegasBridleRollMax) * RuleR(Vegas, BridleMultiplier));
	int rolled;
	std::string vegas_loot_type;
	raid_target && !NPCBypassesVegasRaidLoot() ? vegas_loot_type = "Raid-Bridle" : rare_spawn ? vegas_loot_type = "Rare-Bridle" : vegas_loot_type = "Common-Bridle";

	while (roll_count >= 1) {
		rolled = zone->random.Int(1, roll_max);
		if (rolled >= roll_max) {
			for (int i = 0; i < sizeof(LootdropID) / sizeof(int); i++) {
				if (LootdropID[i] > 0) {
					if (MinLevel[i] > 0 && MaxLevel[i] > 0 && GetLevel() >= MinLevel[i] && GetLevel() <= MaxLevel[i]) {
						const LootDrop_Struct* chosen_loot_drop = nullptr;
						const EQ::ItemData* chosen_item = nullptr;
						VegasLoot("Bridle Roll hit on [{}] for [{}]. Roll [{}] | To Hit [{}].", GetCleanName(), top_client->GetCleanName(), rolled, roll_max); //deleteme
						chosen_loot_drop = database.GetLootDrop(LootdropID[i]);
						if (!chosen_loot_drop) {
							VegasLoot("No bridle loot drop found on [{}] for [{}].", GetCleanName(), top_client->GetCleanName()); //deleteme
							continue;
						}
						VegasLoot("Bridle loot using lootdrop [{}] on [{}] for [{}].", LootdropID[i], GetCleanName(), top_client->GetCleanName()); //deleteme
						chosen_item = database.GetItem(chosen_loot_drop->Entries[zone->random.Int(0, chosen_loot_drop->NumEntries)].item_id);
						if (!chosen_item) {
							VegasLoot("No bridle item found on [{}] for [{}].", GetCleanName(), top_client->GetCleanName()); //deleteme
							continue;
						}
						AddItem(chosen_item->ID, 1, RuleB(Vegas, EquipVegasDrops));
						VegasLoot("Added bridle [{}] to [{}] for [{}].", chosen_item->Name, GetCleanName(), top_client->GetCleanName()); //deleteme
						//Discord send
						if (top_client->CastToClient()->Admin() < RuleI(Vegas, MinStatusToBypassVegasDiscordLogging)) {
							std::ostringstream output;
							output <<
								"Added [" << chosen_item->Name << "](http://vegaseq.com/Allaclone/?a=item&id=" << chosen_item->ID << ") [ID: " << chosen_item->ID << "] [" << (GetNPCTypeID(), IsRaidTarget() ? "Raid" : IsRareSpawn() ? "Rare" : "Common") << "]"
								" to [" << GetCleanName() << "](http://vegaseq.com/Allaclone/?a=npc&id=" << GetNPCTypeID() << ") [ID: " << GetNPCTypeID() << "] [Level: " << int(GetLevel()) << "] [MobDiff: " << int(difficulty) << "]"
								" For [" << top_client->GetCleanName() << "](http://vegaseq.com/charbrowser/index.php?page=character&char=" << top_client->GetCleanName() << ") [ID: " << top_client->CastToClient()->CharacterID() << "] [Level: " << int(top_client->CastToClient()->GetLevel()) << "]"
								" in [" << zone->GetLongName() << "](http://vegaseq.com/Allaclone/?a=zone&name=" << zone->GetShortName() << ") [Instance: " << zone->GetInstanceID() << "]"
								;
							std::string out_final = output.str();
							zone->SendDiscordMessage("rngbridles", out_final);
						}
						//Database Logging
						if (top_client->CastToClient()->Admin() < RuleI(Vegas, MinStatusToBypassVegasLootLogging)) {
							std::string query;
							std::string item_name;
							item_name = chosen_item->Name;
							//std::replace(item_name.begin(), item_name.end(), "\`", "\'");
							query = StringFormat(
								"INSERT INTO rng_bridles "
								"SET `npc_id` = '%i', `npc_name` = '%s', `npc_level` = '%i', `type` = '%s', `npc_diff` = '%f', `item_id` = '%i', `raid_item` = '%i', `item` = '%s', `item_diff` = '%f', `zone_id` = '%i', `zone` = '%s', `zone_instance_id` = '%i', `time` = NOW(), `charid` = '%i', `char` = '%s', `char_level` = '%i' ",
								GetNPCTypeID(), Strings::Escape(GetCleanName()).c_str(), GetLevel(), vegas_loot_type, double(difficulty), chosen_item->ID, 0, Strings::Escape(chosen_item->Name).c_str(), double(chosen_item->difficulty), GetZoneID(), zone->GetShortName(), zone->GetInstanceID(), top_client->CastToClient()->CharacterID(), top_client->CastToClient()->GetCleanName(), top_client->CastToClient()->GetLevel());
							QueryVegasLoot(query, top_client->CastToClient()->GetCleanName(), GetCleanName());
						}
						break;
					}
				}
			}
		}
		--roll_count;
	}
}

void NPC::QueryVegasLoot(std::string query, std::string killer_name, std::string killed_name)
{
	if (query.empty()) {
		Logs::MySQLError, "Failed to save Vegas loot database information. Query was empty.";
		return;
	}
	VegasLootDetail("VegasQueryLoot::[{}]", query); //deleteme
	auto results = database.QueryDatabase(query);
	if (!results.Success()) {
		Logs::MySQLError, "Failed to save database information for [{}] on killing [{}]", killer_name, killed_name;
	}
}
