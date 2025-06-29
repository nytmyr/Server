#include "../command.h"

void command_spell_info(Client* c, const Seperator* sep) {
	uint8 gm_access_level = AccountStatus::Max;

	if (!strcasecmp(sep->arg[1], "help")) {
		c->Message(Chat::White, "Usage: #spellinfo <spell ID>.");
		BotCommandHelpParams p;

		p.description = { "Returns a description of a spell." };

		p.notes = { "Provide a spell id to view the spell's description" };
		p.example_format = { fmt::format("{} [spell id]", sep->arg[0]) };
		p.examples_one = {
			fmt::format("To see the rating of {} [spell id: 93]:", spells[93].name),
			fmt::format("{} 93", sep->arg[0])
		};

		std::string popup_text = c->SendBotCommandHelpWindow(p);
		popup_text = DialogueWindow::Table(popup_text);

		c->SendPopupToClient(sep->arg[0], popup_text.c_str());

		return;
	}

	auto arguments = sep->argnum;
	if (
		arguments < 1 ||
		!sep->IsNumber(1)
		) {
		c->Message(
			Chat::White,
			fmt::format(
				"Usage: {} [Spell ID]",
				sep->arg[0]
			).c_str()
		);
		return;
	}

	auto spell_id = static_cast<uint16>(Strings::ToUnsignedInt(sep->arg[1]));

	auto results = database.QueryDatabase(
		fmt::format(
			"SELECT value FROM db_str WHERE id = {} and type = 6 LIMIT 1",
			spells[spell_id].description_id
		)
	);

	if (!results.Success() || !results.RowCount()) {
		c->Message(Chat::White, "No Spell Information Available for this.");
		return;
	}

	auto row = results.begin();
	std::string spell_desc = row[0];

	auto m = DialogueWindow::TableRow(
		DialogueWindow::TableCell("Spell Effect: ") +
		DialogueWindow::TableCell(spell_desc)
	);

	c->SendPopupToClient(
		fmt::format(
			"Spell: {}", spells[spell_id].name
		).c_str(),
		DialogueWindow::Table(m).c_str()
	);
}