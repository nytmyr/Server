#include "zone/command.h"
#include "world/sof_char_create_data.h"
#include "common/skill_caps.h"
#include "zone/bot_command.h"
#include "zone/zonedb.h"
#include <common/data_bucket.h>

void command_charchange(Client* c, const Seperator* sep) //TODO vegas - add starting stat checks.
{
	if (!RuleB(CharChange, CommandEnabled)) {
		c->Message(Chat::Yellow, "This command is currently disabled.");

		return;
	}

	int lockoutDuration = RuleI(CharChange, LockoutDuration);
	std::string lockoutDurationString = Strings::SecondsToTime(lockoutDuration);

	if (helper_is_help_or_usage(sep->arg[1])) {
		BotCommandHelpParams p;

		p.description = { "Changes your character's race, class, gender and/or deity" };
		p.notes =
		{
			fmt::format(
				"This can only be done once every {} and changes will not be refunded or reverted.",
				lockoutDurationString
			),
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
		p.example_format = { fmt::format("{} [Class] [Race] [Deity] [Gender]", sep->arg[0]) };
		p.examples_one =
		{
			"To change to a Barbarian Warrior worshipping The Tribunal that is a Female:",
			fmt::format(
				"{} {} {} {} {}",
				sep->arg[0],
				Class::Warrior,
				Race::Barbarian,
				Deity::TheTribunal,
				Gender::Female
			)
		};

		std::string popup_text = c->SendBotCommandHelpWindow(p);
		popup_text = DialogueWindow::Table(popup_text);

		c->SendPopupToClient(sep->arg[0], popup_text.c_str());

		if (RuleB(Bots, SendClassRaceOnHelp)) {
			c->Message(
				Chat::Yellow,
				fmt::format(
					"Use {} for information about race/class/deity IDs.",
					Saylink::Silent("^classracelist deities")
				).c_str()
			);
		}

		return;
	}

	if (RuleB(CharChange, RequireSpecificZone) && RuleI(CharChange, RequiredZone) != zone->GetZoneID()) {
		c->Message(Chat::Yellow, "This command can only be done in %s.", ZoneLongName(RuleI(CharChange, RequiredZone), true));

		return;
	}

	std::string reset_key = "CharChange-" + std::to_string(c->CharacterID());
	std::string reset_key_value = DataBucket::GetData(&database, reset_key);

	if (!reset_key_value.empty()) {
		c->Message(
			Chat::Yellow,
			fmt::format(
				"You can only do this once every {}.",
				lockoutDurationString
			).c_str()
		);

		c->Message(
			Chat::Yellow,
			fmt::format(
				"You have {} remaining.",
				Strings::SecondsToTime(Strings::ToInt(DataBucket::GetDataRemaining(&database, reset_key)))
			).c_str()
		);

		return;
	}

	const std::string class_substrs[] = { // TODO - Needs BER
		"WAR", "CLR", "PAL", "RNG",
		"SHD", "DRU", "MNK", "BRD",
		"ROG", "SHM", "NEC", "WIZ",
		"MAG", "ENC", "BST"
	};

	const std::string race_substrs[] = { // TODO - Needs DRK
		"HUM", "BAR", "ERU", "ELF",
		"HIE", "DEF", "HEF", "DWF",
		"TRL", "OGR", "HFL", "GNM",
		"IKS", "VAH"
	};

	const uint16 race_values[] = { // TODO - Needs Drakkin
		Race::Human, Race::Barbarian, Race::Erudite, Race::WoodElf,
		Race::HighElf, Race::DarkElf, Race::HalfElf, Race::Dwarf,
		Race::Troll, Race::Ogre, Race::Halfling, Race::Gnome,
		Race::Iksar, Race::VahShir
	};

	const std::string gender_substrs[] = {
		"Male", "Female",
	};

	const uint16 deity_values[] = {
		Deity::Bertoxxulous,
		Deity::BrellSirilis,
		Deity::CazicThule,
		Deity::ErollisiMarr,
		Deity::Bristlebane,
		Deity::Innoruuk,
		Deity::Karana,
		Deity::MithanielMarr,
		Deity::Prexus,
		Deity::Quellious,
		Deity::RallosZek,
		Deity::RodcetNife,
		Deity::SolusekRo,
		Deity::TheTribunal,
		Deity::Tunare,
		Deity::Veeshan,
		Deity::Agnostic2
	};

	//bool classFound = false;
	bool raceFound = false;
	bool deityFound = false;
	//bool genderFound = false;
	uint16 chosenClass;
	uint16 chosenRace;
	uint16 chosenDeity;
	uint16 chosenGender;

	if (sep->IsNumber(1)) {
		chosenClass = Strings::ToInt(sep->arg[1]);

		if (chosenClass < Class::Warrior || chosenClass > Class::Beastlord) { //TODO - support Berserker
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

		//classFound = true;
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

		//genderFound = true;
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

	std::string create_combo_query = "SELECT * FROM char_create_combinations ORDER BY race, class, deity, start_zone";
	auto create_combo_query_results = database.QueryDatabase(create_combo_query);

	if (!create_combo_query_results.Success()) {
		c->Message(Chat::Red, "Error getting character create combinations.");
		return;
	}

	for (auto row = create_combo_query_results.begin(); row != create_combo_query_results.end(); ++row) {
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

	//RaceClassCombos class_combo;
	bool combo_found = false;
	//bool combo_classFound = false;
	bool combo_raceFound = false;
	bool combo_deityFound = false;
	int combos = create_combos.size();

	for (int i = 0; i < combos; ++i) {
		if (create_combos[i].ExpansionRequired <= RuleI(CharChange, ExpansionLimitForCharChange)) {
			if (create_combos[i].Class == chosenClass) {
				//combo_classFound = true;

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

		std::string window_text;

		window_text.append("<c \"#FFFF\">");

		if (!combo_raceFound) {
			std::string races;

			for (int i = 0; i < combos; ++i) {
				if (
					create_combos[i].ExpansionRequired <= RuleI(CharChange, ExpansionLimitForCharChange) &&
					create_combos[i].Class == chosenClass
				) {
					races.append(
						fmt::format(
							"{} ({})",
							GetRaceIDName(create_combos[i].Race),
							create_combos[i].Race
						)
					);

					races.append(DialogueWindow::Break());
				}
			}

			window_text.append(races);
		}
		else if (!combo_deityFound) {
			std::string deities;

			for (int i = 0; i < combos; ++i) {
				if (
					create_combos[i].ExpansionRequired <= RuleI(CharChange, ExpansionLimitForCharChange) &&
					create_combos[i].Class == chosenClass &&
					create_combos[i].Race == chosenRace
				) {
					deities.append(
						fmt::format(
							"{} ({})",
							Deity::GetName(create_combos[i].Deity),
							create_combos[i].Deity
						)
					);

					deities.append(DialogueWindow::Break());
				}
			}

			window_text.append(deities);
		}

		c->SendPopupToClient(
			fmt::format(
				"Available options for {}{} ({})",
				(
					!combo_raceFound ?
						"" :
						fmt::format(
							"{} ({}) ",
							GetRaceIDName(chosenRace),
							chosenRace
						).c_str()
				),
				GetClassIDName(chosenClass),
				chosenClass
			).c_str(),
			window_text.c_str()
		);

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
		std::string max_level_key;
		std::string max_level_key_value;

		max_level_key = "MaxLvl-" + std::to_string(c->CharacterID());
		max_level_key_value = DataBucket::GetData(&database, max_level_key);

		if (c->GetLevel() >= 10 && Strings::ToUnsignedInt(max_level_key_value) > RuleI(CharChange, LevelScaleStart)) {
			int over_level = Strings::ToUnsignedInt(max_level_key_value) - RuleI(CharChange, LevelScaleStart);

			while (over_level > 0) {
				resetCost *= RuleR(CharChange, ScaleRate);
				--over_level;
			}
		}

		const EQ::ItemData* item = nullptr;
		EQ::SayLinkEngine  linker;

		if (RuleB(CharChange, RequiresAltCurrency)) {
			linker.SetLinkType(EQ::saylink::SayLinkItemData);
			item = database.GetItem(zone->GetCurrencyItemID(RuleI(CharChange, RequiredAltCurrency)));

			if (!item) {
				c->Message(Chat::Yellow, "Unable to find the required alternate currency item in the database. Please report this to the necessary server admin.");
				return;
			}

			linker.SetItemData(item);
		}

		if (strcasecmp(sep->arg[5], "confirm")) {
			c->Message(
				Chat::Lime,
				fmt::format(
					"Would you like to proceed with changing to a {} {} {} worshipping {} for {} {}? This will kick you to the server select --- {}",
					chosenGender == 0 ? "Male" : "Female",
					GetRaceIDName(chosenRace),
					GetClassIDName(chosenClass),
					Deity::GetName(chosenDeity),
					resetCost,
					(RuleB(CharChange, RequiresAltCurrency) ? linker.GenerateLink() : "platinum"),
					Saylink::Silent(
						fmt::format("#charchange {} {} {} {} {}", chosenClass, chosenRace, chosenDeity, chosenGender, "confirm"),
						"Proceed"
					)
				).c_str()
			);
		}
		else if (!strcasecmp(sep->arg[5], "confirm")) {
			//bool isMaxLevel = (c->GetLevel() >= std::max(int(RuleI(Character, MaxLevel)), int(c->GetClientMaxLevel())));
			uint16 maxLevel = std::max(int(RuleI(Character, MaxLevel)), int(c->GetClientMaxLevel()));

			if (RuleB(CharChange, RequiresAltCurrency) && c->GetAlternateCurrencyValue(RuleI(CharChange, RequiredAltCurrency)) < resetCost) {
				c->Message(
					Chat::Yellow,
					fmt::format(
						"You do not have enough {} to change to this combination.",
						(RuleB(CharChange, RequiresAltCurrency) ? linker.GenerateLink() : "platinum")
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
								(item_data->Races != RaceBitmask::All && !(item_data->Races & GetPlayerRaceBit(chosenRace))) ||
								(item_data->Classes != Class::ALL_CLASSES_BITMASK && !(item_data->Classes & GetPlayerClassBit(chosenClass))) ||
								(item_data->Deity != 0 && !(item_data->Deity & Deity::GetBitmask(chosenDeity)))
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
			}
			else {
				c->TakeMoneyFromPP(resetCost * 1000, true);
			}

			c->Message(
				Chat::Lime,
				fmt::format(
					"You have spent {} {}.",
					resetCost,
					(RuleB(CharChange, RequiresAltCurrency) ? linker.GenerateLink() : "platinum")
				).c_str()
			);

			if (c->GetBaseClass() != chosenClass || c->GetBaseRace() != chosenRace) {
				uint16 remainingPoints = c->GetSkillPoints() + ((maxLevel - c->GetLevel()) * 5);

				for (int skillIndex = 0; skillIndex <= EQ::skills::HIGHEST_SKILL; ++skillIndex) {
					uint32 rawSkill = c->GetRawSkill((EQ::skills::SkillType)skillIndex);
					auto maxSkill = SkillCaps::Instance()->GetSkillCap(chosenClass, EQ::skills::SkillType(skillIndex), c->GetLevel()).cap;
					auto maxSkillAtMaxLevel = SkillCaps::Instance()->GetSkillCap(chosenClass, (EQ::skills::SkillType)skillIndex, maxLevel).cap;
					bool trainedSkill = EQ::skills::IsTrainedSkill((EQ::skills::SkillType)skillIndex);

					if (skillIndex == EQ::skills::SkillTinkering && chosenRace != Race::Gnome) {
						c->SetSkill(EQ::skills::SkillTinkering, 0);
					}
					else if (skillIndex == EQ::skills::SkillTinkering && chosenRace == Race::Gnome) {
						c->SetSkill(EQ::skills::SkillTinkering, std::max(50, std::min(int(rawSkill), int(maxSkill))));
					}
					else if (skillIndex == EQ::skills::SkillSneak && (chosenRace == Race::Halfling || chosenRace == Race::VahShir)) {
						c->SetSkill(EQ::skills::SkillSneak, std::max(50, std::min(int(rawSkill), int(maxSkill))));
					}
					else if (skillIndex == EQ::skills::SkillHide && (chosenRace == Race::DarkElf || chosenRace == Race::Halfling || chosenRace == Race::WoodElf)) {
						c->SetSkill(EQ::skills::SkillHide, std::max(50, std::min(int(rawSkill), int(maxSkill))));
					}
					else if (skillIndex == EQ::skills::SkillForage && (chosenRace == Race::Iksar || chosenRace == Race::WoodElf)) {
						c->SetSkill(EQ::skills::SkillForage, std::max(50, std::min(int(rawSkill), int(maxSkill))));
					}
					else if (skillIndex == EQ::skills::SkillSwimming && chosenRace == Race::Iksar) {
						c->SetSkill(EQ::skills::SkillSwimming, std::max(125, std::min(int(rawSkill), int(maxSkill))));
					}
					else if (skillIndex == EQ::skills::SkillSafeFall && chosenRace == Race::VahShir) {
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
							if (trainedSkill && remainingPoints == 0 && rawSkill == 0) {
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
					case Race::Barbarian:
						c->SetLanguageSkill(Language::Barbarian, 0);
						break;
					case Race::DarkElf:
						c->SetLanguageSkill(Language::DarkElvish, 0);
						c->SetLanguageSkill(Language::DarkSpeech, 0);
						c->SetLanguageSkill(Language::ElderElvish, 0);
						c->SetLanguageSkill(Language::Elvish, 0);
						break;
					case Race::Dwarf:
						c->SetLanguageSkill(Language::Dwarvish, 0);
						c->SetLanguageSkill(Language::Gnomish, 0);
						break;
					case Race::Erudite:
						c->SetLanguageSkill(Language::Erudian, 0);
						break;
					case Race::Froglok:
						c->SetLanguageSkill(Language::Froglok, 0);
						c->SetLanguageSkill(Language::Troll, 0);
						break;
					case Race::Gnome:
						c->SetLanguageSkill(Language::Dwarvish, 0);
						c->SetLanguageSkill(Language::Gnomish, 0);
						break;
					case Race::HalfElf:
						c->SetLanguageSkill(Language::Elvish, 0);
						break;
					case Race::Halfling:
						c->SetLanguageSkill(Language::Halfling, 0);
						break;
					case Race::HighElf:
						c->SetLanguageSkill(Language::DarkElvish, 0);
						c->SetLanguageSkill(Language::ElderElvish, 0);
						c->SetLanguageSkill(Language::Elvish, 0);
						break;
					case Race::Human:
						break;
					case Race::Iksar:
						c->SetLanguageSkill(Language::DarkSpeech, 0);
						c->SetLanguageSkill(Language::Lizardman, 0);
						break;
					case Race::Ogre:
						c->SetLanguageSkill(Language::DarkSpeech, 0);
						c->SetLanguageSkill(Language::Ogre, 0);
						break;
					case Race::Troll:
						c->SetLanguageSkill(Language::DarkSpeech, 0);
						c->SetLanguageSkill(Language::Troll, 0);
						break;
					case Race::WoodElf:
						c->SetLanguageSkill(Language::Elvish, 0);
						break;
					case Race::VahShir:
						c->SetLanguageSkill(Language::CombineTongue, 0);
						c->SetLanguageSkill(Language::Erudian, 0);
						c->SetLanguageSkill(Language::VahShir, 0);
						break;
					case Race::Drakkin:
						c->SetLanguageSkill(Language::ElderDragon, 0);
						c->SetLanguageSkill(Language::Dragon, 0);
						break;
					default:
						break;
				}

				switch (chosenRace) {
					case Race::Barbarian:
						c->SetLanguageSkill(Language::CommonTongue, 100);
						c->SetLanguageSkill(Language::Barbarian, 100);
						break;
					case Race::DarkElf:
						c->SetLanguageSkill(Language::CommonTongue, 100);
						c->SetLanguageSkill(Language::DarkElvish, 100);
						c->SetLanguageSkill(Language::DarkSpeech, 100);
						c->SetLanguageSkill(Language::ElderElvish, 100);
						c->SetLanguageSkill(Language::Elvish, 25);
						break;
					case Race::Dwarf:
						c->SetLanguageSkill(Language::CommonTongue, 100);
						c->SetLanguageSkill(Language::Dwarvish, 100);
						c->SetLanguageSkill(Language::Gnomish, 25);
						break;
					case Race::Erudite:
						c->SetLanguageSkill(Language::CommonTongue, 100);
						c->SetLanguageSkill(Language::Erudian, 100);
						break;
					case Race::Froglok:
						c->SetLanguageSkill(Language::CommonTongue, 100);
						c->SetLanguageSkill(Language::Froglok, 100);
						c->SetLanguageSkill(Language::Troll, 25);
						break;
					case Race::Gnome:
						c->SetLanguageSkill(Language::CommonTongue, 100);
						c->SetLanguageSkill(Language::Dwarvish, 25);
						c->SetLanguageSkill(Language::Gnomish, 100);
						break;
					case Race::HalfElf:
						c->SetLanguageSkill(Language::CommonTongue, 100);
						c->SetLanguageSkill(Language::Elvish, 100);
						break;
					case Race::Halfling:
						c->SetLanguageSkill(Language::CommonTongue, 100);
						c->SetLanguageSkill(Language::Halfling, 100);
						break;
					case Race::HighElf:
						c->SetLanguageSkill(Language::CommonTongue, 100);
						c->SetLanguageSkill(Language::DarkElvish, 25);
						c->SetLanguageSkill(Language::ElderElvish, 25);
						c->SetLanguageSkill(Language::Elvish, 100);
						break;
					case Race::Human:
						c->SetLanguageSkill(Language::CommonTongue, 100);
						break;
					case Race::Iksar:
						c->SetLanguageSkill(Language::CommonTongue, RuleI(CharChange, IksarCommonTongue));
						c->SetLanguageSkill(Language::DarkSpeech, 100);
						c->SetLanguageSkill(Language::Lizardman, 100);
						break;
					case Race::Ogre:
						c->SetLanguageSkill(Language::CommonTongue, RuleI(CharChange, OgreCommonTongue));
						c->SetLanguageSkill(Language::DarkSpeech, 100);
						c->SetLanguageSkill(Language::Ogre, 100);
						break;
					case Race::Troll:
						c->SetLanguageSkill(Language::CommonTongue, RuleI(CharChange, TrollCommonTongue));
						c->SetLanguageSkill(Language::DarkSpeech, 100);
						c->SetLanguageSkill(Language::Troll, 100);
						break;
					case Race::WoodElf:
						c->SetLanguageSkill(Language::CommonTongue, 100);
						c->SetLanguageSkill(Language::Elvish, 100);
						break;
					case Race::VahShir:
						c->SetLanguageSkill(Language::CommonTongue, 100);
						c->SetLanguageSkill(Language::CombineTongue, 100);
						c->SetLanguageSkill(Language::Erudian, 25);
						c->SetLanguageSkill(Language::VahShir, 100);
						break;
					case Race::Drakkin:
						c->SetLanguageSkill(Language::CommonTongue, 100);
						c->SetLanguageSkill(Language::ElderDragon, 100);
						c->SetLanguageSkill(Language::Dragon, 100);
						break;
					default:
						break;
				}
			}

			if (c->GetBaseClass() != Class::Rogue && chosenRace == Class::Rogue) {
				c->SetLanguageSkill(Language::ThievesCant, 100);
			}
			else if (c->GetBaseClass() == Class::Rogue && chosenRace != Class::Rogue) {
				c->SetLanguageSkill(Language::ThievesCant, 0);
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
					if (c->HasSpellScribed(deity_spells[i]) && !(deity_req[i] & Deity::GetBitmask(chosenDeity))) {
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

			if (RuleB(CharChange, CharChangeDiscordOutputEnabled)) {
				std::ostringstream output;
				std::string currencyUsed;

				if (RuleB(CharChange, RequiresAltCurrency)) {
					currencyUsed = fmt::format(
						"x [{}]({}?a=item&id={})",
						item->Name,
						RuleS(Website, AllacloneURL),
						zone->GetCurrencyItemID(RuleI(CharChange, RequiredAltCurrency))
				);

				}
				else {
					currencyUsed = " platinum";
				}

				output <<
					"[#charchange] [" << c->GetCleanName() << "](" << RuleS(Website, CharBrowserURL) << "index.php?page=character&char=" << c->GetCleanName() << ") [ID: " << c->CharacterID() << "] did a #charchange for " << int(resetCost) << "" << currencyUsed << ""
					" from a " << (c->GetGender() == 0 ? "Male" : "Female") << " " << GetRaceIDName(c->GetBaseRace()) << " " << GetClassIDName(c->GetBaseClass()) << " worshipping " << Deity::GetName(c->GetDeity()) << ""
					" to a " << (chosenGender == 0 ? "Male" : "Female") << " " << GetRaceIDName(chosenRace) << " " << GetClassIDName(chosenClass) << " worshipping " << Deity::GetName(chosenDeity) << ""
					;
				std::string outFinal = output.str();
				zone->SendDiscordMessage(RuleS(CharChange, CharChangeDiscordWebhook), outFinal);
			}

			std::string original_combo_key = "OriginalClassRaceDeityGender-" + std::to_string(c->CharacterID());
			std::string original_combo_key_value = DataBucket::GetData(&database, original_combo_key);

			if (original_combo_key_value.empty()) {
				DataBucket::SetData(&database, original_combo_key, ("" + std::to_string(c->GetBaseClass()) + "," + std::to_string(c->GetBaseRace()) + "," + std::to_string(c->GetDeity()) + "," + std::to_string(c->GetGender())));
			}

			c->SetBaseRace(chosenRace);
			c->SetBaseClass(chosenClass);
			c->SetBaseGender(chosenGender);
			c->SetDeity(chosenDeity);

			std::vector<RaceClassAllocation> create_allocations;

			std::string point_allocations_query = "SELECT * FROM char_create_point_allocations ORDER BY id";
			auto point_allocations_query_results = database.QueryDatabase(point_allocations_query);

			if (!point_allocations_query_results.Success()) {
				c->Message(Chat::Red, "Error getting character create allocations.");
				return;
			}

			for (auto row = point_allocations_query_results.begin(); row != point_allocations_query_results.end(); ++row) {
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
				DataBucket::SetData(&database, reset_key, std::to_string(resetCost), std::to_string(lockoutDuration));
			}

			c->SetAntiCheatExempt();
			c->Kick("Character Change.");
		}
	}
	else {
		c->Message(Chat::LightBlue, "These settings match your current character. Are you having identity issues?");
	}
}

void bot_command_type_ids(Client* c, const Seperator* sep)
{
	const std::string class_substrs[16] = {
		"WAR", "CLR", "PAL", "RNG",
		"SHD", "DRU", "MNK", "BRD",
		"ROG", "SHM", "NEC", "WIZ",
		"MAG", "ENC", "BST", "BER"
	};

	const std::string race_substrs[16] = {
		"HUM", "BAR", "ERU", "ELF",
		"HIE", "DEF", "HEF", "DWF",
		"TRL", "OGR", "HFL", "GNM",
		"IKS", "VAH", "FRG", "DRK"
	};

	const uint16 race_values[16] = {
		Race::Human, Race::Barbarian, Race::Erudite, Race::WoodElf,
		Race::HighElf, Race::DarkElf, Race::HalfElf, Race::Dwarf,
		Race::Troll, Race::Ogre, Race::Halfling, Race::Gnome,
		Race::Iksar, Race::VahShir, Race::Froglok2, Race::Drakkin
	};

	const std::string deity_substrs[17] = {
		"AGNOSTIC", "BERTOX", "BRELL", "CAZIC",
		"EROLLISI", "BRISTLE", "INNORUUK", "KARANA",
		"MITHANIEL", "PREXUS", "QUELLIOUS", "RALLOS",
		"RODCET", "SOLUSEK", "TRIBUNAL", "TUNARE",
		"VEESHAN"
	};

	const uint16 deity_values[17] = {
		Deity::Agnostic2,
		Deity::Bertoxxulous,
		Deity::BrellSirilis,
		Deity::CazicThule,
		Deity::ErollisiMarr,
		Deity::Bristlebane,
		Deity::Innoruuk,
		Deity::Karana,
		Deity::MithanielMarr,
		Deity::Prexus,
		Deity::Quellious,
		Deity::RallosZek,
		Deity::RodcetNife,
		Deity::SolusekRo,
		Deity::TheTribunal,
		Deity::Tunare,
		Deity::Veeshan
	};

	const std::string gender_substrs[2] = {
		"MALE", "FEMALE"
	};

	const uint16 gender_values[2] = {
		Gender::Male,
		Gender::Female
	};

	std::string window_text;
	std::string message_separator;
	int object_count = 0;
	const int object_max = 4;

	window_text.append(
		fmt::format(
			"<c \"#EDDA74\">Classes{}<c \"#357EC7\">",
			DialogueWindow::Break()
		)
	);

	window_text.append(
		fmt::format(
			"<c \"#D4A017\">--------------------------------------------------------------------<c \"#357EC7\">",
			DialogueWindow::Break()
		)
	);

	window_text.append(DialogueWindow::Break());

	message_separator = " ";
	object_count = 0;

	for (int i = Class::Warrior; i <= Class::Beastlord; ++i) { //TODO change to berserker
		window_text.append(message_separator);

		if (object_count >= object_max) {
			window_text.append(DialogueWindow::Break());
			object_count = 0;
		}

		window_text.append(
			fmt::format("{} ({})",
				class_substrs[i],
				(i)
			)
		);

		++object_count;
		message_separator = ", ";
	}

	window_text.append(DialogueWindow::Break(2));

	window_text.append(
		fmt::format(
			"<c \"#EDDA74\">Races{}<c \"#357EC7\">",
			DialogueWindow::Break()
		)
	);

	window_text.append(
		fmt::format(
			"<c \"#D4A017\">--------------------------------------------------------------------<c \"#357EC7\">",
			DialogueWindow::Break()
		)
	);

	window_text.append(DialogueWindow::Break());

	message_separator = " ";
	object_count = 0;

	for (int i = 0; i < size(race_substrs); ++i) {
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
			"<c \"#EDDA74\">Deities{}<c \"#357EC7\">",
			DialogueWindow::Break()
		)
	);

	window_text.append(
		fmt::format(
			"<c \"#D4A017\">--------------------------------------------------------------------<c \"#357EC7\">",
			DialogueWindow::Break()
		)
	);

	window_text.append(DialogueWindow::Break());

	message_separator = " ";
	object_count = 0;

	for (int i = 0; i < size(deity_substrs); ++i) {
		window_text.append(message_separator);

		if (object_count >= object_max) {
			window_text.append(DialogueWindow::Break());
			object_count = 0;
		}

		window_text.append(
			fmt::format("{} ({})",
				deity_substrs[i],
				deity_values[i]
			)
		);

		++object_count;
		message_separator = ", ";
	}

	window_text.append(DialogueWindow::Break(2));

	window_text.append(
		fmt::format(
			"<c \"#EDDA74\">Genders{}<c \"#357EC7\">",
			DialogueWindow::Break()
		)
	);

	window_text.append(
		fmt::format(
			"<c \"#D4A017\">--------------------------------------------------------------------<c \"#357EC7\">",
			DialogueWindow::Break()
		)
	);

	window_text.append(DialogueWindow::Break());

	message_separator = " ";
	object_count = 0;

	for (int i = 0; i < size(gender_substrs); ++i) {
		window_text.append(message_separator);

		if (object_count >= object_max) {
			window_text.append(DialogueWindow::Break());
			object_count = 0;
		}

		window_text.append(
			fmt::format("{} ({})",
				gender_substrs[i],
				gender_values[i]
			)
		);

		++object_count;
		message_separator = ", ";
	}

	c->SendPopupToClient("Type IDs", window_text.c_str());
}
