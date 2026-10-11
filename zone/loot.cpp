/*	EQEmu: EQEmulator

	Copyright (C) 2001-2026 EQEmu Development Team

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "npc.h"

#include "common/data_bucket.h"
#include "common/data_verification.h"
#include "common/loot.h"
#include "common/repositories/criteria/content_filter_criteria.h"
#include "common/repositories/global_loot_repository.h"
#include "zone/client.h"
#include "zone/entity.h"
#include "zone/global_loot_manager.h"
#include "zone/mob.h"
#include "zone/quest_parser_collection.h"
#include "zone/zonedb.h"

void NPC::AddLootTable(uint32 loottable_id, bool is_global)
{
	// check if it's a GM spawn
	if (!npctype_id) {
		return;
	}

	if (m_resumed_from_zone_suspend) {
		LogZoneState("NPC [{}] is resuming from zone suspend, skipping", GetCleanName());
		return;
	}

	if (!is_global) {
		m_loot_copper   = 0;
		m_loot_silver   = 0;
		m_loot_gold     = 0;
		m_loot_platinum = 0;
	}

	zone->LoadLootTable(loottable_id);

	const auto *l = zone->GetLootTable(loottable_id);
	if (!l) {
		return;
	}

	LogLootDetail(
		"Attempting to load loot [{}] loottable [{}] ({}) is_global [{}]",
		GetCleanName(),
		loottable_id,
		l->name,
		is_global
	);

	auto content_flags = ContentFlags{
		.min_expansion = l->min_expansion,
		.max_expansion = l->max_expansion,
		.content_flags = l->content_flags,
		.content_flags_disabled = l->content_flags_disabled
	};

	if (!WorldContentService::Instance()->DoesPassContentFiltering(content_flags)) {
		return;
	}

	uint32 min_cash = l->mincash;
	uint32 max_cash = l->maxcash;
	if (min_cash > max_cash) {
		const uint32 t = min_cash;
		min_cash = max_cash;
		max_cash = t;
	}

	uint32 cash = 0;
	if (!is_global) {
		if (max_cash > 0 && l->avgcoin > 0 && EQ::ValueWithin(l->avgcoin, min_cash, max_cash)) {
			const float upper_chance  = static_cast<float>(l->avgcoin - min_cash) /
										static_cast<float>(max_cash - min_cash);
			const float avg_cash_roll = static_cast<float>(zone->random.Real(0.0, 1.0));

			if (avg_cash_roll < upper_chance) {
				cash = zone->random.Int(l->avgcoin, max_cash);
			}
			else {
				cash = zone->random.Int(min_cash, l->avgcoin);
			}
		}
		else {
			cash = zone->random.Int(min_cash, max_cash);
		}
	}

	if (cash != 0) {
		if (RuleB(VegasLoot, EnableVegasDrops)) {
			cash *= std::max(0.01f, RuleR(VegasLoot, CashMultiplier));
		}

		m_loot_platinum = cash / 1000;
		cash -= m_loot_platinum * 1000;

		m_loot_gold = cash / 100;
		cash -= m_loot_gold * 100;

		m_loot_silver = cash / 10;
		cash -= m_loot_silver * 10;

		m_loot_copper = cash;
		/*
		//Discord send
		if (top_client->CastToClient()->Admin() < RuleI(VegasLoot, MinStatusToBypassVegasLootDiscordLogging)) {
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
		if (top_client->CastToClient()->Admin() < RuleI(VegasLoot, MinStatusToBypassVegasLootLogging)) {
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

	const uint32 global_loot_multiplier = RuleI(Zone, GlobalLootMultiplier);
	for (auto    &lte: zone->GetLootTableEntries(loottable_id)) {
		for (uint32 k = 1; k <= (lte.multiplier * global_loot_multiplier); k++) {
			const uint8 drop_limit   = lte.droplimit;
			const uint8 minimum_drop = lte.mindrop;
			const float probability  = lte.probability;

			float drop_chance = 0.0f;
			if (EQ::ValueWithin(probability, 0.0f, 100.0f)) {
				drop_chance = static_cast<float>(zone->random.Real(0.0, 100.0));
			}

			if (probability != 0.0 && (probability == 100.0 || drop_chance <= probability)) {
				AddLootDropTable(lte.lootdrop_id, drop_limit, minimum_drop);
			}
		}
	}

	LogLootDetail(
		"Loaded [{}] Loot Table [{}] is_global [{}]",
		GetCleanName(),
		loottable_id,
		is_global
	);
}

void NPC::AddLootDropTable(uint32 lootdrop_id, uint8 drop_limit, uint8 min_drop)
{
	const auto l  = zone->GetLootdrop(lootdrop_id);
	const auto le = zone->GetLootdropEntries(lootdrop_id);
	if (l.id == 0 || le.empty()) {
		return;
	}

	// if this lootdrop is droplimit=0 and mindrop 0, scan list once and return
	if (drop_limit == 0 && min_drop == 0) {
		for (const auto &e: le) {
			for (int j = 0; j < e.multiplier; ++j) {
				if (zone->random.Real(0.0, 100.0) <= e.chance && MeetsLootDropLevelRequirements(e, true)) {
					const EQ::ItemData *database_item = database.GetItem(e.item_id);
					AddLootDrop(database_item, e);
					LogLootDetail(
						"---- NPC (Rolled) [{}] Lootdrop [{}] Item [{}] ({}) Chance [{}] Multiplier [{}]",
						GetCleanName(),
						lootdrop_id,
						database_item->Name,
						e.item_id,
						e.chance,
						e.multiplier
					);
				}
			}
		}

		return;
	}

	if (le.size() > 100 && drop_limit == 0) {
		drop_limit = 10;
	}

	if (drop_limit < min_drop) {
		drop_limit = min_drop;
	}

	float roll_t                   = 0.0f;
	float no_loot_prob             = 1.0f;
	bool  roll_table_chance_bypass = false;
	bool  active_item_list         = false;

	for (const auto &e: le) {
		const EQ::ItemData *db_item = database.GetItem(e.item_id);
		if (db_item && MeetsLootDropLevelRequirements(e)) {
			roll_t += e.chance;

			if (e.chance >= 100) {
				roll_table_chance_bypass = true;
			}
			else {
				no_loot_prob *= (100 - e.chance) / 100.0f;
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

	// translate above for loop using l and le
	for (int i = 0; i < drop_limit; ++i) {
		if (drops < min_drop || roll_table_chance_bypass || (float) zone->random.Real(0.0, 1.0) >= no_loot_prob) {
			float           roll = (float) zone->random.Real(0.0, roll_t);
			for (const auto &e: le) {
				const auto *db_item = database.GetItem(e.item_id);
				if (db_item) {
					// if it doesn't meet the requirements do nothing
					if (!MeetsLootDropLevelRequirements(e)) {
						continue;
					}

					if (roll < e.chance) {
						AddLootDrop(db_item, e);
						drops++;

						uint8 charges = e.multiplier;
						charges = EQ::ClampLower(charges, static_cast<uint8>(1));

						for (int k = 1; k < charges; ++k) {
							float c_roll = static_cast<float>(zone->random.Real(0.0, 100.0));
							if (c_roll <= e.chance) {
								AddLootDrop(db_item, e);
							}
						}

						break;
					}
					else {
						roll -= e.chance;
					}
				}
			}
		}
	}

	UpdateEquipmentLight();
}

bool NPC::MeetsLootDropLevelRequirements(LootdropEntriesRepository::LootdropEntries loot_drop, bool verbose)
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

//if itemlist is null, just send wear changes
void NPC::AddLootDrop(
	const EQ::ItemData *item2,
	LootdropEntriesRepository::LootdropEntries loot_drop,
	bool wear_change,
	uint32 augment_one,
	uint32 augment_two,
	uint32 augment_three,
	uint32 augment_four,
	uint32 augment_five,
	uint32 augment_six,
	uint32 given_by_char_id
)
{
	if (m_resumed_from_zone_suspend) {
		LogZoneState("NPC [{}] is resuming from zone suspend, skipping", GetCleanName());
		return;
	}

	if (!item2) {
		return;
	}

	auto item = new LootItem;

	if (EQEmuLogSys::Instance()->log_settings[Logs::Loot].is_category_enabled == 1) {
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
	item->aug_1             = augment_one;
	item->aug_2             = augment_two;
	item->aug_3             = augment_three;
	item->aug_4             = augment_four;
	item->aug_5             = augment_five;
	item->aug_6             = augment_six;
	item->attuned           = false;
	item->trivial_min_level = loot_drop.trivial_min_level;
	item->trivial_max_level = loot_drop.trivial_max_level;
	item->equip_slot        = EQ::invslot::SLOT_INVALID;
	item->given_by_char_id  = given_by_char_id;

	// unsure if required to equip, YOLO for now
	if (item2->ItemType == EQ::item::ItemTypeBow) {
		SetBowEquipped(true);
	}

	if (item2->ItemType == EQ::item::ItemTypeArrow) {
		SetArrowEquipped(true);
	}

	bool found = false; // track if we found an empty slot we fit into

	int found_slot = INVALID_INDEX; // for multi-slot items

	auto *inst = database.CreateItem(
		item2->ID,
		loot_drop.item_charges,
		augment_one,
		augment_two,
		augment_three,
		augment_four,
		augment_five,
		augment_six
	);

	if (!inst) {
		return;
	}

	if (loot_drop.equip_item > 0) {
		uint8              equipment_slot = UINT8_MAX;
		const EQ::ItemData *compitem      = nullptr;

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
				const uint32 slots = (1 << i);
				if (item2->Slots & slots) {
					if (equipment[i]) {
						compitem = database.GetItem(equipment[i]);
						if (!compitem) {
							continue;
						}

						if (item2->AC > compitem->AC || (item2->AC == compitem->AC && item2->HP > compitem->HP)) {
							// item would be an upgrade
							// check if we're multi-slot, if yes then we have to keep
							// looking in case any of the other slots we can fit into are empty.
							if (item2->Slots != slots) {
								found_slot = i;
							}
							else {
								// Unequip old item
								auto *old_item = GetItem(i);
								if (!old_item) {
									continue;
								}

								old_item->equip_slot = EQ::invslot::SLOT_INVALID;

								equipment[i] = item2->ID;

								found_slot = i;
								found      = true;
							}
						}
					}
					else {
						equipment[i] = item2->ID;

						found_slot = i;
						found      = true;
					}
				}
			}
		}

		// Possible slot was found but not selected. Pick it now.
		if (!found && found_slot >= 0) {
			equipment[found_slot] = item2->ID;

			found = true;
		}

		uint32 equipment_material;
		if (
			item2->Material <= 0 ||
			(
				item2->Slots & (
					(1 << EQ::invslot::slotPrimary) |
					(1 << EQ::invslot::slotSecondary)
				)
			)
			) {
			equipment_material = Strings::ToUnsignedInt(&item2->IDFile[2]);
		}
		else {
			equipment_material = item2->Material;
		}

		if (found_slot == EQ::invslot::slotPrimary) {
			equipment_slot = EQ::textures::weaponPrimary;

			if (item2->Damage > 0) {
				SendAddPlayerState(PlayerState::PrimaryWeaponEquipped);

				if (!RuleB(Combat, ClassicNPCBackstab)) {
					SetFacestab(true);
				}
			}

			if (item2->IsType2HWeapon()) {
				SetTwoHanderEquipped(true);
			}
		}
		else if (
			found_slot == EQ::invslot::slotSecondary &&
			(
				GetOwner() ||
				(CanThisClassDualWield() && zone->random.Roll(NPC_DW_CHANCE)) ||
				item2->Damage == 0
			) &&
			(
				item2->IsType1HWeapon() ||
				item2->ItemType == EQ::item::ItemTypeShield ||
				item2->ItemType == EQ::item::ItemTypeLight
			)
			) {
			equipment_slot = EQ::textures::weaponSecondary;

			if (item2->Damage > 0) {
				SendAddPlayerState(PlayerState::SecondaryWeaponEquipped);
			}
		}
		else if (found_slot == EQ::invslot::slotHead) {
			equipment_slot = EQ::textures::armorHead;
		}
		else if (found_slot == EQ::invslot::slotChest) {
			equipment_slot = EQ::textures::armorChest;
		}
		else if (found_slot == EQ::invslot::slotArms) {
			equipment_slot = EQ::textures::armorArms;
		}
		else if (EQ::ValueWithin(found_slot, EQ::invslot::slotWrist1, EQ::invslot::slotWrist2)) {
			equipment_slot = EQ::textures::armorWrist;
		}
		else if (found_slot == EQ::invslot::slotHands) {
			equipment_slot = EQ::textures::armorHands;
		}
		else if (found_slot == EQ::invslot::slotLegs) {
			equipment_slot = EQ::textures::armorLegs;
		}
		else if (found_slot == EQ::invslot::slotFeet) {
			equipment_slot = EQ::textures::armorFeet;
		}

		if (equipment_slot != UINT8_MAX) {
			if (wear_change) {
				p_wear_change_struct->wear_slot_id = equipment_slot;
				p_wear_change_struct->material     = equipment_material;
			}
		}

		if (found) {
			item->equip_slot = found_slot;
		}
	}

	if (found_slot != INVALID_INDEX) {
		GetInv().PutItem(found_slot, *inst);
	}

	if (parse->HasQuestSub(GetNPCTypeID(), EVENT_LOOT_ADDED)) {
		std::vector<std::any> args = {inst};
		parse->EventNPC(EVENT_LOOT_ADDED, this, nullptr, "", 0, &args);
	}

	item->lootdrop_id = loot_drop.lootdrop_id;
	m_loot_items.push_back(item);

	if (found) {
		CalcBonuses();
	}

	if (IsRecordLootStats()) {
		m_rolled_items.emplace_back(item->item_id);
	}

	if (wear_change && outapp) {
		entity_list.QueueClients(this, outapp);
		safe_delete(outapp);
	}

	UpdateEquipmentLight();

	if (UpdateActiveLight()) {
		SendAppearancePacket(AppearanceType::Light, GetActiveLightType());
	}

	safe_delete(inst);
}

void NPC::AddItem(const EQ::ItemData *item, uint16 charges, bool equip_item)
{
	auto l = LootdropEntriesRepository::NewNpcEntity();

	l.equip_item   = static_cast<uint8>(equip_item ? 1 : 0);
	l.item_charges = charges;

	AddLootDrop(item, l, true);
}

void NPC::AddItem(
	uint32 item_id,
	uint16 charges,
	bool equip_item,
	uint32 augment_one,
	uint32 augment_two,
	uint32 augment_three,
	uint32 augment_four,
	uint32 augment_five,
	uint32 augment_six
)
{
	const auto *item = database.GetItem(item_id);
	if (!item) {
		return;
	}

	auto l = LootdropEntriesRepository::NewNpcEntity();

	l.equip_item   = static_cast<uint8>(equip_item ? 1 : 0);
	l.item_charges = charges;

	AddLootDrop(
		item,
		l,
		true,
		augment_one,
		augment_two,
		augment_three,
		augment_four,
		augment_five,
		augment_six
	);
}

void NPC::AddLootTable()
{
	AddLootTable(m_loottable_id);
}

void NPC::CheckGlobalLootTables()
{
	const auto &l = zone->GetGlobalLootTables(this);
	for (const auto &e: l) {
		AddLootTable(e, true);
	}
}

void ZoneDatabase::LoadGlobalLoot()
{
	const auto &l = GlobalLootRepository::GetWhere(
		*this,
		fmt::format(
			"`enabled` = 1 {}",
			ContentFilterCriteria::apply()
		)
	);

	if (l.empty()) {
		return;
	}

	LogInfo(
		"Loaded [{}] Global Loot Entr{}.",
		Strings::Commify(l.size()),
		l.size() != 1 ? "ies" : "y"
	);

	const std::string &zone_id = std::to_string(zone->GetZoneID());

	for (const auto &e: l) {
		if (!e.zone.empty()) {
			const auto &zones = Strings::Split(e.zone, "|");

			if (!Strings::Contains(zones, zone_id)) {
				continue;
			}
		}

		GlobalLootEntry gle(e.id, e.loottable_id, e.description);

		if (e.min_level) {
			gle.AddRule(GlobalLoot::RuleTypes::LevelMin, e.min_level);
		}

		if (e.max_level) {
			gle.AddRule(GlobalLoot::RuleTypes::LevelMax, e.max_level);
		}

		if (e.rare) {
			gle.AddRule(GlobalLoot::RuleTypes::Rare, e.rare);
		}

		if (e.raid) {
			gle.AddRule(GlobalLoot::RuleTypes::Raid, e.raid);
		}

		if (!e.race.empty()) {
			const auto &races = Strings::Split(e.race, "|");

			for (const auto &r: races) {
				gle.AddRule(GlobalLoot::RuleTypes::Race, Strings::ToInt(r));
			}
		}

		if (!e.class_.empty()) {
			const auto &classes = Strings::Split(e.class_, "|");

			for (const auto &c: classes) {
				gle.AddRule(GlobalLoot::RuleTypes::Class, Strings::ToInt(c));
			}
		}

		if (!e.bodytype.empty()) {
			const auto &bodytypes = Strings::Split(e.bodytype, "|");

			for (const auto &b: bodytypes) {
				gle.AddRule(GlobalLoot::RuleTypes::BodyType, Strings::ToInt(b));
			}
		}

		if (e.hot_zone) {
			gle.AddRule(GlobalLoot::RuleTypes::HotZone, e.hot_zone);
		}

		zone->AddGlobalLootEntry(gle);
	}
}


LootItem *NPC::GetItem(int slot_id)
{
	LootItems::iterator cur, end;
	cur = m_loot_items.begin();
	end = m_loot_items.end();
	for (; cur != end; ++cur) {
		LootItem *item = *cur;
		if (item && item->equip_slot == slot_id) {
			return item;
		}
	}
	return (nullptr);
}

void NPC::RemoveItem(uint32 item_id, uint16 quantity, uint16 slot)
{
	LootItems::iterator cur, end;
	cur = m_loot_items.begin();
	end = m_loot_items.end();
	for (; cur != end; ++cur) {
		LootItem *item = *cur;
		if (item->item_id == item_id && slot <= 0 && quantity <= 0) {
			m_loot_items.erase(cur);
			safe_delete(item);
			UpdateEquipmentLight();
			if (UpdateActiveLight()) { SendAppearancePacket(AppearanceType::Light, GetActiveLightType()); }
			return;
		}
		else if (item->item_id == item_id && item->equip_slot == slot && quantity >= 1) {
			if (item->charges <= quantity) {
				m_loot_items.erase(cur);
				UpdateEquipmentLight();
				if (UpdateActiveLight()) {
					SendAppearancePacket(AppearanceType::Light, GetActiveLightType());
				}
				safe_delete(item);
			}
			else {
				item->charges -= quantity;
			}
			return;
		}
	}
}

void NPC::CheckTrivialMinMaxLevelDrop(Mob *killer)
{
	if (killer == nullptr || !killer->IsClient()) {
		return;
	}

	uint16 killer_level = killer->GetLevel();
	uint8  material;

	auto cur = m_loot_items.begin();
	while (cur != m_loot_items.end()) {
		if (!(*cur)) {
			return;
		}

		uint16 trivial_min_level     = (*cur)->trivial_min_level;
		uint16 trivial_max_level     = (*cur)->trivial_max_level;
		bool   fits_trivial_criteria = (
			(trivial_min_level > 0 && killer_level < trivial_min_level) ||
			(trivial_max_level > 0 && killer_level > trivial_max_level)
		);

		if (fits_trivial_criteria) {
			material = EQ::InventoryProfile::CalcMaterialFromSlot((*cur)->equip_slot);
			if (material != EQ::textures::materialInvalid) {
				SendWearChange(material);
			}

			cur = m_loot_items.erase(cur);
			continue;
		}
		++cur;
	}

	UpdateEquipmentLight();
	if (UpdateActiveLight()) {
		SendAppearancePacket(AppearanceType::Light, GetActiveLightType());
	}
}

void NPC::ClearLootItems()
{
	LootItems::iterator cur, end;
	cur = m_loot_items.begin();
	end = m_loot_items.end();
	for (; cur != end; ++cur) {
		LootItem *item = *cur;
		safe_delete(item);
	}
	m_loot_items.clear();

	UpdateEquipmentLight();
	if (UpdateActiveLight()) {
		SendAppearancePacket(AppearanceType::Light, GetActiveLightType());
	}
}

void NPC::QueryLoot(Client *to, bool is_pet_query)
{
	if (!m_loot_items.empty()) {
		if (!is_pet_query) {
			to->Message(
				Chat::White,
				fmt::format(
					"Loot | {} ({}) ID: {} Loottable ID: {}",
					GetName(),
					GetID(),
					GetNPCTypeID(),
					GetLoottableID()
				).c_str()
			);
		}

		int item_count = 0;

		for (auto current_item: m_loot_items) {
			int item_number = (item_count + 1);
			if (!current_item) {
				LogError("ItemList error, null item.");
				continue;
			}

			if (!current_item->item_id || !database.GetItem(current_item->item_id)) {
				LogError("Database error, invalid item.");
				continue;
			}

			EQ::SayLinkEngine linker;
			linker.SetLinkType(EQ::saylink::SayLinkLootItem);
			linker.SetLootData(current_item);

			to->Message(
				Chat::White,
				fmt::format(
					"Item {} | {} ({}){}",
					item_number,
					linker.GenerateLink().c_str(),
					current_item->item_id,
					(
						current_item->charges > 1 ?
							fmt::format(
								" Amount: {}",
								current_item->charges
							) :
							""
					)
				).c_str()
			);
			item_count++;
		}
	}

	if (!is_pet_query) {
		if (m_loot_platinum || m_loot_gold || m_loot_silver || m_loot_copper) {
			to->Message(
				Chat::White,
				fmt::format(
					"Money | {}",
					Strings::Money(
						m_loot_platinum,
						m_loot_gold,
						m_loot_silver,
						m_loot_copper
					)
				).c_str()
			);
		}
	}
}

bool NPC::HasItem(uint32 item_id)
{
	if (!database.GetItem(item_id)) {
		return false;
	}

	for (auto loot_item: m_loot_items) {
		if (!loot_item) {
			LogError("NPC::HasItem() - ItemList error, null item");
			continue;
		}

		if (!loot_item->item_id || !database.GetItem(loot_item->item_id)) {
			LogError("NPC::HasItem() - Database error, invalid item");
			continue;
		}

		if (loot_item->item_id == item_id) {
			return true;
		}
	}
	return false;
}

uint32 NPC::CountItem(uint32 item_id)
{
	uint32 item_count = 0;
	if (!database.GetItem(item_id)) {
		return item_count;
	}

	for (auto loot_item: m_loot_items) {
		if (!loot_item) {
			LogError("NPC::CountItem() - ItemList error, null item");
			continue;
		}

		if (!loot_item->item_id || !database.GetItem(loot_item->item_id)) {
			LogError("NPC::CountItem() - Database error, invalid item");
			continue;
		}

		if (loot_item->item_id == item_id) {
			item_count += loot_item->charges > 0 ? loot_item->charges : 1;
		}
	}
	return item_count;
}

uint32 NPC::GetLootItemIDBySlot(uint16 loot_slot)
{
	for (auto loot_item: m_loot_items) {
		if (loot_item->lootslot == loot_slot) {
			return loot_item->item_id;
		}
	}
	return 0;
}

uint16 NPC::GetFirstLootSlotByItemID(uint32 item_id)
{
	for (auto loot_item: m_loot_items) {
		if (loot_item->item_id == item_id) {
			return loot_item->lootslot;
		}
	}
	return 0;
}

void NPC::AddLootCash(
	uint32 in_copper,
	uint32 in_silver,
	uint32 in_gold,
	uint32 in_platinum
)
{
	m_loot_copper   = in_copper >= 0 ? in_copper : 0;
	m_loot_silver   = in_silver >= 0 ? in_silver : 0;
	m_loot_gold     = in_gold >= 0 ? in_gold : 0;
	m_loot_platinum = in_platinum >= 0 ? in_platinum : 0;
}

void NPC::RemoveLootCash()
{
	m_loot_copper   = 0;
	m_loot_silver   = 0;
	m_loot_gold     = 0;
	m_loot_platinum = 0;
}

void NPC::AttemptVegasLoot(Mob* top_client, float diff_min, float diff_max, bool raid_item, float bonus_chance) {
	if (!top_client) {
		return;
	}

	if (!RuleB(SSF, SelfFoundVegasLootAllowed) && top_client->IsClient() && SSFLogic::IsAnySelfFound(top_client->CastToClient()->GetSSFFlag())) {
		return;
	}

	BenchTimer benchmark;

	uint16 zone_id = zone->GetZoneID();
	uint8 zone_era; // 0 = Classic, 1 = Kunark, 2 = Velious, 3 = Luclin, 4 = PoP
	uint32 id_min = RuleI(VegasLoot, MinVegasItemID);
	uint32 id_max = RuleI(VegasLoot, MaxVegasItemID);
	bool raid_only = IsRaidTarget() && !RaidTargetsNotCountAsRaid();
	uint32 zone_range_min, zone_range_max;
	GetVegasZoneRange(zone_id, &zone_era, &zone_range_min, &zone_range_max);
	float difficulty_min, difficulty_max, difficulty_bonus_min, difficulty_bonus_max;
	GetVegasDifficultyRange(zone_id, &difficulty_min, &difficulty_max, &difficulty_bonus_min, &difficulty_bonus_max);
	float random_drop_multiplier_scale = 1.0f;
	bool guaranteed_shard_drop = false;
	float shard_multiplier = 1.00;

	difficulty_min = diff_min > 0.0f ? diff_min : difficulty_min;
	difficulty_max = diff_max > 0.0f ? diff_max : difficulty_max;
	raid_only = raid_item ? raid_item : raid_only;


	if (top_client->CastToClient()->GetLevel() < RuleI(VegasLoot, RandomDropScaleLevelLimit)) {
		random_drop_multiplier_scale = std::max(1.0f, RuleR(VegasLoot, RandomDropMultiplierScaling) - (static_cast<float>(top_client->CastToClient()->GetLevel() - 1) / (RuleI(VegasLoot, RandomDropScaleLevelLimit) - 1)));
	}

	difficulty_min *= std::max(0.01f, RuleR(VegasLoot, OverallNormalDropMinDifficultyMultiplier));
	difficulty_max *= std::max(0.01f, RuleR(VegasLoot, OverallNormalDropMaxDifficultyMultiplier));
	difficulty_bonus_min *= std::max(0.01f, RuleR(VegasLoot, OverallBonusDropMinDifficultyMultiplier));
	difficulty_bonus_max *= std::max(0.01f, RuleR(VegasLoot, OverallBonusDropMaxDifficultyMultiplier));

	auto random_drop_multiplier = std::max(0.01f, RuleR(VegasLoot, RandomDropMultiplier)) * random_drop_multiplier_scale; // 2.0x at Level 1, 1.0x by level 30
	auto roll_count = GetRollCount(difficulty);
	auto raid_roll_count = GetRaidRollCount(difficulty);
	auto bonus_roll_count = GetBonusRollCount(difficulty);
	auto roll_max = 0;
	auto roll_to_hit = 0;
	auto bonus_roll_max = 0;
	auto bonus_roll_to_hit = 0;
	auto raid_roll_max = 0;
	auto raid_roll_to_hit = 0;
	int rolled;

	float clamped_bonus = std::clamp(bonus_chance, 0.0f, 100.0f);
	LogVegasLootDetail("RuleR(VegasLoot, RandomDropMultiplier) is [{}], random_drop_multiplier_scale is [{}] random_drop_multiplier is [{}], clamped_bonus is [{}]", RuleR(VegasLoot, RandomDropMultiplier), random_drop_multiplier_scale, random_drop_multiplier, clamped_bonus); //deleteme
	random_drop_multiplier *= 1.0f + (clamped_bonus * 0.01f);
	LogVegasLootDetail("Final random_drop_mulitplier is [{}]", random_drop_multiplier); //deleteme


	if (roll_count > 0) {
		roll_max = GetRollMax();
		roll_to_hit = GetRollToHit(roll_max, random_drop_multiplier);
	}

	if (bonus_roll_count) {
		bonus_roll_max = GetBonusRollMax();
		bonus_roll_to_hit = GetBonusRollToHit(bonus_roll_max, random_drop_multiplier);
	}

	if (raid_roll_count > 0) {
		raid_roll_max = GetRaidRollMax();
		raid_roll_to_hit = GetRaidRollToHit(raid_roll_max, random_drop_multiplier);
	}

	std::string vegas_loot_type;
	std::string added_type;

	LogVegasLoot("Attempting Vegas Loot for [{}] on [{}] - [{}].", top_client->GetCleanName(), GetCleanName(), IsRaidTarget() && RaidTargetsNotCountAsRaid() ? "Raid-Bypass" : IsRaidTarget() ? "Raid" : IsRareSpawn() ? "Rare" : "Normal"); //deleteme
	LogVegasLootDetail(
		"We are looking for item IDs in ZoneID [{}] between [{}] and [{}], with a difficulty between [{}] and [{}], a bonus difficulty between [{}] and [{}], lowest_drop_ids between [{}] and [{}], with a loot drop modifier of [{}] and a scale modifier of [{}] - that [{}] raid_only."
		, zone_id, id_min, id_max, difficulty_min, difficulty_max, difficulty_bonus_min, difficulty_bonus_max, zone_range_min, zone_range_max, random_drop_multiplier, random_drop_multiplier_scale, raid_only ? "are" : "are not"
	); //deleteme
	LogVegasLootDetail("Roll count is [{}], bonus roll count is [{}], raid roll count is [{}], roll max is [{}], bonus roll max is [{}], raid roll max is [{}], roll to hit is [{}], bonus roll to hit is [{}] and raid roll to hit is [{}]"
		, roll_count, bonus_roll_count, raid_roll_count, roll_max, bonus_roll_max, raid_roll_max, roll_to_hit, bonus_roll_to_hit, raid_roll_to_hit
	); //deleteme

	while (bonus_roll_count >= 1) {
		rolled = zone->random.Int(1, std::min(bonus_roll_max, bonus_roll_to_hit));
		LogVegasLootDetail("Bonus rolled a [{}]", rolled); //deleteme

		if (rolled >= std::min(bonus_roll_max, bonus_roll_to_hit) || GuaranteedBonusDrop()) {
			LogVegasLoot("Bonus Roll hit on [{}] for [{}].", GetCleanName(), top_client->GetCleanName()); //deleteme
			shard_multiplier = std::max(0.01f, RuleR(VegasLoot, ShardBonusLootMultiplier));
			guaranteed_shard_drop = true;
			const EQ::ItemData* chosen_item = GetVegasItems(id_min, id_max, difficulty_bonus_min, difficulty_bonus_max, zone_range_min, zone_range_max, raid_only);

			if (chosen_item) {
				added_type = "Added [Bonus]";
				vegas_loot_type = raid_target && !RaidTargetsNotCountAsRaid() ? vegas_loot_type = "Raid-Bonus" : rare_spawn ? vegas_loot_type = "Rare-Bonus" : vegas_loot_type = "Common-Bonus";
				Emote("'s corpse shimmers as it falls to the ground.");
				AddVegasItem(top_client, chosen_item, vegas_loot_type, added_type, raid_only);
			}
		}

		--bonus_roll_count;
	}

	while (roll_count >= 1) {
		raid_only = false;
		rolled = zone->random.Int(1, std::min(roll_max, roll_to_hit));
		LogVegasLootDetail("Rolled a [{}]", rolled); //deleteme

		if (rolled >= std::min(roll_max, roll_to_hit) || GuaranteedNormalDrop()) {
			LogVegasLoot("Normal Roll hit on [{}] for [{}].", GetCleanName(), top_client->GetCleanName()); //deleteme
			const EQ::ItemData* chosen_item = GetVegasItems(id_min, id_max, difficulty_min, difficulty_max, zone_range_min, zone_range_max, raid_only);

			if (chosen_item) {
				added_type = "Added";
				vegas_loot_type = raid_target && !RaidTargetsNotCountAsRaid() ? vegas_loot_type = "Raid" : rare_spawn ? vegas_loot_type = "Rare" : vegas_loot_type = "Common";
				AddVegasItem(top_client, chosen_item, vegas_loot_type, added_type, raid_only);
			}
		}

		--roll_count;
	}

	while (raid_item || raid_roll_count >= 1) {
		raid_only = true;
		rolled = zone->random.Int(1, std::min(raid_roll_max, raid_roll_to_hit));
		LogVegasLootDetail("Raid rolled a [{}]", rolled); //deleteme

		if (rolled >= std::min(raid_roll_max, raid_roll_to_hit) || GuaranteedRaidDrop()) {
			LogVegasLoot("Raid Roll hit on [{}] for [{}].", GetCleanName(), top_client->GetCleanName()); //deleteme
			const EQ::ItemData* chosen_item = GetVegasItems(id_min, id_max, difficulty_min, difficulty_max, zone_range_min, zone_range_max, raid_only);

			if (chosen_item) {
				added_type = "Added [Raid Item]";
				vegas_loot_type = "Raid";
				AddVegasItem(top_client, chosen_item, vegas_loot_type, added_type, raid_only);
			}
		}

		if (raid_item) {
			raid_item = false;
		}
		else {
			--raid_roll_count;
		}
	}

	AttemptVegasShardLoot(top_client, shard_multiplier, guaranteed_shard_drop);
	AttemptVegasSpellLoot(top_client);
	AttemptVegasBagLoot(top_client);
	AttemptVegasBridleLoot(top_client);

	if (GetLevel() >= RuleI(VegasLoot, MinLevelForEpicToken)) {
		AttemptVegasEpicTokenLoot(top_client);
	}

	LogVegasLootDetail("Vegas Loot Operation took [{}]", benchmark.elapsed());
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
			item->ID >= id_min
			&& item->ID <= id_max
			&& item->Difficulty >= difficulty_min
			&& item->Difficulty <= difficulty_max
			&& item->LowestDropNPCID >= zone_range_min
			&& item->LowestDropNPCID <= zone_range_max
			&& (raid_only ? item->RaidOnly : !item->RaidOnly)
		) {
			vitem.id = item->ID;
			vitem.name = item->Name;
			vitem.difficulty = item->Difficulty;
			vitem.maxcharges = item->MaxCharges;
			vitems.push_back(vitem);
		}
	}

	if (vitems.size() > 0) {
		LogVegasLootDetail("Found [{}] items matching the criteria.", vitems.size()); //deleteme
		const EQ::ItemData* chosen_item = database.GetItem(vitems[std::rand() % vitems.size()].id);
		LogVegasLootDetail("Item [{}]-[{}] was chosen.", chosen_item->Name, chosen_item->ID); //deleteme

		return chosen_item;
	}

	LogVegasLootDetail("Didn't find any items matching that criteria"); //deleteme

	return nullptr;
}

void NPC::GetVegasZoneRange(uint16 zoneid, uint8* zone_era, uint32* zone_range_min, uint32* zone_range_max) {
	if (SpecialZonePassesVegasLoot()) { //Special Zones
		auto e = Strings::GetDelimitedStringsMatchingElement(",", RuleS(VegasLoot, SpecialEventZones), std::to_string(zone->GetZoneID()), RuleS(VegasLoot, SpecialEventZoneVersions), std::to_string(zone->GetInstanceVersion()));

		if (e >= 0) {
			try {
				std::vector<std::string> split_string_min, split_string_max;

				split_string_min = Strings::Split(RuleS(VegasLoot, SpecialEventZonesIDMin), ",");
				split_string_max = Strings::Split(RuleS(VegasLoot, SpecialEventZonesIDMax), ",");

				*zone_era       = static_cast<unsigned char>(VegasEra::SpecialEvent);
				*zone_range_min = (e < split_string_min.size() && !split_string_min[e].empty() && Strings::IsNumber(split_string_min[e]))
			? Strings::ToUnsignedInt(split_string_min[e]) : 0;
				*zone_range_max = (e < split_string_max.size() && !split_string_max[e].empty() && Strings::IsNumber(split_string_max[e]))
					? Strings::ToUnsignedInt(split_string_max[e]) : 0;

				LogVegasLootDetail("GetVegasZoneRange - Special Zone: {} [{}] [v{}]- Era [{}], RangeMin [{}], RangeMax [{}].",
					zone->GetShortName(), zone->GetZoneID(), zone->GetInstanceVersion(),
					*zone_era, *zone_range_min, *zone_range_max); //deleteme

				return;

			}

			catch (const std::exception& ex) {
				LogError("Exception in SpecialEventZones or SpecialEventZoneVersions: {}", ex.what());

				*zone_range_min = 0;
				*zone_range_max = 0;
			}
		}
	}

	if ((zoneid < Zones::FIELDOFBONE || zoneid == Zones::SHADOWREST || zoneid == Zones::HIGHPASSHOLD) && (zoneid != Zones::NEKTROPOS && zoneid != Zones::NERIAKD && zoneid != Zones::AVIAK)) { //Classic
		*zone_era       = static_cast<unsigned char>(VegasEra::Classic);
		*zone_range_min = RuleI(VegasLoot, ClassicZoneNPCIDMin);
		*zone_range_max = RuleI(VegasLoot, ClassicZoneNPCIDMax);
	}
	else if (zoneid >= Zones::FIELDOFBONE && zoneid <= Zones::VEESHAN) { //Kunark
		*zone_era       = static_cast<unsigned char>(VegasEra::Kunark);
		*zone_range_min = RuleI(VegasLoot, KunarkZoneNPCIDMin);
		*zone_range_max = RuleI(VegasLoot, KunarkZoneNPCIDMax);
	}
	else if (zoneid >= Zones::ICECLAD && zoneid <= Zones::THURGADINB) { //Velious
		*zone_era       = static_cast<unsigned char>(VegasEra::Velious);
		*zone_range_min = RuleI(VegasLoot, VeliousZoneNPCIDMin);
		*zone_range_max = RuleI(VegasLoot, VeliousZoneNPCIDMax);
	}
	else if (zoneid >= Zones::SHADOWHAVEN && zoneid <= Zones::JAGGEDPINE) { //Luclin
		*zone_era       = static_cast<unsigned char>(VegasEra::Luclin);
		*zone_range_min = RuleI(VegasLoot, LuclinZoneNPCIDMin);
		*zone_range_max = RuleI(VegasLoot, LuclinZoneNPCIDMax);
	}
	else if (zoneid >= Zones::CODECAY && zoneid <= Zones::POTIMEB) { //PoP
		*zone_era       = static_cast<unsigned char>(VegasEra::PlanesOfPower);
		*zone_range_min = RuleI(VegasLoot, PoPZoneNPCIDMin);
		*zone_range_max = RuleI(VegasLoot, PoPZoneNPCIDMax);
	}
}

void NPC::GetVegasDifficultyRange(uint16 zone_id, float* difficulty_min, float* difficulty_max, float* difficulty_bonus_min, float* difficulty_bonus_max) {
	float npc_diff = difficulty;
	float diff_min;
	float diff_max;
	float diff_bonus_min;
	float diff_bonus_max;
	bool is_valid_raid_target = false;

	if (npc_diff < RuleR(VegasLoot, DifficultyFloorMin)) {
		diff_min = RuleR(VegasLoot, DifficultyFloorMin);
		diff_max = RuleR(VegasLoot, DifficultyFloorMax);
		diff_bonus_min = RuleR(VegasLoot, DifficultyFloorBonusMin) * std::max(0.01f, RuleR(VegasLoot, DifficultyFloorBonusMinMultiplier));
		diff_bonus_max = RuleR(VegasLoot, DifficultyFloorBonusMax) * std::max(0.01f, RuleR(VegasLoot, DifficultyFloorBonusMaxMultiplier));
	}

	if (IsRaidTarget() && !RaidTargetsNotCountAsRaid()) {
		is_valid_raid_target = true;
		diff_min = npc_diff * std::max(0.01f, RuleR(VegasLoot, RaidMinDifficultyMultiplier));
		diff_max = npc_diff * std::max(0.01f, RuleR(VegasLoot, RaidMaxDifficultyMultiplier));
		diff_bonus_min = npc_diff * std::max(0.01f, RuleR(VegasLoot, RaidMinDifficultyBonusMultiplier));
		diff_bonus_max = npc_diff * std::max(0.01f, RuleR(VegasLoot, RaidMaxDifficultyBonusMultiplier));
	}
	else if (IsRareSpawn()) {
		diff_min = npc_diff * std::max(0.01f, RuleR(VegasLoot, RareMinDifficultyMultiplier));
		diff_max = npc_diff * std::max(0.01f, RuleR(VegasLoot, RareMaxDifficultyMultiplier));
		diff_bonus_min = npc_diff * std::max(0.01f, RuleR(VegasLoot, RareMinDifficultyBonusMultiplier));
		diff_bonus_max = npc_diff * std::max(0.01f, RuleR(VegasLoot, RareMaxDifficultyBonusMultiplier));
	}
	else {
		diff_min = npc_diff * std::max(0.01f, RuleR(VegasLoot, NormalMinDifficultyMultiplier));
		diff_max = npc_diff * std::max(0.01f, RuleR(VegasLoot, NormalMaxDifficultyMultiplier));
		diff_bonus_min = npc_diff * std::max(0.01f, RuleR(VegasLoot, NormalMinDifficultyBonusMultiplier));
		diff_bonus_max = npc_diff * std::max(0.01f, RuleR(VegasLoot, NormalMaxDifficultyBonusMultiplier));
	}

	if (is_valid_raid_target) {
		*difficulty_min = std::min(diff_min, RuleR(VegasLoot, MinRaidDifficultyCap));
		*difficulty_max = diff_max;
		*difficulty_bonus_min = std::min(diff_bonus_min, RuleR(VegasLoot, MinBonusRaidDifficultyCap));
		*difficulty_bonus_max = diff_bonus_max;
	}
	else {
		*difficulty_min = std::min(diff_min, RuleR(VegasLoot, MinDifficultyCap));
		*difficulty_max = diff_max;
		*difficulty_bonus_min = std::min(diff_bonus_min, RuleR(VegasLoot, MinBonusDifficultyCap));
		*difficulty_bonus_max = diff_bonus_max;
	}
}

bool NPC::IsVegasLootEligible(Mob* top_client) {
	if (!top_client) {
		return false;
	}

	if (HasOwner()) {
		return false;
	}

	if (IsOnVegasCoolDown(top_client)) {
		return false;
	}

	LogVegasLoot("VegasLootProcess::!IsOnVegasCoolDown - attack.cpp - on [{}] for [{}]", GetCleanName(), top_client ? top_client->GetCleanName() : "UNKNOWN"); //deleteme
	int top_hate_char_id = top_client->CastToClient()->CharacterID();
	Raid* raid = entity_list.GetRaidByClient(top_client->CastToClient());
	Group* group = top_client->GetGroup();
	int level_diff = 0;

	if (top_hate_char_id) {
		LogVegasLoot("VegasLootProcess::top_hate_char_id - attack.cpp - on [{}] for [{}]", GetCleanName(), top_client ? top_client->GetCleanName() : "UNKNOWN"); //deleteme
		raid ? level_diff = raid->GetHighestLevel() - GetLevel() : group ? level_diff = group->GetHighestLevel() - GetLevel() : level_diff = top_client->CastToClient()->GetLevel() - GetLevel();

		if (level_diff > RuleI(VegasLoot, LevelDifferenceForVegasDrops)) {
			LogVegasLootDetail("[{}] on [{}]:::Failed level checks [{}] - [{}] - [{} - {} = {}]", top_client->GetCleanName(), GetCleanName(), raid ? "has raid" : "no raid", group ? "has group" : "no group", raid ? raid->GetHighestLevel() : group ? group->GetHighestLevel() : top_client->CastToClient()->GetLevel(), GetLevel(), raid ? raid->GetHighestLevel() - GetLevel() : group ? group->GetHighestLevel() - GetLevel() : top_client->CastToClient()->GetLevel() - GetLevel());

			return false;
		}
	}

	if (SpecialNPCPassesVegasLoot()) {
		LogVegasLoot("VegasLootProcess::SpecialNPCPassesVegasLoot - attack.cpp - on [{}] for [{}]", GetCleanName(), top_client ? top_client->GetCleanName() : "UNKNOWN"); //deleteme
		return true;
	}

	if (SpecialZonePassesVegasLoot()) {
		LogVegasLoot("VegasLootProcess::SpecialZonePassesVegasLoot - attack.cpp - on [{}] for [{}]", GetCleanName(), top_client ? top_client->GetCleanName() : "UNKNOWN"); //deleteme
		return true;
	}

	if (NPCNotCountAsVegasLoot()) {
		LogVegasLoot("VegasLootProcess::NPCNotCountAsVegasLoot - attack.cpp - on [{}] for [{}]", GetCleanName(), top_client ? top_client->GetCleanName() : "UNKNOWN"); //deleteme
		return false;
	}

	if (RaidTargetsNotCountAsRaid()) {
		LogVegasLoot("VegasLootProcess::RaidTargetsNotCountAsRaid - attack.cpp - on [{}] for [{}]", GetCleanName(), top_client ? top_client->GetCleanName() : "UNKNOWN"); //deleteme
		return false;
	}

	if (GetDifficulty() <= RuleR(VegasLoot, MinDifficultyToTriggerVegasLoot)) {
		LogVegasLoot("VegasLootProcess::MinDifficultyToTriggerVegasLoot - attack.cpp - on [{}] for [{}]", GetCleanName(), top_client ? top_client->GetCleanName() : "UNKNOWN"); //deleteme
		return false;
	}

	if (GetNPCTypeID() < RuleI(VegasLoot, MinNPCIDToTriggerVegasLoot) || GetNPCTypeID() > RuleI(VegasLoot, MaxNPCIDToTriggerVegasLoot)) {
		LogVegasLoot("VegasLootProcess::MinNPCIDToTriggerVegasLoot/MaxNPCIDToTriggerVegasLoot - attack.cpp - on [{}] for [{}]", GetCleanName(), top_client ? top_client->GetCleanName() : "UNKNOWN"); //deleteme
		return false;
	}

	if (RuleB(VegasLoot, RequireLoottableForVegasLoot) && GetLoottableID() < 1) {
		LogVegasLoot("VegasLootProcess::RequireLoottableForVegasLoot - attack.cpp - on [{}] for [{}]", GetCleanName(), top_client ? top_client->GetCleanName() : "UNKNOWN"); //deleteme
		return false;
	}

	return true;
}

bool NPC::IsOnVegasCoolDown(Mob* top_client) {
	if (top_client->CastToClient()->Admin() >= RuleI(VegasLoot, MinStatusToBypassVegasLootCooldown)) {
		LogVegasLoot("VegasLootProcess::MinStatusToBypassVegasLootCooldown - attack.cpp - on [{}] for [{}]", GetCleanName(), top_client ? top_client->GetCleanName() : "UNKNOWN"); //deleteme
		return false;
	}

	uint32 nid = GetNPCTypeID();
	uint32 s2 = GetSpawnPointID();

	uint32 top_hate_character_id = top_client->CastToClient()->CharacterID();
	uint32 npc_instance = zone->GetInstanceID();
	uint64 respawn_time = GetRespawnTime();
	std::string key;
	std::string bucket_value;

	//if (GetLevel() <= RuleB(MaxLevelBeforeRespawnLockoutStopsForVegasLoot) && respawn_time <= RuleI(VegasLoot, QuickRespawnMaxToTriggerLockout)) {
		key = std::to_string(top_hate_character_id) + "_" + std::to_string(s2) + "_" + std::to_string(nid) + "_" + std::to_string(npc_instance) + "_respawnrnghold";
		LogVegasLootDetail("_respawnrnghold Respawn Key Check [{}]", key); //deleteme
		bucket_value = DataBucket::GetData(&database, key);

		if (Strings::IsNumber(bucket_value) && Strings::ToUnsignedInt(bucket_value) == 2) {
			LogVegasLoot("Failed respawnrnghold check for [{}]", GetCleanName()); //deleteme
			Emote("'s fortunes poof.");

			return true;
		}
		//else {
		//	DataBucket::SetData(&database, key, "1", std::to_string(RuleI(VegasLoot, QuickRespawnVegasLootLockoutPeriod)));
		//}
	//}

	std::string zone_name = zone->GetShortName();
	key = std::to_string(top_hate_character_id) + "_" + (zone_name) + "_" + std::to_string(nid) + "_" + std::to_string(npc_instance);
	LogVegasLootDetail("Lockout Respawn Key Check [{}]", key); //deleteme
	bucket_value = DataBucket::GetData(&database, key);

	if (!bucket_value.empty()) {
		LogVegasLoot("Failed lockout check for [{}]", GetCleanName()); //deleteme
		Emote("'s fortunes poof.");

		return true;
	}

	return false;
}

bool NPC::NPCNotCountAsVegasLoot() {
	return GetSkipVegasLoot() || Strings::DelimitedStringContains(",", RuleS(VegasLoot, NPCNotCountAsVegasLoot), std::to_string(GetNPCTypeID()));
}

bool NPC::GuaranteedNormalDrop() {
	return Strings::DelimitedStringContains(",", RuleS(VegasLoot, GuaranteedNormalDrops), std::to_string(GetNPCTypeID()));
}

bool NPC::GuaranteedBonusDrop() {
	return Strings::DelimitedStringContains(",", RuleS(VegasLoot, GuaranteedBonusDrops), std::to_string(GetNPCTypeID()));
}

bool NPC::GuaranteedRaidDrop() {
	return Strings::DelimitedStringContains(",", RuleS(VegasLoot, GuaranteedRaidDrops), std::to_string(GetNPCTypeID()));
}

bool NPC::RaidTargetsNotCountAsRaid() {
	return GetRaidTargetNotCountAsRaid() || Strings::DelimitedStringContains(",", RuleS(VegasLoot, RaidTargetsNotCountAsRaid), std::to_string(GetNPCTypeID()));
}

bool NPC::SpecialNPCPassesVegasLoot() {
	return GetPassVegasLoot() || Strings::DelimitedStringContains(",", RuleS(VegasLoot, NPCSpecialIDPassesLootChecks), std::to_string(GetNPCTypeID()));
}

bool NPC::SpecialZonePassesVegasLoot() {
	return Strings::DelimitedStringContains(",", RuleS(VegasLoot, SpecialEventZones), std::to_string(zone->GetZoneID()), RuleS(VegasLoot, SpecialEventZoneVersions), std::to_string(zone->GetInstanceVersion()));
}

bool NPC::NonRaidNPCCanGetRaidLoot() {
	return GetNonRaidTargetThatCanRaidRoll() || Strings::DelimitedStringContains(",", RuleS(VegasLoot, NonRaidTargetsThatCanRaidRoll), std::to_string(GetNPCTypeID()));
}

int NPC::GetRollCount(float npc_diff) {
	if (IsRaidTarget() && !RaidTargetsNotCountAsRaid()) {
		if (npc_diff < RuleR(VegasLoot, RaidTargetNormalRollMaxDifficulty)) {
			return RuleI(VegasLoot, RaidTargetNormalRollsCount);
		}

		return 0;
	}

	if (IsRareSpawn()) {
		return RuleI(VegasLoot, RareTargetRollsCount);
	}

	return RuleI(VegasLoot, NormalTargetRollsCount);
}

int NPC::GetRaidRollCount(float npc_diff) {
	if (NonRaidNPCCanGetRaidLoot()) {
		if (IsRareSpawn()) {
			return RuleI(VegasLoot, RareTargetRaidRollsCount);
		}

		return RuleI(VegasLoot, NormalTargetRaidRollsCount);
	}

	if (IsRaidTarget() && !RaidTargetsNotCountAsRaid()) {
		auto e = Strings::GetElementMatchingOperator(",", RuleS(VegasLoot, RaidTargetTierThresholds), npc_diff, ComparisonType::Greater);

		if (e >= 0) {
			auto raid_rolls = Strings::Split(RuleS(VegasLoot, RaidTargetTierRollCounts), ",");

			return (!raid_rolls.empty() && raid_rolls.size() >= e && Strings::IsNumber(raid_rolls[e])) ? Strings::ToInt(raid_rolls[e]) : 0;
		}

		return RuleI(VegasLoot, RaidTargetBaseRaidRollsCount);
	}

	return 0;
}

int NPC::GetRollMax() {
	if (IsRaidTarget() && !RaidTargetsNotCountAsRaid()) {
		return RuleI(VegasLoot, RaidVegasRollUpperLimit);
	}

	if (IsRareSpawn()) {
		return RuleI(VegasLoot, RareVegasRollUpperLimit);
	}

	return RuleI(VegasLoot, NormalVegasRollUpperLimit);
}

int NPC::GetRollToHit(auto roll_max, auto random_drop_multiplier) {
	if (IsRaidTarget() && !RaidTargetsNotCountAsRaid()) {
		return static_cast<int>((roll_max / random_drop_multiplier) * std::max(0.01f, RuleR(VegasLoot, RaidTargetRollMultiplier)));
	}

	if (IsRareSpawn()) {
		return static_cast<int>((roll_max / random_drop_multiplier) * std::max(0.01f, RuleR(VegasLoot, RareTargetRollMultiplier)));
	}
	LogVegasLootDetail("roll_max is [{}], random_drop_multiplier is [{}], RuleR(VegasLoot, NormalTargetRollMultiplier) is [{}]", roll_max, random_drop_multiplier, RuleR(VegasLoot, NormalTargetRollMultiplier)); //deleteme

	return static_cast<int>((roll_max / random_drop_multiplier) * std::max(0.01f, RuleR(VegasLoot, NormalTargetRollMultiplier)));
}

int NPC::GetBonusRollCount(float npc_diff) {
	if (IsRaidTarget() && !RaidTargetsNotCountAsRaid()) {
		return RuleI(VegasLoot, RaidBonusVegasRollCount);
	}

	if (IsRareSpawn()) {
		return RuleI(VegasLoot, RareBonusVegasRollCount);
	}

	return RuleI(VegasLoot, NormalBonusVegasRollCount);
}

int NPC::GetBonusRollMax() {
	if (IsRaidTarget() && !RaidTargetsNotCountAsRaid()) {
		return RuleI(VegasLoot, RaidBonusVegasRollUpperLimit);
	}

	if (IsRareSpawn()) {
		return RuleI(VegasLoot, RareBonusVegasRollUpperLimit);
	}


	return RuleI(VegasLoot, NormalBonusVegasRollUpperLimit);
}

int NPC::GetBonusRollToHit(auto roll_max, auto random_drop_multiplier) {
	if (IsRaidTarget() && !RaidTargetsNotCountAsRaid()) {
		return static_cast<int>((roll_max / random_drop_multiplier) * std::max(0.01f, RuleR(VegasLoot, RaidTargetBonusRollMultiplier)));
	}

	if (IsRareSpawn()) {
		return static_cast<int>((roll_max / random_drop_multiplier) * std::max(0.01f, RuleR(VegasLoot, RareTargetBonusRollMultiplier)));
	}

	return static_cast<int>((roll_max / random_drop_multiplier) * std::max(0.01f, RuleR(VegasLoot, NormalTargetBonusRollMultiplier)));
}

int NPC::GetRaidRollMax() {
	if (IsRaidTarget() && !RaidTargetsNotCountAsRaid()) {
		return RuleI(VegasLoot, RaidTargetRaidVegasRollUpperLimit);
	}

	if (IsRareSpawn()) {
		return RuleI(VegasLoot, RareTargetRaidVegasRollUpperLimit);
	}

	return RuleI(VegasLoot, NormalTargetRaidVegasRollUpperLimit);
}

int NPC::GetRaidRollToHit(auto roll_max, auto random_drop_multiplier) {
	if (IsRaidTarget() && !RaidTargetsNotCountAsRaid()) {
		return static_cast<int>((roll_max / random_drop_multiplier) * std::max(0.01f, RuleR(VegasLoot, RaidTargetRaidRollMultiplier)));
	}

	if (IsRareSpawn()) {
		return static_cast<int>((roll_max / random_drop_multiplier) * std::max(0.01f, RuleR(VegasLoot, RareTargetRaidRollMultiplier)));
	}

	return static_cast<int>((roll_max / random_drop_multiplier) * std::max(0.01f, RuleR(VegasLoot, NormalTargetRaidRollMultiplier)));
}

void NPC::AttemptVegasShardLoot(Mob* top_client, float shard_multiplier, bool guaranteed_shard_drop) {
	if (guaranteed_shard_drop || (zone->random.Int(1, RuleI(VegasLoot, ShardRollUpperLimit)) >= RuleI(VegasLoot, ShardRollUpperLimit))) {
		int shard_amount = GetShardAmount();
		uint8 shard_add_counter = 0;
		std::string query;
		std::string npc_name = GetCleanName();
		std::string vegas_loot_type;

		shard_amount *= shard_multiplier;

		if (!shard_amount) {
			return;
		}

		const EQ::ItemData* shardItem = database.GetItem(RuleI(Vegas, ShardItemID));

		if (!shardItem) {
			return;
		}

		if (zone->IsHotzone()) {
			shard_amount *= std::max(0.01f, RuleR(VegasLoot, ShardHotZoneMultiplier));
		}

		if (guaranteed_shard_drop) {
			raid_target && !RaidTargetsNotCountAsRaid() ? vegas_loot_type = "Raid-Bonus" : rare_spawn ? vegas_loot_type = "Rare-Bonus" : vegas_loot_type = "Common-Bonus";

			if (zone->IsHotzone()) {
				raid_target && !RaidTargetsNotCountAsRaid() ? shard_amount = EQ::Clamp(shard_amount, 1, int((RuleI(VegasLoot, RaidTargetBonusShardDropCap) * std::max(0.01f, RuleR(VegasLoot, ShardHotZoneCapMultiplier))))) : rare_spawn ? shard_amount = EQ::Clamp(shard_amount, 1, int((RuleI(VegasLoot, RareTargetBonusShardDropCap) * RuleR(VegasLoot, ShardHotZoneCapMultiplier)))) : shard_amount = EQ::Clamp(shard_amount, 1, int((RuleI(VegasLoot, NormalTargetBonusShardDropCap) * RuleR(VegasLoot, ShardHotZoneCapMultiplier))));
			}
			else {
				raid_target && !RaidTargetsNotCountAsRaid() ? shard_amount = EQ::Clamp(shard_amount, 1, RuleI(VegasLoot, RaidTargetBonusShardDropCap)) : rare_spawn ? shard_amount = EQ::Clamp(shard_amount, 1, RuleI(VegasLoot, RareTargetBonusShardDropCap)) : shard_amount = EQ::Clamp(shard_amount, 1, RuleI(VegasLoot, NormalTargetBonusShardDropCap));
			}
		}
		else {
			raid_target && !RaidTargetsNotCountAsRaid() ? vegas_loot_type = "Raid" : rare_spawn ? vegas_loot_type = "Rare" : vegas_loot_type = "Common";

			if (zone->IsHotzone()) {
				raid_target && !RaidTargetsNotCountAsRaid() ? shard_amount = EQ::Clamp(shard_amount, 1, int((RuleI(VegasLoot, RaidTargetShardDropCap) * std::max(0.01f, RuleR(VegasLoot, ShardHotZoneCapMultiplier))))) : rare_spawn ? shard_amount = EQ::Clamp(shard_amount, 1, int((RuleI(VegasLoot, RareTargetShardDropCap) * RuleR(VegasLoot, ShardHotZoneCapMultiplier)))) : shard_amount = EQ::Clamp(shard_amount, 1, int((RuleI(VegasLoot, NormalTargetShardDropCap) * RuleR(VegasLoot, ShardHotZoneCapMultiplier))));
			}
			else {
				raid_target && !RaidTargetsNotCountAsRaid() ? shard_amount = EQ::Clamp(shard_amount, 1, RuleI(VegasLoot, RaidTargetShardDropCap)) : rare_spawn ? shard_amount = EQ::Clamp(shard_amount, 1, RuleI(VegasLoot, RareTargetShardDropCap)) : shard_amount = EQ::Clamp(shard_amount, 1, RuleI(VegasLoot, NormalTargetShardDropCap));
			}
		}

		shard_amount = int(shard_amount);

		if (shard_amount >= shardItem->StackSize) {
			shard_add_counter = int(shard_amount / shardItem->StackSize);
		}

		if (top_client->CastToClient()->Admin() < RuleI(VegasLoot, MinStatusToBypassVegasLootLogging)) {
			std::string key = "ShardsDropped";
			std::string bucket_value = DataBucket::GetData(&database, key);

			if (Strings::IsNumber(bucket_value) && Strings::ToUnsignedInt(bucket_value) > 0) {
				DataBucket::SetData(&database, key, std::to_string(Strings::ToUnsignedInt(bucket_value) + shard_amount));
			}
			else {
				DataBucket::SetData(&database, key, std::to_string(1));
			}
		}

		//Discord send
		if (top_client->CastToClient()->Admin() < RuleI(VegasLoot, MinStatusToBypassVegasLootDiscordLogging)) {
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
		if (top_client->CastToClient()->Admin() < RuleI(VegasLoot, MinStatusToBypassVegasLootLogging)) {
			query = StringFormat(
				"INSERT INTO rng_shards "
				"SET `npc_id` = '%i', `npc_name` = '%s', `npc_level` = '%i', `type` = '%s', `npc_diff` = '%f', `shard_amount` = '%i', `zone_id` = '%i', `zone` = '%s', `zone_instance_id` = '%i', `time` = NOW(), `charid` = '%i', `char` = '%s', `char_level` = '%i' ",
				GetNPCTypeID(), Strings::Escape(GetCleanName()).c_str(), GetLevel(), vegas_loot_type, double(difficulty), shard_amount, zone->GetZoneID(), zone->GetShortName(), zone->GetInstanceID(), top_client->CastToClient()->CharacterID(), top_client->CastToClient()->GetCleanName(), top_client->CastToClient()->GetLevel());
			QueryVegasLoot(query, top_client->CastToClient()->GetCleanName(), GetCleanName());
		}

		if (shard_amount < shardItem->StackSize) {
			AddItem(RuleI(Vegas, ShardItemID), shard_amount, RuleB(VegasLoot, EquipVegasDrops));
			LogVegasLoot("Added [{}x] [{}] to [{}] for [{}]", shard_amount, database.CreateItemLink(RuleI(Vegas, ShardItemID)), GetCleanName(), top_client->GetCleanName()); //deleteme
		}
		else {
			while (shard_add_counter >= 1) {
				if (shard_amount >= shardItem->StackSize) {
					AddItem(RuleI(Vegas, ShardItemID), shardItem->StackSize, RuleB(VegasLoot, EquipVegasDrops));
					LogVegasLoot("Added [{}]x [{}] to [{}] for [{}]", shardItem->StackSize, database.CreateItemLink(RuleI(Vegas, ShardItemID)), GetCleanName(), top_client->GetCleanName()); //deleteme
					shard_amount = shard_amount - shardItem->StackSize;
				}

				if (shard_amount < shardItem->StackSize) {
					AddItem(RuleI(Vegas, ShardItemID), shard_amount, RuleB(VegasLoot, EquipVegasDrops));
					LogVegasLoot("Added [{}x] [{}] to [{}] for [{}]", shard_amount, database.CreateItemLink(RuleI(Vegas, ShardItemID)), GetCleanName(), top_client->GetCleanName()); //deleteme
				}

				--shard_add_counter;
			}
		}
	}
}

int NPC::GetShardAmount() {
	int shard_amount;
	float npc_diff = GetDifficulty();

	if ((npc_diff / sqrt(npc_diff)) <= 75) {
		shard_amount = npc_diff / sqrt(npc_diff);
	}
	else {
		shard_amount = (((npc_diff / sqrt(npc_diff)) / 10) + 75);
	}

	shard_amount *= std::max(0.01f, RuleR(VegasLoot, ShardMultiplier));

	return shard_amount;
}

void NPC::AttemptVegasEpicTokenLoot(Mob* top_client) {
	int roll_count;
	int roll_max = 1;
	int roll_to_hit = 1;
	std::string query;
	std::string vegas_loot_type;
	std::string npc_name = GetCleanName();

	if (IsRaidTarget() && !RaidTargetsNotCountAsRaid()) {
		roll_max    = RuleI(VegasLoot, RaidTargetEpicTokenRollMax);
		roll_to_hit = static_cast<int>((roll_max / std::max(0.01f, RuleR(VegasLoot, EpicTokenRaidRollMultiplier))) - (GetShardAmount() / std::max(0.01f, RuleR(VegasLoot, ShardMultiplier))));
		roll_count  = RuleI(VegasLoot, RaidTargetEpicTokenRollCount);
	}
	else if (IsRareSpawn()) {
		roll_max    = RuleI(VegasLoot, RareTargetEpicTokenRollMax);
		roll_to_hit = static_cast<int>(roll_max / std::max(0.01f, RuleR(VegasLoot, EpicTokenRareRollMultiplier)));
		roll_count  = RuleI(VegasLoot, RareTargetEpicTokenRollCount);
	}
	else {
		roll_max    = RuleI(VegasLoot, EpicTokenRollMax);
		roll_to_hit = static_cast<int>(roll_max / std::max(0.01f, RuleR(VegasLoot, EpicTokenMultiplier)));
		roll_count  = RuleI(VegasLoot, NormalTargetEpicTokenRollCount);
	}

	int rolled = zone->random.Int(1, roll_max);
	int to_hit = std::min(roll_max, roll_to_hit);

	while (roll_count >= 1) {
		LogVegasLoot("[Tries left: {}] Roll was [{}] To Hit [{}] on [{}] [ID: {}] For [{}] [ID: {}]", (roll_count - 1), rolled, to_hit, GetCleanName(), GetNPCTypeID(), top_client->GetCleanName(), top_client->CastToClient()->CharacterID()); //deleteme
		if (rolled >= to_hit) {
			AddItem(RuleI(Vegas, EpicTokenItemID), 1, false);
			LogVegasLoot("Epic Token hit! Added an [{}] to [{}] [ID: {}] For [{}] [ID: {}] Roll was [{}] To Hit [{}]", database.CreateItemLink(RuleI(Vegas, EpicTokenItemID)), GetCleanName(), GetNPCTypeID(), top_client->GetCleanName(), top_client->CastToClient()->CharacterID(), rolled, to_hit); //deleteme

			if (top_client->CastToClient()->Admin() < RuleI(VegasLoot, MinStatusToBypassVegasLootLogging)) {
				std::string key = "EpicTokenCount";
				std::string bucket_value = DataBucket::GetData(&database, key);
				if (Strings::IsNumber(bucket_value) && Strings::ToUnsignedInt(bucket_value) > 0) {
					DataBucket::SetData(&database, key, std::to_string(Strings::ToUnsignedInt(bucket_value) + 1));
				}
				else {
					DataBucket::SetData(&database, key, std::to_string(1));
				}
			}

			//Discord send
			if (top_client->CastToClient()->Admin() < RuleI(VegasLoot, MinStatusToBypassVegasLootDiscordLogging)) {
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
			if (top_client->CastToClient()->Admin() < RuleI(VegasLoot, MinStatusToBypassVegasLootLogging)) {
				raid_target && !RaidTargetsNotCountAsRaid() ? vegas_loot_type = "Raid" : vegas_loot_type = "Non-Raid";
				query = StringFormat(
					"INSERT INTO rng_tokens "
					"SET `npc_id` = '%i', `npc_name` = '%s', `npc_level` = '%i', `type` = '%s', `npc_diff` = '%f', `roll` = '%i', `tohit` = '%i', `zone_id` = '%i', `zone` = '%s', `zone_instance_id` = '%i', `time` = NOW(), `charid` = '%i', `char` = '%s', `char_level` = '%i' ",
					GetNPCTypeID(), Strings::Escape(GetCleanName()).c_str(), GetLevel(), vegas_loot_type, double(difficulty), rolled, to_hit, zone->GetZoneID(), zone->GetShortName(), zone->GetInstanceID(), top_client->CastToClient()->CharacterID(), top_client->CastToClient()->GetCleanName(), top_client->CastToClient()->GetLevel());
				QueryVegasLoot(query, top_client->CastToClient()->GetCleanName(), GetCleanName());
			}
		}
		else {
			if (IsRaidTarget() && !RaidTargetsNotCountAsRaid()) {
				//Discord send
				if (top_client->CastToClient()->Admin() < RuleI(VegasLoot, MinStatusToBypassVegasLootDiscordLogging)) {
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

void NPC::AttemptVegasSpellLoot(Mob* top_client) {
	auto spell_tier_min_levels = Strings::GetFloatVectorFromDelimitedString(RuleS(VegasLoot, VegasSpellTiersMinLevels), ",");
	auto spell_tier_max_levels = Strings::GetFloatVectorFromDelimitedString(RuleS(VegasLoot, VegasSpellTiersMaxLevels), ",");
	auto spell_tier_lootdrop_ids = Strings::GetFloatVectorFromDelimitedString(RuleS(VegasLoot, VegasSpellTiersLootDropIDs), ",");
	auto spell_tier_bonus_low_lootdrop_ids = Strings::GetFloatVectorFromDelimitedString(RuleS(VegasLoot, VegasSpellTiersBonusLowLootDropIDs), ",");
	auto spell_tier_bonus_high_lootdrop_ids = Strings::GetFloatVectorFromDelimitedString(RuleS(VegasLoot, VegasSpellTiersBonusHighLootDropIDs), ",");
	int roll_count = RuleI(VegasLoot, VegasSpellRollCount);
	int roll_max = int(RuleI(VegasLoot, VegasSpellRollMax) / std::max(0.01f, RuleR(VegasLoot, SpellMultiplier)));
	auto npc_level = GetLevel();
	int rolled;
	std::string vegas_loot_type;

	//LogVegasLootDetail("INFO: Rule sizes - spell_tier_min_levels ({}), spell_tier_max_levels ({}), spell_tier_lootdrop_ids ({}), spell_tier_bonus_low_lootdrop_ids ({}), spell_tier_bonus_high_lootdrop_ids ({}).", spell_tier_min_levels.size(), spell_tier_max_levels.size(), spell_tier_lootdrop_ids.size(), spell_tier_bonus_low_lootdrop_ids.size(), spell_tier_bonus_high_lootdrop_ids.size()); // Debug

	if (
		spell_tier_min_levels.size() == spell_tier_max_levels.size() &&
		spell_tier_lootdrop_ids.size() == spell_tier_bonus_low_lootdrop_ids.size() &&
		spell_tier_bonus_high_lootdrop_ids.size() == spell_tier_min_levels.size()
	) {
		while (roll_count >= 1) {
			const std::vector<float>* chosen_type = nullptr;

			rolled = zone->random.Int(1, roll_max);

			if (rolled >= (RuleI(VegasLoot, VegasSpellRollMaxHit) / std::max(0.01f, RuleR(VegasLoot, SpellMultiplier)))) {
				LogVegasLoot("Max Spell Roll hit on [{}] for [{}]. Roll [{}] | To Hit [{}].", GetCleanName(), top_client->GetCleanName(), rolled, RuleI(VegasLoot, VegasSpellRollMaxHit)); // Debug
				raid_target && !RaidTargetsNotCountAsRaid() ? vegas_loot_type = "Raid-Max" : rare_spawn ? vegas_loot_type = "Rare-Max" : vegas_loot_type = "Common-Max";

				chosen_type = &spell_tier_bonus_high_lootdrop_ids;
			}
			else if (rolled >= (RuleI(VegasLoot, VegasSpellRollNormalHit) / std::max(0.01f, RuleR(VegasLoot, SpellMultiplier)))) {
				LogVegasLoot("Normal Spell Roll hit on [{}] for [{}]. Roll [{}] | To Hit [{}].", GetCleanName(), top_client->GetCleanName(), rolled, RuleI(VegasLoot, VegasSpellRollNormalHit)); // Debug
				raid_target && !RaidTargetsNotCountAsRaid() ? vegas_loot_type = "Raid-Normal" : rare_spawn ? vegas_loot_type = "Rare-Normal" : vegas_loot_type = "Common-Normal";

				chosen_type = &spell_tier_lootdrop_ids;
			}
			else if (rolled >= (RuleI(VegasLoot, VegasSpellRollMinHit) / std::max(0.01f, RuleR(VegasLoot, SpellMultiplier)))) {
				LogVegasLoot("Min Spell Roll hit on [{}] for [{}]. Roll [{}] | To Hit [{}].", GetCleanName(), top_client->GetCleanName(), rolled, RuleI(VegasLoot, VegasSpellRollMinHit)); // Debug
				raid_target && !RaidTargetsNotCountAsRaid() ? vegas_loot_type = "Raid-Min" : rare_spawn ? vegas_loot_type = "Rare-Min" : vegas_loot_type = "Common-Min";

				chosen_type = &spell_tier_bonus_low_lootdrop_ids;
			}
			else {
				--roll_count;

				continue;
			}

			if (chosen_type->size() != spell_tier_min_levels.size()) {
				LogVegasLoot("ERROR: chosen_type size doesn't match the rules."); // Debug

				return;
			}

			for (int i = 0; i < chosen_type->size(); i++) {
				uint32 x = static_cast<uint32>((*chosen_type)[i]);

				if (x <= 0) {
					continue;
				}

				if (spell_tier_min_levels[i] <= 0 || spell_tier_max_levels[i] <= 0) {
					LogVegasLoot("Invalid tier [{}]: min_level=[{}], max_level=[{}], chosen_type[{}]=[{}], skipping.", i, spell_tier_min_levels[i], spell_tier_max_levels[i], i, x); // Debug

					continue;
				}

				if (npc_level >= spell_tier_min_levels[i] && npc_level <= spell_tier_max_levels[i]) {
					const EQ::ItemData* chosen_item = nullptr;
					auto chosen_loot_drop = zone->GetLootdrop(x);
					const auto chosen_loot_drop_entries = zone->GetLootdropEntries(x);

					if (chosen_loot_drop.id == 0 || chosen_loot_drop_entries.empty()) {
						continue;
					}

					chosen_item = database.GetItem(chosen_loot_drop_entries[zone->random.Int(0, chosen_loot_drop_entries.size())].item_id);

					if (!chosen_item) {
						break;
					}

					AddItem(chosen_item->ID, 1, RuleB(VegasLoot, EquipVegasDrops));
					LogVegasLoot("Added spell [{}] to [{}] for [{}]", database.CreateItemLink(chosen_item->ID), GetCleanName(), top_client->GetCleanName());

					//Discord send
					if (top_client->CastToClient()->Admin() < RuleI(VegasLoot, MinStatusToBypassVegasLootDiscordLogging)) {
						std::ostringstream output;
						output <<
							"Added [" << vegas_loot_type  << "] [" << chosen_item->Name << "](http://vegaseq.com/Allaclone/?a=item&id=" << chosen_item->ID << ") [ID: " << chosen_item->ID << "] [" << (IsRaidTarget() ? "Raid" : IsRareSpawn() ? "Rare" : "Common") << "]"
							" to [" << GetCleanName() << "](http://vegaseq.com/Allaclone/?a=npc&id=" << GetNPCTypeID() << ") [ID: " << GetNPCTypeID() << "] [Level: " << int(npc_level) << "] [MobDiff: " << int(difficulty) << "]"
							" For [" << top_client->GetCleanName() << "](http://vegaseq.com/charbrowser/index.php?page=character&char=" << top_client->GetCleanName() << ") [ID: " << top_client->CastToClient()->CharacterID() << "] [Level: " << int(top_client->CastToClient()->GetLevel()) << "]"
							" in [" << zone->GetLongName() << "](http://vegaseq.com/Allaclone/?a=zone&name=" << zone->GetShortName() << ") [Instance: " << zone->GetInstanceID() << "]"
							;
						std::string out_final = output.str();

						zone->SendDiscordMessage("rngspells", out_final);
					}

					//Database Logging
					if (top_client->CastToClient()->Admin() < RuleI(VegasLoot, MinStatusToBypassVegasLootLogging)) {
						std::string query;
						std::string item_name;
						item_name = chosen_item->Name;
						//std::replace(item_name.begin(), item_name.end(), "\`", "\'");

						query = StringFormat(
							"INSERT INTO rng_spells "
							"SET `npc_id` = '%i', `npc_name` = '%s', `npc_level` = '%i', `type` = '%s', `npc_diff` = '%f', `item_id` = '%i', `raid_item` = '%i', `item` = '%s', `item_diff` = '%f', `zone_id` = '%i', `zone` = '%s', `zone_instance_id` = '%i', `time` = NOW(), `charid` = '%i', `char` = '%s', `char_level` = '%i' ",
							GetNPCTypeID(), Strings::Escape(GetCleanName()).c_str(), npc_level, vegas_loot_type, double(difficulty), chosen_item->ID, 0, Strings::Escape(chosen_item->Name).c_str(), double(chosen_item->Difficulty), GetZoneID(), zone->GetShortName(), zone->GetInstanceID(), top_client->CastToClient()->CharacterID(), top_client->CastToClient()->GetCleanName(), top_client->CastToClient()->GetLevel());
						QueryVegasLoot(query, top_client->CastToClient()->GetCleanName(), GetCleanName());
					}

					break;
				}
			}

			--roll_count;
		}
	}
	else {
		LogVegasLoot("ERROR: Spell Tier rules are mismatched sizes."); // Debug
	}
}

void NPC::AttemptVegasBagLoot(Mob* top_client) {
	std::vector<float> bag_tier_min_levels = Strings::GetFloatVectorFromDelimitedString(RuleS(VegasLoot, VegasBagTiersMinLevels), ",");
	std::vector<float> bag_tier_max_levels = Strings::GetFloatVectorFromDelimitedString(RuleS(VegasLoot, VegasBagTiersMaxLevels), ",");
	std::vector<float> bag_tier_lootdrop_ids = Strings::GetFloatVectorFromDelimitedString(RuleS(VegasLoot, VegasBagTiersLootDropIDs), ",");
	int roll_count = RuleI(VegasLoot, VegasBagRollCount);
	int roll_max = int(RuleI(VegasLoot, VegasBagRollMax) / std::max(0.01f, RuleR(VegasLoot, BagMultiplier)));
	auto npc_level = GetLevel();
	int rolled;
	std::string vegas_loot_type = raid_target && !RaidTargetsNotCountAsRaid() ? "Raid-Bag" : rare_spawn ? "Rare-Bag" : "Common-Bag";


	//LogVegasLootDetail("INFO: Rule sizes - bag_tier_min_levels ({}), bag_tier_max_levels ({}), bag_tier_lootdrop_ids ({}).", bag_tier_min_levels.size(), bag_tier_max_levels.size(), bag_tier_lootdrop_ids.size()); // Debug

	if (
		bag_tier_min_levels.size() == bag_tier_max_levels.size() &&
		bag_tier_max_levels.size() == bag_tier_lootdrop_ids.size()
	) {
		while (roll_count >= 1) {
			rolled = zone->random.Int(1, roll_max);

			if (rolled >= roll_max) {
				for (int i = 0; i < bag_tier_lootdrop_ids.size(); i++) {
					uint32 x = static_cast<uint32>((bag_tier_lootdrop_ids)[i]);

					if (x <= 0) {
						continue;
					}

					if (bag_tier_min_levels[i] <= 0 || bag_tier_max_levels[i] <= 0) {
						LogVegasLoot("Invalid tier [{}]: min_level=[{}], max_level=[{}], chosen_type[{}]=[{}], skipping.", i, bag_tier_min_levels[i], bag_tier_max_levels[i], i, x); // Debug

						continue;
					}

					if (npc_level >= bag_tier_min_levels[i] && npc_level <= bag_tier_max_levels[i]) {
						const EQ::ItemData* chosen_item = nullptr;
						auto chosen_loot_drop = zone->GetLootdrop(x);
						const auto chosen_loot_drop_entries = zone->GetLootdropEntries(x);

						if (chosen_loot_drop.id == 0 || chosen_loot_drop_entries.empty()) {
							continue;
						}

						chosen_item = database.GetItem(chosen_loot_drop_entries[zone->random.Int(0, chosen_loot_drop_entries.size())].item_id);

						if (!chosen_item) {
							break;
						}

						AddItem(chosen_item->ID, 1, RuleB(VegasLoot, EquipVegasDrops));
						LogVegasLoot("Added bag [{}] to [{}] for [{}]", database.CreateItemLink(chosen_item->ID), GetCleanName(), top_client->GetCleanName());

						//Discord send
						if (top_client->CastToClient()->Admin() < RuleI(VegasLoot, MinStatusToBypassVegasLootDiscordLogging)) {
							std::ostringstream output;
							output <<
								"Added [" << chosen_item->Name << "](http://vegaseq.com/Allaclone/?a=item&id=" << chosen_item->ID << ") [ID: " << chosen_item->ID << "] [" << (IsRaidTarget() ? "Raid" : IsRareSpawn() ? "Rare" : "Common") << "]"
								" to [" << GetCleanName() << "](http://vegaseq.com/Allaclone/?a=npc&id=" << GetNPCTypeID() << ") [ID: " << GetNPCTypeID() << "] [Level: " << int(npc_level) << "] [MobDiff: " << int(difficulty) << "]"
								" For [" << top_client->GetCleanName() << "](http://vegaseq.com/charbrowser/index.php?page=character&char=" << top_client->GetCleanName() << ") [ID: " << top_client->CastToClient()->CharacterID() << "] [Level: " << int(top_client->CastToClient()->GetLevel()) << "]"
								" in [" << zone->GetLongName() << "](http://vegaseq.com/Allaclone/?a=zone&name=" << zone->GetShortName() << ") [Instance: " << zone->GetInstanceID() << "]"
								;
							std::string out_final = output.str();

							zone->SendDiscordMessage("rngbags", out_final);
						}

						//Database Logging
						if (top_client->CastToClient()->Admin() < RuleI(VegasLoot, MinStatusToBypassVegasLootLogging)) {
							std::string query;
							std::string item_name;
							item_name = chosen_item->Name;
							//std::replace(item_name.begin(), item_name.end(), "\`", "\'");
							query = StringFormat(
								"INSERT INTO rng_bags "
								"SET `npc_id` = '%i', `npc_name` = '%s', `npc_level` = '%i', `type` = '%s', `npc_diff` = '%f', `item_id` = '%i', `raid_item` = '%i', `item` = '%s', `item_diff` = '%f', `zone_id` = '%i', `zone` = '%s', `zone_instance_id` = '%i', `time` = NOW(), `charid` = '%i', `char` = '%s', `char_level` = '%i' ",
								GetNPCTypeID(), Strings::Escape(GetCleanName()).c_str(), npc_level, vegas_loot_type, double(difficulty), chosen_item->ID, 0, Strings::Escape(chosen_item->Name).c_str(), double(chosen_item->Difficulty), GetZoneID(), zone->GetShortName(), zone->GetInstanceID(), top_client->CastToClient()->CharacterID(), top_client->CastToClient()->GetCleanName(), top_client->CastToClient()->GetLevel());
							QueryVegasLoot(query, top_client->CastToClient()->GetCleanName(), GetCleanName());
						}

						break;
					}
				}
			}

			--roll_count;
		}
	}
}

void NPC::AttemptVegasBridleLoot(Mob* top_client) {
	std::vector<float> bridle_tier_min_levels = Strings::GetFloatVectorFromDelimitedString(RuleS(VegasLoot, VegasBridleTiersMinLevels), ",");
	std::vector<float> bridle_tier_max_levels = Strings::GetFloatVectorFromDelimitedString(RuleS(VegasLoot, VegasBridleTiersMaxLevels), ",");
	std::vector<float> bridle_tier_lootdrop_ids = Strings::GetFloatVectorFromDelimitedString(RuleS(VegasLoot, VegasBridleTiersLootDropIDs), ",");
	int roll_count = RuleI(VegasLoot, VegasBridleRollCount);
	int roll_max = int(RuleI(VegasLoot, VegasBridleRollMax) / std::max(0.01f, RuleR(VegasLoot, BridleMultiplier)));
	auto npc_level = GetLevel();
	int rolled;
	std::string vegas_loot_type = raid_target && !RaidTargetsNotCountAsRaid() ? "Raid-Bridle" : rare_spawn ? "Rare-Bridle" : "Common-Bridle";


	//LogVegasLootDetail("INFO: Rule sizes - bridle_tier_min_levels ({}), bridle_tier_max_levels ({}), bridle_tier_lootdrop_ids ({}).", bridle_tier_min_levels.size(), bridle_tier_max_levels.size(), bridle_tier_lootdrop_ids.size()); // Debug

	if (
		bridle_tier_min_levels.size() == bridle_tier_max_levels.size() &&
		bridle_tier_max_levels.size() == bridle_tier_lootdrop_ids.size()
	) {
		while (roll_count >= 1) {
			rolled = zone->random.Int(1, roll_max);

			if (rolled >= roll_max) {
				for (int i = 0; i < bridle_tier_lootdrop_ids.size(); i++) {
					uint32 x = static_cast<uint32>((bridle_tier_lootdrop_ids)[i]);

					if (x <= 0) {
						continue;
					}

					if (bridle_tier_min_levels[i] <= 0 || bridle_tier_max_levels[i] <= 0) {
						LogVegasLoot("Invalid tier [{}]: min_level=[{}], max_level=[{}], chosen_type[{}]=[{}], skipping.", i, bridle_tier_min_levels[i], bridle_tier_max_levels[i], i, x); // Debug

						continue;
					}

					if (npc_level >= bridle_tier_min_levels[i] && npc_level <= bridle_tier_max_levels[i]) {
						const EQ::ItemData* chosen_item = nullptr;
						auto chosen_loot_drop = zone->GetLootdrop(x);
						const auto chosen_loot_drop_entries = zone->GetLootdropEntries(x);

						if (chosen_loot_drop.id == 0 || chosen_loot_drop_entries.empty()) {
							continue;
						}

						chosen_item = database.GetItem(chosen_loot_drop_entries[zone->random.Int(0, chosen_loot_drop_entries.size())].item_id);

						if (!chosen_item) {
							break;
						}

						AddItem(chosen_item->ID, 1, RuleB(VegasLoot, EquipVegasDrops));
						LogVegasLoot("Added bridle [{}] to [{}] for [{}]", database.CreateItemLink(chosen_item->ID), GetCleanName(), top_client->GetCleanName());

						//Discord send
						if (top_client->CastToClient()->Admin() < RuleI(VegasLoot, MinStatusToBypassVegasLootDiscordLogging)) {
							std::ostringstream output;
							output <<
								"Added [" << chosen_item->Name << "](http://vegaseq.com/Allaclone/?a=item&id=" << chosen_item->ID << ") [ID: " << chosen_item->ID << "] [" << (IsRaidTarget() ? "Raid" : IsRareSpawn() ? "Rare" : "Common") << "]"
								" to [" << GetCleanName() << "](http://vegaseq.com/Allaclone/?a=npc&id=" << GetNPCTypeID() << ") [ID: " << GetNPCTypeID() << "] [Level: " << int(npc_level) << "] [MobDiff: " << int(difficulty) << "]"
								" For [" << top_client->GetCleanName() << "](http://vegaseq.com/charbrowser/index.php?page=character&char=" << top_client->GetCleanName() << ") [ID: " << top_client->CastToClient()->CharacterID() << "] [Level: " << int(top_client->CastToClient()->GetLevel()) << "]"
								" in [" << zone->GetLongName() << "](http://vegaseq.com/Allaclone/?a=zone&name=" << zone->GetShortName() << ") [Instance: " << zone->GetInstanceID() << "]"
								;
							std::string out_final = output.str();

							zone->SendDiscordMessage("rngbridles", out_final);
						}

						//Database Logging
						if (top_client->CastToClient()->Admin() < RuleI(VegasLoot, MinStatusToBypassVegasLootLogging)) {
							std::string query;
							std::string item_name;
							item_name = chosen_item->Name;
							//std::replace(item_name.begin(), item_name.end(), "\`", "\'");
							query = StringFormat(
								"INSERT INTO rng_bridles "
								"SET `npc_id` = '%i', `npc_name` = '%s', `npc_level` = '%i', `type` = '%s', `npc_diff` = '%f', `item_id` = '%i', `raid_item` = '%i', `item` = '%s', `item_diff` = '%f', `zone_id` = '%i', `zone` = '%s', `zone_instance_id` = '%i', `time` = NOW(), `charid` = '%i', `char` = '%s', `char_level` = '%i' ",
								GetNPCTypeID(), Strings::Escape(GetCleanName()).c_str(), npc_level, vegas_loot_type, double(difficulty), chosen_item->ID, 0, Strings::Escape(chosen_item->Name).c_str(), double(chosen_item->Difficulty), GetZoneID(), zone->GetShortName(), zone->GetInstanceID(), top_client->CastToClient()->CharacterID(), top_client->CastToClient()->GetCleanName(), top_client->CastToClient()->GetLevel());
							QueryVegasLoot(query, top_client->CastToClient()->GetCleanName(), GetCleanName());
						}

						break;
					}
				}
			}

			--roll_count;
		}
	}
}

void NPC::QueryVegasLoot(std::string query, std::string killer_name, std::string killed_name)
{
	if (query.empty()) {
		Logs::MySQLError, "Failed to save Vegas loot database information. Query was empty.";
		return;
	}

	LogVegasLootDetail("VegasQueryLoot::[{}]", query); //deleteme

	auto results = database.QueryDatabase(query);

	if (!results.Success()) {
		Logs::MySQLError, "Failed to save database information for [{}] on killing [{}]", killer_name, killed_name;
	}
}

void NPC::AddVegasItem(Mob* top_client, const EQ::ItemData* chosen_item, std::string loot_type, std::string added_type, bool raid_only, bool equip) {
	std::string query;
	std::string npc_name = GetCleanName();

	AddItem(chosen_item, chosen_item->MaxCharges, equip);
	LogVegasLoot("Added [{}] to [{}] for [{}]", database.CreateItemLink(chosen_item->ID), GetCleanName(), top_client->GetCleanName());

	if (top_client->CastToClient()->Admin() < RuleI(VegasLoot, MinStatusToBypassVegasLootLogging)) {
		std::string key = "TotalRandomDrops";
		std::string bucket_value = DataBucket::GetData(&database, key);

		if (Strings::IsNumber(bucket_value) && Strings::ToUnsignedInt(bucket_value) > 0) {
			DataBucket::SetData(&database, key, std::to_string(Strings::ToUnsignedInt(bucket_value) + 1));
		}
		else {
			DataBucket::SetData(&database, key, std::to_string(1));
		}
	}

	//Discord send
	if (top_client->CastToClient()->Admin() < RuleI(VegasLoot, MinStatusToBypassVegasLootDiscordLogging)) {
		std::ostringstream output;
		output <<
			"" << added_type.c_str() << " [" << chosen_item->Name << "](http://vegaseq.com/Allaclone/?a=item&id=" << chosen_item->ID << ") [ID: " << chosen_item->ID << "] [GS:" << int(chosen_item->GearScore) << "] [" << (IsRaidTarget() ? "Raid-Bonus" : IsRareSpawn() ? "Rare-Bonus" : "Common-Bonus") << "]"
			" to [" << GetCleanName() << "](http://vegaseq.com/Allaclone/?a=npc&id=" << GetNPCTypeID() << ") [ID: " << GetNPCTypeID() << "] [Level: " << int(GetLevel()) << "] [MobDiff: " << int(difficulty) << "]"
			" For [" << top_client->GetCleanName() << "](http://vegaseq.com/charbrowser/index.php?page=character&char=" << top_client->GetCleanName() << ") [ID: " << top_client->CastToClient()->CharacterID() << "] [Level: " << int(top_client->CastToClient()->GetLevel()) << "]"
			" in [" << zone->GetLongName() << "](http://vegaseq.com/Allaclone/?a=zone&name=" << zone->GetShortName() << ") [Instance: " << zone->GetInstanceID() << "]"
			;
		std::string out_final = output.str();
		zone->SendDiscordMessage("rngitems", out_final);
	}

	//Database Logging
	if (top_client->CastToClient()->Admin() < RuleI(VegasLoot, MinStatusToBypassVegasLootLogging)) {
		std::string item_name = chosen_item->Name;
		//std::replace(item_name.begin(), item_name.end(), "\`", "\'");
		query = StringFormat(
			"INSERT INTO rng_items "
			"SET `npc_id` = '%i', `npc_name` = '%s', `npc_level` = '%i', `type` = '%s', `npc_diff` = '%f', `item_id` = '%i', `raid_item` = '%i', `item` = '%s', `item_diff` = '%f', `item_gearscore` = '%f', `zone_id` = '%i', `zone` = '%s', `zone_instance_id` = '%i', `time` = NOW(), `charid` = '%i', `char` = '%s', `char_level` = '%i' ",
			GetNPCTypeID(), Strings::Escape(GetCleanName()).c_str(), GetLevel(), loot_type, double(difficulty), chosen_item->ID, raid_only, Strings::Escape(chosen_item->Name).c_str(), double(chosen_item->Difficulty), double(chosen_item->GearScore), GetZoneID(), zone->GetShortName(), zone->GetInstanceID(), top_client->CastToClient()->CharacterID(), top_client->CastToClient()->GetCleanName(), top_client->CastToClient()->GetLevel());
		QueryVegasLoot(query, top_client->CastToClient()->GetCleanName(), GetCleanName());
	}
}
