#include "../client.h"

void command_healthreshold(Client* c, const Seperator* sep)
{
	int arguments = sep->argnum;
	if (!arguments || !strcasecmp(sep->arg[1], "help")) {
		c->Message(Chat::White, "usage: #healthreshold [help | current | percent of total health].");
		c->Message(Chat::White, "note: Used to control at what percent of health you will begin being regular healed by bots at.");
		c->Message(Chat::White, "note: Use [current] to check the current setting.");
		c->Message(Chat::White, "note: Set to 0 to disable this type of heal from bots.");
		c->Message(Chat::White, "note: The default threshold is 55 percent.");
		return;	
	}
	
	Client* player = c;
	uint32 thresholddata = 0;
	if (sep->IsNumber(1)) {
		thresholddata = atoi(sep->arg[1]);
		int thresholdcheck = thresholddata;
		if (thresholdcheck >= 0 && thresholdcheck <= 150) {
			c->Message(Chat::White, "Your Regular Heal Threshold has been set to %u percent.", thresholddata);
			player->SetClientHealThreshold(thresholddata);
		} else {
			c->Message(Chat::White, "You must enter a value between 0 and 150.");
			return;
		}
	}
	else if (!strcasecmp(sep->arg[1], "current")) {
		c->Message(Chat::White, "Your current Regular Heal Threshold is %u percent.", player->GetClientHealThreshold());
	}
	else {
		c->Message(Chat::White, "Incorrect argument, use #healthreshold help for a list of options.");
	}
}
