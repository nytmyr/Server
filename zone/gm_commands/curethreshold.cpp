#include "../client.h"

void command_curethreshold(Client* c, const Seperator* sep)
{
	int arguments = sep->argnum;
	if (!arguments || !strcasecmp(sep->arg[1], "help")) {
		c->Message(Chat::White, "usage: #curethreshold [help | current | percent of total health].");
		c->Message(Chat::White, "note: Used to control the percent of health you will start being cured by bots at.");
		c->Message(Chat::White, "note: Use [current] to check the current setting.");
		c->Message(Chat::White, "note: The default threshold is 150 percent (always get cured).");
		return;
	}

	Client* player = c;
	uint32 thresholddata = 0;
	if (sep->IsNumber(1)) {
		thresholddata = atoi(sep->arg[1]);
		int thresholdcheck = thresholddata;
		if (thresholdcheck >= 0 && thresholdcheck <= 150) {
			c->Message(Chat::White, "Your Cure Threshold has been set to %u percent.", thresholddata);
			player->SetClientCureThreshold(thresholddata);
		}
		else {
			c->Message(Chat::White, "You must enter a value between 0 and 150.");
			return;
		}
	}
	else if (!strcasecmp(sep->arg[1], "current")) {
		c->Message(Chat::White, "Your current Cure Threshold is %u percent.", player->GetClientCureThreshold());
	}
	else {
		c->Message(Chat::White, "Incorrect argument, use #curethreshold help for a list of options.");
	}
}
