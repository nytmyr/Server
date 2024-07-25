#include "../bot_command.h"

void bot_command_spell_delays(Client* c, const Seperator* sep)
{
	if (helper_command_alias_fail(c, "bot_command_spell_delays", sep->arg[0], "spelldelays"))
		return;
	if (helper_is_help_or_usage(sep->arg[1])) {
		//c->Message(Chat::Cyan, "usage: %s [spelltype ID | spelltype shortname] [current | value in milliseconds] ([actionable: target | byname | ownergroup | ownerraid | targetgroup | namesgroup | healrotationtargets | mmr | byclass | byrace | spawned] ([actionable_name]))", sep->arg[0]);
		//c->Message(Chat::DarkGray, "----------------------------------------");
		//c->Message(Chat::Magenta, "example: [%s 9 8000 spawned] or [%s dots 8000 spawned] would set the delay between casts for all DoT spells to 8 seconds", sep->arg[0], sep->arg[0]);
		//c->Message(Chat::DarkGray, "----------------------------------------");
		//c->Message(Chat::Yellow, "note: Use [current] to check the current setting");
		//c->Message(Chat::DarkGray, "----------------------------------------");
		//c->CastToBot()->SendSpellTypesWindow(c, sep->arg[0], "", "", true);

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
					DialogueWindow::ColorMessage(
						bright_green,
						fmt::format(
							"{} [Type ID] [value] [actionable]",
							sep->arg[0]
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
						bright_green,
						fmt::format(
							"{} [Type Shortname] [value] [actionable]",
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
							"To set DoTs to have an 8 second delay:",
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
						heroic_color,
						fmt::format(
							"{} dots 8000 spawned",
							sep->arg[0]
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
						heroic_color,
						fmt::format(
							"{} 8 8000 spawned",
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
						color_green,
						fmt::format(
							"{}Use current to check the current setting",
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
						heroic_color,
						fmt::format(
							"{} dots current spawned",
							sep->arg[0]
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
						heroic_color,
						fmt::format(
							"{} 8 current spawned",
							sep->arg[0]
						)
					)
				)
			)
		);

		popup_text = DialogueWindow::Table(popup_text);

		c->SendPopupToClient(sep->arg[0], popup_text.c_str());
		c->CastToBot()->SendSpellTypesWindow(c, sep->arg[0], "", "", true);
		c->Message(
			Chat::Yellow,
			fmt::format(
				"Use {} for information about race/class IDs.",
				Saylink::Silent("^classracelist")
			).c_str()
		);

		return;
	}

	const int ab_mask = ActionableBots::ABM_Type1;

	std::string arg1 = sep->arg[1];

	if (!arg1.compare("listid") || !arg1.compare("listname")){
		c->CastToBot()->SendSpellTypesWindow(c, sep->arg[0], sep->arg[1], sep->arg[2]);
		return;
	}

	std::string arg2 = sep->arg[2];
	int ab_arg = 2;
	bool current_check = false;
	uint16 spellType = 0;
	uint32 typeValue = 0;

	// String/Int type checks
	if (sep->IsNumber(1)) {
		spellType = atoi(sep->arg[1]);

		if (spellType < BotSpellTypes::START || spellType > BotSpellTypes::END) {
			c->Message(Chat::Yellow, "You must choose a valid spell type. Spell types range from %i to %i", BotSpellTypes::START, BotSpellTypes::END);

			return;
		}
	}
	else {
		if (c->GetSpellTypeIDByShortName(arg1) != UINT16_MAX) {
			spellType = c->GetSpellTypeIDByShortName(arg1);
		}
		else {
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
	}

	// Enable/Disable/Current checks
	if (sep->IsNumber(2)) {
		typeValue = atoi(sep->arg[2]);
		++ab_arg;
		if (typeValue < 1 || typeValue > 60000) {
			c->Message(Chat::Yellow, "You must enter a value between 1-60000 (1ms to 60s).");

			return;
		}
	}
	else if (!arg2.compare("current")) {
		++ab_arg;
		current_check = true;
	}
	else {
		c->Message(
			Chat::Yellow,
			fmt::format(
				"Incorrect argument, use {} for information regarding this command.",
				Saylink::Silent(
					fmt::format("{} help", sep->arg[0])
				)
			).c_str()
		);

		c->Message(
			Chat::Yellow,
			fmt::format(
				"Use {} for information about race/class IDs.",
				Saylink::Silent("^create help")
			).c_str()
		);

		return;
	}

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
	for (auto my_bot : sbl) {
		if (!first_found) {
			first_found = my_bot;
		}
		if (current_check) {
			c->Message(
				Chat::Green,
				fmt::format(
					"{} says, 'My current {} delay is {} seconds.'",
					my_bot->GetCleanName(),
					c->GetSpellTypeNameByID(spellType),
					my_bot->GetSpellDelay(spellType) / 1000.00
				).c_str()
			);
		}
		else {
			my_bot->SetSpellDelay(spellType, typeValue);
			++success_count;
		}
	}
	if (!current_check) {
		if (success_count == 1 && first_found) {
			c->Message(
				Chat::Green,
				fmt::format(
					"{} says, 'My {} delay was set to {} seconds.'",
					first_found->GetCleanName(),
					c->GetSpellTypeNameByID(spellType),
					first_found->GetSpellDelay(spellType) / 1000.00
				).c_str()
			);
		}
		else {
			c->Message(
				Chat::Green,
				fmt::format(
					"{} of your bots set their {} delay to {} seconds.",
					success_count,
					c->GetSpellTypeNameByID(spellType),
					typeValue / 1000.00
				).c_str()
			);
		}
	}
}
