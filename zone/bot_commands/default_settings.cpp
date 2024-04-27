#include "../bot_command.h"

void bot_command_default_settings(Client* c, const Seperator* sep)
{
	if (helper_command_alias_fail(c, "bot_command_default_settings", sep->arg[0], "defaultsettings")) {
		return;
	}

	if (helper_is_help_or_usage(sep->arg[1])) {
		c->Message(Chat::White, "usage: %s (<target>) ([option: all | misc | spellsettings | spelltypesettings | holds | delays | minthresholds | maxthresholds | aggrocheck | minmanapct | maxmanapct | idlepriority | engagedpriority | pursuepriority | aegroup])", sep->arg[0]);
		c->Message(Chat::White, "example: target the bot you want to restore default hold settings to and tope ^defaultsettings holds");
		c->Message(Chat::White, "note: optional argument 'misc' will default show helm, follow distance, stop melee level, archery settings, pet type, hold damage shields, hold resists, behind mob status, caster range and illusion block.");
		c->Message(Chat::White, "note: optional argument 'spellsettings' will default ^spellsettings options.");
		c->Message(Chat::White, "note: optional argument 'spelltypesettings' will default all settings for different spell types.");
		c->Message(Chat::White, "note: optional argument 'all' will default all settings.");
		c->Message(Chat::White, "note: optional argument 'holds' will default all hold settings by spell type.");
		c->Message(Chat::White, "note: optional argument 'delays' will default all delay settings by spell type.");
		c->Message(Chat::White, "note: optional argument 'minthresholds' will default all minimum threshold settings by spell type.");
		c->Message(Chat::White, "note: optional argument 'maxthresholds' will default all maximum threshold settings by spell type.");
		c->Message(Chat::White, "note: optional argument 'aggrocheck' will default all aggro check settings by spell type.");
		c->Message(Chat::White, "note: optional argument 'minmanapct' will default all Minimum Mana settings by spell type.");
		c->Message(Chat::White, "note: optional argument 'maxmanapct' will default all Maximum Mana settings by spell type.");
		c->Message(Chat::White, "note: optional argument 'minhppct' will default all Minimum HP settings by spell type.");
		c->Message(Chat::White, "note: optional argument 'maxhppct' will default all Maximum HP settings by spell type.");
		c->Message(Chat::White, "note: optional argument 'idlepriority' will default all idle spell type priority settings by spell type.");
		c->Message(Chat::White, "note: optional argument 'engagedpriority' will default all engaged spell type priority settings by spell type.");
		c->Message(Chat::White, "note: optional argument 'pursuepriority' will default all pursue spell type priority settings by spell type.");
		c->Message(Chat::White, "note: optional argument 'aegroup' will default all ae/group counts for each spell type.");
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
		std::string indent = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
		std::string bullet = "- ";

		std::string usageLine =
			fmt::format(
				"usage: {} [target] [option]",
				sep->arg[0]
			)
			;

		std::string popup_text = DialogueWindow::TableRow(
			DialogueWindow::TableCell(
				fmt::format(
					"{}",
					DialogueWindow::ColorMessage(bright_green, usageLine)
				)
			)
		);

		popup_text += DialogueWindow::TableRow(
			DialogueWindow::TableCell(
				fmt::format(
					"{}",
					DialogueWindow::ColorMessage(heroic_color, fillerLine)
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

		popup_text += DialogueWindow::TableRow(
			DialogueWindow::TableCell(
				fmt::format(
					"{}",
					DialogueWindow::ColorMessage(heroic_color, fillerLine)
				)
			)
		);

		popup_text += DialogueWindow::TableRow(
			DialogueWindow::TableCell(
				fmt::format(
					"{}",
					DialogueWindow::ColorMessage(
						color_green,
						"Target the bot you want to restore delay settings to and type:"
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
						"^defaultsettings delays"
					)
				)
			)
		);

		popup_text += DialogueWindow::TableRow(
			DialogueWindow::TableCell(
				fmt::format(
					"{}",
					DialogueWindow::ColorMessage(heroic_color, fillerLine)
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

		popup_text += DialogueWindow::TableRow(
			DialogueWindow::TableCell(
				fmt::format(
					"{}",
					DialogueWindow::ColorMessage(heroic_color, fillerLine)
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

		popup_text += DialogueWindow::TableRow(
			DialogueWindow::TableCell(
				fmt::format(
					"{}",
					DialogueWindow::ColorMessage(heroic_color, fillerLine)
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
							"{}The remainder restore that specific type",
							bullet
						)
					)
				)
			)
		);

		popup_text = DialogueWindow::Table(popup_text);

		c->SendPopupToClient(sep->arg[0], popup_text.c_str());
		return;
	}

	auto tar = c->GetTarget();

	if (!tar || !tar->IsBot() || tar->GetOwner() != c) {
		c->Message(Chat::Yellow, "You must target a bot you own to use this command.", sep->arg[1]);
		return;
	}

	if (!strcasecmp(sep->arg[1], "misc")) {
		for (uint16 i = BotBaseSettings::START; i <= BotBaseSettings::END; ++i) {
			tar->CastToBot()->SetBotBaseSetting(i, tar->CastToBot()->GetDefaultBotBaseSetting(i));
			c->Message(Chat::Yellow, "%s's miscellanous settings were restored.", tar->GetCleanName());
		}
	}
	else if (!strcasecmp(sep->arg[1], "holds")) {
		for (uint16 i = BotSpellTypes::START; i <= BotSpellTypes::END; ++i) {
			tar->CastToBot()->SetSpellHold(i, tar->CastToBot()->GetDefaultSpellHold(i));
			c->Message(Chat::Yellow, "%s's hold settings were restored.", tar->GetCleanName());
		}
	}
	else if (!strcasecmp(sep->arg[1], "delays")) {
		for (uint16 i = BotSpellTypes::START; i <= BotSpellTypes::END; ++i) {
			tar->CastToBot()->SetSpellDelay(i, tar->CastToBot()->GetDefaultSpellDelay(i));
			c->Message(Chat::Yellow, "%s's delay settings were restored.", tar->GetCleanName());
		}
	}
	else if (!strcasecmp(sep->arg[1], "minthresholds")) {
		for (uint16 i = BotSpellTypes::START; i <= BotSpellTypes::END; ++i) {
			tar->CastToBot()->SetSpellMinThreshold(i, tar->CastToBot()->GetDefaultSpellMinThreshold(i));
			c->Message(Chat::Yellow, "%s's minimum threshold settings were restored.", tar->GetCleanName());
		}
	}
	else if (!strcasecmp(sep->arg[1], "maxthresholds")) {
		for (uint16 i = BotSpellTypes::START; i <= BotSpellTypes::END; ++i) {
			tar->CastToBot()->SetSpellMaxThreshold(i, tar->CastToBot()->GetDefaultSpellMaxThreshold(i));
			c->Message(Chat::Yellow, "%s's maximum threshold settings were restored.", tar->GetCleanName());
		}
	}
	else if (!strcasecmp(sep->arg[1], "aggrocheck")) {
		for (uint16 i = BotSpellTypes::START; i <= BotSpellTypes::END; ++i) {
			tar->CastToBot()->SetSpellTypeAggroCheck(i, tar->CastToBot()->GetDefaultSpellTypeAggroCheck(i));
			c->Message(Chat::Yellow, "%s's aggro check settings were restored.", tar->GetCleanName());
		}
	}
	else if (!strcasecmp(sep->arg[1], "minmanapct")) {
		for (uint16 i = BotSpellTypes::START; i <= BotSpellTypes::END; ++i) {
			tar->CastToBot()->SetSpellTypeMinManaLimit(i, tar->CastToBot()->GetDefaultSpellTypeMinManaLimit(i));
			c->Message(Chat::Yellow, "%s's min mana settings were restored.", tar->GetCleanName());
		}
	}
	else if (!strcasecmp(sep->arg[1], "maxmanapct")) {
		for (uint16 i = BotSpellTypes::START; i <= BotSpellTypes::END; ++i) {
			tar->CastToBot()->SetSpellTypeMaxManaLimit(i, tar->CastToBot()->GetDefaultSpellTypeMaxManaLimit(i));
			c->Message(Chat::Yellow, "%s's max mana settings were restored.", tar->GetCleanName());
		}
	}
	else if (!strcasecmp(sep->arg[1], "minhppct")) {
		for (uint16 i = BotSpellTypes::START; i <= BotSpellTypes::END; ++i) {
			tar->CastToBot()->SetSpellTypeMinHPLimit(i, tar->CastToBot()->GetDefaultSpellTypeMinHPLimit(i));
			c->Message(Chat::Yellow, "%s's min hp settings were restored.", tar->GetCleanName());
		}
	}
	else if (!strcasecmp(sep->arg[1], "maxhppct")) {
		for (uint16 i = BotSpellTypes::START; i <= BotSpellTypes::END; ++i) {
			tar->CastToBot()->SetSpellTypeMaxHPLimit(i, tar->CastToBot()->GetDefaultSpellTypeMaxHPLimit(i));
			c->Message(Chat::Yellow, "%s's max hp settings were restored.", tar->GetCleanName());
		}
	}
	else if (!strcasecmp(sep->arg[1], "idlepriority")) {
		for (uint16 i = BotSpellTypes::START; i <= BotSpellTypes::END; ++i) {
			tar->CastToBot()->SetSpellTypePriority(i, BotPriorityCategories::Idle, tar->CastToBot()->GetDefaultSpellTypePriority(i, BotPriorityCategories::Idle, tar->GetClass()));
			c->Message(Chat::Yellow, "%s's idle priority settings were restored.", tar->GetCleanName());
		}
	}
	else if (!strcasecmp(sep->arg[1], "engagedpriority")) {
		for (uint16 i = BotSpellTypes::START; i <= BotSpellTypes::END; ++i) {
			tar->CastToBot()->SetSpellTypePriority(i, BotPriorityCategories::Engaged, tar->CastToBot()->GetDefaultSpellTypePriority(i, BotPriorityCategories::Engaged, tar->GetClass()));
			c->Message(Chat::Yellow, "%s's engaged priority settings were restored.", tar->GetCleanName());
		}
	}
	else if (!strcasecmp(sep->arg[1], "pursuepriority")) {
		for (uint16 i = BotSpellTypes::START; i <= BotSpellTypes::END; ++i) {
			tar->CastToBot()->SetSpellTypePriority(i, BotPriorityCategories::Pursue, tar->CastToBot()->GetDefaultSpellTypePriority(i, BotPriorityCategories::Pursue, tar->GetClass()));
			c->Message(Chat::Yellow, "%s's pursue priority settings were restored.", tar->GetCleanName());
		}
	}
	else if (!strcasecmp(sep->arg[1], "aegroup")) {
		for (uint16 i = BotSpellTypes::START; i <= BotSpellTypes::END; ++i) {
			tar->CastToBot()->SetSpellTypeAEOrGroupTargetCount(i, tar->CastToBot()->GetDefaultSpellTypeAEOrGroupTargetCount(i));
			c->Message(Chat::Yellow, "%s's ae/group count settings were restored.", tar->GetCleanName());
		}
	}
	else if (!strcasecmp(sep->arg[1], "spellsettings")) {
		tar->CastToBot()->ResetBotSpellSettings();
		c->Message(Chat::Yellow, "%s's ^spellsettings were restored.", tar->GetCleanName());
	}
	else if (!strcasecmp(sep->arg[1], "spelltypesettings")) { //TODO
		for (uint16 i = BotSpellTypes::START; i <= BotSpellTypes::END; ++i) {
			tar->CastToBot()->SetSpellHold(i, tar->CastToBot()->GetDefaultSpellHold(i));
			tar->CastToBot()->SetSpellDelay(i, tar->CastToBot()->GetDefaultSpellDelay(i));
			tar->CastToBot()->SetSpellMinThreshold(i, tar->CastToBot()->GetDefaultSpellMinThreshold(i));
			tar->CastToBot()->SetSpellMaxThreshold(i, tar->CastToBot()->GetDefaultSpellMaxThreshold(i));
			tar->CastToBot()->SetSpellTypeAggroCheck(i, tar->CastToBot()->GetDefaultSpellTypeAggroCheck(i));
			tar->CastToBot()->SetSpellTypeMinManaLimit(i, tar->CastToBot()->GetDefaultSpellTypeMinManaLimit(i));
			tar->CastToBot()->SetSpellTypeMaxManaLimit(i, tar->CastToBot()->GetDefaultSpellTypeMaxManaLimit(i));
			tar->CastToBot()->SetSpellTypeMinHPLimit(i, tar->CastToBot()->GetDefaultSpellTypeMinHPLimit(i));
			tar->CastToBot()->SetSpellTypeMaxHPLimit(i, tar->CastToBot()->GetDefaultSpellTypeMaxHPLimit(i));
			tar->CastToBot()->SetSpellTypePriority(i, BotPriorityCategories::Idle, tar->CastToBot()->GetDefaultSpellTypePriority(i, BotPriorityCategories::Idle, tar->GetClass()));
			tar->CastToBot()->SetSpellTypePriority(i, BotPriorityCategories::Engaged, tar->CastToBot()->GetDefaultSpellTypePriority(i, BotPriorityCategories::Engaged, tar->GetClass()));
			tar->CastToBot()->SetSpellTypePriority(i, BotPriorityCategories::Pursue, tar->CastToBot()->GetDefaultSpellTypePriority(i, BotPriorityCategories::Pursue, tar->GetClass()));
			tar->CastToBot()->SetSpellTypeAEOrGroupTargetCount(i, tar->CastToBot()->GetDefaultSpellTypeAEOrGroupTargetCount(i));
			c->Message(Chat::Yellow, "%s's spell type settings were restored.", tar->GetCleanName());
		}
	}
	else if (!strcasecmp(sep->arg[1], "all")) { //TODO
		for (uint16 i = BotBaseSettings::START; i <= BotBaseSettings::END; ++i) {
			tar->CastToBot()->SetBotBaseSetting(i, tar->CastToBot()->GetDefaultBotBaseSetting(i));
		}

		for (uint16 i = BotSpellTypes::START; i <= BotSpellTypes::END; ++i) {
			tar->CastToBot()->SetSpellHold(i, tar->CastToBot()->GetDefaultSpellHold(i));
			tar->CastToBot()->SetSpellDelay(i, tar->CastToBot()->GetDefaultSpellDelay(i));
			tar->CastToBot()->SetSpellMinThreshold(i, tar->CastToBot()->GetDefaultSpellMinThreshold(i));
			tar->CastToBot()->SetSpellMaxThreshold(i, tar->CastToBot()->GetDefaultSpellMaxThreshold(i));
			tar->CastToBot()->SetSpellTypeAggroCheck(i, tar->CastToBot()->GetDefaultSpellTypeAggroCheck(i));
			tar->CastToBot()->SetSpellTypeMinManaLimit(i, tar->CastToBot()->GetDefaultSpellTypeMinManaLimit(i));
			tar->CastToBot()->SetSpellTypeMaxManaLimit(i, tar->CastToBot()->GetDefaultSpellTypeMaxManaLimit(i));
			tar->CastToBot()->SetSpellTypeMinHPLimit(i, tar->CastToBot()->GetDefaultSpellTypeMinHPLimit(i));
			tar->CastToBot()->SetSpellTypeMaxHPLimit(i, tar->CastToBot()->GetDefaultSpellTypeMaxHPLimit(i));
			tar->CastToBot()->SetSpellTypePriority(i, BotPriorityCategories::Idle, tar->CastToBot()->GetDefaultSpellTypePriority(i, BotPriorityCategories::Idle, tar->GetClass()));
			tar->CastToBot()->SetSpellTypePriority(i, BotPriorityCategories::Engaged, tar->CastToBot()->GetDefaultSpellTypePriority(i, BotPriorityCategories::Engaged, tar->GetClass()));
			tar->CastToBot()->SetSpellTypePriority(i, BotPriorityCategories::Pursue, tar->CastToBot()->GetDefaultSpellTypePriority(i, BotPriorityCategories::Pursue, tar->GetClass()));
			tar->CastToBot()->SetSpellTypeAEOrGroupTargetCount(i, tar->CastToBot()->GetDefaultSpellTypeAEOrGroupTargetCount(i));
		};

		tar->CastToBot()->ResetBotSpellSettings();

		c->Message(Chat::Yellow, "%s's settings were restored.", tar->GetCleanName());

	}
	else {
		c->Message(Chat::Yellow, "Invalid argument, use %s help for more information.", sep->arg[0]);
		return;
	}
}
//LoadDefaultBotSettings
