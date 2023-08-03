#include "../client.h"

void command_getgearscore(Client* c, const Seperator* sep)
{
	if (!strcasecmp(sep->arg[1], "help")) {
		c->Message(Chat::White, "Usage: #getgearscore with an item on your cursor.");
		return;
	}

	const auto item_instance = c->GetInv().GetItem(EQ::invslot::slotCursor);
	if (!item_instance) {
		c->Message(Chat::Yellow, "You do not have an item on your cursor.");
		return;
	}
	else {
		const EQ::ItemData* item = nullptr;
		EQ::SayLinkEngine  linker;
		linker.SetLinkType(EQ::saylink::SayLinkItemData);
		item = item_instance->GetItem();
		if (item) {
			linker.SetItemData(item);
			float gear_score = item->GearScore;
			if (gear_score && gear_score != 0 && gear_score != -99999) {
				c->Message(
					Chat::Lime,
					fmt::format(
						"{} has a GearScore of {}.",
						linker.GenerateLink(),
						gear_score
					).c_str()
				);
			}
			else {
				c->Message(
					Chat::Lime,
					fmt::format(
						"{} does not have a GearScore.",
						linker.GenerateLink()
					).c_str()
				);
			}
		}
		else {
			c->Message(Chat::Yellow, "Error finding item.");
		}
	}
}
