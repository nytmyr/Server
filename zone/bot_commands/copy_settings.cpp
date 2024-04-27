#include "../bot_command.h"

void bot_command_copy_settings(Client* c, const Seperator* sep)
{
	if (helper_command_alias_fail(c, "bot_command_copy_settings", sep->arg[0], "copysettings")) {
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
				"usage: {} [from] [to] [option]",
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
						"To copy all settings from Bota to Botb:"
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
						"^copysettings all Bota Botb"
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
							"{}[misc] copies all miscellaneous options such as:",
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
							"{}[spellsettings] will copy ^spellsettings options",
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
							"{}[spelltypesettings] copies all spell type settings",
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
							"{}[all] copies all settings",
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
							"{}The remainder copy that specific type",
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

	auto from = entity_list.GetBotByBotName(sep->arg[1]);

	if (!from) {
		c->Message(Chat::Yellow, "Could not find %s.", sep->arg[1]);
		return;
	}

	if (!from->IsBot()) {
		c->Message(Chat::Yellow, "%s is not a bot.", from->GetCleanName());
		return;
	}

	if (!from->GetOwner()) {
		c->Message(Chat::Yellow, "Could not find %s's owner.", from->GetCleanName());
	}

	if (RuleB(Bots, CopySettingsOwnBotsOnly) && from->GetOwner() != c) {
		c->Message(Chat::Yellow, "You name a bot you own to use this command.");
		return;
	}

	if (!RuleB(Bots, AllowCopySettingsAnon) && from->GetOwner() != c && from->GetOwner()->CastToClient()->GetAnon()) {
		c->Message(Chat::Yellow, "You name a bot you own to use this command.");
		return;
	}

	auto to = ActionableBots::AsNamed_ByBot(c, sep->arg[2]);

	if (!to) {
		c->Message(Chat::Yellow, "Could not find %s.", sep->arg[1]);
		return;
	}
	
	if (!to->IsBot()) {
		c->Message(Chat::Yellow, "%s is not a bot.", to->GetCleanName());
		return;
	}

	if (!to->GetOwner()) {
		c->Message(Chat::Yellow, "Could not find %s's owner.", to->GetCleanName());
	}
	
	if (to->GetOwner() != c) {
		c->Message(Chat::Yellow, "You must name a spawned bot that you own to use this command.");
		return;
	}

	if (to == from) {
		c->Message(Chat::Yellow, "You cannot copy to the same bot that you're copying from.");
		return;
	}

	if (!strcasecmp(sep->arg[3], "misc")) {
		from->CopySettings(to, BotSettingCategories::BaseSetting);
		c->Message(Chat::Yellow, "%s's miscellaneous settings were copied to %s.", from->GetCleanName(), to->GetCleanName());
	}
	else if (!strcasecmp(sep->arg[3], "holds")) {
		from->CopySettings(to, BotSettingCategories::SpellHold);
		c->Message(Chat::Yellow, "%s's hold settings were copied to %s.", from->GetCleanName(), to->GetCleanName());
	}
	else if (!strcasecmp(sep->arg[3], "delays")) {
		from->CopySettings(to, BotSettingCategories::SpellDelay);
		c->Message(Chat::Yellow, "%s's delay settings were copied to %s.", from->GetCleanName(), to->GetCleanName());
	}
	else if (!strcasecmp(sep->arg[3], "minthresholds")) {
		from->CopySettings(to, BotSettingCategories::SpellMinThreshold);
		c->Message(Chat::Yellow, "%s's minimum threshold settings were copied to %s.", from->GetCleanName(), to->GetCleanName());
	}
	else if (!strcasecmp(sep->arg[3], "maxthresholds")) {
		from->CopySettings(to, BotSettingCategories::SpellMaxThreshold);
		c->Message(Chat::Yellow, "%s's maximum threshold settings were copied to %s.", from->GetCleanName(), to->GetCleanName());
	}
	else if (!strcasecmp(sep->arg[3], "aggrocheck")) {
		from->CopySettings(to, BotSettingCategories::SpellTypeAggroCheck);
		c->Message(Chat::Yellow, "%s's aggro check settings were copied to %s.", from->GetCleanName(), to->GetCleanName());
	}
	else if (!strcasecmp(sep->arg[3], "minmanapct")) {
		from->CopySettings(to, BotSettingCategories::SpellTypeMinManaPct);
		c->Message(Chat::Yellow, "%s's min mana settings were copied to %s.", from->GetCleanName(), to->GetCleanName());
	}
	else if (!strcasecmp(sep->arg[3], "maxmanapct")) {
		from->CopySettings(to, BotSettingCategories::SpellTypeMaxManaPct);
		c->Message(Chat::Yellow, "%s's max mana settings were copied to %s.", from->GetCleanName(), to->GetCleanName());
	}
	else if (!strcasecmp(sep->arg[3], "minhppct")) {
		from->CopySettings(to, BotSettingCategories::SpellTypeMinHPPct);
		c->Message(Chat::Yellow, "%s's min hp settings were copied to %s.", from->GetCleanName(), to->GetCleanName());
	}
	else if (!strcasecmp(sep->arg[3], "maxhppct")) {
		from->CopySettings(to, BotSettingCategories::SpellTypeMaxHPPct);
		c->Message(Chat::Yellow, "%s's max hp settings were copied to %s.", from->GetCleanName(), to->GetCleanName());
	}
	else if (!strcasecmp(sep->arg[3], "idlepriority")) {
		from->CopySettings(to, BotSettingCategories::SpellTypeIdlePriority);
		c->Message(Chat::Yellow, "%s's idle priority settings were copied to %s.", from->GetCleanName(), to->GetCleanName());
	}
	else if (!strcasecmp(sep->arg[3], "engagedpriority")) {
		from->CopySettings(to, BotSettingCategories::SpellTypeEngagedPriority);
		c->Message(Chat::Yellow, "%s's engaged priority settings were copied to %s.", from->GetCleanName(), to->GetCleanName());
	}
	else if (!strcasecmp(sep->arg[3], "pursuepriority")) {
		from->CopySettings(to, BotSettingCategories::SpellTypePursuePriority);
		c->Message(Chat::Yellow, "%s's pursue priority settings were copied to %s.", from->GetCleanName(), to->GetCleanName());
	}
	else if (!strcasecmp(sep->arg[3], "aegroup")) {
		from->CopySettings(to, BotSettingCategories::SpellTypeAEOrGroupTargetCount);
		c->Message(Chat::Yellow, "%s's ae/group count settings were copied to %s.", from->GetCleanName(), to->GetCleanName());
	}
	else if (!strcasecmp(sep->arg[3], "spellsettings")) { //TODO		
		from->CopyBotSpellSettings(to);
		c->Message(Chat::Yellow, "%s's ^spellsettings were copied to %s.", from->GetCleanName(), to->GetCleanName());
	}
	else if (!strcasecmp(sep->arg[3], "spelltypesettings")) { //TODO		
		from->CopySettings(to, BotSettingCategories::SpellHold);
		from->CopySettings(to, BotSettingCategories::SpellDelay);
		from->CopySettings(to, BotSettingCategories::SpellMinThreshold);
		from->CopySettings(to, BotSettingCategories::SpellMaxThreshold);
		from->CopySettings(to, BotSettingCategories::SpellTypeAggroCheck);
		from->CopySettings(to, BotSettingCategories::SpellTypeMinManaPct);
		from->CopySettings(to, BotSettingCategories::SpellTypeMaxManaPct);
		from->CopySettings(to, BotSettingCategories::SpellTypeMinHPPct);
		from->CopySettings(to, BotSettingCategories::SpellTypeMaxHPPct);
		from->CopySettings(to, BotSettingCategories::SpellTypeIdlePriority);
		from->CopySettings(to, BotSettingCategories::SpellTypeEngagedPriority);
		from->CopySettings(to, BotSettingCategories::SpellTypePursuePriority);
		from->CopySettings(to, BotSettingCategories::SpellTypeAEOrGroupTargetCount);
		c->Message(Chat::Yellow, "%s's spell type settings were copied to %s.", from->GetCleanName(), to->GetCleanName());
	}
	else if (!strcasecmp(sep->arg[3], "all")) {
		from->CopySettings(to, BotSettingCategories::BaseSetting);
		from->CopySettings(to, BotSettingCategories::SpellHold);
		from->CopySettings(to, BotSettingCategories::SpellDelay);
		from->CopySettings(to, BotSettingCategories::SpellMinThreshold);
		from->CopySettings(to, BotSettingCategories::SpellMaxThreshold);
		from->CopySettings(to, BotSettingCategories::SpellTypeAggroCheck);
		from->CopySettings(to, BotSettingCategories::SpellTypeMinManaPct);
		from->CopySettings(to, BotSettingCategories::SpellTypeMaxManaPct);
		from->CopySettings(to, BotSettingCategories::SpellTypeMinHPPct);
		from->CopySettings(to, BotSettingCategories::SpellTypeMaxHPPct);
		from->CopySettings(to, BotSettingCategories::SpellTypeIdlePriority);
		from->CopySettings(to, BotSettingCategories::SpellTypeEngagedPriority);
		from->CopySettings(to, BotSettingCategories::SpellTypePursuePriority);
		from->CopySettings(to, BotSettingCategories::SpellTypeAEOrGroupTargetCount);
		from->CopyBotSpellSettings(to);
		c->Message(Chat::Yellow, "%s's settings were copied to %s.", from->GetCleanName(), to->GetCleanName());
	}
	else {
		c->Message(Chat::Yellow, "Invalid argument, use %s help for more information.", sep->arg[0]);
		return;
	}
}
