/*	EQEmu: EQEmulator

	Copyright (C) 2001-2026 EQEmu Development Team

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "zone/bot_command.h"

void bot_command_discipline(Client* c, const Seperator* sep)
{
	if (helper_command_alias_fail(c, "bot_command_discipline", sep->arg[0], "discipline")) {
		c->Message(Chat::White, "note: Tells applicable bots to use the specified disciplines.");

		return;
	}

	if (helper_is_help_or_usage(sep->arg[1])) {
		BotCommandHelpParams p;

		p.description =
		{
			"Tells applicable bots to use the specified disciplines."
		};
		p.notes = { "Aside from Lay On Hands and Harm Touch, you will need to know the spell ID of the discipline to tell a bot to attempt to use it." };
		p.example_format = { fmt::format("{} [aggressive | defensive | spell ID]  [actionable, default: spawned]", sep->arg[0]) };
		p.examples_one =
		{
			"To tell all bots to use an aggressive discipline:",
			fmt::format(
				"{} aggressive spawned",
				sep->arg[0]
			)
		};
		p.examples_two =
		{
			"To tell Warrior bots to use a defensive discipline:",
			fmt::format(
				"{} defensive byclass {}",
				sep->arg[0],
				Class::Warrior
			)
		};
		p.examples_three =
		{
			"To tell all bots to use their Fearless discipline:",
			fmt::format(
				"{} 4587 spawned",
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

	std::string arg1 = sep->arg[1];
	std::string arg2 = sep->arg[2];
	int ab_arg = 2;
	bool aggressive = false;
	bool defensive = false;
	Mob* tar = c->GetTarget();
	uint16 spell_id = UINT16_MAX;

	if (!arg1.compare("aggressive")) {
		aggressive = true;
	}
	else if (!arg1.compare("defensive")) {
		defensive = true;
	}
	else if (sep->IsNumber(1)) {
		spell_id = atoi(sep->arg[1]);

		if (!IsValidSpell(spell_id)) {
			c->Message(Chat::Yellow, "You must enter a valid spell ID.");
			return;
		}

		if (!IsDiscipline(spell_id)) {
			c->Message(
				Chat::Yellow,
					fmt::format(
						"You cannot use {} to cast [{}] as it is not a discipline. Use ^cast instead.",
						sep->arg[0],
						spells[spell_id].name
					).c_str()
			);

			return;
		}
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

	bool is_success = false;
	uint16 success_count = 0;
	Bot* first_found = nullptr;
	uint8 failure_type = CastFailures::Undeclared;

	for (auto bot_iter : sbl) {
		if (!bot_iter->ValidStateCheck(c)) {
			failure_type = CastFailures::InvalidState;
			continue;
		}

		bot_iter->SetCastFailure(failure_type);

		if (bot_iter->HasDiscBuff()) {
			bot_iter->RaidGroupSay("I currently have an active discipline.");
			failure_type = CastFailures::AlreadyHasDiscBuff;
			continue;
		}

		if (spell_id == UINT16_MAX) { // Aggressive/Defensive type
			const std::vector<BotSpells_wIndex>& bot_spell_list = bot_iter->BotGetSpellsByType(aggressive ? BotSpellTypes::DiscAggressive : BotSpellTypes::DiscDefensive);

			for (int i = bot_spell_list.size() - 1; i >= 0; i--) {
				if (!IsValidSpell(bot_spell_list[i].spellid)) {
					failure_type = CastFailures::InvalidSpell;
					continue;
				}

				auto spell = spells[bot_spell_list[i].spellid];

				if (!bot_iter->CheckDisciplineReuseTimer(spell.id)) {
					uint32 remaining_time = (bot_iter->GetDisciplineReuseRemainingTime(spell.id) / 1000);

					bot_iter->OwnerMessage(
						fmt::format(
							"I can use this discipline in {}.",
							Strings::SecondsToTime(remaining_time)
						)
					);

					continue;
				}

				if (bot_iter->AttemptForcedCastSpell(tar, spell.id, true)) {
					if (!first_found) {
						first_found = bot_iter;
					}

					is_success = true;
					++success_count;
					spell_id = spell.id;
				}
				else {
					uint8 bot_failure_type = bot_iter->GetCastFailure();

					if (failure_type != CastFailures::Undeclared && failure_type != CastFailures::AlreadyHasDiscBuff) {
						failure_type = bot_failure_type;
					}
				}
			}
		}
		else { // Direct spell ID
			if (!IsValidSpell(spell_id)) {
				failure_type = CastFailures::InvalidSpell;
				continue;
			}

			auto spell = spells[spell_id];

			if (!IsDiscipline(spell_id)) {
				c->Message(
					Chat::Yellow,
						fmt::format(
							"You cannot use {} to cast [{}] as it is not a discipline.",
							sep->arg[0],
							spells[spell_id].name
						).c_str()
				);

				return;
			}

			if (!bot_iter->CanUseBotSpell(spell_id)) {
				if (failure_type == CastFailures::Undeclared) {
					failure_type = CastFailures::DoesNotHaveSpell;
				}

				continue;
			}

			if (!bot_iter->CheckDisciplineReuseTimer(spell_id)) {
				uint32 remaining_time = (bot_iter->GetDisciplineReuseRemainingTime(spell_id) / 1000);

				bot_iter->OwnerMessage(
					fmt::format(
						"I can use this item in {}.",
						Strings::SecondsToTime(remaining_time)
					)
				);

				continue;
			}

			if (bot_iter->AttemptForcedCastSpell(tar, spell_id, true)) {
				if (!first_found) {
					first_found = bot_iter;
				}

				is_success = true;
				++success_count;

				if (IsSingleBotCastedSpell(spell_id)) {
					break;
				}
			}
			else {
				uint8 bot_failure_type = bot_iter->GetCastFailure();

				if (failure_type != CastFailures::Undeclared && failure_type != CastFailures::DoesNotHaveSpell) {
					failure_type = bot_failure_type;
				}
			}
		}
	}

	if (!is_success) {
		c->Message(
			Chat::Yellow,
			fmt::format("No bots were capable of doing that. Reason: {}.",
				Bot::GetCastFailuresNames(failure_type)
			).c_str()
		);
	}
	else {
		if (aggressive || defensive) {
			c->Message(
				Chat::Yellow,
				fmt::format(
					"{} {} {} {} discipline.",
					((success_count == 1 && first_found) ? first_found->GetCleanName() : (fmt::format("{}", success_count).c_str())),
					((success_count == 1 && first_found) ? "used" : "of your bots used"),
					(aggressive ? "an" : "a"),
					(aggressive ? "aggressive" : "defensive")
				).c_str()
			);
		}
		else {
			c->Message(
				Chat::Yellow,
				fmt::format(
					"{} {} their {} [#{}] discipline.",
					((success_count == 1 && first_found) ? first_found->GetCleanName() : (fmt::format("{}", success_count).c_str())),
					((success_count == 1 && first_found) ? "used" : "of your bots used"),
					spells[spell_id].name,
					spell_id
				).c_str()
			);
		}
	}
}
