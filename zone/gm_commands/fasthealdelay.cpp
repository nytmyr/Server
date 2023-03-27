#include "../client.h"

void command_fasthealdelay(Client* c, const Seperator* sep)
{
	int arguments = sep->argnum;
	if (!arguments || !strcasecmp(sep->arg[1], "help")) {
		c->Message(Chat::White, "usage: #fasthealdelay [help | current | value in milliseconds. For example, 5000 = 5 seconds].");
		c->Message(Chat::White, "note: Used to control how often you will be fast healed by bots.");
		c->Message(Chat::White, "note: Use [current] to check the current setting.");
		c->Message(Chat::White, "note: The default interval is 2500 (2.5 seconds).");
		return;	
	}
	
	Client* player = c;
	uint32 delaydata = 0;
	if (sep->IsNumber(1)) {
		delaydata = atoi(sep->arg[1]);
		int delaycheck = delaydata;
		if (delaycheck >= 1 && delaycheck <= 60000) {
			player->SetClientFastHealDelay(delaydata);
			c->Message(Chat::White, "Your Fast Heal Delay has been set to %.2f seconds.", delaycheck / 1000.00);
		} else {
			c->Message(Chat::White, "You must enter a value between 1 and 60000.");
			return;
		}
	}
	else if (!strcasecmp(sep->arg[1], "current")) {
		float delayduration = player->GetClientFastHealDelay() / 1000.00;
		c->Message(Chat::White, "Your current Fast Heal Delay is %.2f seconds.", delayduration);
	}
	else {
		c->Message(Chat::White, "Incorrect argument, use #fasthealdelay help for a list of options.");
	}
}
