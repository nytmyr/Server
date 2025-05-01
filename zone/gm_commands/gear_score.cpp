#include "../client.h"
#include "../../common/veq_item_data.h"

void command_gear_score(Client* c, const Seperator* sep) {
	uint8 gm_access_level = AccountStatus::Max;

	if (!strcasecmp(sep->arg[1], "help")) {
		c->Message(Chat::White, "Usage: #gearscore with an item on your cursor.");
		BotCommandHelpParams p;

		p.description = { "Checks an item's Gear Score." };

		if (c->Admin() >= gm_access_level) {
			p.notes =
				{
					"GM Options:",
					"- 'update' recalculate the Gear Score of the item on your cursor.",
					"- 'all' recalculate the Gear Score of every item in the database.",
					"- 'byclass' can be used after any type to get class-specific scores.",
				};
		}
		else {
			p.notes =
				{
					"You can use options like:",
					"- 'byclass' to specify a class ID to get the Gear Score for that class"
					"- 'min' to display the lowest Gear Score of all classes",
					"- 'max' to display the highest Gear Score of all classes",
					"- 'avg' to display the average Gear Score of all classes",
					"- 'full' to display the raw Gear Score of an item where class does not matter"
				};
		}
		p.example_format = { fmt::format("{} [class id / option]", sep->arg[0]) };
		p.examples_one = { "To see the average Gear Score of an item:", fmt::format("{} avg", sep->arg[0]) };
		p.examples_two = { "To see the max Gear Score of an item:", fmt::format("{} max", sep->arg[0]) };
		p.examples_three = { "To see the Gear Score for Wizards:", fmt::format("{} byclass {}", sep->arg[0], Class::Wizard) };

		std::string popup_text = c->SendBotCommandHelpWindow(p);
		popup_text = DialogueWindow::Table(popup_text);

		c->SendPopupToClient(sep->arg[0], popup_text.c_str());

		if (c->Admin() >= gm_access_level) {
			SendGearScoreTypePrompts(c);
		}

		return;
	}

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

	if (!strcasecmp(sep->arg[1], "update") && !strcasecmp(sep->arg[2], "all") && c->Admin() >= gm_access_level) {
		BenchTimer timer;
		timer.reset();

		BaseVeqGearScoreRepository::Truncate(database);
		LogTestDebug("delete took {}", std::to_string(timer.elapsed())); //deleteme
		const EQ::ItemData *item_to_update = nullptr;
		std::vector<BaseVeqGearScoreRepository::VeqGearScore> v;
		uint32 updated_count = 0;
		uint32 count = 0;

		const std::string query = "SELECT MAX(id) FROM items";
		auto results = database.QueryDatabase(query);

		if (!results.Success() || results.RowCount() == 0) {
			count = 0;
		}

		auto& row = results.begin();

		if (row[0]) {
			count = Strings::ToUnsignedInt(row[0]);
		}

		for (uint32 x = 0; x <= count; x++) {
			item_to_update = database.GetItem(x);

			if (item_to_update) {
				++updated_count;
				auto i = UpdateGearScores(item_to_update, true);

				v.emplace_back(i);
			}
		}

		if (!v.empty()) {
			BaseVeqGearScoreRepository::InsertMany(content_db, v);
		}

		LogTestDebug("{} items were updated in {}", updated_count, std::to_string(timer.elapsed())); //deleteme

		return;
	}

	const auto item_instance = c->GetInv().GetItem(EQ::invslot::slotCursor);

	if (!item_instance) {
		c->Message(Chat::Yellow, "You do not have an item on your cursor.");
		return;
	}
	else {
		const EQ::ItemData *item = item_instance->GetItem();

		if (item) {
			uint8 class_id = c->GetClass();

			if (!strcasecmp(sep->arg[1], "update") && c->Admin() >= gm_access_level) {
				UpdateGearScores(item);

				LogTestDebug(
					"{} [{}] has been updated with a Gear Score of {}. {} for {}s [{}].",
					item->Name,
					item->ID,
					GetGearScoreByClass(item->ID, Class::ALL_CLASSES_BITMASK),
					GetGearScoreByClass(item->ID, class_id),
					GetClassIDName(class_id),
					class_id
				); //deleteme

				return;
			}
			else {
				if (!strcasecmp(sep->arg[1], "byclass") && sep->IsNumber(2) && EQ::ValueWithin(atoi(sep->arg[2]), Class::Warrior, Class::Berserker)) {
					class_id = atoi(sep->arg[2]);
				}

				uint16 search_type = GearScoreTypes::Class;

				if (c->Admin() >= gm_access_level) {
					if (!strcasecmp(sep->arg[2], "byclass") && sep->IsNumber(3) && EQ::ValueWithin(atoi(sep->arg[3]), Class::Warrior, Class::Berserker)) {
						class_id = atoi(sep->arg[3]);
					}

					if (sep->IsNumber(1)) {
						search_type = atoi(sep->arg[1]);
					}
					else {
						std::string type_name = sep->arg[1];

						if (!type_name.empty()) {
							search_type = GetGearScoreTypeByShortname(type_name);
						}
					}
				}
				else if (!strcasecmp(sep->arg[1], "max")) {
					search_type = GearScoreTypes::Max;
				}
				else if (!strcasecmp(sep->arg[1], "avg")) {
					search_type = GearScoreTypes::Average;
				}
				else if (!strcasecmp(sep->arg[1], "min")) {
					search_type = GearScoreTypes::Max;
				}
				else if (!strcasecmp(sep->arg[1], "full")) {
					search_type = GearScoreTypes::Full;
				}

				EQ::SayLinkEngine linker;
				linker.SetLinkType(EQ::saylink::SayLinkItemData);
				linker.SetItemData(item);
				float gear_score = GetGearScoreByType(search_type, item, item->ID, class_id);

				c->Message(
					Chat::Lime,
					fmt::format(
						"{} has a{}Gear Score of {}{}",
						linker.GenerateLink(),
						(
							search_type > GearScoreTypes::Class && IsValidGearScoreType(search_type) ?
							fmt::format(
								" {} ",
								GetGearScoreTypeByID(search_type)
							).c_str() :
							" "
						),
						gear_score,
						(
							c->Admin() < gm_access_level && search_type != GearScoreTypes::Class ?
								"." :
								search_type <= GearScoreTypes::Full ?
									"." :
									fmt::format(
										" for {}s.",
										GetClassIDName(class_id)
									).c_str()
						)
					).c_str()
				);
			}
		}
		else {
			c->Message(Chat::Yellow, "Error finding item.");
		}
	}
}