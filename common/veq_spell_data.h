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
	constexpr float DirectMana	                 	 = 2.00f;
	constexpr float ManaDamageOverTime               = 1.00f;
	constexpr float Slow                 	 		 = 5.00f;
	constexpr float Group                 	 	 	 = 2.00f;
	constexpr float AE                 	 	 		 = 3.00f;
	constexpr float PBAE                 	 	     = 4.00f;
	constexpr float AERain                 	 	     = 1.00f;
	constexpr float ACDebuff                 	 	 = 6.00f;
	constexpr float ATKDebuff                 	 	 = 6.00f;
	constexpr float Snare                 	 	 	 = 3.00f;
	constexpr float BaseStatDebuff                 	 = 2.00f;
	constexpr float Blind                 	 	 	 = 5.00f;
	constexpr float StunBase                 	 	 = 50.00f;
	constexpr float StunMultiplier                 	 = 50.00f;

	// Utility
	constexpr float Lifetap                 	 	 = 1.50f;
	constexpr float Manatap                 	 	 = 1.20f;
	constexpr float Pacify                 	 		 = 50.00f;
	constexpr float Charm                 	 		 = 20.00f;
	constexpr float Fear                 	 		 = 10.00f;
	constexpr float BindAffinity                 	 = 50.00f;
	constexpr float Gate                 	 		 = 100.00f;

	// Beneficials
	constexpr float InstantHeal                 	 = 1.00f;
	constexpr float HealOverTime                 	 = 0.35f;
	constexpr float InstantMana                 	 = 2.00f;
	constexpr float ManaOverTime                 	 = 1.00f;
	constexpr float CompleteHeal                 	 = 1.00f;
	constexpr float Haste                 	 		 = 5.00f;
	constexpr float ACBuff                 	 		 = 6.00f;
	constexpr float ATKBuff                 	 	 = 6.00f;
	constexpr float MovementSpeed                 	 = 3.00f;
	constexpr float BaseStatBuff                 	 = 2.00f;
	constexpr float Invisibility                 	 = 75.00f;
	constexpr float SeeInvis                 	 	 = 10.00f;
	constexpr float WaterBreathing                 	 = 35.00f;
	constexpr float AddFaction                 	 	 = 1.00f;
	constexpr float StaminaLoss                 	 = 1.00f;
	constexpr float CureBlind                 	 	 = 25.00f;

	// Spell options
	constexpr float TargetTypeOnly                 	 = 0.5f;
}

VeqSpellScoreRepository::VeqSpellScore UpdateSpellScores(Mob* caster, const SPDat_Spell_Struct spell, uint8 spell_level = UINT8_MAX, bool bulk_update = false);
float GetSpellScore(uint32 spell_id);
float CalculateSpellScore(Mob* caster, uint32 spell_id, uint8 spell_level = RuleI(Character, MaxLevel));

VeqSpellScoreRepository::VeqSpellScore UpdateFullScore(Mob* caster, const SPDat_Spell_Struct spell, uint8 spell_level = UINT8_MAX, bool bulk_update = false);

// Detrimental
float CalculateDirectDamageScore(Mob* caster, uint32 spell_id, uint8 spell_level = RuleI(Character, MaxLevel));
float CalculateDamageOverTimeScore(Mob* caster, uint32 spell_id, uint8 spell_level = RuleI(Character, MaxLevel));
float CalculateSlowScore(Mob* caster, uint32 spell_id, uint8 spell_level = RuleI(Character, MaxLevel));
float CalculateAERainScore(Mob* caster, uint32 spell_id, uint8 spell_level = RuleI(Character, MaxLevel));
//float CalculateSnareScore(Mob* caster, uint32 spell_id, uint8 spell_level = RuleI(Character, MaxLevel));

// Utility
float CalculateLifeTapScore(Mob* caster, uint32 spell_id, uint8 spell_level = RuleI(Character, MaxLevel));

// Beneficial
float CalculateHealScore(Mob* caster, uint32 spell_id, uint8 spell_level = RuleI(Character, MaxLevel));
float CalculateHealOverTimeScore(Mob* caster, uint32 spell_id, uint8 spell_level = RuleI(Character, MaxLevel));
float CalculateHasteScore(Mob* caster, uint32 spell_id, uint8 spell_level = RuleI(Character, MaxLevel));
//float CalculateACScore(Mob* caster, uint32 spell_id, uint8 spell_level = RuleI(Character, MaxLevel));
//float CalculateATKScore(Mob* caster, uint32 spell_id, uint8 spell_level = RuleI(Character, MaxLevel));
//float CalculateMovementSpeedScore(Mob* caster, uint32 spell_id, uint8 spell_level = RuleI(Character, MaxLevel));
//float CalculateSTRScore(Mob* caster, uint32 spell_id, uint8 spell_level = RuleI(Character, MaxLevel));
//float CalculateDEXScore(Mob* caster, uint32 spell_id, uint8 spell_level = RuleI(Character, MaxLevel));
//float CalculateAGIScore(Mob* caster, uint32 spell_id, uint8 spell_level = RuleI(Character, MaxLevel));
//float CalculateSTAScore(Mob* caster, uint32 spell_id, uint8 spell_level = RuleI(Character, MaxLevel));
//float CalculateINTScore(Mob* caster, uint32 spell_id, uint8 spell_level = RuleI(Character, MaxLevel));
//float CalculateWISScore(Mob* caster, uint32 spell_id, uint8 spell_level = RuleI(Character, MaxLevel));
//float CalculateCHAScore(Mob* caster, uint32 spell_id, uint8 spell_level = RuleI(Character, MaxLevel));

// FOR PCs
// add cast time
// add recovery time
// add recast time
// add resist factors +/- or unresistable
// add recourse checks
// add hate modifier
// add spell hate given
// add no detrimental spell aggro (pc only)
// add target type
// add restrictions (in/out of combat, must be sneak?, target, caster, zone, time of day, pc/npc only
// add cannot dispell
// add knockback

// FOR NPCs
// Iterate through and grab all unique spells for spell lists up until npc_spells.parent_list = 0
// DO MIN/MAX LEVEL CHECKS FOR SPELL LISTS in npc_spells_entries
// Add proc effect
// Add proc chance
// Add recast_delay variable from npc_spells_entries
// Add resist_adjust variable from npc_spells_entries

// FOR PCs?
// add range? (regular, aoe)
// add dot not stackable check?
// add persist after death?
// add focus max targets?
// add Focus Song Base Effect Cap?
// add max targets?
// add max hits allowed?
// add max hits type??
// add mana cost?
// add Max Critical Chance?

// FOR NPCs?
// Is mana_cost necessary in npc_spells_entries?
// Add min_hp/max_hp variable from npc_spells_entries?
// Add range proc?
// Add range proc chance?
// Add defensive proc?
// Add defensive proc chance?

#endif /*COMMON_VEQ_SPELL_DATA_H*/