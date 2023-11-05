#include "client.h"
//#include "mob.h"

void command_market(Client* c, const Seperator* sep)
{
	if (!RuleB(Market, MarketCommandEnabled)) {
		c->Message(Chat::Yellow, "This command is currently disabled.");
		return;
	}

	if (RuleB(Market, MarketRequireSpecificZone)) {
		auto zone_id = zone->GetZoneID();
		if (RuleB(Market, MarketAllowPoK) && RuleB(Market, MarketAllowBazaar) && (zone_id != 202 || zone_id != 151)) {
			c->Message(Chat::Yellow, "This command can only be done in %s and %s.", ZoneLongName(151, true), ZoneLongName(202, true));
			return;
		}
		else if (!RuleB(Market, MarketAllowPoK) && RuleB(Market, MarketAllowBazaar) && zone_id != 151) {
			c->Message(Chat::Yellow, "This command can only be done in %s.", ZoneLongName(151, true));
				return;
		}
		else if (RuleB(Market, MarketAllowPoK) && !RuleB(Market, MarketAllowBazaar) && zone_id != 202) {
			c->Message(Chat::Yellow, "This command can only be done in %s.", ZoneLongName(202, true));
				return;
		}
	}
}
