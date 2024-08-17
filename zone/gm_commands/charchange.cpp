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
	std::string bucketValue = DataBucket::GetData(reset_key);

	if (Strings::IsNumber(bucketValue) && Strings::ToUnsignedInt(bucketValue) > 0) {
		c->Message(
			Chat::Yellow,
			fmt::format(
				"You can only do this once every {} days."
				, RuleI(CharChange, LockoutDuration)
			).c_str()
		);
		c->Message(
			Chat::Yellow,
			fmt::format(
				"You have {} remaining.",
				Strings::SecondsToTime(Strings::ToInt(DataBucket::GetDataRemaining(reset_key)))
			).c_str()
		);

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
			//1234567890123456789212345678931234567894123456789512323 //53 character max per line
		std::vector<std::string> description =
		{
			"Changes your character's race, class, gender and/or deity"
		};

		std::vector<std::string> notes =
		{
			fmt::format(
				"This can only be done once every {} days and changes will not be refunded or reverted."
				, RuleI(CharChange, LockoutDuration)
			),
			"<br>"
			"- Set each option to what you want, they must be compatible and available combinations.",
			"- If you choose to keep a setting, you will not be charged for it.",
			"- All incompatible items must be unequipped.",
			"- Base stats adjusted as needed.",
			"- All disciplines will be unscribed if class is changed.",
			"- All unusable spells will be unscribed.",
			"*If usable at a higher level it will only be unmemmed.*",
			"- Adjusts all skills, tradeskills and languages as needed.",
			"- Specialization Skills are reset to 0 if class is changed.",
		};

		std::vector<std::string> example_format =
		{
			fmt::format(
				"{} [Class] [Race] [Deity] [Gender]"
				, sep->arg[0]
			)
		};
			//12345678901234567892123456789312345678941234567895123 //53 character max per line
		std::vector<std::string> examples_one =
		{
			"To change to a Barbarian Warrior worshipping The Tribunal that is a Female:",
			fmt::format(
				"{} {} {} {} {}",
				sep->arg[0],
				WARRIOR,
				BARBARIAN,
				EQ::deity::DeityTheTribunal,
				FEMALE
			)
		};

		std::vector<std::string> examples_two = { };
			//1234567890123456789212345678931234567894123456789512323 //53 character max per line
		std::vector<std::string> examples_three = { };
		std::vector<std::string> actionables = { };
		std::vector<std::string> options = { };
		std::vector<std::string> options_one = { };
			//1234567890123456789212345678931234567894123456789512323 //53 character max per line
		std::vector<std::string> options_two = { };
		std::vector<std::string> options_three = { };

		std::string popup_text = c->SendCommandHelpWindow(
			c,
			description,
			notes,
			example_format,
			examples_one, examples_two, examples_three,
			actionables,
			options,
			options_one, options_two, options_three
		);

		popup_text = DialogueWindow::Table(popup_text);

		c->SendPopupToClient(sep->arg[0], popup_text.c_str());
		c->Message(
			Chat::Yellow,
			fmt::format(
				"Use {} for information about race/class/deity/gender IDs.",
				Saylink::Silent("^typeids")
			).c_str()
		);

		return;
	}

	bool classFound = false;
	bool raceFound = false;
	bool deityFound = false;
	bool genderFound = false;
	uint16 chosenClass;
	uint16 chosenRace;
	uint16 chosenDeity;
	uint16 chosenGender;

	if (sep->IsNumber(1)) {
		chosenClass = Strings::ToInt(sep->arg[1]);

		if (chosenClass < 1 || chosenClass > 15) {
			c->Message(
				Chat::Yellow,
				fmt::format(
					"Please choose a valid class, use {} for information regarding this command.",
					Saylink::Silent(
						fmt::format("{} help", sep->arg[0])
					)
				).c_str()
			);

			return;
		}

		classFound = true;
	}
	else {
		c->Message(
			Chat::Yellow,
			fmt::format(
				"Please choose a valid class, use {} for information regarding this command.",
				Saylink::Silent(
					fmt::format("{} help", sep->arg[0])
				)
			).c_str()
		);

		return;
	}

	if (sep->IsNumber(2)) {
		chosenRace = Strings::ToInt(sep->arg[2]);

		for (int i : race_values) {
			if (i == chosenRace) {
				raceFound = true;
			}
		}

		if (!raceFound) {
			c->Message(
				Chat::Yellow,
				fmt::format(
					"Please choose a valid race, use {} for information regarding this command.",
					Saylink::Silent(
						fmt::format("{} help", sep->arg[0])
					)
				).c_str()
			);

			return;
		}
	}
	else {
		c->Message(
			Chat::Yellow,
			fmt::format(
				"Please choose a valid class, use {} for information regarding this command.",
				Saylink::Silent(
					fmt::format("{} help", sep->arg[0])
				)
			).c_str()
		);

		return;
	}

	if (sep->IsNumber(3)) {
		chosenDeity = Strings::ToInt(sep->arg[3]);

		for (int i : deity_values) {
			if (i == chosenDeity) {
				deityFound = true;
			}
		}

		if (!deityFound) {
			c->Message(
				Chat::Yellow,
				fmt::format(
					"Please choose a valid deity, use {} for information regarding this command.",
					Saylink::Silent(
						fmt::format("{} help", sep->arg[0])
					)
				).c_str()
			);

			return;
		}
	}
	else {
		c->Message(
			Chat::Yellow,
			fmt::format(
				"Please choose a valid deity, use {} for information regarding this command.",
				Saylink::Silent(
					fmt::format("{} help", sep->arg[0])
				)
			).c_str()
		);

		return;
	}

	if (sep->IsNumber(4)) {
		chosenGender = Strings::ToInt(sep->arg[4]);

		if (chosenGender != 0 && chosenGender != 1) {
			c->Message(
				Chat::Yellow,
				fmt::format(
					"Please choose a valid gender, use {} for information regarding this command.",
					Saylink::Silent(
						fmt::format("{} help", sep->arg[0])
					)
				).c_str()
			);

			return;
		}
		genderFound = true;
	}
	else {
		c->Message(
			Chat::Yellow,
			fmt::format(
				"Please choose a valid gender, use {} for information regarding this command.",
				Saylink::Silent(
					fmt::format("{} help", sep->arg[0])
				)
			).c_str()
		);

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
	bool combo_classFound = false;
	bool combo_raceFound = false;
	bool combo_deityFound = false;
	int combos = create_combos.size();

	for (int i = 0; i < combos; ++i) {
		if (create_combos[i].ExpansionRequired <= RuleI(CharChange, ExpansionLimitForCharChange)) {
			if (create_combos[i].Class == chosenClass) {
				combo_classFound = true;

				if (create_combos[i].Race == chosenRace) {
					combo_raceFound = true;

					if (create_combos[i].Deity == chosenDeity) {
						combo_deityFound = true;
						combo_found = true;
						break;
					}
				}
			}
		}
	}

	if (!combo_found) {
		c->Message(
			Chat::Yellow,
			fmt::format(
				"Please choose a valid class, race and deity combination, use {} for information regarding this command.",
				Saylink::Silent(
					fmt::format("{} help", sep->arg[0])
				)
			).c_str()
		);

		if (combo_classFound && combo_raceFound) {
			int deity_check = 0;
			std::string window_text;
			std::string message_separator = " ";

			window_text.append("<c \"#FFFF\">");

			const int object_max = 1;
			auto object_count = 0;

			for (int i = 0; i < combos; ++i) {
				if (create_combos[i].ExpansionRequired <= RuleI(CharChange, ExpansionLimitForCharChange) && create_combos[i].Class == chosenClass && create_combos[i].Race == chosenRace && create_combos[i].Deity != deity_check) {
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
					GetRaceIDName(chosenRace),
					chosenRace,
					GetPlayerClassName(chosenClass),
					chosenClass
				).c_str(),
				window_text.c_str()
			);
		}
		else if (combo_classFound && !combo_raceFound) {
			int race_check = 0;
			std::string window_text;
			std::string message_separator = " ";

			window_text.append("<c \"#FFFF\">");

			const int object_max = 1;
			auto object_count = 0;

			for (int i = 0; i < combos; ++i) {
				if (create_combos[i].ExpansionRequired <= RuleI(CharChange, ExpansionLimitForCharChange) && create_combos[i].Class == chosenClass && create_combos[i].Race != race_check) {
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
					GetPlayerClassName(chosenClass),
					chosenClass
				).c_str(),
				window_text.c_str()
			);
		}

		return;
	}

	auto baseClass = c->GetBaseClass();
	auto baseRace = c->GetBaseRace();
	auto baseGender = c->GetBaseGender();
	auto baseDeity = c->GetDeity();
	int resetCost = 0;

	if (baseGender != chosenGender) {
		resetCost += RuleI(CharChange, GenderCost);
	}

	if (baseRace != chosenRace) {
		resetCost += RuleI(CharChange, RaceCost);
	}

	if (baseClass != chosenClass) {
		resetCost += RuleI(CharChange, ClassCost);
	}

	if (baseDeity != chosenDeity) {
		resetCost += RuleI(CharChange, DeityCost);
	}

	if (resetCost != 0) {
		resetCost += RuleI(CharChange, BaseCost);
		std::string key;
		std::string bucketValue;
		key = "MaxLvl-" + std::to_string(c->CharacterID());
		bucketValue = DataBucket::GetData(key);

		if (c->GetLevel() >= 10 && Strings::ToUnsignedInt(bucketValue) > RuleI(CharChange, LevelScaleStart)) {
			int over_level = Strings::ToUnsignedInt(bucketValue) - RuleI(CharChange, LevelScaleStart);

			while (over_level > 0) {
				resetCost *= RuleR(CharChange, ScaleRate);
				--over_level;
			}
		}

		const EQ::ItemData* item = nullptr;
		EQ::SayLinkEngine  linker;
		linker.SetLinkType(EQ::saylink::SayLinkItemData);
		item = database.GetItem(zone->GetCurrencyItemID(RuleI(CharChange, RequiredAltCurrency)));
		linker.SetItemData(item);

		if (strcasecmp(sep->arg[5], "confirm")) {
			c->Message(
				Chat::Lime,
				fmt::format(
					"Would you like to proceed with changing to a {} {} {} worshipping {} for {} {}? This will kick you to the server select --- {}",
					chosenGender == 0 ? "Male" : "Female",
					GetRaceIDName(chosenRace),
					GetPlayerClassName(chosenClass),
					EQ::deity::DeityName(static_cast<EQ::deity::DeityType>(chosenDeity)),
					resetCost,
					linker.GenerateLink(),
					Saylink::Silent(
						fmt::format("#charchange {} {} {} {} {}", chosenClass, chosenRace, chosenDeity, chosenGender, "confirm"),
						"Proceed"
					)
				).c_str()
			);
		}
		else if (!strcasecmp(sep->arg[5], "confirm")) {
			bool isMaxLevel = (c->GetLevel() >= std::max(int(RuleI(Character, MaxLevel)), int(c->GetClientMaxLevel())));
			uint16 maxLevel = std::max(int(RuleI(Character, MaxLevel)), int(c->GetClientMaxLevel()));

			if (RuleB(CharChange, RequiresAltCurrency) && c->GetAlternateCurrencyValue(RuleI(CharChange, RequiredAltCurrency)) < resetCost) {
				c->Message(
					Chat::Yellow,
					fmt::format(
						"You do not have enough {} to change to this combination.",
						linker.GenerateLink()
					).c_str()
				);

				return;
			}
			else {
				if (!RuleB(CharChange, RequiresAltCurrency) && c->GetCarriedMoney() < (resetCost * 1000)) {
					c->Message(Chat::Yellow, "You do not have enough money to change to this combination");

					return;
				}
			}

			if (c->GetBaseClass() != chosenClass || c->GetBaseRace() != chosenRace || c->GetDeity() != chosenDeity) {
				const EQ::ItemInstance* item_instance = nullptr;

				for (int slot_id = EQ::invslot::EQUIPMENT_BEGIN; slot_id <= EQ::invslot::EQUIPMENT_END; ++slot_id) {
					item_instance = c->GetInv().GetItem(slot_id);

					if (item_instance) {
						const EQ::ItemData* item_data = database.GetItem(item_instance->GetID());

						if (item_data) {
							if (
								(item_data->Races != PLAYER_RACE_ALL_MASK && !(item_data->Races & GetPlayerRaceBit(chosenRace))) ||
								(item_data->Classes != PLAYER_CLASS_ALL_MASK && !(item_data->Classes & GetPlayerClassBit(chosenClass))) ||
								(item_data->Deity != 0 && !(item_data->Deity & EQ::deity::ConvertDeityTypeToDeityTypeBit(EQ::deity::DeityType(chosenDeity))))
							) {
								c->Message(Chat::Red, "Your chosen class/race/deity combination cannot equip %s. Please unequip this and any other incompatible items before continuing.", item_data->Name);

								return;
							}
						}
					}
				}
			}

			if (RuleB(CharChange, RequiresAltCurrency)) {
				c->SetAlternateCurrencyValue(RuleI(CharChange, RequiredAltCurrency), (c->GetAlternateCurrencyValue(RuleI(CharChange, RequiredAltCurrency)) - resetCost));
				c->Message(
					Chat::Lime,
					fmt::format(
						"You have spent {} {}.",
						resetCost,
						linker.GenerateLink()
					).c_str()
				);
			}
			else {
				c->TakeMoneyFromPP(resetCost * 1000, true);
				c->Message(Chat::Lime, "You have spent {} {}.", resetCost);
			}

			if (c->GetBaseClass() != chosenClass || c->GetBaseRace() != chosenRace) {
				uint16 remainingPoints = c->GetSkillPoints() + ((maxLevel - c->GetLevel()) * 5);

				for (int skillIndex = 0; skillIndex <= EQ::skills::HIGHEST_SKILL; ++skillIndex) {
					uint32 rawSkill = c->GetRawSkill((EQ::skills::SkillType)skillIndex);
					uint16 maxSkill = content_db.GetSkillCap(chosenClass, (EQ::skills::SkillType)skillIndex, c->GetLevel());
					uint16 maxSkillAtMaxLevel = content_db.GetSkillCap(chosenClass, (EQ::skills::SkillType)skillIndex, maxLevel);
					bool trainedSkill = EQ::skills::IsTrainedSkill(skillIndex);

					if (skillIndex == EQ::skills::SkillTinkering && chosenRace != GNOME) {
						c->SetSkill(EQ::skills::SkillTinkering, 0);
					}
					else if (skillIndex == EQ::skills::SkillTinkering && chosenRace == GNOME) {
						c->SetSkill(EQ::skills::SkillTinkering, std::max(50, std::min(int(rawSkill), int(maxSkill))));
					}
					else if (skillIndex == EQ::skills::SkillSneak && (chosenRace == HALFLING || chosenRace == VAHSHIR)) {
						c->SetSkill(EQ::skills::SkillSneak, std::max(50, std::min(int(rawSkill), int(maxSkill))));
					}
					else if (skillIndex == EQ::skills::SkillHide && (chosenRace == DARK_ELF || chosenRace == HALFLING || chosenRace == WOOD_ELF)) {
						c->SetSkill(EQ::skills::SkillHide, std::max(50, std::min(int(rawSkill), int(maxSkill))));
					}
					else if (skillIndex == EQ::skills::SkillForage && (chosenRace == IKSAR || chosenRace == WOOD_ELF)) {
						c->SetSkill(EQ::skills::SkillForage, std::max(50, std::min(int(rawSkill), int(maxSkill))));
					}
					else if (skillIndex == EQ::skills::SkillSwimming && chosenRace == IKSAR) {
						c->SetSkill(EQ::skills::SkillSwimming, std::max(125, std::min(int(rawSkill), int(maxSkill))));
					}
					else if (skillIndex == EQ::skills::SkillSafeFall && chosenRace == VAHSHIR) {
						c->SetSkill(EQ::skills::SkillSafeFall, std::max(50, std::min(int(rawSkill), int(maxSkill))));
					}
					else if (EQ::skills::IsSpecializedSkill((EQ::skills::SkillType)skillIndex)) {
						if (c->GetBaseClass() != chosenClass) {
							c->SetSkill((EQ::skills::SkillType)skillIndex, 0);
						}
					}
					else {
						if (maxSkillAtMaxLevel == 0) {
							c->SetSkill((EQ::skills::SkillType)skillIndex, 0);
						}
						else {
							if (trainedSkill && remainingPoints == 0 && rawSkill == 0) { //TODO add checks to not increase ones that can be skilled up from 0
								c->SetSkill((EQ::skills::SkillType)skillIndex, std::max(1, std::min(int(rawSkill), int(maxSkill))));
							}
							else if (trainedSkill && remainingPoints > 0 && rawSkill == 0) {
								--remainingPoints;
							}
							else {
								c->SetSkill((EQ::skills::SkillType)skillIndex, std::min(int(rawSkill), int(maxSkill)));
							}
						}
					}
				}
			}

			if (c->GetBaseRace() != chosenRace) {
				switch (c->GetBaseRace()) {
					case BARBARIAN:
					{
						c->SetLanguageSkill(LANG_BARBARIAN, 0);
						break;
					}
					case DARK_ELF:
					{
						c->SetLanguageSkill(LANG_DARK_ELVISH, 0);
						c->SetLanguageSkill(LANG_DARK_SPEECH, 0);
						c->SetLanguageSkill(LANG_ELDER_ELVISH, 0);
						c->SetLanguageSkill(LANG_ELVISH, 0);
						break;
					}
					case DWARF:
					{
						c->SetLanguageSkill(LANG_DWARVISH, 0);
						c->SetLanguageSkill(LANG_GNOMISH, 0);
						break;
					}
					case ERUDITE:
					{
						c->SetLanguageSkill(LANG_ERUDIAN, 0);
						break;
					}
					case FROGLOK:
					{
						c->SetLanguageSkill(LANG_FROGLOK, 0);
						c->SetLanguageSkill(LANG_TROLL, 0);
						break;
					}
					case GNOME:
					{
						c->SetLanguageSkill(LANG_DWARVISH, 0);
						c->SetLanguageSkill(LANG_GNOMISH, 0);
						break;
					}
					case HALF_ELF:
					{
						c->SetLanguageSkill(LANG_ELVISH, 0);
						break;
					}
					case HALFLING:
					{
						c->SetLanguageSkill(LANG_HALFLING, 0);
						break;
					}
					case HIGH_ELF:
					{
						c->SetLanguageSkill(LANG_DARK_ELVISH, 0);
						c->SetLanguageSkill(LANG_ELDER_ELVISH, 0);
						c->SetLanguageSkill(LANG_ELVISH, 0);
						break;
					}
					case HUMAN:
					{
						break;
					}
					case IKSAR:
					{
						c->SetLanguageSkill(LANG_DARK_SPEECH, 0);
						c->SetLanguageSkill(LANG_LIZARDMAN, 0);
						break;
					}
					case OGRE:
					{
						c->SetLanguageSkill(LANG_DARK_SPEECH, 0);
						c->SetLanguageSkill(LANG_OGRE, 0);
						break;
					}
					case TROLL:
					{
						c->SetLanguageSkill(LANG_DARK_SPEECH, 0);
						c->SetLanguageSkill(LANG_TROLL, 0);
						break;
					}
					case WOOD_ELF:
					{
						c->SetLanguageSkill(LANG_ELVISH, 0);
						break;
					}
					case VAHSHIR:
					{
						c->SetLanguageSkill(LANG_COMBINE_TONGUE, 0);
						c->SetLanguageSkill(LANG_ERUDIAN, 0);
						c->SetLanguageSkill(LANG_VAH_SHIR, 0);
						break;
					}
					case DRAKKIN:
					{
						c->SetLanguageSkill(LANG_ELDER_DRAGON, 0);
						c->SetLanguageSkill(LANG_DRAGON, 0);
						break;
					}
				}

				switch (chosenRace) {
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

			if (c->GetBaseClass() != ROGUE && chosenRace == ROGUE) {
				c->SetLanguageSkill(LANG_THIEVES_CANT, 100);
			}
			else if (c->GetBaseClass() == ROGUE && chosenRace != ROGUE) {
				c->SetLanguageSkill(LANG_THIEVES_CANT, 0);
			}

			if (c->GetBaseClass() != chosenClass) {
				c->UntrainDiscAll(true);
				int spell_id = 0;

				for (int i = 0; i < EQ::spells::SPELLBOOK_SIZE; i++) {
					spell_id = c->GetSpellByBookSlot(i);

					if (spell_id > 0) {
						if (spells[spell_id].classes[chosenClass - 1] > maxLevel) {
							c->UnscribeSpellBySpellID(spell_id, true);
						}

						if (spells[spell_id].classes[chosenClass - 1] > c->GetLevel()) {
							c->UnmemSpellBySpellID(spell_id);
						}
					}
				}

				c->SaveSpells();
				//c->UnmemSpellAll(true);
				//c->UnscribeSpellAll(true);
			}

			if (c->GetDeity() != chosenDeity) {
				int deity_spells[] = { 1798, 1799, 1800, 1884, 1885, 1886, 1887, 1888, 1891, 1894, 1895, 1896, 1897, 1898, 1899 };
				int deity_req[] = { 4096, 1024, 128, 16384, 8, 64, 4, 32768, 2048, 512, 256, 16, 2, 32, 8192 };

				for (int i = 0; i <= 14; ++i) {
					if (c->HasSpellScribed(deity_spells[i]) && !(deity_req[i] & EQ::deity::ConvertDeityTypeToDeityTypeBit(EQ::deity::DeityType(chosenDeity)))) {
						c->UnmemSpellBySpellID(deity_spells[i]);
						c->UnscribeSpellBySpellID(deity_spells[i], true);
						c->SaveSpells();
					}
				}
				c->SaveSpells();
			}


			if (c->GetBaseClass() != chosenClass) {
				c->RefundAA();
				c->SendClearPlayerAA();
				database.DeleteCharacterAAs(c->CharacterID());
			}

			if (RuleB(CharChange, DiscordOutputEnabled)) {
				//Discord send
				std::ostringstream output;
				output <<
					"[#charchange] [" << c->GetCleanName() << "](http://vegaseq.com/charbrowser/index.php?page=character&char=" << c->GetCleanName() << ") [ID: " << c->CharacterID() << "] [Max Level: " << bucketValue << "] did a #charchange for " << int(resetCost) << "x [Shard](http://vegaseq.com/allaclone/?a=item&id=" << RuleI(Vegas, ShardItemID) << ")"
					" from a " << (c->GetGender() == 0 ? "Male" : "Female") << " " << GetRaceIDName(c->GetBaseRace()) << " " << GetPlayerClassName(c->GetBaseClass()) << " worshipping " << EQ::deity::DeityName(static_cast<EQ::deity::DeityType>(c->GetDeity())) << ""
					" to a " << (chosenGender == 0 ? "Male" : "Female") << " " << GetRaceIDName(chosenRace) << " " << GetPlayerClassName(chosenClass) << " worshipping " << EQ::deity::DeityName(static_cast<EQ::deity::DeityType>(chosenDeity)) << ""
					;
				std::string outFinal = output.str();
				zone->SendDiscordMessage("charchanges", outFinal);
			}

			std::string key = "OriginalClassRaceDeityGender-" + std::to_string(c->CharacterID());
			std::string bucketValue = DataBucket::GetData(key);

			if (bucketValue.empty()) {
				DataBucket::SetData(key, ("" + std::to_string(c->GetBaseClass()) + "," + std::to_string(c->GetBaseRace()) + "," + std::to_string(c->GetDeity()) + "," + std::to_string(c->GetGender())));
			}

			c->SetBaseRace(chosenRace);
			c->SetBaseClass(chosenClass);
			c->SetBaseGender(chosenGender);
			c->SetDeity(chosenDeity);

			std::vector<RaceClassAllocation> create_allocations;

			std::string query = "SELECT * FROM char_create_point_allocations ORDER BY id";
			auto results = database.QueryDatabase(query);

			if (!results.Success()) {
				c->Message(Chat::Red, "Error getting character create allocations.");
				return;
			}

			for (auto row = results.begin(); row != results.end(); ++row) {
				RaceClassAllocation allocate;
				allocate.Index = Strings::ToInt(row[0]);
				allocate.BaseStats[0] = Strings::ToInt(row[1]);
				allocate.BaseStats[1] = Strings::ToInt(row[2]);
				allocate.BaseStats[2] = Strings::ToInt(row[3]);
				allocate.BaseStats[3] = Strings::ToInt(row[4]);
				allocate.BaseStats[4] = Strings::ToInt(row[5]);
				allocate.BaseStats[5] = Strings::ToInt(row[6]);
				allocate.BaseStats[6] = Strings::ToInt(row[7]);
				allocate.DefaultPointAllocation[0] = Strings::ToInt(row[8]);
				allocate.DefaultPointAllocation[1] = Strings::ToInt(row[9]);
				allocate.DefaultPointAllocation[2] = Strings::ToInt(row[10]);
				allocate.DefaultPointAllocation[3] = Strings::ToInt(row[11]);
				allocate.DefaultPointAllocation[4] = Strings::ToInt(row[12]);
				allocate.DefaultPointAllocation[5] = Strings::ToInt(row[13]);
				allocate.DefaultPointAllocation[6] = Strings::ToInt(row[14]);

				create_allocations.push_back(allocate);
			}

			int allocs = create_allocations.size();
			bool allocFound = false;

			for (int i = 0; i < combos; ++i) {
				if (
					create_combos[i].Class == chosenClass &&
					create_combos[i].Deity == chosenDeity &&
					create_combos[i].Race == chosenRace					 
				) {
					for (int x = 0; x < allocs; ++x) {
						if (create_combos[i].AllocationIndex == create_allocations[x].Index) {
							c->SetStartZone(create_combos[i].Zone);

							c->SetBaseSTR(create_allocations[x].BaseStats[0] + create_allocations[x].DefaultPointAllocation[0]);
							c->SetBaseSTA(create_allocations[x].BaseStats[1] + create_allocations[x].DefaultPointAllocation[1]);
							c->SetBaseDEX(create_allocations[x].BaseStats[2] + create_allocations[x].DefaultPointAllocation[2]);
							c->SetBaseAGI(create_allocations[x].BaseStats[3] + create_allocations[x].DefaultPointAllocation[3]);
							c->SetBaseINT(create_allocations[x].BaseStats[4] + create_allocations[x].DefaultPointAllocation[4]);
							c->SetBaseWIS(create_allocations[x].BaseStats[5] + create_allocations[x].DefaultPointAllocation[5]);
							c->SetBaseCHA(create_allocations[x].BaseStats[6] + create_allocations[x].DefaultPointAllocation[6]);

							allocFound = true;
							break;
						}
					}
				}
				
				if (allocFound) {
					break;
				}
			}

			c->Save();
			c->CampAllBots();

			if (c->Admin() < 100) {
				DataBucket::SetData(reset_key, std::to_string(1), ("D" + std::to_string(RuleI(CharChange, LockoutDuration))));
			}

			key = "AntiCheatExempt-" + std::to_string(c->CharacterID());
			DataBucket::SetData(key, std::to_string(1), std::to_string(RuleI(Cheat, AntiCheatExemptDuration)));
			c->Kick("Character Change.");
		}
	}
	else {
		c->Message(Chat::LightBlue, "These settings match your current character. Are you having identity issues?");
	}
}
