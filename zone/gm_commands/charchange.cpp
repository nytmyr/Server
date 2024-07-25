#include "client.h"
#include "mob.h"
#include "../../world/sof_char_create_data.h"

void command_char_change(Client* c, const Seperator* sep)
{

	if (!RuleB(CharChange, CommandEnabled)) {
		c->Message(Chat::Yellow, "This command is currently disabled.");
		return;
	}

	if (RuleB(CharChange, RequireSpecificZone) && RuleI(CharChange, RequiredZone) != zone->GetZoneID()) {
		c->Message(Chat::Yellow, "This command can only be done in %s.", ZoneLongName(RuleI(CharChange, RequiredZone), true));
		return;
	}

	std::string reset_key = "CharChange-" + std::to_string(c->CharacterID());
	std::string bucket_value = DataBucket::GetData(reset_key);
	if (Strings::IsNumber(bucket_value) && Strings::ToUnsignedInt(bucket_value) > 0) {
		c->Message(Chat::Yellow, "You can only do this once every 30 days.");
		c->Message(
			Chat::Yellow,
			fmt::format(
				"You have {} remaining.",
				Strings::SecondsToTime(Strings::ToInt(DataBucket::GetDataRemaining(reset_key)))
			).c_str()
		);
		return;
	}

	Mob* target = c->CastToMob();

	int arguments = sep->argnum;
	if (!arguments) {
		c->Message(Chat::White, "Usage: #charchange [Class] [Race] [Deity] [Gender]");
		c->Message(Chat::White, "Use '#charchange help' for a list of class, race, deity and gender numbers.");
		c->Message(Chat::White, "Set each option to what you want, they must be compatible and available combinations.");
		c->Message(Chat::White, "If you choose to keep a setting, you will not be charged for it.");
		c->Message(Chat::White, "All incompatible items must be unequipped prior to changing.");
		c->Message(Chat::White, "This will unscribe all Disciplines, all unusable spells by your new class (spells that are usable at higher levels will remain in your spellbook although unmemmed if they are  currently memmed). Deity limited spells will be removed on a deity mismatch.");
		c->Message(Chat::White, "This will adjust all Skills, Specialized Skills (reset to 0), Tradeskills and Languages as needed for your chosen options.");
		c->Message(Chat::White, "These changes will not be refunded or reverted. Change at your own risk.");
		c->Message(Chat::White, "This can only be done once every 30 days.");
		return;
	}

	const std::string class_substrs[] = {
		"WAR", "CLR", "PAL", "RNG",
		"SHD", "DRU", "MNK", "BRD",
		"ROG", "SHM", "NEC", "WIZ",
		"MAG", "ENC", "BST"
	};

	const std::string race_substrs[] = {
		"HUM", "BAR", "ERU", "ELF",
		"HIE", "DEF", "HEF", "DWF",
		"TRL", "OGR", "HFL", "GNM",
		"IKS", "VAH"
	};

	const uint16 race_values[] = {
		RACE_HUMAN_1, RACE_BARBARIAN_2, RACE_ERUDITE_3, RACE_WOOD_ELF_4,
		RACE_HIGH_ELF_5, RACE_DARK_ELF_6, RACE_HALF_ELF_7, RACE_DWARF_8,
		RACE_TROLL_9, RACE_OGRE_10, RACE_HALFLING_11, RACE_GNOME_12,
		RACE_IKSAR_128, RACE_VAH_SHIR_130
	};

	const std::string gender_substrs[] = {
		"Male", "Female",
	};

	const uint16 deity_values[] = {
		EQ::deity::DeityType::DeityBertoxxulous,
		EQ::deity::DeityType::DeityBrellSirilis,
		EQ::deity::DeityType::DeityCazicThule,
		EQ::deity::DeityType::DeityErollisiMarr,
		EQ::deity::DeityType::DeityBristlebane,
		EQ::deity::DeityType::DeityInnoruuk,
		EQ::deity::DeityType::DeityKarana,
		EQ::deity::DeityType::DeityMithanielMarr,
		EQ::deity::DeityType::DeityPrexus,
		EQ::deity::DeityType::DeityQuellious,
		EQ::deity::DeityType::DeityRallosZek,
		EQ::deity::DeityType::DeityRodcetNife,
		EQ::deity::DeityType::DeitySolusekRo,
		EQ::deity::DeityType::DeityTheTribunal,
		EQ::deity::DeityType::DeityTunare,
		EQ::deity::DeityType::DeityVeeshan,
		EQ::deity::DeityType::DeityAgnostic
	};

	if (helper_is_help_or_usage(sep->arg[1])) {
		c->Message(Chat::White, "Usage: #charchange [Class] [Race] [Deity] [Gender]");
		c->Message(Chat::White, "Use '#charchange help' for a list of class, race, deity and gender numbers.");
		c->Message(Chat::White, "Set each option to what you want, they must be compatibile and available combinations.");
		c->Message(Chat::White, "If you choose to keep a setting, you will not be charged for it.");
		c->Message(Chat::White, "All incompatible items must be unequipped prior to changing.");
		c->Message(Chat::White, "This will unscribe all Disciplines, all unusable spells by your new class (spells that are usable at higher levels will remain in your spellbook although unmemmed if they are  currently memmed). Deity limited spells will be removed on a deity mismatch.");
		c->Message(Chat::White, "This will adjust all Skills, Specialized Skills (reset to 0), Tradeskills and Languages as needed for your chosen options.");
		c->Message(Chat::White, "These changes will not be refunded or reverted. Change at your own risk.");
		c->Message(Chat::White, "This can only be done once every 30 days.");

		std::string window_text;
		std::string message_separator;
		int object_count = 0;
		const int object_max = 4;

		window_text.append(
			fmt::format(
				"Classes{}<c \"#FFFF\">",
				DialogueWindow::Break()
			)
		);

		message_separator = " ";
		object_count = 0;
		for (int i = 0; i <= 14; ++i) {
			window_text.append(message_separator);

			if (object_count >= object_max) {
				window_text.append(DialogueWindow::Break());
				object_count = 0;
			}

			window_text.append(
				fmt::format("{} ({})",
					class_substrs[i],
					(i) + 1
				)
			);

			++object_count;
			message_separator = ", ";
		}

		window_text.append(DialogueWindow::Break(2));

		window_text.append(
			fmt::format(
				"<c \"#FFFFFF\">Races{}<c \"#FFFF\">",
				DialogueWindow::Break()
			)
		);

		message_separator = " ";
		object_count = 0;
		for (int i = 0; i <= 13; ++i) {
			window_text.append(message_separator);

			if (object_count >= object_max) {
				window_text.append(DialogueWindow::Break());
				object_count = 0;
			}

			window_text.append(
				fmt::format("{} ({})",
					race_substrs[i],
					race_values[i]
				)
			);

			++object_count;
			message_separator = ", ";
		}

		window_text.append(DialogueWindow::Break(2));

		window_text.append(
			fmt::format(
				"<c \"#FFFFFF\">Deities{}<c \"#FFFF\">",
				DialogueWindow::Break()
			)
		);

		message_separator = " ";
		object_count = 0;
		for (int i = 0; i <= 16; ++i) {
			window_text.append(message_separator);

			if (object_count >= object_max) {
				window_text.append(DialogueWindow::Break());
				object_count = 0;
			}

			window_text.append(
				fmt::format("{} ({})",
					EQ::deity::DeityName(static_cast<EQ::deity::DeityType>(deity_values[i])),
					deity_values[i]
				)
			);

			++object_count;
			message_separator = ", ";
		}

		window_text.append(DialogueWindow::Break(2));

		window_text.append(
			fmt::format(
				"<c \"#FFFFFF\">Genders{}<c \"#FFFF\">",
				DialogueWindow::Break()
			)
		);

		message_separator = " ";
		for (int i = 0; i <= 1; ++i) {
			window_text.append(message_separator);

			window_text.append(
				fmt::format("{} ({})",
					gender_substrs[i],
					i
				)
			);

			message_separator = ", ";
		}

		c->SendPopupToClient("Character Change Options", window_text.c_str());

		return;
	}

	bool class_found = false;
	bool race_found = false;
	bool deity_found = false;
	bool gender_found = false;
	uint16 chosen_class;
	uint16 chosen_race;
	uint16 chosen_deity;
	uint16 chosen_gender;

	if (sep->IsNumber(1)) {
		chosen_class = Strings::ToInt(sep->arg[1]);
		if (chosen_class < 1 || chosen_class > 15) {
			c->Message(Chat::Yellow, "Please choose a valid class.");
			return;
		}
		class_found = true;
	}
	else {
		c->Message(Chat::Yellow, "Please choose a valid class.");
		return;
	}

	if (sep->IsNumber(2)) {
		chosen_race = Strings::ToInt(sep->arg[2]);
		for (int i : race_values) {
			if (i == chosen_race) {
				race_found = true;
			}
		}
		if (!race_found) {
			c->Message(Chat::Yellow, "Please choose a valid race.");
			return;
		}
	}
	else {
		c->Message(Chat::Yellow, "Please choose a valid race.");
		return;
	}

	if (sep->IsNumber(3)) {
		chosen_deity = Strings::ToInt(sep->arg[3]);
		for (int i : deity_values) {
			if (i == chosen_deity) {
				deity_found = true;
			}
		}
		if (!deity_found) {
			c->Message(Chat::Yellow, "Please choose a valid deity.");
			return;
		}
	}
	else {
		c->Message(Chat::Yellow, "Please choose a valid deity.");
		return;
	}

	if (sep->IsNumber(4)) {
		chosen_gender = Strings::ToInt(sep->arg[4]);
		if (chosen_gender != 0 && chosen_gender != 1) {
			c->Message(Chat::Yellow, "Please choose a valid gender.");
			return;
		}
		gender_found = true;
	}
	else {
		c->Message(Chat::Yellow, "Please choose a valid gender.");
		return;
	}

	std::vector<RaceClassCombos> create_combos;

	std::string query = "SELECT * FROM char_create_combinations ORDER BY race, class, deity, start_zone";
	auto results = database.QueryDatabase(query);
	if (!results.Success()) {
		c->Message(Chat::Red, "Error getting character create combinations.");
		return;
	}

	for (auto row = results.begin(); row != results.end(); ++row) {
		RaceClassCombos combo;
		combo.AllocationIndex = Strings::ToInt(row[0]);
		combo.Race = Strings::ToInt(row[1]);
		combo.Class = Strings::ToInt(row[2]);
		combo.Deity = Strings::ToInt(row[3]);
		combo.Zone = Strings::ToInt(row[4]);
		combo.ExpansionRequired = Strings::ToInt(row[5]);

		create_combos.push_back(combo);
	}

	std::vector<int> classes_found;
	std::vector<int> races_found;
	std::vector<int> deities_found;

	RaceClassCombos class_combo;
	bool combo_found = false;
	bool combo_class_found = false;
	bool combo_race_found = false;
	bool combo_deity_found = false;
	int combos = create_combos.size();
	for (int i = 0; i < combos; ++i) {
		if (create_combos[i].ExpansionRequired <= RuleI(CharChange, ExpansionLimitForCharChange)) {
			if (create_combos[i].Class == chosen_class) {
				combo_class_found = true;
				if (create_combos[i].Race == chosen_race) {
					combo_race_found = true;
					if (create_combos[i].Deity == chosen_deity) {
						combo_deity_found = true;
						combo_found = true;
						break;
					}
				}
			}
		}
	}

	if (!combo_found) {
		c->Message(Chat::Yellow, "Please choose a valid class, race and deity combination. Use #charchange help for info.");
		if (combo_class_found && combo_race_found) {
			int deity_check = 0;
			std::string window_text;
			std::string message_separator = " ";

			window_text.append("<c \"#FFFF\">");

			const int object_max = 1;
			auto object_count = 0;

			for (int i = 0; i < combos; ++i) {
				if (create_combos[i].ExpansionRequired <= RuleI(CharChange, ExpansionLimitForCharChange) && create_combos[i].Class == chosen_class && create_combos[i].Race == chosen_race && create_combos[i].Deity != deity_check) {
					deity_check = create_combos[i].Deity;

					window_text.append(message_separator);

					if (object_count >= object_max) {
						window_text.append(DialogueWindow::Break());
						object_count = 0;
					}

					window_text.append(
						fmt::format(
							"{} ({})",
							EQ::deity::DeityName(static_cast<EQ::deity::DeityType>(create_combos[i].Deity)),
							create_combos[i].Deity
						)
					);

					++object_count;
					message_separator = " ";
				}
			}

			c->SendPopupToClient(
				fmt::format(
					"Player Deities for {} ({}) {} ({})",
					GetRaceIDName(chosen_race),
					chosen_race,
					GetPlayerClassName(chosen_class),
					chosen_class
				).c_str(),
				window_text.c_str()
			);
		}
		else if (combo_class_found && !combo_race_found) {
			int race_check = 0;
			std::string window_text;
			std::string message_separator = " ";

			window_text.append("<c \"#FFFF\">");

			const int object_max = 1;
			auto object_count = 0;

			for (int i = 0; i < combos; ++i) {
				if (create_combos[i].ExpansionRequired <= RuleI(CharChange, ExpansionLimitForCharChange) && create_combos[i].Class == chosen_class && create_combos[i].Race != race_check) {
					race_check = create_combos[i].Race;

					window_text.append(message_separator);

					if (object_count >= object_max) {
						window_text.append(DialogueWindow::Break());
						object_count = 0;
					}

					window_text.append(
						fmt::format(
							"{} ({})",
							GetRaceIDName(create_combos[i].Race),
							create_combos[i].Race
						)
					);

					++object_count;
					message_separator = " ";
				}
			}

			c->SendPopupToClient(
				fmt::format(
					"Player Races for {} ({})",
					GetPlayerClassName(chosen_class),
					chosen_class
				).c_str(),
				window_text.c_str()
			);
		}
		return;
	}

	auto base_class = c->GetBaseClass();
	auto base_race = c->GetBaseRace();
	auto base_gender = c->GetBaseGender();
	auto base_deity = c->GetDeity();
	int reset_cost = 0;

	if (base_gender != chosen_gender) {
		reset_cost += RuleI(CharChange, GenderCost);
	}
	if (base_race != chosen_race) {
		reset_cost += RuleI(CharChange, RaceCost);
	}
	if (base_class != chosen_class) {
		reset_cost += RuleI(CharChange, ClassCost);
	}
	if (base_deity != chosen_deity) {
		reset_cost += RuleI(CharChange, DeityCost);
	}
	if (reset_cost != 0) {
		reset_cost += RuleI(CharChange, BaseCost);
		std::string key;
		std::string bucket_value;
		key = "MaxLvl-" + std::to_string(c->CharacterID());
		bucket_value = DataBucket::GetData(key);
		if (c->GetLevel() >= 10 && Strings::ToUnsignedInt(bucket_value) > RuleI(CharChange, LevelScaleStart)) {
			int over_level = Strings::ToUnsignedInt(bucket_value) - RuleI(CharChange, LevelScaleStart);
			while (over_level > 0) {
				reset_cost *= RuleR(CharChange, ScaleRate);
				--over_level;
			}
		}
		const EQ::ItemData* item = nullptr;
		EQ::SayLinkEngine  linker;
		linker.SetLinkType(EQ::saylink::SayLinkItemData);
		item = database.GetItem(zone->GetCurrencyItemID(RuleI(CharChange, RequiredAltCurrency)));
		linker.SetItemData(item);

		c->Message(
			Chat::Lime,
			fmt::format(
				"Would you like to proceed with changing to a {} {} {} worshipping {} for {} {}? This will kick you to the server select --- {}",
				chosen_gender == 0 ? "Male" : "Female",
				GetRaceIDName(chosen_race),
				GetPlayerClassName(chosen_class),
				EQ::deity::DeityName(static_cast<EQ::deity::DeityType>(chosen_deity)),
				reset_cost,
				linker.GenerateLink(),
				Saylink::Silent(
					fmt::format("#charchange {} {} {} {} {}", chosen_class, chosen_race, chosen_deity, chosen_gender, "confirm"),
					"Proceed"
				)
			).c_str()
		);
		if (!strcasecmp(sep->arg[5], "confirm")) {
			if (RuleB(CharChange, RequiresAltCurrency) && c->GetAlternateCurrencyValue(RuleI(CharChange, RequiredAltCurrency)) < reset_cost) {
				c->Message(
					Chat::Lime,
					fmt::format(
						"You do not have enough {} to change to this combination.",
						linker.GenerateLink()
					).c_str()
				);
				return;
			}
			else {
				if (!RuleB(CharChange, RequiresAltCurrency) && c->GetCarriedMoney() < (reset_cost * 1000)) {
					c->Message(Chat::Lime, "You do not have enough money to change to this combination");
					return;
				}
			}
			if (c->GetBaseClass() != chosen_class || c->GetBaseRace() != chosen_race || c->GetDeity() != chosen_deity) {
				const EQ::ItemInstance* item_instance = nullptr;
				//int slot_found = 0;
				for (int slot_id = EQ::invslot::EQUIPMENT_BEGIN; slot_id <= EQ::invslot::EQUIPMENT_END; ++slot_id) {
					item_instance = c->GetInv().GetItem(slot_id);
					if (item_instance) {
						//c->Message(Chat::Yellow, "You must unequip all items to continue.");
						//return;
						const EQ::ItemData* item_data = database.GetItem(item_instance->GetID());
						if (item_data) {
							if (
								(item_data->Races != PLAYER_RACE_ALL_MASK && !(item_data->Races & GetPlayerRaceBit(chosen_race)))
								|| (item_data->Classes != PLAYER_CLASS_ALL_MASK && !(item_data->Classes & GetPlayerClassBit(chosen_class)))
								|| (item_data->Deity != 0 && !(item_data->Deity & EQ::deity::ConvertDeityTypeToDeityTypeBit(EQ::deity::DeityType(chosen_deity))))
								) {
								c->Message(Chat::Red, "Your chosen class/race/deity combination cannot equip %s. Please unequip this and any other incompatible items before continuing.", item_data->Name);
								return;
								//slot_found = c->GetInv().FindFreeSlotForTradeItem(item_instance);
								//if (slot_found < 0 || slot_found == 33) {
								//	c->Message(Chat::Red, "Your chosen class/race/deity combination cannot equip %s and you do not have enough space for it. Clear out some inventory slots and try again.", item_data->Name);
								//	return;
								//}
								//else {
								//	c->Message(Chat::Yellow, "%s is not equippable by your chosen combination.", item_data->Name);
								//	c->RemoveItem(item_data->ID, item_instance->GetCharges());
								//	c->PutLootInInventory(slot_found, *item_instance);
								//}
							}
						}
					}
				}
			}
			if (RuleB(CharChange, RequiresAltCurrency)) {
				c->SetAlternateCurrencyValue(RuleI(CharChange, RequiredAltCurrency), (c->GetAlternateCurrencyValue(RuleI(CharChange, RequiredAltCurrency)) - reset_cost));
				c->Message(
					Chat::Lime,
					fmt::format(
						"You have spent {} {}.",
						reset_cost,
						linker.GenerateLink()
					).c_str()
				);
			}
			else {
				c->TakeMoneyFromPP(reset_cost * 1000, true);
				c->Message(Chat::Lime, "You have spent {} {}.", reset_cost);
					return;
			}
			if (c->GetBaseClass() != chosen_class || c->GetBaseRace() != chosen_race) {
				for (int sindex = 0; sindex <= EQ::skills::HIGHEST_SKILL; ++sindex) {
					uint32 raw_skill = c->GetRawSkill((EQ::skills::SkillType)sindex);
					uint16 max_skill = content_db.GetSkillCap(chosen_class, (EQ::skills::SkillType)sindex, c->GetLevel());
					if (sindex == EQ::skills::SkillTinkering && chosen_race != GNOME) {
						c->SetSkill(EQ::skills::SkillTinkering, 0);
					}
					else if (sindex == EQ::skills::SkillTinkering && chosen_race == GNOME) {
						c->SetSkill(EQ::skills::SkillTinkering, std::max(50, std::min(int(raw_skill), int(max_skill))));
					}
					else if (sindex == EQ::skills::SkillSneak && (chosen_race == HALFLING || chosen_race == VAHSHIR)) {
						c->SetSkill(EQ::skills::SkillSneak, std::max(50, std::min(int(raw_skill), int(max_skill))));
					}
					else if (sindex == EQ::skills::SkillSneak && (chosen_race != HALFLING && chosen_race != VAHSHIR) && !max_skill) {
						c->SetSkill(EQ::skills::SkillSneak, 0);
					}
					else if (sindex == EQ::skills::SkillHide && (chosen_race == DARK_ELF || chosen_race == HALFLING || chosen_race == WOOD_ELF)) {
						c->SetSkill(EQ::skills::SkillHide, std::max(50, std::min(int(raw_skill), int(max_skill))));
					}
					else if (sindex == EQ::skills::SkillHide && (chosen_race != DARK_ELF && chosen_race != HALFLING && chosen_race != WOOD_ELF) && !max_skill) {
						c->SetSkill(EQ::skills::SkillHide, 0);
					}
					else if (sindex == EQ::skills::SkillForage && (chosen_race == IKSAR || chosen_race == WOOD_ELF)) {
						c->SetSkill(EQ::skills::SkillForage, std::max(50, std::min(int(raw_skill), int(max_skill))));
					}
					else if (sindex == EQ::skills::SkillForage && (chosen_race != IKSAR && chosen_race != WOOD_ELF) && !max_skill) {
						c->SetSkill(EQ::skills::SkillForage, 0);
					}
					else if (sindex == EQ::skills::SkillSwimming && chosen_race == IKSAR) {
						c->SetSkill(EQ::skills::SkillSwimming, std::max(125, std::min(int(raw_skill), int(max_skill))));
					}
					else if (sindex == EQ::skills::SkillSafeFall && chosen_race != VAHSHIR && !max_skill) {
						c->SetSkill(EQ::skills::SkillSafeFall, 0);
					}
					else if (sindex == EQ::skills::SkillSafeFall && chosen_race == VAHSHIR) {
						c->SetSkill(EQ::skills::SkillSafeFall, std::max(50, std::min(int(raw_skill), int(max_skill))));
					}
					else if (sindex == EQ::skills::SkillSpecializeAbjure) {
						c->SetSkill(EQ::skills::SkillSpecializeAbjure, 0);
					}
					else if (sindex == EQ::skills::SkillSpecializeAlteration) {
						c->SetSkill(EQ::skills::SkillSpecializeAlteration, 0);
					}
					else if (sindex == EQ::skills::SkillSpecializeAlteration) {
						c->SetSkill(EQ::skills::SkillSpecializeAlteration, 0);
					}
					else if (sindex == EQ::skills::SkillSpecializeConjuration) {
						c->SetSkill(EQ::skills::SkillSpecializeConjuration, 0);
					}
					else if (sindex == EQ::skills::SkillSpecializeDivination) {
						c->SetSkill(EQ::skills::SkillSpecializeDivination, 0);
					}
					else if (sindex == EQ::skills::SkillSpecializeEvocation) {
						c->SetSkill(EQ::skills::SkillSpecializeEvocation, 0);
					}
					else {
						c->SetSkill((EQ::skills::SkillType)sindex, std::min(int(raw_skill), int(max_skill)));
					}
				}
			}
			if (c->GetBaseRace() != chosen_race) {
				for (int language_skill = 0; language_skill < LANG_UNKNOWN; ++language_skill) {
					c->SetLanguageSkill(language_skill, 0);
				}
				switch (chosen_race) {
					case BARBARIAN:
					{
						c->SetLanguageSkill(LANG_COMMON_TONGUE, 100);
						c->SetLanguageSkill(LANG_BARBARIAN, 100);
						break;
					}
					case DARK_ELF:
					{
						c->SetLanguageSkill(LANG_COMMON_TONGUE, 100);
						c->SetLanguageSkill(LANG_DARK_ELVISH, 100);
						c->SetLanguageSkill(LANG_DARK_SPEECH, 100);
						c->SetLanguageSkill(LANG_ELDER_ELVISH, 100);
						c->SetLanguageSkill(LANG_ELVISH, 25);
						break;
					}
					case DWARF:
					{
						c->SetLanguageSkill(LANG_COMMON_TONGUE, 100);
						c->SetLanguageSkill(LANG_DWARVISH, 100);
						c->SetLanguageSkill(LANG_GNOMISH, 25);
						break;
					}
					case ERUDITE:
					{
						c->SetLanguageSkill(LANG_COMMON_TONGUE, 100);
						c->SetLanguageSkill(LANG_ERUDIAN, 100);
						break;
					}
					case FROGLOK:
					{
						c->SetLanguageSkill(LANG_COMMON_TONGUE, 100);
						c->SetLanguageSkill(LANG_FROGLOK, 100);
						c->SetLanguageSkill(LANG_TROLL, 25);
						break;
					}
					case GNOME:
					{
						c->SetLanguageSkill(LANG_COMMON_TONGUE, 100);
						c->SetLanguageSkill(LANG_DWARVISH, 25);
						c->SetLanguageSkill(LANG_GNOMISH, 100);
						break;
					}
					case HALF_ELF:
					{
						c->SetLanguageSkill(LANG_COMMON_TONGUE, 100);
						c->SetLanguageSkill(LANG_ELVISH, 100);
						break;
					}
					case HALFLING:
					{
						c->SetLanguageSkill(LANG_COMMON_TONGUE, 100);
						c->SetLanguageSkill(LANG_HALFLING, 100);
						break;
					}
					case HIGH_ELF:
					{
						c->SetLanguageSkill(LANG_COMMON_TONGUE, 100);
						c->SetLanguageSkill(LANG_DARK_ELVISH, 25);
						c->SetLanguageSkill(LANG_ELDER_ELVISH, 25);
						c->SetLanguageSkill(LANG_ELVISH, 100);
						break;
					}
					case HUMAN:
					{
						c->SetLanguageSkill(LANG_COMMON_TONGUE, 100);
						break;
					}
					case IKSAR:
					{
						c->SetLanguageSkill(LANG_COMMON_TONGUE, RuleI(CharChange, IksarCommonTongue));
						c->SetLanguageSkill(LANG_DARK_SPEECH, 100);
						c->SetLanguageSkill(LANG_LIZARDMAN, 100);
						break;
					}
					case OGRE:
					{
						c->SetLanguageSkill(LANG_COMMON_TONGUE, RuleI(CharChange, OgreCommonTongue));
						c->SetLanguageSkill(LANG_DARK_SPEECH, 100);
						c->SetLanguageSkill(LANG_OGRE, 100);
						break;
					}
					case TROLL:
					{
						c->SetLanguageSkill(LANG_COMMON_TONGUE, RuleI(CharChange, TrollCommonTongue));
						c->SetLanguageSkill(LANG_DARK_SPEECH, 100);
						c->SetLanguageSkill(LANG_TROLL, 100);
						break;
					}
					case WOOD_ELF:
					{
						c->SetLanguageSkill(LANG_COMMON_TONGUE, 100);
						c->SetLanguageSkill(LANG_ELVISH, 100);
						break;
					}
					case VAHSHIR:
					{
						c->SetLanguageSkill(LANG_COMMON_TONGUE, 100);
						c->SetLanguageSkill(LANG_COMBINE_TONGUE, 100);
						c->SetLanguageSkill(LANG_ERUDIAN, 25);
						c->SetLanguageSkill(LANG_VAH_SHIR, 100);
						break;
					}
					case DRAKKIN:
					{
						c->SetLanguageSkill(LANG_COMMON_TONGUE, 100);
						c->SetLanguageSkill(LANG_ELDER_DRAGON, 100);
						c->SetLanguageSkill(LANG_DRAGON, 100);
						break;
					}
				}
			}
			if (c->GetBaseClass() == ROGUE && chosen_race != ROGUE) {
				c->SetLanguageSkill(LANG_THIEVES_CANT, 0);
			}
			if (c->GetBaseClass() != chosen_class) {
				c->UntrainDiscAll(true);
				int spell_id = 0;
				for (int i = 0; i < EQ::spells::SPELLBOOK_SIZE; i++) {
					spell_id = c->GetSpellByBookSlot(i);
					if (spell_id > 0) {
						if (spells[spell_id].classes[chosen_class - 1] > 65) {
							c->UnscribeSpellBySpellID(spell_id, true);
						}
						if (spells[spell_id].classes[chosen_class - 1] > c->GetLevel()) {
							c->UnmemSpellBySpellID(spell_id);
						}
					}
				}
				c->SaveSpells();
				//c->UnmemSpellAll(true);
				//c->UnscribeSpellAll(true);
			}
			else {
				if (c->GetDeity() != chosen_deity) {
					int deity_spells[] = { 1798, 1799, 1800, 1884, 1885, 1886, 1887, 1888, 1891, 1894, 1895, 1896, 1897, 1898, 1899 };
					int deity_req[] = { 4096, 1024, 128, 16384, 8, 64, 4, 32768, 2048, 512, 256, 16, 2, 32, 8192 };
					for (int i = 0; i <= 14; ++i) {
						if (c->HasSpellScribed(deity_spells[i]) && !(deity_req[i] & EQ::deity::ConvertDeityTypeToDeityTypeBit(EQ::deity::DeityType(chosen_deity)))) {
							c->UnmemSpellBySpellID(deity_spells[i]);
							c->UnscribeSpellBySpellID(deity_spells[i], true);
							c->SaveSpells();
						}
					}
					c->SaveSpells();
				}
			}
			if (c->GetBaseClass() != chosen_class) {
				c->RefundAA();
				c->SendClearPlayerAA();
				database.DeleteCharacterAAs(c->CharacterID());
			}
			if (RuleB(CharChange, DiscordOutputEnabled)) {
				//Discord send
				std::ostringstream output;
				output <<
					"[#charchange] [" << c->GetCleanName() << "](http://vegaseq.com/charbrowser/index.php?page=character&char=" << c->GetCleanName() << ") [ID: " << c->CharacterID() << "] [Max Level: " << bucket_value << "] did a #charchange for " << int(reset_cost) << "x [Shard](http://vegaseq.com/allaclone/?a=item&id=" << RuleI(Vegas, ShardItemID) << ")"
					" from a " << (c->GetGender() == 0 ? "Male" : "Female") << " " << GetRaceIDName(c->GetBaseRace()) << " " << GetPlayerClassName(c->GetBaseClass()) << " worshipping " << EQ::deity::DeityName(static_cast<EQ::deity::DeityType>(c->GetDeity())) << ""
					" to a " << (chosen_gender == 0 ? "Male" : "Female") << " " << GetRaceIDName(chosen_race) << " " << GetPlayerClassName(chosen_class) << " worshipping " << EQ::deity::DeityName(static_cast<EQ::deity::DeityType>(chosen_deity)) << ""
					;
				std::string out_final = output.str();
				zone->SendDiscordMessage("charchanges", out_final);
			}

			c->SetBaseRace(chosen_race);
			c->SetBaseClass(chosen_class);
			c->SetBaseGender(chosen_gender);
			c->SetDeity(chosen_deity);
			c->Save();
			c->CampAllBots();

			DataBucket::SetData(reset_key, std::to_string(1), "D30");

			c->Kick("Character Change.");
		}
	}
	else {
		c->Message(Chat::LightBlue, "These settings match your current character. Are you having identity issues?");
	}
}
