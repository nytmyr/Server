void bot_command_spelltype_ids(Client* c, const Seperator* sep)
{
	if (helper_is_help_or_usage(sep->arg[1])) {
		c->Message(
			Chat::Yellow,
			fmt::format(
				"Use {} to display IDs 0-19.",
				Saylink::Silent(fmt::format("{} 0-19", sep->arg[0]))
			).c_str()
		);

		c->Message(
			Chat::Yellow,
			fmt::format(
				"Use {} to display IDs 20-39.",
				Saylink::Silent(fmt::format("{} 20-39", sep->arg[0]))
			).c_str()
		);

		c->Message(
			Chat::Yellow,
			fmt::format(
				"Use {} to display IDs 20-39.",
				Saylink::Silent(fmt::format("{} 20-39", sep->arg[0]))
			).c_str()
		);

		c->Message(
			Chat::Yellow,
			fmt::format(
				"Use {} to display IDs 40+.",
				Saylink::Silent(fmt::format("{} 40+", sep->arg[0]))
			).c_str()
		);

		c->Message(
			Chat::Yellow,
			fmt::format(
				"Use {} to display commanded IDs.",
				Saylink::Silent(fmt::format("{} commanded", sep->arg[0]))
			).c_str()
		);

		c->Message(
			Chat::Yellow,
			fmt::format(
				"Use {} to display client IDs.",
				Saylink::Silent(fmt::format("{} client", sep->arg[0]))
			).c_str()
		);


		return;
	}

	std::string arg = sep->arg[1];

	if (arg.empty()) {
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

	SendSpellTypeWindow(c, sep);
}

void bot_command_spelltype_names(Client* c, const Seperator* sep)
{
	if (helper_is_help_or_usage(sep->arg[1])) {
		c->Message(
			Chat::Yellow,
			fmt::format(
				"Use {} to display names for IDs 0-19.",
				Saylink::Silent(fmt::format("{} 0-19", sep->arg[0]))
			).c_str()
		);

		c->Message(
			Chat::Yellow,
			fmt::format(
				"Use {} to display names for IDs 20-39.",
				Saylink::Silent(fmt::format("{} 20-39", sep->arg[0]))
			).c_str()
		);

		c->Message(
			Chat::Yellow,
			fmt::format(
				"Use {} to display names for IDs 20-39.",
				Saylink::Silent(fmt::format("{} 20-39", sep->arg[0]))
			).c_str()
		);

		c->Message(
			Chat::Yellow,
			fmt::format(
				"Use {} to display names for IDs 40+.",
				Saylink::Silent(fmt::format("{} 40+", sep->arg[0]))
			).c_str()
		);

		c->Message(
			Chat::Yellow,
			fmt::format(
				"Use {} to display commanded names.",
				Saylink::Silent(fmt::format("{} commanded", sep->arg[0]))
			).c_str()
		);

		c->Message(
			Chat::Yellow,
			fmt::format(
				"Use {} to display client names.",
				Saylink::Silent(fmt::format("{} client", sep->arg[0]))
			).c_str()
		);


		return;
	}

	std::string arg = sep->arg[1];

	if (arg.empty()) {
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

	SendSpellTypeWindow(c, sep);
}
