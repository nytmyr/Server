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

void bot_command_class_race_list(Client* c, const Seperator* sep)
{
	if (helper_is_help_or_usage(sep->arg[1])) {
		c->Message(
			Chat::White,
			fmt::format(
				"Usage: {} deities to display a list of deities as well",
				sep->arg[0]
			).c_str()
		);

		return;
	}

	const std::string class_substrs[17] = {
		"",
		"WAR", "CLR", "PAL", "RNG",
		"SHD", "DRU", "MNK", "BRD",
		"ROG", "SHM", "NEC", "WIZ",
		"MAG", "ENC", "BST", "BER"
	};

	const std::string race_substrs[17] = {
		"",
		"HUM", "BAR", "ERU", "ELF",
		"HIE", "DEF", "HEF", "DWF",
		"TRL", "OGR", "HFL", "GNM",
		"IKS", "VAH", "FRG", "DRK"
	};

	const uint16 race_values[17] = {
		Race::Doug,
		Race::Human, Race::Barbarian, Race::Erudite, Race::WoodElf,
		Race::HighElf, Race::DarkElf, Race::HalfElf, Race::Dwarf,
		Race::Troll, Race::Ogre, Race::Halfling, Race::Gnome,
		Race::Iksar, Race::VahShir, Race::Froglok2, Race::Drakkin
	};

	const std::string deity_substrs[18] = {
		"",
		"Bertoxxulous", "Brell Serilis", "Cazic-Thule", "Erollisi Marr",
		"Bristlebane", "Innoruuk", "Karana", "Mithaniel Marr",
		"Prexus", "Quellious", "Rallos Zek", "Rodcet Nife",
		"Solusek Ro", "The Tribunal", "Tunare", "Veeshan", "Agnostic"
	};

	const uint16 deity_values[18] = {
		Deity::Agnostic1,
		Deity::Bertoxxulous,
		Deity::BrellSirilis,
		Deity::CazicThule,
		Deity::ErollisiMarr,
		Deity::Bristlebane,
		Deity::Innoruuk,
		Deity::Karana,
		Deity::MithanielMarr,
		Deity::Prexus,
		Deity::Quellious,
		Deity::RallosZek,
		Deity::RodcetNife,
		Deity::SolusekRo,
		Deity::TheTribunal,
		Deity::Tunare,
		Deity::Veeshan,
		Deity::Agnostic2
	};

	std::string window_text;
	std::string message_separator;
	int object_count = 0;
	const int object_max = 4;

	window_text.append(
		fmt::format(
			"<c \"#EDDA74\">Classes{}<c \"#357EC7\">",
			DialogueWindow::Break()
		)
	);

	window_text.append(
		fmt::format(
			"<c \"#D4A017\">--------------------------------------------------------------------<c \"#357EC7\">",
			DialogueWindow::Break()
		)
	);

	window_text.append(DialogueWindow::Break());

	message_separator = " ";
	object_count = 0;

	for (int i = 0; i <= 15; ++i) {
		window_text.append(message_separator);

		if (object_count >= object_max) {
			window_text.append(DialogueWindow::Break());
			object_count = 0;
		}

		window_text.append(
			fmt::format("{} ({})",
				class_substrs[i + 1],
				(i + 1)
			)
		);

		++object_count;
		message_separator = ", ";
	}

	window_text.append(DialogueWindow::Break(2));

	window_text.append(
		fmt::format(
			"<c \"#EDDA74\">Races{}<c \"#357EC7\">",
			DialogueWindow::Break()
		)
	);

	window_text.append(
		fmt::format(
			"<c \"#D4A017\">--------------------------------------------------------------------<c \"#357EC7\">",
			DialogueWindow::Break()
		)
	);

	window_text.append(DialogueWindow::Break());

	message_separator = " ";
	object_count = 0;

	for (int i = 0; i <= 15; ++i) {
		window_text.append(message_separator);

		if (object_count >= object_max) {
			window_text.append(DialogueWindow::Break());
			object_count = 0;
		}

		window_text.append(
			fmt::format("{} ({})",
				race_substrs[i + 1],
				race_values[i + 1]
			)
		);

		++object_count;
		message_separator = ", ";
	}

	std::string arg1 = sep->arg[1];

	if (!arg1.compare("deities")) {
		window_text.append(DialogueWindow::Break(2));

		window_text.append(
			fmt::format(
				"<c \"#EDDA74\">Deities{}<c \"#357EC7\">",
				DialogueWindow::Break()
			)
		);

		window_text.append(
			fmt::format(
				"<c \"#D4A017\">--------------------------------------------------------------------<c \"#357EC7\">",
				DialogueWindow::Break()
			)
		);

		window_text.append(DialogueWindow::Break());

		message_separator = " ";
		object_count = 0;

		for (int i = 0; i <= 16; ++i) {
			if (object_count == 1) {
				window_text.append(DialogueWindow::Break());
				object_count = 0;
			}

			window_text.append(
				fmt::format("{} ({})",
					deity_substrs[i + 1],
					deity_values[i + 1]
				)
			);

			++object_count;
		}
	}

	c->SendPopupToClient("Bot Creation Options", window_text.c_str());

	return;
}
