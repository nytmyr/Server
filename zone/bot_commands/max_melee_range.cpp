#include "../bot_command.h"

void bot_command_max_melee_range(Client* c, const Seperator* sep)
{
	if (helper_command_alias_fail(c, "bot_command_max_melee_range", sep->arg[0], "maxmeleerange")) {
		return;
	}

	if (helper_is_help_or_usage(sep->arg[1])) {
		c->Message(Chat::White, "usage: %s [current | value: 0-1] ([actionable: target | byname | ownergroup | ownerraid | namesgroup | healrotation | mmr | byclass | byrace | default: spawned] ([actionable_name])", sep->arg[0]);
		c->Message(Chat::White, "note: Use [current] to check the current setting.");
		c->Message(Chat::White, "note: Set to 0 to have a bot attack at its normal attack range.");
		c->Message(Chat::White, "note: Set to 1 to force a bot to stay at max melee range.");
		c->Message(Chat::White, "note: This is disabled by default. (0).");
		return;
	}

	const int ab_mask = (ActionableBots::ABM_Target | ActionableBots::ABM_Type2);

	std::string arg1 = sep->arg[1];
	int ab_arg = 1;
	bool current_check = false;
	uint32 setStatus = 0;
	if (sep->IsNumber(1)) {
		ab_arg = 2;
		setStatus = atoi(sep->arg[1]);
		if (setStatus != 0 && setStatus != 1) {
			c->Message(Chat::White, "You must enter either 0 for disabled or 1 for enabled.");
			return;
		}
	}
	else if (!arg1.compare("current")) {
		ab_arg = 2;
		current_check = true;
	}
	else {
		c->Message(Chat::White, "Incorrect argument, use %s help for a list of options.", sep->arg[0]);
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
					"{} says, 'My current Max Melee Range status is {}.'",
					my_bot->GetCleanName(),
					my_bot->GetMaxMeleeRange() ? "enabled" : "disabled"
				).c_str()
			);
		}
		else {
			my_bot->SetMaxMeleeRange(setStatus);
			++success_count;
		}
	}

	if (!current_check) {
		if (success_count == 1 && first_found) {
			c->Message(
				Chat::Green,
				fmt::format(
					"{} says, 'My Max Melee Range status was {}.'",
					first_found->GetCleanName(),
					setStatus ? "enabled" : "disabled"
				).c_str()
			);
		}
		else {
			c->Message(
				Chat::Green,
				fmt::format(
					"{} of your bots set their Max Melee Range status to {}.",
					success_count,
					setStatus ? "enabled" : "disabled"
				).c_str()
			);
		}
	}
}
