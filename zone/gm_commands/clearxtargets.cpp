void command_clearxtargets(Client* c, const Seperator* sep)
{
	auto reuse_timer = RuleI(Character, ClearXTargetDelay);

	int arguments = sep->argnum;
	if (!strcasecmp(sep->arg[1], "help")) {
		c->Message(Chat::White, "usage: #resetxtargets.");
		c->Message(Chat::White, "note: Use this if your Extended Target window is bugged or has lingering targets that are invalid.");
		c->Message(Chat::White, "note: This can only be used every {} seconds.", reuse_timer);
		return;
	}

	if (reuse_timer) {
		uint32 time_left = c->GetPTimers().GetRemainingTime(pTimerClearXTarget);
		if (!c->GetPTimers().Expired(&database, pTimerClearXTarget, false)) {
			c->Message(
				Chat::White,
				fmt::format(
					"You must wait {} before using this command again.",
					Strings::SecondsToTime(time_left)
				).c_str()
			);
			return;
		}
	}

	if (reuse_timer) {
		c->Message(Chat::White, "Extended Target window has been cleared.");
		c->GetPTimers().Start(pTimerClearXTarget, reuse_timer);
	}

	c->RemoveAllAutoXTargets();
}
