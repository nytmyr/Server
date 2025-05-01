/*	EQEMu: Everquest Server Emulator

Copyright (C) 2001-2016 EQEMu Development Team (http://eqemulator.net)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; version 2 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY except by those people which sell it, which
    are required to give you total support for your newly bought product;
    without even the implied warranty of MERCHANTABILITY or FITNESS FOR
    A PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  04111-1307  USA
*/

#ifndef COMMON_VEQ_SPELL_DATA_H
#define COMMON_VEQ_SPELL_DATA_H

#include "spdat.h"
#include "../zone/mob.h"
#include "repositories/veq_spell_score_repository.h"

namespace VegasSpellTypeMultipliers {
	// Detrimentals
	constexpr float DirectDamage                 	 = 1.00f;
	constexpr float DamageOverTime                 	 = 0.35f;
	constexpr float Slow                 	 		 = 5.00f;
	constexpr float AE                 	 	 		 = 3.00f;
	constexpr float PBAE                 	 	     = 4.00f;
	constexpr float AERain                 	 	     = 1.00f;

	// Utility
	constexpr float Lifetap                 	 	 = 1.00f;

	// Beneficials
	constexpr float InstantHeal                 	 = 1.00f;
	constexpr float HealOverTime                 	 = 0.35f;
	constexpr float CompleteHeal                 	 = 1.00f;
}

VeqSpellScoreRepository::VeqSpellScore UpdateSpellScores(Mob* caster, const SPDat_Spell_Struct spell, uint8 spell_level = UINT8_MAX, bool bulk_update = false);
float GetSpellScore(uint32 spell_id);
float CalculateSpellScore(Mob* caster, uint32 spell_id, uint8 spell_level = RuleI(Character, MaxLevel));

// Detrimental
float CalculateDirectDamageScore(Mob* caster, uint32 spell_id, uint8 spell_level = RuleI(Character, MaxLevel));
float CalculateDamageOverTimeScore(Mob* caster, uint32 spell_id, uint8 spell_level = RuleI(Character, MaxLevel));
float CalculateSlowScore(Mob* caster, uint32 spell_id, uint8 spell_level = RuleI(Character, MaxLevel));
float CalculateAERainScore(Mob* caster, uint32 spell_id, uint8 spell_level = RuleI(Character, MaxLevel));

// Utility
// float CalculateLifeTapScore(caster, spell.id, spell_level = RuleI(Character, MaxLevel));

// Beneficial
// float CalculateHealScore(caster, spell.id, spell_level = RuleI(Character, MaxLevel));
// float CalculateHealOverTimeScore(caster, spell.id, spell_level = RuleI(Character, MaxLevel));

#endif /*COMMON_VEQ_SPELL_DATA_H*/