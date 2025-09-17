#include <ranges>

#include "../bot_command.h"
#include "boost/mpl/vector/aux_/empty.hpp"

void bot_command_item_use(Client* c, const Seperator* sep)
{
	if (helper_is_help_or_usage(sep->arg[1])) {
		BotCommandHelpParams p;

		p.description = { "Outputs a list of bots that can use the item on your cursor."};
		p.notes = {
			fmt::format(
				"- If you are swapping an existing item on a bot, it is recommended to remove both the item from your cursor and the item from the bot before attempting to give the bot the item to prevent desyncs.",
				"- If you are attempting to give an item that be used in multiple slots, be sure the slot you intend to give it to is empty first.",
				"- You can combine multiple filter types."
			)
		};
		p.example_format ={ fmt::format("{} [filter] [actionable]", sep->arg[0]) };
		p.examples_one =
		{
			"To check only plate wearing bots:",
			fmt::format(
				"{} plate",
				sep->arg[0]
			)
		};
		p.examples_two =
		{
			"To check only upgrades based on gearscore for Warriors:",
			fmt::format(
				"{} gearscore byclass 1",
				sep->arg[0]
			)
		};
		p.examples_three =
		{
			"To check both gearscore upgrades and melee upgrades:",
			fmt::format(
				"{} gearscore melee",
				sep->arg[0]
			)
		};
		p.actionables = { "target, byname, ownergroup, ownerraid, targetgroup, namesgroup, healrotationtargets, mmr, byclass, byrace, spawned" };
		p.options = { "empty, caster, hybrid, melee, wiscaster, intcaster, plate, chain, leather, cloth, haste, gearscore" };
		p.options_one =
		{
			"[empty] search empty slots",
			"[caster] search only caster bots",
			"[hybrid] search only hybrid bots",
			"[melee] search only melee bots",
			"[wiscaster] search only Wisdom-based Caster bots",
			"[intcaster] search only Intelligence-based Caster bots",
			"[plate] search only Plate-wearing bots",
			"[chain] search only Chain-wearing bots",
			"[leather] search only Leather-wearing bots",
			"[cloth] search only Cloth-wearing bots",
			"[haste] search only bots that have a lesser haste value than the item",
			"[gearscore] search only bots that have a lesser gearscore than the item in the equippable slot"
		};

		std::string popup_text = c->SendBotCommandHelpWindow(p);
		popup_text = DialogueWindow::Table(popup_text);

		c->SendPopupToClient(sep->arg[0], popup_text.c_str());

		if (RuleB(Bots, SendClassRaceOnHelp)) {
			c->Message(
				Chat::Yellow,
				fmt::format(
					"Use {} for information about race/class IDs.",
					Saylink::Silent("^classracelist")
				).c_str()
			);
		}

		return;
	}

	constexpr int Filter_Empty      = 0;
	constexpr int Filter_Caster     = 1;
	constexpr int Filter_Hybrid     = 2;
	constexpr int Filter_Melee      = 3;
	constexpr int Filter_WisCaster  = 4;
	constexpr int Filter_IntCaster  = 5;
	constexpr int Filter_Plate      = 6;
	constexpr int Filter_Chain      = 7;
	constexpr int Filter_Leather    = 8;
	constexpr int Filter_Cloth      = 9;
	constexpr int Filter_Haste      = 10;
	constexpr int Filter_GearScore  = 11;

	struct Cmd {
		std::string cmd{}; // command
		int id; // id
	};

	std::vector<Cmd> filters = {
		Cmd{.cmd = "empty", .id = Filter_Empty },
		Cmd{.cmd = "caster", .id = Filter_Caster },
		Cmd{.cmd = "hybrid", .id = Filter_Hybrid },
		Cmd{.cmd = "melee", .id = Filter_Melee },
		Cmd{.cmd = "wiscaster", .id = Filter_WisCaster },
		Cmd{.cmd = "intcaster", .id = Filter_IntCaster },
		Cmd{.cmd = "plate", .id = Filter_Plate },
		Cmd{.cmd = "chain", .id = Filter_Chain },
		Cmd{.cmd = "leather", .id = Filter_Leather },
		Cmd{.cmd = "cloth", .id = Filter_Cloth },
		Cmd{.cmd = "haste", .id = Filter_Haste },
		Cmd{.cmd = "gearscore", .id = Filter_GearScore },
	};

	bool empty_only = false;
	int8 class_mask = 0;
	bool caster_only = false;
	bool hybrid_only = false;
	bool melee_only = false;
	bool wis_caster_only = false;
	bool int_caster_only = false;
	bool plate_only = false;
	bool chain_only = false;
	bool leather_only = false;
	bool cloth_only = false;
	bool haste_only = false;
	int haste_value = 0;
	bool gear_score_only = false;
	int ab_arg = 2;
	bool valid_filter = false;

	for (int i = 1; i <= sep->argnum; ++i) {
		for (const auto &cmd: filters) {
			if (cmd.cmd == Strings::ToLower(sep->arg[i])) {
				switch (cmd.id) {
					case Filter_Empty:
						empty_only = true;
						break;
					case Filter_Caster:
						caster_only = true;
						break;
					case Filter_Hybrid:
						hybrid_only = true;
						break;
					case Filter_Melee:
						melee_only = true;
						break;
					case Filter_WisCaster:
						wis_caster_only = true;
						break;
					case Filter_IntCaster:
						int_caster_only = true;
						break;
					case Filter_Plate:
						plate_only = true;
						break;
					case Filter_Chain:
						chain_only = true;
						break;
					case Filter_Leather:
						leather_only = true;
						break;
					case Filter_Cloth:
						cloth_only = true;
						break;
					case Filter_Haste:
						haste_only = true;
						break;
					case Filter_GearScore:
						gear_score_only = true;
						break;
				}

				if (i > 1) {
					++ab_arg;
				}

				valid_filter = true;
			}
		}
	}

	if (!valid_filter) {
		--ab_arg;
	}

	const auto item_instance = c->GetInv().GetItem(EQ::invslot::slotCursor);

	if (!item_instance) {
		c->Message(Chat::Yellow, "No item found on cursor! For help use %s help.", sep->arg[0]);

		return;
	}

	auto item_data = item_instance->GetItem();

	if (!item_data) {
		c->Message(Chat::Yellow, "No data found for cursor item!");

		return;
	}

	if (item_data->ItemClass != EQ::item::ItemClassCommon || item_data->Slots == 0) {
		c->Message(Chat::Yellow, "'%s' is not an equipable item!", item_data->Name);

		return;
	}

	std::vector<int16> equipable_slot_list;

	for (int16 equipable_slot = EQ::invslot::EQUIPMENT_BEGIN; equipable_slot <= EQ::invslot::EQUIPMENT_END; ++equipable_slot) {
		if (item_data->Slots & (1 << equipable_slot)) {
			equipable_slot_list.emplace_back(equipable_slot);
		}
	}

	const int ab_mask = ActionableBots::ABM_Type1;
	std::string actionable_arg = sep->arg[ab_arg];

	if (actionable_arg.empty()) {
		actionable_arg = "spawned";
	}

	std::string class_race_arg = sep->arg[ab_arg];
	bool class_race_check = false;

	if (!class_race_arg.compare("byclass") || !class_race_arg.compare("byrace")) {
		class_race_check = true;
	}

	std::vector<Bot*> sbl;

	if (ActionableBots::PopulateSBL(c, actionable_arg, sbl, ab_mask, !class_race_check ? sep->arg[ab_arg + 1] : nullptr, class_race_check ? atoi(sep->arg[ab_arg + 1]) : 0) == ActionableBots::ABT_None) {
		return;
	}

	sbl.erase(std::remove(sbl.begin(), sbl.end(), nullptr), sbl.end());

	EQ::SayLinkEngine linker;
	linker.SetLinkType(EQ::saylink::SayLinkItemData);

	for (const auto& bot_iter : sbl) {
		if (!bot_iter) {
			continue;
		}

		if (caster_only && !IsCasterClass(bot_iter->GetClass())) {
			continue;
		}

		if (hybrid_only && !IsSpellFighterClass(bot_iter->GetClass())) {
			continue;
		}

		if (melee_only && !IsNonSpellFighterClass(bot_iter->GetClass())) {
			continue;
		}

		if (wis_caster_only && !IsWISCasterClass(bot_iter->GetClass())) {
			continue;
		}

		if (int_caster_only && !IsINTCasterClass(bot_iter->GetClass())) {
			continue;
		}

		if (plate_only && !IsPlateClass(bot_iter->GetClass())) {
			continue;
		}

		if (chain_only && !IsChainClass(bot_iter->GetClass())) {
			continue;
		}

		if (leather_only && !IsLeatherClass(bot_iter->GetClass())) {
			continue;
		}

		if (cloth_only && !IsClothClass(bot_iter->GetClass())) {
			continue;
		}

		if (
			(!RuleB(Bots, AllowBotEquipAnyRaceGear) && ((~item_data->Races) & GetPlayerRaceBit(bot_iter->GetRace()))) ||
			(!RuleB(Bots, AllowBotEquipAnyClassGear) && ((~item_data->Classes) & GetPlayerClassBit(bot_iter->GetClass())))
		) {
			continue;
		}

		std::list<int16> refined_equipable_slot_list;
		bool skip_bot = false;
		const EQ::ItemData* equipped_item = nullptr;
		const EQ::ItemInstance* equipped_inst = nullptr;

		for (const auto& slot_iter : equipable_slot_list) {
			// needs more failure criteria - this should cover the bulk for now
			if (slot_iter == EQ::invslot::slotSecondary && item_data->Damage && !bot_iter->CanThisClassDualWield()) {
				continue;
			}

			if (item_data->ReqLevel > bot_iter->GetLevel()) {
				continue;
			}

			haste_value = 0;
			equipped_item = nullptr;
			equipped_inst = nullptr;

			
			for (int16 equipable_slot = EQ::invslot::EQUIPMENT_BEGIN; equipable_slot <= EQ::invslot::EQUIPMENT_END; ++equipable_slot) {
				equipped_inst = bot_iter->GetInv()[equipable_slot];

				if (equipped_inst && equipped_inst->GetItem()) {
					equipped_item = equipped_inst->GetItem();

					if (item_data->CheckLoreConflict(equipped_item)) {
						skip_bot = true;
						break;
					}

					if (haste_only) {
						if (equipped_item->Haste > haste_value) {
							haste_value = equipped_item->Haste;
						}
					}
				}
			}

			if (skip_bot) {
				break;
			}

			if (haste_only && item_data->Haste < haste_value) {
				continue;
			}

			equipped_inst = bot_iter->GetInv()[slot_iter];

			if (equipped_inst) {
				if (empty_only) {
					continue;
				}

				auto equipped_data = equipped_inst->GetItem();

				if (gear_score_only && equipped_data && item_data->GearScore <= equipped_data->GearScore) {
					continue;
				}
			}

			refined_equipable_slot_list.push_back(slot_iter);
		}

		if (skip_bot) {
			continue;
		}

		if (refined_equipable_slot_list.empty()) {
			continue;
		}

		for (auto slot_iter : refined_equipable_slot_list) {
			equipped_item = nullptr;
			equipped_inst = nullptr;

			equipped_inst = bot_iter->GetInv()[slot_iter];

			if (equipped_inst && equipped_inst->GetItem()) {
				equipped_item = equipped_inst->GetItem();
			}

			if (equipped_item) {
				linker.SetItemData(equipped_item);

				c->Message(
					Chat::Say,
					fmt::format(
						"{} says, 'I can use that for my {} instead of my {}! Would you like to {}?'",
						Saylink::Silent(
							fmt::format(
								"^inventorygive byname {}",
								bot_iter->GetCleanName()
							),
							bot_iter->GetCleanName()
						),
						EQ::invslot::GetInvPossessionsSlotName(slot_iter),
						linker.GenerateLink(),
						Saylink::Silent(
							fmt::format(
								"^inventoryremove {} byname {}",
								slot_iter,
								bot_iter->GetCleanName()
							),
							"remove my item"
						)
					).c_str()
				);
			}
			else {
				c->Message(
					Chat::Say,
					fmt::format(
						"{} says, 'I can use that for my {}! Would you like to {}?'",
						Saylink::Silent(
							fmt::format(
								"^inventorygive byname {}",
								bot_iter->GetCleanName()
							),
							bot_iter->GetCleanName()
						),
						EQ::invslot::GetInvPossessionsSlotName(slot_iter),
						Saylink::Silent(
							fmt::format(
								"^inventorygive byname {}",
								bot_iter->GetCleanName()
							),
							"give it to me"
						)
					).c_str()
				);
			}

			if (RuleB(Bots, DoResponseAnimations)) {
				bot_iter->DoAnim(29);
			}
		}
	}
}
