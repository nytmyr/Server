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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#include "veq_spell_data.h"
#include "../zone/bot.h"

VeqSpellScoreRepository::VeqSpellScore UpdateSpellScores(Mob* caster, const SPDat_Spell_Struct spell, uint8 spell_level, bool bulk_update) {
	auto e = VeqSpellScoreRepository::NewEntity();

	if (spell_level == UINT8_MAX) {
		spell_level = GetSpellMinimumLevel(spell.id);

		if (spell_level > RuleI(Character, MaxLevel)) {
			spell_level = 80; // NPC only spell
		}
		else {
			spell_level = RuleI(Character, MaxLevel);
		}
	}

	e.id = spell.id;
	e.name = spell.name;
	e.spell_level = spell_level;
	e.spell_score = CalculateSpellScore(caster, spell.id, spell_level); // TODO use this for NPCs to check their spell score, items for their cast level or min of req/click/proc/worn/focus level (or max player level?)
	e.spell_type = Bot::GetSpellTypeNameByID(GetCorrectBotSpellType(BotSpellTypes::SummonCorpse, spell.id));

	// Detrimental
	e.dd_score = CalculateDirectDamageScore(caster, spell.id, spell_level);
	e.dot_score = CalculateDamageOverTimeScore(caster, spell.id, spell_level);
	e.slow_score = CalculateSlowScore(caster, spell.id, spell_level);
	e.ae_rain_score = CalculateAERainScore(caster, spell.id, spell_level);

	// Utility
	//e.lifetap_score = CalculateLifeTapScore(caster, spell.id, spell_level);

	// Beneficial
	//e.heal_score = CalculateHealScore(caster, spell.id, spell_level);
	//e.hot_score = CalculateHealOverTimeScore(caster, spell.id, spell_level);

	if (bulk_update) {
		return e;
	}
	else {
		VeqSpellScoreRepository::ReplaceOne(database, e);
	}
}

float GetSpellScore(uint32 spell_id) {
	const auto& l = VeqSpellScoreRepository::GetWhere(
		database,
		fmt::format(
			"`id` = {}",
			spell_id
		)
	);

	if (l.empty()) {
		return 0.0f;
	}

	auto e = l.front();

	return e.spell_score;
}

float CalculateSpellScore(Mob* caster, uint32 spell_id, uint8 spell_level) {
	if (!IsValidSpell(spell_id)) {
		return 0.0f;
	}

	const SPDat_Spell_Struct spell = spells[spell_id];
	float spell_score = 0.0f;

	// Detrimental
	spell_score += CalculateDirectDamageScore(caster, spell.id, spell_level);
	spell_score += CalculateDamageOverTimeScore(caster, spell.id, spell_level);
	spell_score += CalculateSlowScore(caster, spell.id, spell_level);
	spell_score += CalculateAERainScore(caster, spell.id, spell_level);

	// Utility
	//spell_score += CalculateLifeTapScore(caster, spell.id, spell_level);

	// Beneficial
	//spell_score += CalculateHealScore(caster, spell.id, spell_level);
	//spell_score += CalculateHealOverTimeScore(caster, spell.id, spell_level);

	if (IsPBAESpell(spell_id)) {
		spell_score *= (0.8f + std::max(1.0f, spell.aoe_range / 100.0f)) * VegasSpellTypeMultipliers::PBAE;
	}
	else if (IsAnyAESpell(spell_id)) {
		spell_score *= (0.5f + std::max(1.0f, spell.aoe_range / 100.0f)) * VegasSpellTypeMultipliers::AE;
	}

	return spell_score;
}

float CalculateDirectDamageScore(Mob* caster, uint32 spell_id, uint8 spell_level) {
	if (!IsValidSpell(spell_id)) {
		return 0.0f;
	}

	if (!IsDetrimentalSpell(spell_id)) {
		return 0.0f;
	}

	const SPDat_Spell_Struct spell = spells[spell_id];
	float spell_score = 0.0f;

	for (int i = 0; i < EFFECT_COUNT; i++) {
		if (IsBlankSpellEffect(spell_id, i)) {
			continue;
		}

		const auto effect_id = spell.effect_id[i];

		bool is_valid_spell = spell.base_value[i] < 0 &&
			(
				effect_id == SE_CurrentHPOnce ||
				(effect_id == SE_CurrentHP && spell.buff_duration == 0 && spell.aoe_duration == 0)
			);

		if (is_valid_spell) {
			auto spell_value = caster->CalcSpellEffectValue_formula(spell.formula[i], spell.base_value[i], spell.max_value[i], spell_level, spell_id); //deleteme
			LogTestDebugDetail("Checking {} [{}] as a Slow --- effect value of {}.", spell.name, spell.id, spell_value); //deleteme
			spell_score +=
				caster->CalcSpellEffectValue_formula(
					spell.formula[i],
					spell.base_value[i],
					spell.max_value[i],
					spell_level,
					spell_id
				);
		}
	}

	return spell_score * VegasSpellTypeMultipliers::DirectDamage;
}

