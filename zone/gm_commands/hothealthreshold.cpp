#include "../client.h"

void command_hothealthreshold(Client* c, const Seperator* sep)
{
	int arguments = sep->argnum;
	if (!arguments || !strcasecmp(sep->arg[1], "help")) {
		c->Message(Chat::White, "usage: #hothealthreshold [help | current | percent of total health].");
		c->Message(Chat::White, "note: Used to control at what percent of health you will begin being HoT healed by bots at.");
		c->Message(Chat::White, "note: Use [current] to check the current setting.");
		c->Message(Chat::White, "note: Set to 0 to disable this type of heal from bots.");
		c->Message(Chat::White, "note: The default threshold is 85 percent.");
		return;	
	}
	
	Client* player = c;
	uint32 thresholddata = 0;
	if (sep->IsNumber(1)) {
		thresholddata = atoi(sep->arg[1]);
		int thresholdcheck = thresholddata;
		if (thresholdcheck >= 0 && thresholdcheck <= 150) {
			c->Message(Chat::White, "Your HoT Heal Threshold has been set to %u percent.", thresholddata);
			player->SetClientHotHealThreshold(thresholddata);
		} else {
			c->Message(Chat::White, "You must enter a value between 0 and 150.");
			return;
		}
	}
	else if (!strcasecmp(sep->arg[1], "current")) {
		c->Message(Chat::White, "Your current HoT Heal Threshold is %u percent.", player->GetClientHotHealThreshold());
	}
	else {
		c->Message(Chat::White, "Incorrect argument, use #hothealthreshold help for a list of options.");
	}
}
