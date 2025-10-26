#include "../bot_command.h"

void bot_command_depart(Client* c, const Seperator* sep)
{
	if (helper_command_alias_fail(c, "bot_command_depart", sep->arg[0], "depart")) {
		c->Message(Chat::White, "note: Tells bots to list their port locations or port to a specific location.");

		return;
	}

	if (helper_is_help_or_usage(sep->arg[1])) {
		BotCommandHelpParams p;

		p.description = { "Tells bots to list their port locations or port to a specific location." };
		p.notes =
		{
			"- This will interrupt any spell currently being cast by bots told to use the command.",
			"- Bots will still check to see if they have the spell in their spell list, whether the target is immune, spell is allowed and all other sanity checks for spells"
		};
		p.example_format = { fmt::format("{} [list | zone shortname] [optional: single | group | ae] [actionable, default: spawned]", sep->arg[0]) };
		p.examples_one =
		{
			"To tell everyone to list their portable locations:",
			fmt::format("{} list spawned",
				sep->arg[0]
			)
		};
		p.examples_two =
		{
			"To tell all bots to port to Nexus:",
			fmt::format(
				"{} nexus spawned",
				sep->arg[0]
			)
		};
		p.examples_three =
		{
			"To tell Druidbot to single target port to Butcher:",
			fmt::format(
				"{} butcher single byname Druidbot",
				sep->arg[0]
			)
		};
		p.actionables = { "target, byname, ownergroup, ownerraid, targetgroup, namesgroup, healrotationtargets, mmr, byclass, byrace, spawned" };

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

	bool single = false;
	bool group = false;
	bool ae = false;
	std::string single_arg = sep->arg[2];
	bool list = false;
	std::string destination = sep->arg[1];
	int ab_arg = 2;
	
	if (!single_arg.compare("single")) {
		++ab_arg;
		single = true;
	}
	else if (!single_arg.compare("group")) {
		++ab_arg;
		group = true;
	}
	else if (!single_arg.compare("ae")) {
		++ab_arg;
		ae = true;
	}

	if (!destination.compare("list") || destination.empty()) {
		list = true;

		if (destination.empty()) {
			c->Message(
				Chat::Yellow,
				fmt::format(
					"Use {} for information regarding this command.",
					Saylink::Silent(
						fmt::format("{} help", sep->arg[0])
					)
				).c_str()
			);
		}
	}

	Mob* tar = c->GetTarget();

	if (!tar) {
		tar = c;
	}

	std::string arg_string = sep->arg[ab_arg];

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

	BotSpell bot_spell;
	bot_spell.SpellId = 0;
	bot_spell.SpellIndex = 0;
	bot_spell.ManaCost = 0;

	bool is_success = false;
	std::vector<std::tuple<std::string, uint32_t, std::string, uint16_t, uint8_t, uint8_t>> list_zones;
	uint8 failure_type = CastFailures::Undeclared;

	for (auto bot_iter : sbl) {
		if (!bot_iter->ValidStateCheck(c)) {
			failure_type = CastFailures::InvalidState;
			continue;
		}

		bot_iter->SetCastFailure(CastFailures::Undeclared);

		bot_iter->SetCommandedSpell(true);

		std::vector<BotSpell_wPriority> bot_spell_list_itr = bot_iter->GetPrioritizedBotSpellsBySpellType(bot_iter, BotSpellTypes::Teleport, tar);

		for (std::vector<BotSpell_wPriority>::iterator itr = bot_spell_list_itr.begin(); itr != bot_spell_list_itr.end(); ++itr) {
			if (!IsValidSpell(itr->SpellId)) {
				failure_type = CastFailures::InvalidSpell;
				continue;
			}

			if (
				(single && spells[itr->SpellId].target_type != ST_Target) ||
				(group && !IsGroupSpell(itr->SpellId)) ||
				(ae && !IsAnyAESpell(itr->SpellId))
			) {
				failure_type = CastFailures::InvalidSpell;
				continue;
			}

			if (list) {
				auto zone = spells[itr->SpellId].teleport_zone;
				auto target_type = spells[itr->SpellId].target_type;
				auto bot_class = bot_iter->GetClass();

				auto it = std::find_if(list_zones.begin(), list_zones.end(),
					[&](const auto& entry) {
						return std::get<0>(entry) == zone &&
							   std::get<4>(entry) == target_type &&
							   std::get<5>(entry) == bot_class;
					});

				if (it != list_zones.end()) {
					continue;
				}

				list_zones.push_back({
					spells[itr->SpellId].teleport_zone,
					bot_iter->GetBotID(),
					bot_iter->GetCleanName(),
					itr->SpellId,
					spells[itr->SpellId].target_type,
					bot_iter->GetClass()
				});

				continue;
			}

			if (destination.compare(spells[itr->SpellId].teleport_zone)) {
				continue;
			}

			bot_iter->SetHasLoS(tar != bot_iter && BotSpellTypeRequiresLoS(BotSpellTypes::Teleport) ? bot_iter->DoLosChecks(tar) : true);

			if (!bot_iter->HasLoS() && BotRequiresLoSToCast(BotSpellTypes::Teleport, itr->SpellId)) {
				failure_type = CastFailures::LineOfSight;
				continue;
			}

			if (IsInvulnerabilitySpell(itr->SpellId)) {
				tar = bot_iter; //target self for invul type spells
			}

			if (bot_iter->IsCasting()) {
				bot_iter->RaidGroupSay(
					fmt::format(
						"Interrupting {}. I have been commanded to try to cast a [{}] spell, {} on {}.",
						bot_iter->CastingSpellID() ? spells[bot_iter->CastingSpellID()].name : "my spell",
						bot_iter->GetSpellTypeNameByID(BotSpellTypes::Teleport),
						spells[itr->SpellId].name,
						tar->GetCleanName()
					).c_str()
				);

				bot_iter->InterruptSpell();
			}

			if (bot_iter->CastSpell(itr->SpellId, tar->GetID(), EQ::spells::CastingSlot::Gem2, -1, -1)) {
				bot_iter->SetCastedSpellType(BotSpellTypes::Teleport);

				bot_iter->RaidGroupSay(
					fmt::format(
						"Casting {} [{}] on {}.",
						GetSpellName(itr->SpellId),
						bot_iter->GetSpellTypeNameByID(BotSpellTypes::Teleport),
						(tar == bot_iter ? "myself" : tar->GetCleanName())
					).c_str()
				);

				is_success = true;
			}
			else {
				failure_type = bot_iter->GetCastFailure();
			}

			bot_iter->SetCommandedSpell(false);

			if (is_success) {
				return;
			}
		}

		bot_iter->SetCommandedSpell(false);
	}

	if (list && !list_zones.empty()) {
		std::sort(list_zones.begin(), list_zones.end(),
			[](const auto& a, const auto& b) {
				if (std::get<0>(a) != std::get<0>(b)) { // Sort by zone
					return std::get<0>(a) < std::get<0>(b);
				}

				if (std::get<4>(a) != std::get<4>(b)) { // Sort by target type
					return std::get<4>(a) < std::get<4>(b);
				}

				if (std::get<5>(a) != std::get<5>(b)) { // Sort by bot class
					return std::get<5>(a) < std::get<5>(b);
				}

				return std::get<2>(a) < std::get<2>(b); // Sort by bot name
			}
		);

		for (const auto& entry : list_zones) {
			auto my_bot = entity_list.GetBotByBotID(std::get<1>(entry));

			if (my_bot) {
				std::string target_desc;
				uint16_t spell_id = std::get<3>(entry);
				uint8_t target_type = std::get<4>(entry);

				if (target_type == ST_Target) {
					target_desc = "Single";
				} else if (IsGroupSpell(spell_id)) {
					target_desc = "Group";
				} else {
					target_desc = "AE";
				}

				my_bot->RaidGroupSay(
					fmt::format(
						"I can port you to {} [{}].",
						Saylink::Silent(
							fmt::format(
								"{} {} byname {}",
								sep->arg[0],
								std::get<0>(entry),
								std::get<2>(entry)
							).c_str(),
							ZoneLongName(ZoneID(std::get<0>(entry).c_str()))
						),
						target_desc
					).c_str()
				);
			}
		}
	}

	if (
		(list && list_zones.empty()) || 
		(!list && !is_success)
	) {
		c->Message(
			Chat::Yellow,
			fmt::format(
				"No bots are capable of casting that on {}. Reason: {}.",
				tar ? tar->GetCleanName() : "your target",
				Bot::GetCastFailuresNames(failure_type)
			).c_str()
		);
	}
}