float CalculateDamageOverTimeScore(Mob* caster, uint32 spell_id, uint8 spell_level) {
	if (!IsValidSpell(spell_id)) {
		return 0.0f;
	}

	if (!IsDetrimentalSpell(spell_id)) {
		return 0.0f;
	}

	const SPDat_Spell_Struct spell = spells[spell_id];
	float spell_score = 0.0f;

	for (int i = 0; i < EFFECT_COUNT; i++) {
		if (IsBlankSpellEffect(spell_id, i)) {
			continue;
		}

		const auto effect_id = spell.effect_id[i];

		bool is_valid_spell =
			effect_id == SE_CurrentHP &&
			spell.base_value[i] < 0 &&
			spell.buff_duration > 0;

		if (!is_valid_spell) {
			continue;
		}

		float duration = CalcBuffDuration_formula(spell_level, spell.buff_duration_formula, spell.buff_duration);

		duration = std::min(int(duration), 60); // Only score up to 5 minutes.

		for (int x = 0; x < duration; x++) {
			auto spell_value = caster->CalcSpellEffectValue_formula(spell.formula[i], spell.base_value[i], spell.max_value[i], spell_level, spell_id, x); //deleteme
			LogTestDebugDetail("Checking {} [{}] as a DoT --- effect value of {} with {} ticks remaining.", spell.name, spell.id, spell_value, duration - x); //deleteme
			spell_score +=
				caster->CalcSpellEffectValue_formula(
					spell.formula[i],
					spell.base_value[i],
					spell.max_value[i],
					spell_level,
					spell_id,
					x
				);
		}
	}

	return spell_score * VegasSpellTypeMultipliers::DamageOverTime;
}

float CalculateSlowScore(Mob* caster, uint32 spell_id, uint8 spell_level) {
	if (!IsValidSpell(spell_id)) {
		return 0.0f;
	}

	if (!IsDetrimentalSpell(spell_id)) {
		return 0.0f;
	}

	const SPDat_Spell_Struct spell = spells[spell_id];
	float spell_score = 0.0f;

	for (int i = 0; i < EFFECT_COUNT; i++) {
		if (IsBlankSpellEffect(spell_id, i)) {
			continue;
		}

		const auto effect_id = spell.effect_id[i];

		bool is_valid_spell =
			effect_id == SE_AttackSpeed &&
			spell.max_value[i] < 100;

		if (is_valid_spell) {
			float value = caster->CalcSpellEffectValue_formula(
				spell.formula[i],
				spell.base_value[i],
				spell.max_value[i],
				spell_level,
				spell_id
			);
			LogTestDebugDetail("Checking {} [{}] as a Slow --- effect value of {}.", spell.name, spell.id, value); //deleteme
			spell_score += abs(value - 100) * VegasSpellTypeMultipliers::Slow;
		}
	}

	return spell_score;
}

float CalculateAERainScore(Mob* caster, uint32 spell_id, uint8 spell_level) {
	if (!IsValidSpell(spell_id)) {
		return 0.0f;
	}

	if (!IsDetrimentalSpell(spell_id)) {
		return 0.0f;
	}

	const SPDat_Spell_Struct spell = spells[spell_id];
	float spell_score = 0.0f;

	for (int i = 0; i < EFFECT_COUNT; i++) {
		if (IsBlankSpellEffect(spell_id, i)) {
			continue;
		}

		const auto effect_id = spell.effect_id[i];

		bool is_valid_spell =
			effect_id == SE_CurrentHP &&
			spell.base_value[i] < 0 &&
			spell.aoe_range > 0 &&
			spell.aoe_duration > 1000;

		if (!is_valid_spell) {
			continue;
		}

		int duration = spell.aoe_duration / 2500;

		duration = CalcBuffDuration_formula(spell_level, spell.buff_duration_formula, spell.buff_duration);
		duration = std::min(duration, 60); // Only score up to 5 minutes.

		for (int x = 0; x < duration; x++) {
			auto spell_value = caster->CalcSpellEffectValue_formula(spell.formula[i], spell.base_value[i], spell.max_value[i], spell_level, spell_id, x); //deleteme
			LogTestDebugDetail("Checking {} [{}] as an AERain --- effect value of {} with {} ticks remaining.", spell.name, spell.id, spell_value, duration - x); //deleteme
			spell_score +=
				caster->CalcSpellEffectValue_formula(
					spell.formula[i],
					spell.base_value[i],
					spell.max_value[i],
					spell_level,
					spell_id,
					x
				);
		}
	}

	return spell_score * VegasSpellTypeMultipliers::AERain;
}