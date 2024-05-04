#include "../bot_command.h"

void bot_command_spell_max_thresholds(Client* c, const Seperator* sep)
{
	if (helper_command_alias_fail(c, "bot_command_spell_max_thresholds", sep->arg[0], "spellmaxthresholds"))
		return;
	if (helper_is_help_or_usage(sep->arg[1])) {
		c->Message(Chat::White, "usage: %s [spelltype ID | spelltype Shortname] [current | %% value of target's health] ([actionable: target | byname | ownergroup | ownerraid | targetgroup | namesgroup | mmr | byclass | byrace | spawned] ([actionable_name])).", sep->arg[0]);
		c->Message(Chat::White, "example: [%s 9 95 spawned] or [%s dots 95 spawned] would tell the bot to start casting DoTs when the target's health reaches 95 %%.", sep->arg[0], sep->arg[0]);
		c->Message(Chat::White, "note: Can only be used for Casters or Hybrids.");
		c->Message(Chat::White, "note: Use [current] to check the current setting.");
		c->Message(
			Chat::White,
			fmt::format(
				"note: Use {} for a list of spell types by ID or {} for a list of spell types by short name.",
				Saylink::Silent(
					fmt::format("{} listid", sep->arg[0])
				),
				Saylink::Silent(
					fmt::format("{} listname", sep->arg[0])
				)
			).c_str()
		);
		//TODO add list of types
		return;
	}

	const int ab_mask = ActionableBots::ABM_Type1;

	std::string arg1 = sep->arg[1];

	// List output
	if (!arg1.compare("listid") || !arg1.compare("listname")) {
		const std::string& color_red = "red_1";
		const std::string& color_blue = "royal_blue";
		const std::string& color_green = "forest_green";
		const std::string& bright_green = "green";
		const std::string& bright_red = "red";
		const std::string& heroic_color = "gold";

		std::string fillerLine = "-----------";
		std::string spellTypeField = "Spell Type";
		std::string pluralS = "s";
		std::string idField = "ID";
		std::string shortnameField = "Short Name";

		std::string popup_text = DialogueWindow::TableRow(
			DialogueWindow::TableCell(
				fmt::format(
					"{}",
					DialogueWindow::ColorMessage(bright_green, spellTypeField)
				)
			) +
			DialogueWindow::TableCell(
				fmt::format(
					"{}",
					(!arg1.compare("listid") ? DialogueWindow::ColorMessage(bright_green, idField) : DialogueWindow::ColorMessage(bright_green, shortnameField))
				)
			)
		);

		popup_text += DialogueWindow::TableRow(
			DialogueWindow::TableCell(
				fmt::format(
					"{}",
					DialogueWindow::ColorMessage(heroic_color, fillerLine)
				)
			) +
			DialogueWindow::TableCell(
				fmt::format(
					"{}",
					DialogueWindow::ColorMessage(heroic_color, fillerLine)
				)
			)
		);

		for (int i = BotSpellTypes::START; i <= BotSpellTypes::END; ++i) {
			popup_text += DialogueWindow::TableRow(
				DialogueWindow::TableCell(
					fmt::format(
						"{}{}",
						DialogueWindow::ColorMessage(color_green, c->CastToBot()->GetSpellTypeNameByID(i)),
						DialogueWindow::ColorMessage(color_green, pluralS)
					)
				) +
				DialogueWindow::TableCell(
					fmt::format(
						"{}",
						(!arg1.compare("listid") ? DialogueWindow::ColorMessage(color_blue, std::to_string(i)) : DialogueWindow::ColorMessage(color_blue, c->CastToBot()->GetSpellTypeShortNameByID(i)))
					)
				)
			);
		}

		popup_text = DialogueWindow::Table(popup_text);

		c->SendPopupToClient("Spell Types", popup_text.c_str());

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
	else if (c->CastToBot()->GetSpellTypeIDByShortName(arg1) != INVALID_ID) {
		spellType = c->CastToBot()->GetSpellTypeIDByShortName(arg1);
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

	// Enable/Disable/Current checks
	if (sep->IsNumber(2)) {
		typeValue = atoi(sep->arg[2]);
		++ab_arg;
		if (typeValue < 0 || typeValue > 150) {
			c->Message(Chat::Yellow, "You must enter a value between 0-150 (0%% to 150%% of health).");

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
		if (!IsCasterClass(my_bot->GetClass()) && !IsHybridClass(my_bot->GetClass())) {
			continue;
		}
		if (!first_found) {
			first_found = my_bot;
		}
		if (current_check) {
			c->Message(
				Chat::Green,
				fmt::format(
					"{} says, 'My current max threshold for {}s is {}%%.'",
					my_bot->GetCleanName(),
					c->CastToBot()->GetSpellTypeNameByID(spellType),
					my_bot->GetSpellMaxThreshold(spellType)
				).c_str()
			);
		}
		else {
			my_bot->SetSpellMaxThreshold(spellType, typeValue);
			++success_count;
		}
	}
	if (!current_check) {
		if (success_count == 1 && first_found) {
			c->Message(
				Chat::Green,
				fmt::format(
					"{} says, 'My max threshold for {}s was set to {}%%.'",
					first_found->GetCleanName(),
					c->CastToBot()->GetSpellTypeNameByID(spellType),
					first_found->GetSpellMaxThreshold(spellType)
				).c_str()
			);
		}
		else {
			c->Message(
				Chat::Green,
				fmt::format(
					"{} of your bots set max threshold for {}s to {}%%.",
					success_count,
					c->CastToBot()->GetSpellTypeNameByID(spellType),
					typeValue
				).c_str()
			);
		}
	}
}
