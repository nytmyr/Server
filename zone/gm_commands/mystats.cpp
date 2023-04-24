#include "../client.h"

void command_mystats(Client *c, const Seperator *sep)
{
	if (c->GetTarget() && c->GetPet() && (c->GetTarget()->IsPet() && c->GetTarget() == c->GetPet())) {
		c->GetTarget()->ShowStats(c);
	}
	else if (c->GetTarget() && c->GetTarget()->IsBot() && (c->GetTarget()->GetOwner()->CastToClient()->CharacterID() == c->CharacterID())) {
		c->GetTarget()->ShowStats(c);
	}
	else {
		c->ShowStats(c);
	}
}

