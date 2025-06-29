#include "../command.h"
#include "../../common/veq_spell_data.h"

void command_spell_score(Client* c, const Seperator* sep) {
	uint8 gm_access_level = AccountStatus::Max;

	if (!strcasecmp(sep->arg[1], "help")) {
		if (c->Admin() >= gm_access_level) {
			c->Message(Chat::White, "Usage: #spellscore [spell ID] [optional: option].");
			c->Message(Chat::White, "Usage: #spellscore update all.");
		}
		else {
			c->Message(Chat::White, "Usage: #spellscore [spell ID].");
		}
		BotCommandHelpParams p;

		p.description = { "Checks a spell's score rating." };

		if (c->Admin() >= gm_access_level) {
			p.notes =
				{
					"Provide a spell id to view the spell's rating",
					"---",
					"GM Options:",
					fmt::format("- '{} [spell_id] update' recalculate the spell's score", sep->arg[0]),
					fmt::format("- '{} update all' recalculate every spell's score in the database", sep->arg[0]),
				};
		}
		else {
			p.notes = { "Provide a spell id to view the spell's rating" };
		}
		p.example_format = { fmt::format("{} [spell id] [option]", sep->arg[0]) };
		p.examples_one = {
			fmt::format("To see the rating of {} [spell id: 93]:", spells[93].name),
			fmt::format("{} 93", sep->arg[0])
		};

		if (c->Admin() >= gm_access_level) {
			p.examples_one = {
				fmt::format("To update the rating of {} [spell id: 93]:", spells[93].name),
				fmt::format("{} 93 update", sep->arg[0])
			};
			p.examples_three = {
				"To update the rating of all spells:",
				fmt::format("{} update all", sep->arg[0])
			};
		}

		std::string popup_text = c->SendBotCommandHelpWindow(p);
		popup_text = DialogueWindow::Table(popup_text);

		c->SendPopupToClient(sep->arg[0], popup_text.c_str());

		/* TODO
		if (c->Admin() >= gm_access_level) {
			SendGearScoreTypePrompts(c);
		}
		*/
		return;
	}

	/* TODO
	if (
		c->Admin() >= gm_access_level && !strcasecmp(sep->arg[1], "gearscoretypes")) {
		std::string type_id = sep->arg[2];

		uint8 min_count = 0;
		uint8 max_count = 0;

		if (GearScoreTypes::END <= 19) {
			min_count = GearScoreTypes::START;
			max_count = GearScoreTypes::END;
		} else if (!type_id.compare("0-19")) {
			min_count = GearScoreTypes::START;
			max_count = std::min(static_cast<uint16_t>(19), static_cast<uint16_t>(GearScoreTypes::END));
		} else if (!type_id.compare("20-39") && static_cast<uint16_t>(GearScoreTypes::END) >= 20) {
			min_count = std::min(static_cast<uint16_t>(20), static_cast<uint16_t>(GearScoreTypes::END));
			max_count = std::min(static_cast<uint16_t>(39), static_cast<uint16_t>(GearScoreTypes::END));
		} else if (!type_id.compare("40-59") && static_cast<uint16_t>(GearScoreTypes::END) >= 40) {
			min_count = std::min(static_cast<uint16_t>(40), static_cast<uint16_t>(GearScoreTypes::END));
			max_count = std::min(static_cast<uint16_t>(59), static_cast<uint16_t>(GearScoreTypes::END));
		} else if (!type_id.compare("60-79") && static_cast<uint16_t>(GearScoreTypes::END) >= 50) {
			min_count = std::min(static_cast<uint16_t>(60), static_cast<uint16_t>(GearScoreTypes::END));
			max_count = std::min(static_cast<uint16_t>(79), static_cast<uint16_t>(GearScoreTypes::END));
		} else if (!type_id.compare("80+") && static_cast<uint16_t>(GearScoreTypes::END) >= 80) {
			min_count = std::min(static_cast<uint16_t>(80), static_cast<uint16_t>(GearScoreTypes::END));
			max_count = GearScoreTypes::END;
		} else {
			c->Message(Chat::Yellow, "You must choose a valid range option");

			return;
		}

		const std::string &gold = "gold";
		const std::string &slate_blue = "slate_blue";
		const std::string &forest_green = "forest_green";
		const std::string &goldenrod = "goldenrod";

		std::string filler_line = "-----------";

		std::string popup_text = DialogueWindow::TableRow(
			DialogueWindow::TableCell(DialogueWindow::ColorMessage(goldenrod, "Gear Score Type"))
			+
			DialogueWindow::TableCell(DialogueWindow::ColorMessage(goldenrod, "ID"))
		);

		popup_text += DialogueWindow::TableRow(
			DialogueWindow::TableCell(DialogueWindow::ColorMessage(gold, filler_line))
			+
			DialogueWindow::TableCell(DialogueWindow::ColorMessage(gold, filler_line))
		);

		for (int i = min_count; i <= max_count; ++i) {
			popup_text += DialogueWindow::TableRow(
				DialogueWindow::TableCell(DialogueWindow::ColorMessage(forest_green, GetGearScoreTypeByID(i)))
				+
				DialogueWindow::TableCell(DialogueWindow::ColorMessage(slate_blue, std::to_string(i)))
			);
		}

		popup_text = DialogueWindow::Table(popup_text);

		c->SendPopupToClient("Gear Score Types", popup_text.c_str());

		return;
	}
	*/

	if (!strcasecmp(sep->arg[1], "update") && c->Admin() >= gm_access_level) {
		if (!strcasecmp(sep->arg[2], "all")) {
			BenchTimer timer;
			timer.reset();

			VeqSpellScoreRepository::Truncate(database);
			LogSpellScore("delete took {}", std::to_string(timer.elapsed())); //deleteme
			std::vector<VeqSpellScoreRepository::VeqSpellScore> v;
			uint32 updated_count = 0;
			uint32 count = 0;

			const std::string query = "SELECT MAX(id) FROM spells_new";
			auto results = database.QueryDatabase(query);

			if (!results.Success() || results.RowCount() == 0) {
				count = 0;
			}

			auto &row = results.begin();

			if (row[0]) {
				count = Strings::ToUnsignedInt(row[0]);
			}

			for (uint32 x = 0; x <= count; x++) {
				if (!IsValidSpell(x)) {
					continue;
				}

				const SPDat_Spell_Struct spell = spells[x];

				++updated_count;
				//auto i = UpdateSpellScores(c->CastToMob(), spell, 255, true); // TODO add level option in command
				auto i = UpdateFullScore(c->CastToMob(), spell, 255, true); // TODO add level option in command

				v.emplace_back(i);

			}

			if (!v.empty()) {
				VeqSpellScoreRepository::InsertMany(content_db, v);
			}

			LogSpellScore("{} spells were updated in {}", updated_count, std::to_string(timer.elapsed())); //deleteme

			return;
		} else if (sep->IsNumber(2) && IsValidSpell(atoi(sep->arg[2]))) {
			auto spell_id = atoi(sep->arg[2]);

			BenchTimer timer;
			timer.reset();

			const SPDat_Spell_Struct spell = spells[spell_id];

			//UpdateSpellScores(c, spell, 255); // TODO add level option in command
			UpdateFullScore(c, spell, 255); // TODO add level option in command
			LogSpellScore("{} [#{}] was updated in {}",
						  spell.name,
						  spell_id,
						  std::to_string(timer.elapsed())); //deleteme

			return;
		}
	}
	else if (sep->IsNumber(1) && IsValidSpell(atoi(sep->arg[1]))) {
		auto spell_id = atoi(sep->arg[1]);
		float gear_score = GetSpellScore(spell_id);

		c->Message(
			Chat::Lime,
			fmt::format(
				"{} [#{}] has a Spell Score of {}",
				Saylink::Silent(fmt::format("#spellinfo {}", spell_id), spells[spell_id].name),
				spell_id,
				gear_score
			).c_str()
		);
	}
	else {
		c->Message(Chat::Yellow,
			fmt::format(
				"You must provide a valid spell ID. Use {} for information regarding this command.",
				Saylink::Silent(fmt::format("{} help", sep->arg[0])).c_str()
			).c_str()
		);
	}
}