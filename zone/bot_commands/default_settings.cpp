#include "../bot_command.h"

void bot_command_default_settings(Client* c, const Seperator* sep)
{
	if (helper_command_alias_fail(c, "bot_command_default_settings", sep->arg[0], "defaultsettings")) {
		return;
	}

	if (helper_is_help_or_usage(sep->arg[1])) {
		const std::string& color_red = "red_1";
		const std::string& color_blue = "royal_blue";
		const std::string& color_green = "forest_green";
		const std::string& bright_green = "green";
		const std::string& bright_red = "red";
		const std::string& heroic_color = "gold";

		std::string fillerLine = "--------------------------------------------------------------------";
		std::string fillerDia = DialogueWindow::TableRow(DialogueWindow::TableCell(fmt::format("{}", DialogueWindow::ColorMessage(heroic_color, fillerLine))));
		std::string indent = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
		std::string bullet = "- ";

		std::string popup_text = DialogueWindow::TableRow(
			DialogueWindow::TableCell(
				fmt::format(
					"{}",
					DialogueWindow::ColorMessage(
						bright_green,
						fmt::format(
							"{}{}{}How to use:",
							indent,
							indent,
							indent
						)
					)
				)
			)
		);

		popup_text += DialogueWindow::TableRow(
			DialogueWindow::TableCell(
				fmt::format(
					"{}",
					DialogueWindow::ColorMessage(bright_green, 
						fmt::format(
							"{} [option] [actionable]",
							sep->arg[0]
						)
					)
				)
			)
		);

		popup_text += fillerDia;

		popup_text += DialogueWindow::TableRow(
			DialogueWindow::TableCell(
				fmt::format(
					"{}",
					DialogueWindow::ColorMessage(
						color_blue,
						fmt::format(
							"{}{}{}Available actionables are:",
							indent,
							indent,
							indent
						)
					)
				)
			)
		);

		popup_text += DialogueWindow::TableRow(
			DialogueWindow::TableCell(
				fmt::format(
					"{}",
					DialogueWindow::ColorMessage(
						color_blue,
						fmt::format(
							"{}target | byname | ownergroup | ownerraid | targetgroup",
							bullet
						)
					)
				)
			)
		);

		popup_text += DialogueWindow::TableRow(
			DialogueWindow::TableCell(
				fmt::format(
					"{}",
					DialogueWindow::ColorMessage(
						color_blue,
						fmt::format(
							"{}namesgroup | healrotationtargets | mmr | byclass",
							bullet
						)
					)
				)
			)
		);

		popup_text += DialogueWindow::TableRow(
			DialogueWindow::TableCell(
				fmt::format(
					"{}",
					DialogueWindow::ColorMessage(
						color_blue,
						fmt::format(
							"{}byrace | spawned",
							bullet
						)
					)
				)
			)
		);

		popup_text += fillerDia;

		popup_text += DialogueWindow::TableRow(
			DialogueWindow::TableCell(
				fmt::format(
					"{}",
					DialogueWindow::ColorMessage(
						color_blue,
						fmt::format(
							"{}{}{}Available options are:",
							indent,
							indent,
							indent
						)
					)
				)
			)
		);

		popup_text += DialogueWindow::TableRow(
			DialogueWindow::TableCell(
				fmt::format(
					"{}",
					DialogueWindow::ColorMessage(
						color_blue,
						fmt::format(
							"{}all | misc | spellsettings | spelltypesettings | holds",
							bullet
						)
					)
				)
			)
		);

		popup_text += DialogueWindow::TableRow(
			DialogueWindow::TableCell(
				fmt::format(
					"{}",
					DialogueWindow::ColorMessage(
						color_blue,
						fmt::format(
							"{}delays | minthresholds | maxthresholds | aggrocheck",
							bullet
						)
					)
				)
			)
		);

		popup_text += DialogueWindow::TableRow(
			DialogueWindow::TableCell(
				fmt::format(
					"{}",
					DialogueWindow::ColorMessage(
						color_blue,
						fmt::format(
							"{}minmanapct | maxmanapct | minhppct | maxhppct",
							bullet
						)
					)
				)
			)
		);

		popup_text += DialogueWindow::TableRow(
			DialogueWindow::TableCell(
				fmt::format(
					"{}",
					DialogueWindow::ColorMessage(
						color_blue,
						fmt::format(
							"{}idlepriority | engagedpriority | pursuepriority | aegroup",
							bullet
						)
					)
				)
			)
		);

		popup_text += fillerDia;

		popup_text += DialogueWindow::TableRow(
			DialogueWindow::TableCell(
				fmt::format(
					"{}",
					DialogueWindow::ColorMessage(
						color_green,
						"To restore delays for Clerics:"
					)
				)
			)
		);

		popup_text += DialogueWindow::TableRow(
			DialogueWindow::TableCell(
				fmt::format(
					"{}",
					DialogueWindow::ColorMessage(
						heroic_color,
						"^defaultsettings delays byclass 2"
					)
				)
			)
		);

		popup_text += fillerDia;

		popup_text += DialogueWindow::TableRow(
			DialogueWindow::TableCell(
				fmt::format(
					"{}",
					DialogueWindow::ColorMessage(
						color_green,
						fmt::format(
							"{}[misc] restores all miscellaneous options such as:",
							bullet
						)
					)
				)
			)
		);

		popup_text += DialogueWindow::TableRow(
			DialogueWindow::TableCell(
				fmt::format(
					"{}",
					DialogueWindow::ColorMessage(
						color_blue,
						fmt::format(
							"{}^showhelm, ^followd, ^stopmeleelevel",
							bullet
						)
					)
				)
			)
		);

		popup_text += DialogueWindow::TableRow(
			DialogueWindow::TableCell(
				fmt::format(
					"{}",
					DialogueWindow::ColorMessage(
						color_blue,
						fmt::format(
							"{}^enforcespellsettings, ^bottoggleranged, ^petsettype",
							bullet
						)
					)
				)
			)
		);

		popup_text += DialogueWindow::TableRow(
			DialogueWindow::TableCell(
				fmt::format(
					"{}",
					DialogueWindow::ColorMessage(
						color_blue,
						fmt::format(
							"{}^behindmob, ^casterrange and ^illusionblock",
							bullet
						)
					)
				)
			)
		);

		popup_text += fillerDia;

		popup_text += DialogueWindow::TableRow(
			DialogueWindow::TableCell(
				fmt::format(
					"{}",
					DialogueWindow::ColorMessage(
						color_green,
						fmt::format(
							"{}[spellsettings] will restore ^spellsettings options",
							bullet
						)
					)
				)
			)
		);

		popup_text += DialogueWindow::TableRow(
			DialogueWindow::TableCell(
				fmt::format(
					"{}",
					DialogueWindow::ColorMessage(
						color_green,
						fmt::format(
							"{}[spelltypesettings] restores all spell type settings",
							bullet
						)
					)
				)
			)
		);

		popup_text += DialogueWindow::TableRow(
			DialogueWindow::TableCell(
				fmt::format(
					"{}",
					DialogueWindow::ColorMessage(
						color_green,
						fmt::format(
							"{}[all] restores all settings",
							bullet
						)
					)
				)
			)
		);

		popup_text += fillerDia;

		popup_text += DialogueWindow::TableRow(
			DialogueWindow::TableCell(
				fmt::format(
					"{}",
					DialogueWindow::ColorMessage(
						color_green,
						fmt::format(
							"{}The remaining options restore that specific type",
							bullet
						)
					)
				)
			)
		);

		popup_text = DialogueWindow::Table(popup_text);

		c->SendPopupToClient(sep->arg[0], popup_text.c_str());

		c->Message(
			Chat::Yellow,
			fmt::format(
				"Use {} for information about race/class IDs.",
				Saylink::Silent("^classracelist")
			).c_str()
		);

		return;
	}

	bool validOption = false;
	std::vector<std::string> options = { "all", "misc", "spellsettings", "spelltypesettings", "holds", "delays", "minthresholds", "maxthresholds", "aggrocheck", "minmanapct", "maxmanapct", "minhppct", "maxhppct", "idlepriority", "engagedpriority", "pursuepriority", "aegroup" };
	for (int i = 0; i < options.size(); i++) {
		if (sep->arg[1] == options[i]) {
			validOption = true;
			break;
		}
	}

	if (!validOption) {
		c->Message(
			Chat::Yellow,
			fmt::format(
				"Incorrect argument, use {} for information regarding this command.",
				Saylink::Silent(
					fmt::format("{} help", sep->arg[0])
				)
			).c_str()
		);

		return;
	}

	const int ab_mask = ActionableBots::ABM_Type1;
	int ab_arg = 2;
	std::string class_race_arg = sep->arg[ab_arg];
	bool class_race_check = false;
	if (!class_race_arg.compare("byclass") || !class_race_arg.compare("byrace")) {
		class_race_check = true;
	}

	std::list<Bot*> sbl;
	if (ActionableBots::PopulateSBL(c, sep->arg[ab_arg], sbl, ab_mask, !class_race_check ? sep->arg[ab_arg + 1] : nullptr, class_race_check ? atoi(sep->arg[ab_arg + 1]) : 0) == ActionableBots::ABT_None) {
		return;
	}

	sbl.remove(nullptr);

	Bot* first_found = nullptr;
	int success_count = 0;
	std::string output = "";
	for (auto my_bot : sbl) {
		if (!first_found) {
			first_found = my_bot;
		}

		if (!strcasecmp(sep->arg[1], "misc")) {
			for (uint16 i = BotBaseSettings::START; i <= BotBaseSettings::END; ++i) {
				my_bot->SetBotBaseSetting(i, my_bot->GetDefaultBotBaseSetting(i));
				output = "miscellanous settings";
			}
		}
		else if (!strcasecmp(sep->arg[1], "holds")) {
			for (uint16 i = BotSpellTypes::START; i <= BotSpellTypes::END; ++i) {
				my_bot->SetSpellHold(i, my_bot->GetDefaultSpellHold(i));
				output = "hold settings";
			}
		}
		else if (!strcasecmp(sep->arg[1], "delays")) {
			for (uint16 i = BotSpellTypes::START; i <= BotSpellTypes::END; ++i) {
				my_bot->SetSpellDelay(i, my_bot->GetDefaultSpellDelay(i));
				output = "delay settings";
			}
		}
		else if (!strcasecmp(sep->arg[1], "minthresholds")) {
			for (uint16 i = BotSpellTypes::START; i <= BotSpellTypes::END; ++i) {
				my_bot->SetSpellMinThreshold(i, my_bot->GetDefaultSpellMinThreshold(i));
				output = "minimum threshold settings";
			}
		}
		else if (!strcasecmp(sep->arg[1], "maxthresholds")) {
			for (uint16 i = BotSpellTypes::START; i <= BotSpellTypes::END; ++i) {
				my_bot->SetSpellMaxThreshold(i, my_bot->GetDefaultSpellMaxThreshold(i));
				output = "maximum threshold settings";
			}
		}
		else if (!strcasecmp(sep->arg[1], "aggrocheck")) {
			for (uint16 i = BotSpellTypes::START; i <= BotSpellTypes::END; ++i) {
				my_bot->SetSpellTypeAggroCheck(i, my_bot->GetDefaultSpellTypeAggroCheck(i));
				output = "aggro check settings";
			}
		}
		else if (!strcasecmp(sep->arg[1], "minmanapct")) {
			for (uint16 i = BotSpellTypes::START; i <= BotSpellTypes::END; ++i) {
				my_bot->SetSpellTypeMinManaLimit(i, my_bot->GetDefaultSpellTypeMinManaLimit(i));
				output = "min mana settings";
			}
		}
		else if (!strcasecmp(sep->arg[1], "maxmanapct")) {
			for (uint16 i = BotSpellTypes::START; i <= BotSpellTypes::END; ++i) {
				my_bot->SetSpellTypeMaxManaLimit(i, my_bot->GetDefaultSpellTypeMaxManaLimit(i));
				output = "max mana settings";
			}
		}
		else if (!strcasecmp(sep->arg[1], "minhppct")) {
			for (uint16 i = BotSpellTypes::START; i <= BotSpellTypes::END; ++i) {
				my_bot->SetSpellTypeMinHPLimit(i, my_bot->GetDefaultSpellTypeMinHPLimit(i));
				output = "min hp settings";
			}
		}
		else if (!strcasecmp(sep->arg[1], "maxhppct")) {
			for (uint16 i = BotSpellTypes::START; i <= BotSpellTypes::END; ++i) {
				my_bot->SetSpellTypeMaxHPLimit(i, my_bot->GetDefaultSpellTypeMaxHPLimit(i));
				output = "max hp settings";
			}
		}
		else if (!strcasecmp(sep->arg[1], "idlepriority")) {
			for (uint16 i = BotSpellTypes::START; i <= BotSpellTypes::END; ++i) {
				my_bot->SetSpellTypePriority(i, BotPriorityCategories::Idle, my_bot->GetDefaultSpellTypePriority(i, BotPriorityCategories::Idle, my_bot->GetClass()));
				output = "idle priority settings";
			}
		}
		else if (!strcasecmp(sep->arg[1], "engagedpriority")) {
			for (uint16 i = BotSpellTypes::START; i <= BotSpellTypes::END; ++i) {
				my_bot->SetSpellTypePriority(i, BotPriorityCategories::Engaged, my_bot->GetDefaultSpellTypePriority(i, BotPriorityCategories::Engaged, my_bot->GetClass()));
				output = "engaged priority settings";
			}
		}
		else if (!strcasecmp(sep->arg[1], "pursuepriority")) {
			for (uint16 i = BotSpellTypes::START; i <= BotSpellTypes::END; ++i) {
				my_bot->SetSpellTypePriority(i, BotPriorityCategories::Pursue, my_bot->GetDefaultSpellTypePriority(i, BotPriorityCategories::Pursue, my_bot->GetClass()));
				output = "pursue priority settings";
			}
		}
		else if (!strcasecmp(sep->arg[1], "aegroup")) {
			for (uint16 i = BotSpellTypes::START; i <= BotSpellTypes::END; ++i) {
				my_bot->SetSpellTypeAEOrGroupTargetCount(i, my_bot->GetDefaultSpellTypeAEOrGroupTargetCount(i));
				output = "ae/group count settings";
			}
		}
		else if (!strcasecmp(sep->arg[1], "spellsettings")) {
			my_bot->ResetBotSpellSettings();
			output = "^spellsettings";
		}
		else if (!strcasecmp(sep->arg[1], "spelltypesettings")) {
			for (uint16 i = BotSpellTypes::START; i <= BotSpellTypes::END; ++i) {
				my_bot->SetSpellHold(i, my_bot->GetDefaultSpellHold(i));
				my_bot->SetSpellDelay(i, my_bot->GetDefaultSpellDelay(i));
				my_bot->SetSpellMinThreshold(i, my_bot->GetDefaultSpellMinThreshold(i));
				my_bot->SetSpellMaxThreshold(i, my_bot->GetDefaultSpellMaxThreshold(i));
				my_bot->SetSpellTypeAggroCheck(i, my_bot->GetDefaultSpellTypeAggroCheck(i));
				my_bot->SetSpellTypeMinManaLimit(i, my_bot->GetDefaultSpellTypeMinManaLimit(i));
				my_bot->SetSpellTypeMaxManaLimit(i, my_bot->GetDefaultSpellTypeMaxManaLimit(i));
				my_bot->SetSpellTypeMinHPLimit(i, my_bot->GetDefaultSpellTypeMinHPLimit(i));
				my_bot->SetSpellTypeMaxHPLimit(i, my_bot->GetDefaultSpellTypeMaxHPLimit(i));
				my_bot->SetSpellTypePriority(i, BotPriorityCategories::Idle, my_bot->GetDefaultSpellTypePriority(i, BotPriorityCategories::Idle, my_bot->GetClass()));
				my_bot->SetSpellTypePriority(i, BotPriorityCategories::Engaged, my_bot->GetDefaultSpellTypePriority(i, BotPriorityCategories::Engaged, my_bot->GetClass()));
				my_bot->SetSpellTypePriority(i, BotPriorityCategories::Pursue, my_bot->GetDefaultSpellTypePriority(i, BotPriorityCategories::Pursue, my_bot->GetClass()));
				my_bot->SetSpellTypeAEOrGroupTargetCount(i, my_bot->GetDefaultSpellTypeAEOrGroupTargetCount(i));
				output = "spell type settings";
			}
		}
		else if (!strcasecmp(sep->arg[1], "all")) {
			for (uint16 i = BotBaseSettings::START; i <= BotBaseSettings::END; ++i) {
				my_bot->SetBotBaseSetting(i, my_bot->GetDefaultBotBaseSetting(i));
			}

			for (uint16 i = BotSpellTypes::START; i <= BotSpellTypes::END; ++i) {
				my_bot->SetSpellHold(i, my_bot->GetDefaultSpellHold(i));
				my_bot->SetSpellDelay(i, my_bot->GetDefaultSpellDelay(i));
				my_bot->SetSpellMinThreshold(i, my_bot->GetDefaultSpellMinThreshold(i));
				my_bot->SetSpellMaxThreshold(i, my_bot->GetDefaultSpellMaxThreshold(i));
				my_bot->SetSpellTypeAggroCheck(i, my_bot->GetDefaultSpellTypeAggroCheck(i));
				my_bot->SetSpellTypeMinManaLimit(i, my_bot->GetDefaultSpellTypeMinManaLimit(i));
				my_bot->SetSpellTypeMaxManaLimit(i, my_bot->GetDefaultSpellTypeMaxManaLimit(i));
				my_bot->SetSpellTypeMinHPLimit(i, my_bot->GetDefaultSpellTypeMinHPLimit(i));
				my_bot->SetSpellTypeMaxHPLimit(i, my_bot->GetDefaultSpellTypeMaxHPLimit(i));
				my_bot->SetSpellTypePriority(i, BotPriorityCategories::Idle, my_bot->GetDefaultSpellTypePriority(i, BotPriorityCategories::Idle, my_bot->GetClass()));
				my_bot->SetSpellTypePriority(i, BotPriorityCategories::Engaged, my_bot->GetDefaultSpellTypePriority(i, BotPriorityCategories::Engaged, my_bot->GetClass()));
				my_bot->SetSpellTypePriority(i, BotPriorityCategories::Pursue, my_bot->GetDefaultSpellTypePriority(i, BotPriorityCategories::Pursue, my_bot->GetClass()));
				my_bot->SetSpellTypeAEOrGroupTargetCount(i, my_bot->GetDefaultSpellTypeAEOrGroupTargetCount(i));
			};

			my_bot->ResetBotSpellSettings();

			output = "settings";

		}

		++success_count;
	}

	if (success_count == 1) {
		c->Message(
			Chat::Green,
			fmt::format(
				"{} says, 'My {} were restored.'",
				first_found->GetCleanName(),
				output
			).c_str()
		);
	}
	else {
		c->Message(
			Chat::Green,
			fmt::format(
				"{} of your bot's {} were restored.",
				success_count,
				output
			).c_str()
		);
	}
}
