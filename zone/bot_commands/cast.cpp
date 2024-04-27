#include "../bot_command.h"

void bot_command_cast(Client* c, const Seperator* sep)
{
	if (helper_is_help_or_usage(sep->arg[1])) {
		c->Message(Chat::White, "usage: <target> %s [spelltype ID | spelltype shortname]", sep->arg[0]);
		c->Message(
			Chat::White,
			fmt::format(
				"note: Use {}, {}, {} for a list of spell types by ID or {}, {}, {} for a list of spell types by short name.",
				Saylink::Silent(
					fmt::format("{} listid 0-19", sep->arg[0])
				),
				Saylink::Silent(
					fmt::format("{} listid 20-39", sep->arg[0])
				),
				Saylink::Silent(
					fmt::format("{} listid 40+", sep->arg[0])
				),
				Saylink::Silent(
					fmt::format("{} listname 0-19", sep->arg[0])
				),
				Saylink::Silent(
					fmt::format("{} listname 20-39", sep->arg[0])
				),
				Saylink::Silent(
					fmt::format("{} listname 40+", sep->arg[0])
				)
			).c_str()
		);
		return;
	}

	std::string arg1 = sep->arg[1];

	if (!arg1.compare("listid") || !arg1.compare("listname")) {
		std::string arg2 = sep->arg[2];
		uint8 minCount = 0;
		uint8 maxCount = 0;

		if (BotSpellTypes::END <= 19) {
			minCount = BotSpellTypes::START;
			maxCount = BotSpellTypes::END;
		}
		else if (!arg2.compare("0-19")) {
			minCount = BotSpellTypes::START;
			maxCount = 19;
		}
		else if (!arg2.compare("20-39")) {
			minCount = std::min(static_cast<uint8_t>(20), static_cast<uint8_t>(BotSpellTypes::END));
			maxCount = std::min(static_cast<uint8_t>(39), static_cast<uint8_t>(BotSpellTypes::END));
		}
		else if (!arg2.compare("40+")) {
			minCount = std::min(static_cast<uint8_t>(40), static_cast<uint8_t>(BotSpellTypes::END));
			maxCount = BotSpellTypes::END;
		}
		else {
			c->Message(Chat::Yellow, "You must choose a valid range option");

			return;
		}

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

		for (int i = minCount; i <= maxCount; ++i) {
			popup_text += DialogueWindow::TableRow(
				DialogueWindow::TableCell(
					fmt::format(
						"{}{}",
						DialogueWindow::ColorMessage(color_green, c->GetSpellTypeNameByID(i)),
						DialogueWindow::ColorMessage(color_green, pluralS)
					)
				) +
				DialogueWindow::TableCell(
					fmt::format(
						"{}",
						(!arg1.compare("listid") ? DialogueWindow::ColorMessage(color_blue, std::to_string(i)) : DialogueWindow::ColorMessage(color_blue, c->GetSpellTypeShortNameByID(i)))
					)
				)
			);
		}

		popup_text = DialogueWindow::Table(popup_text);

		c->SendPopupToClient("Spell Types", popup_text.c_str());

		return;
	}

	uint16 spellType;

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

	if (
		spellType == BotSpellTypes::PetBuffs ||
		spellType == BotSpellTypes::PetCompleteHeals ||
		spellType == BotSpellTypes::PetFastHeals ||
		spellType == BotSpellTypes::PetGroupCompleteHeals ||
		spellType == BotSpellTypes::PetGroupHeals ||
		spellType == BotSpellTypes::PetGroupHoTHeals ||
		spellType == BotSpellTypes::PetHoTHeals ||
		spellType == BotSpellTypes::PetRegularHeals ||
		spellType == BotSpellTypes::PetVeryFastHeals
	) {
		c->Message(Chat::Yellow, "Pet type heals and buffs are not supported, use the regular spell type.");
		return;
	}

	if (spellType == BotSpellTypes::Resurrect) {
		c->Message(
			Chat::White,
			fmt::format(
				"Resurrect is not supported, use {}.",
				Saylink::Silent("^resurrect")
			).c_str()
		);

		return;
	}

	if (spellType == BotSpellTypes::Charm) {
		c->Message(
			Chat::White,
			fmt::format(
				"Charm is not supported, use {}.",
				Saylink::Silent("^charm")
			).c_str()
		);

		return;
	}

	//TODO make some types not require target or default to themselves
	Mob* tar = nullptr;

	if (spellType != BotSpellTypes::Escape && spellType != BotSpellTypes::Pet) {
		if (!c->GetTarget()) {
			c->Message(Chat::Yellow, "You need a target for that.");
			return;
		}

		tar = c->GetTarget();
	}
	
	if (spellType == BotSpellTypes::Stun || spellType == BotSpellTypes::AEStun) {
		if (tar->GetSpecialAbility(SpecialAbility::StunImmunity)) {
			c->Message(
				Chat::Yellow,
				fmt::format(
					"{} is immune to stuns.",
					tar->GetCleanName()
				).c_str()
			);

			return;
		}
	}

	if (
		spellType == BotSpellTypes::GroupCures ||
		spellType == BotSpellTypes::Cure ||
		spellType == BotSpellTypes::RegularHeal ||
		spellType == BotSpellTypes::GroupCompleteHeals ||
		spellType == BotSpellTypes::CompleteHeal ||
		spellType == BotSpellTypes::FastHeals ||
		spellType == BotSpellTypes::VeryFastHeals ||
		spellType == BotSpellTypes::GroupHeals ||
		spellType == BotSpellTypes::GroupHoTHeals ||
		spellType == BotSpellTypes::HoTHeals
		) {
		if (!tar->IsOfClientBot() && !(tar->IsPet() && tar->GetOwner() && tar->GetOwner()->IsOfClientBot())) {
			c->Message(
				Chat::Yellow,
				fmt::format(
					"{} is an invalid target.",
					tar->GetCleanName()
				).c_str()
			);
			return;
		}
	}

	bool findMore = true;
	bool isSuccess = false;
	BotSpell botSpell;
	botSpell.SpellId = 0;
	botSpell.SpellIndex = 0;
	botSpell.ManaCost = 0;

	const int ab_mask = ActionableBots::ABM_Type2;
	std::string ab_arg(sep->arg[2]);

	if (ab_arg.empty()) {
		ab_arg = "spawned";
	}

	std::string class_race_arg(sep->arg[2]);
	bool class_race_check = false;
	if (!class_race_arg.compare("byclass") || !class_race_arg.compare("byrace")) {
		class_race_check = true;
	}

	std::list<Bot*> sbl;
	if (ActionableBots::PopulateSBL(c, ab_arg.c_str(), sbl, ab_mask, !class_race_check ? sep->arg[3] : nullptr, class_race_check ? atoi(sep->arg[3]) : 0) == ActionableBots::ABT_None) {
		return;
	}

	for (auto bot_iter : sbl) {
		if (!bot_iter->IsInGroupOrRaid()) {
			continue;
		}

		if (spellType == BotSpellTypes::Stun || spellType == BotSpellTypes::AEStun) {
			findMore = false;
			botSpell = bot_iter->GetBestBotSpellForStunByTargetType(bot_iter, (spellType == BotSpellTypes::Stun ? ST_Target : ST_TargetOptional), spellType, IS_AE_BOT_SPELL_TYPE(spellType), tar);
		}
		else if (
			spellType == BotSpellTypes::Nuke ||
			spellType == BotSpellTypes::AENukes ||
			spellType == BotSpellTypes::PBAENuke ||
			spellType == BotSpellTypes::AERains ||
			spellType == BotSpellTypes::PBAENuke
		) {
			botSpell = bot_iter->GetBestBotSpellForNukeByBodyType(bot_iter, tar->GetBodyType(), spellType, IS_AE_BOT_SPELL_TYPE(spellType), tar);

			if (!IsValidSpell(botSpell.SpellId) && bot_iter->GetClass() == Class::Wizard) {
				botSpell = bot_iter->GetBestBotWizardNukeSpellByTargetResists(bot_iter, tar, spellType);
			}
		}
		else if (
			spellType == BotSpellTypes::RegularHeal ||
			spellType == BotSpellTypes::GroupCompleteHeals ||
			spellType == BotSpellTypes::CompleteHeal ||
			spellType == BotSpellTypes::FastHeals ||
			spellType == BotSpellTypes::VeryFastHeals ||
			spellType == BotSpellTypes::GroupHeals ||
			spellType == BotSpellTypes::GroupHoTHeals ||
			spellType == BotSpellTypes::HoTHeals
		) {
			findMore = false;
			botSpell = bot_iter->GetSpellByHealType(spellType, tar);
		}
		else if (
			spellType == BotSpellTypes::GroupCures ||
			spellType == BotSpellTypes::Cure
		) {
			findMore = false;
			botSpell = bot_iter->GetBestBotSpellForCure(bot_iter, tar, spellType);
		}
		else if (spellType == BotSpellTypes::Pet) {
			tar = bot_iter;
			findMore = false;
			if (bot_iter->HasPet() || bot_iter->IsBotCharmer()) {
				continue;
			}
			if (bot_iter->GetClass() == Class::Magician) {
				botSpell = bot_iter->GetBestBotMagicianPetSpell(bot_iter, spellType);
			}
			else {
				botSpell = bot_iter->GetFirstBotSpellBySpellType(bot_iter, spellType);
			}
		}
		else if (spellType == BotSpellTypes::Escape) {
			tar = bot_iter;
		}

		if (IsValidSpell(botSpell.SpellId)) {
			LogTestDebug("{} says, 'I found {} [#{}] for {} on {}", bot_iter->GetCleanName(), spells[botSpell.SpellId].name, botSpell.SpellId, bot_iter->GetSpellTypeNameByID(spellType), tar->GetCleanName()); //deleteme
		}
		else {
			LogTestDebug("{} says, 'I did not find a spell for {} on {}", bot_iter->GetCleanName(), bot_iter->GetSpellTypeNameByID(spellType), tar->GetCleanName()); //deleteme
		}

		if (!IsValidSpell(botSpell.SpellId) && findMore) {
			std::list<BotSpell_wPriority> botSpellList = bot_iter->GetPrioritizedBotSpellsBySpellType(bot_iter, spellType, tar, IS_AE_BOT_SPELL_TYPE(spellType));

			for (const auto& s : botSpellList) {
				if (!IsValidSpell(s.SpellId)) {
					continue;
				}

				if (bot_iter->CommandedDoSpellCast(s.SpellIndex, tar, s.ManaCost)) {
					bot_iter->BotGroupSay(bot_iter, "Casting %s [%s] on %s.", GetSpellName(s.SpellId), bot_iter->GetSpellTypeNameByID(spellType), tar->GetCleanName());
					isSuccess = true;

					break;
				}
			}
		}
		else {
			if (IsValidSpell(botSpell.SpellId) && bot_iter->CommandedDoSpellCast(botSpell.SpellIndex, tar, botSpell.ManaCost)) {
				bot_iter->BotGroupSay(bot_iter, "Casting %s [%s] on %s.", GetSpellName(botSpell.SpellId), bot_iter->GetSpellTypeNameByID(spellType), tar->GetCleanName());
				isSuccess = true;
			}
		}

		if (isSuccess) {
			break;
		}
	}

	if (!isSuccess) {
		helper_no_available_bots(c);
	}
}
