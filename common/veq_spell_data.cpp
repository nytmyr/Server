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

	if (!IsValidSpell(spell.id)) {
		if (bulk_update) {
			return e;
		}
		else {
			VeqSpellScoreRepository::ReplaceOne(database, e);
		}
	}

	if (spell_level == UINT8_MAX) {
		spell_level = GetSpellMinimumLevel(spell.id);

		if (spell_level > RuleI(Character, MaxLevel)) {
			spell_level = RuleI(NPC, MaxNPCLevel); // NPC only spell
		}
		else {
			spell_level = RuleI(Character, MaxLevel);
		}
	}

	e.id = spell.id;
	e.name = spell.name;
	e.lowest_spell_level = GetSpellMinimumLevel(spell.id);
	e.lowest_class = GetSpellMinimumClass(spell.id);
	e.spell_score = CalculateSpellScore(caster, spell.id, spell_level); // TODO use this for NPCs to check their spell score, items for their cast level or min of req/click/proc/worn/focus level (or max player level?)
	e.spell_type = Bot::GetSpellTypeNameByID(GetCorrectBotSpellType(BotSpellTypes::SummonCorpse, spell.id));
	e.duration = CalcBuffDuration_formula(spell_level, spell.buff_duration_formula, spell.buff_duration);

	// Detrimental
	e.dd_score = CalculateDirectDamageScore(caster, spell.id, spell_level);
	e.dot_score = CalculateDamageOverTimeScore(caster, spell.id, spell_level);
	e.slow_score = CalculateSlowScore(caster, spell.id, spell_level);
	e.ae_rain_score = CalculateAERainScore(caster, spell.id, spell_level);

	// Utility
	e.lifetap_score = CalculateLifeTapScore(caster, spell.id, spell_level);

	// Beneficial
	e.heal_score = CalculateHealScore(caster, spell.id, spell_level);
	e.hot_score = CalculateHealOverTimeScore(caster, spell.id, spell_level);
	e.haste_score = CalculateHasteScore(caster, spell.id, spell_level);

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
	spell_score += CalculateLifeTapScore(caster, spell.id, spell_level);

	// Beneficial
	spell_score += CalculateHealScore(caster, spell.id, spell_level);
	spell_score += CalculateHealOverTimeScore(caster, spell.id, spell_level);

	if (IsPBAESpell(spell_id)) {
		spell_score *= (0.8f + (std::fmin(2.5f, spell.aoe_range / 100.0f)) * VegasSpellTypeMultipliers::PBAE);
	}
	else if (IsAnyAESpell(spell_id)) {
		spell_score *= (0.5f + (std::fmin(2.5f, spell.aoe_range / 100.0f)) * VegasSpellTypeMultipliers::AE);
	}
	else if (IsGroupSpell(spell_id)) {
		spell_score *= (std::fmin(1.0f, spell.aoe_range / 100.0f) * VegasSpellTypeMultipliers::Group);
	}

	// spell_score *= (4000 / spell.cast_time); // < 4 second = bonus, > 4 second = penalty
	// spell_score *= 1 + -(spell.recovery_time / 2000); // penalty the longer the recovery time is
	// spell_score *= (4000 / spell.recast_time); // < 4 second = bonus, > 4 second = penalty
	// add resist factors +/- or unresistable

	return spell_score;
}

// Detrimental
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
			float spell_value = caster->CalcSpellEffectValue_formula(spell.formula[i], spell.base_value[i], spell.max_value[i], spell_level, spell_id); //deleteme
			LogSpellScoreDetail("Checking {} [{}] as a DD --- effect value of {}.", spell.name, spell.id, spell_value); //deleteme
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

	return abs(spell_score * VegasSpellTypeMultipliers::DirectDamage);
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

		float duration = std::fmax(1.0f, std::fmin(spell.buff_duration, 50.0f)); // Only score up to 5 minutes.

		for (int x = 0; x < duration; x++) {
			float spell_value = caster->CalcSpellEffectValue_formula(spell.formula[i], spell.base_value[i], spell.max_value[i], spell_level, spell_id, x); //deleteme
			LogSpellScoreDetail("Checking {} [{}] as a DoT --- effect value of {} with {} ticks remaining.", spell.name, spell.id, spell_value, duration - x); //deleteme
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

	return abs(spell_score * VegasSpellTypeMultipliers::DamageOverTime);
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
			LogSpellScoreDetail("Checking {} [{}] as a Slow --- effect value of {}.", spell.name, spell.id, value); //deleteme
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

		float duration = std::fmax(1.0f, std::fmin(spell.aoe_duration, 50.0f)); // Only score up to 5 minutes.

		for (int x = 0; x < duration; x++) {
			float spell_value = caster->CalcSpellEffectValue_formula(spell.formula[i], spell.base_value[i], spell.max_value[i], spell_level, spell_id, x); //deleteme
			LogSpellScoreDetail("Checking {} [{}] as an AERain --- effect value of {} with {} ticks remaining.", spell.name, spell.id, spell_value, duration - x); //deleteme
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

	return abs(spell_score * VegasSpellTypeMultipliers::AERain);
}

// Utility
float CalculateLifeTapScore(Mob* caster, uint32 spell_id, uint8 spell_level) {
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
			spell.base_value[i] < 0 &&
			(
				effect_id == SE_CurrentHPOnce ||
				(effect_id == SE_CurrentHP && spell.buff_duration == 0 && spell.aoe_duration == 0)
			) &&
			(
				spell.target_type == ST_Tap ||
				spell.target_type == ST_TargetAETap ||
				spell_id == SPELL_ANCIENT_LIFEBANE
			);

		if (is_valid_spell) {
			float spell_value = caster->CalcSpellEffectValue_formula(spell.formula[i], spell.base_value[i], spell.max_value[i], spell_level, spell_id); //deleteme
			LogSpellScoreDetail("Checking {} [{}] as a Lifetap --- effect value of {}.", spell.name, spell.id, spell_value); //deleteme
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

	return abs(spell_score * VegasSpellTypeMultipliers::Lifetap);
}

// Beneficial
float CalculateHealScore(Mob* caster, uint32 spell_id, uint8 spell_level) {
	if (!IsValidSpell(spell_id)) {
		return 0.0f;
	}

	if (IsDetrimentalSpell(spell_id)) {
		return 0.0f;
	}

	const SPDat_Spell_Struct spell = spells[spell_id];
	float spell_score = 0.0f;

	for (int i = 0; i < EFFECT_COUNT; i++) {
		if (IsBlankSpellEffect(spell_id, i)) {
			continue;
		}

		const auto effect_id = spell.effect_id[i];

		bool is_valid_spell = spell.base_value[i] > 0 &&
							  (
								  effect_id == SE_CurrentHPOnce ||
								  (effect_id == SE_CurrentHP && spell.buff_duration == 0 && spell.aoe_duration == 0)
							  );

		if (is_valid_spell) {
			float spell_value = caster->CalcSpellEffectValue_formula(spell.formula[i], spell.base_value[i], spell.max_value[i], spell_level, spell_id); //deleteme
			LogSpellScoreDetail("Checking {} [{}] as a Instant Heal --- effect value of {}.", spell.name, spell.id, spell_value); //deleteme
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

	return spell_score * VegasSpellTypeMultipliers::InstantHeal;
}

float CalculateHealOverTimeScore(Mob* caster, uint32 spell_id, uint8 spell_level) {
	if (!IsValidSpell(spell_id)) {
		return 0.0f;
	}

	if (IsDetrimentalSpell(spell_id)) {
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
			effect_id == SE_HealOverTime &&
			spell.base_value[i] > 0 &&
			spell.buff_duration > 0;

		if (!is_valid_spell) {
			continue;
		}

		float duration = std::fmax(1.0f, std::fmin(spell.buff_duration, 50.0f)); // Only score up to 5 minutes.

		for (int x = 0; x < duration; x++) {
			float spell_value = caster->CalcSpellEffectValue_formula(spell.formula[i], spell.base_value[i], spell.max_value[i], spell_level, spell_id, x); //deleteme
			LogSpellScoreDetail("Checking {} [{}] as a HoT --- effect value of {} with {} ticks remaining.", spell.name, spell.id, spell_value, duration - x); //deleteme
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

	return spell_score * VegasSpellTypeMultipliers::HealOverTime;
}

float CalculateHasteScore(Mob* caster, uint32 spell_id, uint8 spell_level) {
	if (!IsValidSpell(spell_id)) {
		return 0.0f;
	}

	if (IsDetrimentalSpell(spell_id)) {
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
			spell.max_value[i] > 100;

		if (is_valid_spell) {
			float value = caster->CalcSpellEffectValue_formula(
				spell.formula[i],
				spell.base_value[i],
				spell.max_value[i],
				spell_level,
				spell_id
			);
			LogSpellScoreDetail("Checking {} [{}] as a Haste --- effect value of {}.", spell.name, spell.id, value); //deleteme
			spell_score += (value - 100) * VegasSpellTypeMultipliers::Haste;
		}
	}

	return spell_score;
}

// TODO Test
VeqSpellScoreRepository::VeqSpellScore UpdateFullScore(Mob* caster, const SPDat_Spell_Struct spell, uint8 spell_level, bool bulk_update) {
	auto e = VeqSpellScoreRepository::NewEntity();

	if (!IsValidSpell(spell.id)) {
		if (bulk_update) {
			return e;
		}
		else {
			VeqSpellScoreRepository::ReplaceOne(database, e);
		}
	}

	int spell_id = spell.id;
	e.id = spell_id;

	if (spell_level == UINT8_MAX) {
		spell_level = GetSpellMinimumLevel(spell.id);

		if (spell_level > RuleI(Character, MaxLevel)) {
			spell_level = RuleI(NPC, MaxNPCLevel); // NPC only spell
		}
		else {
			spell_level = RuleI(Character, MaxLevel);
		}
	}

	e.name = spell.name;
	e.lowest_spell_level = GetSpellMinimumLevel(spell.id);
	e.lowest_class = GetSpellMinimumClass(spell.id);

	uint16 spell_type = GetCorrectBotSpellType(BotSpellTypes::SummonCorpse, spell.id);

	bool use_parent_spell_type =
		!IsPetBotSpellType(spell_type) &&
		!IsAEBotSpellType(spell_type) &&
		!IsCommandedBotSpellType(spell_type) &&
		!IsHealBotSpellType(spell_type);

	if (use_parent_spell_type) {
		spell_type = Bot::GetParentSpellType(spell_type);
	}

	switch (spell_type) {
		case BotSpellTypes::PreCombatBuff:
		case BotSpellTypes::InCombatBuffSong:
		case BotSpellTypes::OutOfCombatBuffSong:
		case BotSpellTypes::PreCombatBuffSong:
			spell_type = BotSpellTypes::Buff;

			break;
		default:
			break;
	}

	e.spell_type = Bot::GetSpellTypeNameByID(spell_type);
	float duration = CalcBuffDuration_formula(spell_level, spell.buff_duration_formula, spell.buff_duration);
	e.duration = duration;
	e.target_type = spell.target_type;

	bool is_self_only = spell.target_type == ST_Self;
	float spell_score = 0.0f;

	for (int i = 0; i < EFFECT_COUNT; i++) {
		if (IsBlankSpellEffect(spell_id, i)) {
			continue;
		}

		int effect_id = spell.effect_id[i];
		//float spell_value = caster->CalcSpellEffectValue(spell_id, effect_id, spell.base_value[effect_id], spell_level);
		float spell_value = caster->CalcSpellEffectValue_formula(spell.formula[effect_id], spell.base_value[effect_id], spell.max_value[effect_id], spell_level, spell_id);
		/*
		 * TODO:
		 *  - Fix Lich's from scoring DoTs, adjust MoT to be appropriate like mana regen
		 */
		switch (effect_id) {
			case SE_CurrentHP:
			case SE_CurrentHPOnce: { // DDs, Instant Heals, DoTs, HoTs, Lifetaps
				bool is_instant_spell = spell.base_value[i] != 0 && spell.buff_duration == 0 && spell.aoe_duration == 0;

				if (effect_id == SE_CurrentHPOnce || is_instant_spell) {
					LogSpellScoreDetail("Checking {} [{}] as a {} --- effect value of {}.", spell.name, spell_id, (IsLifetapSpell(spell_id) ? "LifeTap" : spell.base_value[i] < 0 ? "DD" : "Instant Heal"), spell_value); //deleteme

					if (spell.base_value[i] < 0) { // DD/Lifetap
						if (IsLifetapSpell(spell_id)) { // Lifetap
							spell_value = IsBeneficialSpell(spell_id) ? spell_value : abs(spell_value);
							spell_value *= VegasSpellTypeMultipliers::Lifetap;
							spell_value = IsTargetTypeOnlySpell(spell_id) ? spell_value * VegasSpellTypeMultipliers::TargetTypeOnly : spell_value;
							spell_score += spell_value;
							e.lifetap_score += spell_value;
						}
						else { // DD
							spell_value = IsBeneficialSpell(spell_id) ? spell_value : abs(spell_value);
							spell_value *= VegasSpellTypeMultipliers::DirectDamage;
							spell_value = IsTargetTypeOnlySpell(spell_id) ? spell_value * VegasSpellTypeMultipliers::TargetTypeOnly : spell_value;
							spell_score += spell_value;
							e.dd_score += spell_value;
						}
					} else { // Instant Heal
						spell_value *= VegasSpellTypeMultipliers::InstantHeal;
						spell_value = IsTargetTypeOnlySpell(spell_id) ? spell_value * VegasSpellTypeMultipliers::TargetTypeOnly : spell_value;
						spell_score += spell_value;
						e.heal_score += spell_value;
					}
				}
				else {
					bool is_duration_spell = spell.base_value[i] != 0 && spell.buff_duration > 0;

					if (is_duration_spell) {
						duration = std::fmax(1.0f, std::fmin(duration, 50.0f)); // Only score up to 5 minutes.

						for (int x = 0; x < duration; x++) {
							spell_value = caster->CalcSpellEffectValue_formula(spell.formula[i], spell.base_value[i], spell.max_value[i], spell_level, spell_id, x);
							LogSpellScoreDetail("Checking {} [{}] as a {} --- effect value of {} with {} ticks remaining [{} total].", spell.name, spell_id, (spell.base_value[i] < 0 ? "DoT" : "HoT"), spell_value, duration - x, Strings::SecondsToTime(duration * 6)); //deleteme

							if (spell.base_value[i] < 0) { // DoT
								spell_value = IsBeneficialSpell(spell_id) ? spell_value : abs(spell_value);
								spell_value *= VegasSpellTypeMultipliers::DamageOverTime;
								spell_value = IsTargetTypeOnlySpell(spell_id) ? spell_value * VegasSpellTypeMultipliers::TargetTypeOnly : spell_value;
								spell_score += spell_value;
								e.dot_score += spell_value;
							} else { // HoT
								if (spell.formula[i] == 122) { //Splurt
									spell_value = IsBeneficialSpell(spell_id) ? spell_value : abs(spell_value);
								}

								spell_value *= VegasSpellTypeMultipliers::HealOverTime;
								spell_value = IsTargetTypeOnlySpell(spell_id) ? spell_value * VegasSpellTypeMultipliers::TargetTypeOnly : spell_value;
								spell_score += spell_value;
								e.hot_score += spell_value;
							}
						}
					}
				}

				break;
			}
			case SE_ArmorClass: {
					duration = std::fmax(1.0f, std::fmin(duration, 50.0f)); // Only score up to 5 minutes.
					LogSpellScoreDetail("Checking {} [{}] as a {} --- effect value of {} for {} ticks [{}].", spell.name, spell_id, (spell.base_value[i] < 0 ? "AC Debuff" : "AC Buff"), spell_value, duration, Strings::SecondsToTime(duration * 6)); //deleteme

					if (spell.base_value[i] < 0) { // Debuff
						spell_value = IsBeneficialSpell(spell_id) ? spell_value : abs(spell_value);
						spell_value += (spell_value * (duration / 100));
						spell_value *= VegasSpellTypeMultipliers::ACDebuff;
						spell_score += spell_value;
						e.ac_debuff_score += spell_value;
					} else { // Buff
						spell_value += (spell_value * ((duration / 4) / 100));
						spell_value *= VegasSpellTypeMultipliers::ACBuff;
						spell_score += spell_value;
						e.ac_buff_score += spell_value;
					}
				}

				break;
			case SE_ATK: {
					duration = std::fmax(1.0f, std::fmin(duration, 50.0f)); // Only score up to 5 minutes.
					LogSpellScoreDetail("Checking {} [{}] as a {} --- effect value of {} for {} ticks [{}].", spell.name, spell_id, (spell.base_value[i] < 0 ? "ATK Debuff" : "ATK Buff"), spell_value, duration, Strings::SecondsToTime(duration * 6)); //deleteme

					if (spell.base_value[i] < 0) { // Debuff
						spell_value = IsBeneficialSpell(spell_id) ? spell_value : abs(spell_value);
						spell_value += (spell_value * (duration / 100));
						spell_value *= VegasSpellTypeMultipliers::ATKDebuff;
						spell_score += spell_value;
						e.atk_debuff_score += spell_value;
					} else { // Buff
						spell_value += (spell_value * ((duration / 4) / 100));
						spell_value *= VegasSpellTypeMultipliers::ATKBuff;
						spell_score += spell_value;
						e.atk_buff_score += spell_value;
					}
				}

				break;
			case SE_MovementSpeed: {
				if (spell.base_value[i] < -500) {
					continue;
				}

				duration = std::fmax(1.0f, std::fmin(duration, 50.0f)); // Only score up to 5 minutes.
				LogSpellScoreDetail("Checking {} [{}] as a {} --- effect value of {} for {} ticks [{}].", spell.name, spell_id, (spell.base_value[i] < 0 ? "Snare" : "Movement Speed"), spell_value, duration, Strings::SecondsToTime(duration * 6)); //deleteme

				if (spell.base_value[i] < 0) { // Snare
					spell_value = IsBeneficialSpell(spell_id) ? spell_value : abs(spell_value);
					spell_value += (spell_value * (duration / 100));
					spell_value *= VegasSpellTypeMultipliers::Snare;
					spell_score += spell_value;
					e.snare_score += spell_value;
				} else { // Movement Speed
					spell_value += (spell_value * ((duration / 4) / 100));
					spell_value *= VegasSpellTypeMultipliers::MovementSpeed;
					spell_score += spell_value;
					e.movement_speed_score += spell_value;
				}

				break;
			}
			case SE_STR:
			case SE_DEX:
			case SE_AGI:
			case SE_STA:
			case SE_INT:
			case SE_WIS:
			case SE_CHA: {
				duration = std::fmax(1.0f, std::fmin(duration, 50.0f)); // Only score up to 5 minutes.
				LogSpellScoreDetail("Checking {} [{}] as a {} --- effect value of {} for {} ticks [{}].", spell.name, spell_id, (spell.base_value[i] < 0 ? "Base Stat Debuff" : "Base Stat Buff"), spell_value, duration, Strings::SecondsToTime(duration * 6)); //deleteme

				if (spell.base_value[i] < 0) { // Debuff
					spell_value = IsBeneficialSpell(spell_id) ? spell_value : abs(spell_value);
					spell_value += (spell_value * (duration / 100));
					spell_value *= VegasSpellTypeMultipliers::BaseStatDebuff;
					spell_score += spell_value;
					e.base_stat_debuff_score += spell_value;
				} else { // Buff
					spell_value += (spell_value * ((duration / 4) / 100));
					spell_value *= VegasSpellTypeMultipliers::BaseStatBuff;
					spell_score += spell_value;
					e.base_stat_score += spell_value;
				}

				break;
			}
			case SE_AttackSpeed: {
				spell_value = caster->CalcSpellEffectValue(spell_id, effect_id, spell.base_value[effect_id], spell_level);
				duration = std::fmax(1.0f, std::fmin(duration, 50.0f)); // Only score up to 5 minutes.
				LogSpellScoreDetail("Checking {} [{}] as a {} --- effect value of {} for {} ticks [{}].", spell.name, spell_id, (spell.base_value[i] < 100 ? "Slow" : "Haste"), (spell_value - 100), duration, Strings::SecondsToTime(duration * 6)); //deleteme

				if (spell.base_value[i] < 100) { // Slow
					spell_value = IsBeneficialSpell(spell_id) ? (spell_value - 100) : abs(spell_value - 100);
					spell_value += (spell_value * (duration / 100));
					spell_value *= VegasSpellTypeMultipliers::Slow;
					spell_value = IsTargetTypeOnlySpell(spell_id) ? spell_value * VegasSpellTypeMultipliers::TargetTypeOnly : spell_value;
					spell_score += spell_value;
					e.slow_score += spell_value;
				} else { // Haste
					spell_value = spell_value - 100;
					spell_value += (spell_value * ((duration / 4) / 100));
					spell_value *= VegasSpellTypeMultipliers::Haste;
					spell_value = IsTargetTypeOnlySpell(spell_id) ? spell_value * VegasSpellTypeMultipliers::TargetTypeOnly : spell_value;
					spell_score += spell_value;
					e.haste_score += spell_value;
				}

				break;
			}
			case SE_Invisibility: {
				duration = std::fmax(1.0f, std::fmin(duration, 50.0f)); // Only score up to 5 minutes.
				LogSpellScoreDetail("Checking {} [{}] as an {} --- effect value of {} for {} ticks [{}].", spell.name, spell_id, "Invis", spell_value, duration, Strings::SecondsToTime(duration * 6)); //deleteme
				spell_value += (spell_value * (duration / 100));
				spell_value *= VegasSpellTypeMultipliers::Invisibility;
				spell_score += spell_value;
				e.invis_score += spell_value;

				break;
			}
			case SE_SeeInvis: {
				duration = std::fmax(1.0f, std::fmin(duration, 50.0f)); // Only score up to 5 minutes.
				LogSpellScoreDetail("Checking {} [{}] as an {} --- effect value of {} for {} ticks [{}].", spell.name, spell_id, "See Invis", spell_value, duration, Strings::SecondsToTime(duration * 6)); //deleteme
				spell_value += (spell_value * (duration / 100));
				spell_value *= VegasSpellTypeMultipliers::SeeInvis;
				spell_score += spell_value;
				e.see_invis_score += spell_value;

				break;
			}
			case SE_WaterBreathing: {
				duration = std::fmax(1.0f, std::fmin(duration, 50.0f)); // Only score up to 5 minutes.
				LogSpellScoreDetail("Checking {} [{}] as an {} --- effect value of {} for {} ticks [{}].", spell.name, spell_id, "Water Breathing", spell_value, duration, Strings::SecondsToTime(duration * 6)); //deleteme
				spell_value += (spell_value * (duration / 100));
				spell_value *= VegasSpellTypeMultipliers::WaterBreathing;
				spell_score += spell_value;
				e.water_breathing_score += spell_value;

				break;
			}
			case SE_CurrentMana:
			case SE_CurrentManaOnce: {
				bool is_instant_spell = spell.base_value[i] != 0 && spell.buff_duration == 0 && spell.aoe_duration == 0;

				if (effect_id == SE_CurrentHPOnce || is_instant_spell) {
					LogSpellScoreDetail("Checking {} [{}] as a {} --- effect value of {}.", spell.name, spell_id, (IsManaTapSpell(spell_id) ? "ManaTap" : spell.base_value[i] < 0 ? "Mana DD" : "Instant Mana"), spell_value); //deleteme

					if (spell.base_value[i] < 0) { // Mana DD/ManaTap
						if (IsManaTapSpell(spell_id)) { // ManaTap
							spell_value = IsBeneficialSpell(spell_id) ? spell_value : abs(spell_value);
							spell_value *= VegasSpellTypeMultipliers::Manatap;
							spell_value = IsTargetTypeOnlySpell(spell_id) ? spell_value * VegasSpellTypeMultipliers::TargetTypeOnly : spell_value;
							spell_score += spell_value;
							e.manatap_score += spell_value;
						}
						else { // Mana DD
							spell_value = IsBeneficialSpell(spell_id) ? spell_value : abs(spell_value);
							spell_value *= VegasSpellTypeMultipliers::DirectMana;
							spell_value = IsTargetTypeOnlySpell(spell_id) ? spell_value * VegasSpellTypeMultipliers::TargetTypeOnly : spell_value;
							spell_score += spell_value;
							e.mana_dd_score += spell_value;
						}
					} else { // Instant Mana
						spell_value *= VegasSpellTypeMultipliers::InstantMana;
						spell_value = IsTargetTypeOnlySpell(spell_id) ? spell_value * VegasSpellTypeMultipliers::TargetTypeOnly : spell_value;
						spell_score += spell_value;
						e.mana_score += spell_value;
					}
				}
				else {
					bool is_duration_spell = spell.base_value[i] != 0 && spell.buff_duration > 0;

					if (is_duration_spell) {
						duration = std::fmax(1.0f, std::fmin(duration, 50.0f)); // Only score up to 5 minutes.

						for (int x = 0; x < duration; x++) {
							spell_value = caster->CalcSpellEffectValue_formula(spell.formula[i], spell.base_value[i], spell.max_value[i], spell_level, spell_id, x);
							LogSpellScoreDetail("Checking {} [{}] as a {} --- effect value of {} with {} ticks remaining [{} total].", spell.name, spell_id, (spell.base_value[i] < 0 ? "ManaDoT" : "MoT"), spell_value, duration - x, Strings::SecondsToTime(duration * 6)); //deleteme

							if (spell.base_value[i] < 0) { // MDoT
								spell_value = IsBeneficialSpell(spell_id) ? spell_value : abs(spell_value);
								spell_value *= VegasSpellTypeMultipliers::ManaDamageOverTime;
								spell_value = IsTargetTypeOnlySpell(spell_id) ? spell_value * VegasSpellTypeMultipliers::TargetTypeOnly : spell_value;
								spell_score += spell_value;
								e.m_dot_score += spell_value;
							} else { // MoT
								if (spell.formula[i] == 122) { //Splurt
									spell_value = IsBeneficialSpell(spell_id) ? spell_value : abs(spell_value);
								}

								spell_value *= VegasSpellTypeMultipliers::ManaOverTime;
								spell_value = IsTargetTypeOnlySpell(spell_id) ? spell_value * VegasSpellTypeMultipliers::TargetTypeOnly : spell_value;
								spell_score += spell_value;
								e.mot_score += spell_value;
							}
						}
					}
				}

				break;
			}
			//case SE_NPCFrenzy:
			//case SE_NPCAwareness:
			//case SE_ChangeFrenzyRad:
			//case SE_Harmony:
			case SE_Lull: {
				spell_value = 0.0f;
				float level_cap_multiplier = 1.0f;
				auto frenzy_effect_index = GetSpellEffectIndex(spell_id, SE_ChangeFrenzyRad);
				auto harmony_effect_index = GetSpellEffectIndex(spell_id, SE_Harmony);

				if (frenzy_effect_index > 0) {
					level_cap_multiplier += spell.max_value[frenzy_effect_index];
					spell_value += spell.base_value[frenzy_effect_index];
					LogSpellScoreDetail("{} [{}] has a frenzy effect value of {} with a level_cap_multiplier of {}.", spell.name, spell_id, spell.base_value[frenzy_effect_index], level_cap_multiplier); //deleteme
				}

				if (harmony_effect_index > 0) {
					level_cap_multiplier += spell.max_value[harmony_effect_index];
					spell_value += spell.base_value[frenzy_effect_index];
					LogSpellScoreDetail("{} [{}] has a harmony effect value of {} with a level_cap_multiplier of {}.", spell.name, spell_id, spell.base_value[frenzy_effect_index], level_cap_multiplier); //deleteme
				}

				if (frenzy_effect_index > 0 && harmony_effect_index > 0) {
					level_cap_multiplier = (level_cap_multiplier / 2.0f) / RuleI(Character, MaxLevel);
					spell_value = spell_value / 2.0f;
				}
				else {
					level_cap_multiplier = level_cap_multiplier / RuleI(Character, MaxLevel);
				}

				duration = std::fmax(1.0f, std::fmin(duration, 50.0f)); // Only score up to 5 minutes.
				spell_value = 1.0f / spell_value;
				spell_value += (spell_value * (duration / 100));
				spell_value *= VegasSpellTypeMultipliers::Pacify;
				spell_value *= level_cap_multiplier;
				spell_value = IsTargetTypeOnlySpell(spell_id) ? spell_value * VegasSpellTypeMultipliers::TargetTypeOnly : spell_value;
				spell_score += spell_value;
				e.pacify_score += spell_value;
				LogSpellScoreDetail("Checking {} [{}] as a {} --- effect value of {} for {} ticks [{}] with a level_cap_multiplier of {}.", spell.name, spell_id, "Pacify", spell_value, duration, Strings::SecondsToTime(duration * 6), level_cap_multiplier); //deleteme

				break;
			}
			case SE_AddFaction: {
				LogSpellScoreDetail("Checking {} [{}] as an {} --- effect value of {}.", spell.name, spell_id, "Add Faction", spell_value); //deleteme
				spell_value *= VegasSpellTypeMultipliers::AddFaction;
				spell_value = IsTargetTypeOnlySpell(spell_id) ? spell_value * VegasSpellTypeMultipliers::TargetTypeOnly : spell_value;
				spell_score += spell_value;
				e.add_faction_score += spell_value;

				break;
			}
			case SE_Blind: {
				bool is_cure = (spell.good_effect && spell.base_value[i] > 0);
				spell_value = is_cure ? VegasSpellTypeMultipliers::CureBlind : VegasSpellTypeMultipliers::Blind;

				duration = is_cure ? 1.0f : std::fmax(1.0f, std::fmin(duration, 50.0f)); // Only score up to 5 minutes.
				LogSpellScoreDetail("Checking {} [{}] as an {} --- effect value of {} for {} ticks [{}].", spell.name, spell_id, "Blind", spell_value, duration, Strings::SecondsToTime(duration * 6)); //deleteme
				spell_value *= duration;
				spell_value = IsTargetTypeOnlySpell(spell_id) ? spell_value * VegasSpellTypeMultipliers::TargetTypeOnly : spell_value;
				spell_score += spell_value;
				e.blind_score += spell_value;

				break;
			}
			case SE_Stun: {
				spell_value = VegasSpellTypeMultipliers::StunBase;
				duration = std::fmin(30.0f, spell.base_value[i] / 1000.0f); // Only score up to 30 seconds
				auto max_level = spell.max_value[i] == 0 ? RuleI(NPC, MaxNPCLevel) : spell.max_value[i];
				float level_cap_multiplier = max_level / RuleI(NPC, MaxNPCLevel);
				LogSpellScoreDetail("Checking {} [{}] as a {} --- effect value of {} for {} seconds up to level {} and level_cap_multiplier of {}.", spell.name, spell_id, "Stun", spell_value, duration, spell.max_value[i], 1.0f + level_cap_multiplier); //deleteme
				spell_value += (VegasSpellTypeMultipliers::StunMultiplier * duration);
				spell_value = IsTargetTypeOnlySpell(spell_id) ? spell_value * VegasSpellTypeMultipliers::TargetTypeOnly : spell_value;
				spell_value *= level_cap_multiplier;
				spell_score += spell_value;
				e.stun_score += spell_value;

				break;
			}
			case SE_Charm: {
				spell_value = VegasSpellTypeMultipliers::Charm;

				duration = std::fmax(1.0f, std::fmin(duration, 50.0f)); // Only score up to 5 minutes.
				auto max_level = spell.max_value[i] == 0 ? RuleI(NPC, MaxNPCLevel) : spell.max_value[i];
				float level_cap_multiplier = max_level / RuleI(NPC, MaxNPCLevel);
				LogSpellScoreDetail("Checking {} [{}] as a {} --- effect value of {} for {} seconds up to level {} and level_cap_multiplier of {}.", spell.name, spell_id, "Charm", spell_value, duration, spell.max_value[i], 1.0f + level_cap_multiplier); //deleteme
				spell_value += (VegasSpellTypeMultipliers::Charm * duration);
				spell_value = IsTargetTypeOnlySpell(spell_id) ? spell_value * VegasSpellTypeMultipliers::TargetTypeOnly : spell_value;
				spell_value *= level_cap_multiplier;
				spell_score += spell_value;
				e.charm_score += spell_value;

				break;
			}
			case SE_Fear: {
				spell_value = VegasSpellTypeMultipliers::Fear;

				duration = std::fmax(1.0f, std::fmin(duration, 50.0f)); // Only score up to 5 minutes.
				auto max_level = spell.max_value[i] == 0 ? RuleI(NPC, MaxNPCLevel) : spell.max_value[i];
				float level_cap_multiplier = max_level / RuleI(NPC, MaxNPCLevel);
				LogSpellScoreDetail("Checking {} [{}] as a {} --- effect value of {} for {} seconds up to level {} and level_cap_multiplier of {}.", spell.name, spell_id, "Fear", spell_value, duration, spell.max_value[i], 1.0f + level_cap_multiplier); //deleteme
				spell_value += (VegasSpellTypeMultipliers::Fear * duration);
				spell_value = IsTargetTypeOnlySpell(spell_id) ? spell_value * VegasSpellTypeMultipliers::TargetTypeOnly : spell_value;
				spell_value *= level_cap_multiplier;
				spell_score += spell_value;
				e.fear_score += spell_value;

				break;
			}
			case SE_Stamina: {
				LogSpellScoreDetail("Checking {} [{}] as a {} --- effect value of {}.", spell.name, spell_id, "Stamina", spell_value); //deleteme
				spell_score += spell_value;
				e.stamina_score += spell_value;
				break;

				bool is_instant_spell = spell.base_value[i] != 0 && spell.buff_duration == 0 && spell.aoe_duration == 0;

				if (effect_id == SE_CurrentHPOnce || is_instant_spell) {
					LogSpellScoreDetail("Checking {} [{}] as a {} --- effect value of {}.", spell.name, spell_id, (IsStaminaTapSpell(spell_id) ? "StaminaTap" : spell.base_value[i] < 0 ? "Stamina DD" : "Instant Stamina"), spell_value); //deleteme

					if (spell.base_value[i] < 0) { // Stamina DD/StaminaTap
						if (IsStaminaTapSpell(spell_id)) { // StaminaTap
							spell_value = IsBeneficialSpell(spell_id) ? spell_value : abs(spell_value);
							spell_value *= VegasSpellTypeMultipliers::Staminatap;
							spell_value = IsTargetTypeOnlySpell(spell_id) ? spell_value * VegasSpellTypeMultipliers::TargetTypeOnly : spell_value;
							spell_score += spell_value;
							e.staminatap_score += spell_value;
						}
						else { // Stamina DD
							spell_value = IsBeneficialSpell(spell_id) ? spell_value : abs(spell_value);
							spell_value *= VegasSpellTypeMultipliers::DirectStamina;
							spell_value = IsTargetTypeOnlySpell(spell_id) ? spell_value * VegasSpellTypeMultipliers::TargetTypeOnly : spell_value;
							spell_score += spell_value;
							e.stamina_dd_score += spell_value;
						}
					} else { // Instant Stamina
						spell_value *= VegasSpellTypeMultipliers::InstantStamina;
						spell_value = IsTargetTypeOnlySpell(spell_id) ? spell_value * VegasSpellTypeMultipliers::TargetTypeOnly : spell_value;
						spell_score += spell_value;
						e.stamina_score += spell_value;
					}
				}
				else {
					bool is_duration_spell = spell.base_value[i] != 0 && spell.buff_duration > 0;

					if (is_duration_spell) {
						duration = std::fmax(1.0f, std::fmin(duration, 50.0f)); // Only score up to 5 minutes.

						for (int x = 0; x < duration; x++) {
							spell_value = caster->CalcSpellEffectValue_formula(spell.formula[i], spell.base_value[i], spell.max_value[i], spell_level, spell_id, x);
							LogSpellScoreDetail("Checking {} [{}] as a {} --- effect value of {} with {} ticks remaining [{} total].", spell.name, spell_id, (spell.base_value[i] < 0 ? "StaminaDoT" : "SoT"), spell_value, duration - x, Strings::SecondsToTime(duration * 6)); //deleteme

							if (spell.base_value[i] < 0) { // MDoT
								spell_value = IsBeneficialSpell(spell_id) ? spell_value : abs(spell_value);
								spell_value *= VegasSpellTypeMultipliers::StaminaDamageOverTime;
								spell_value = IsTargetTypeOnlySpell(spell_id) ? spell_value * VegasSpellTypeMultipliers::TargetTypeOnly : spell_value;
								spell_score += spell_value;
								e.s_dot_score += spell_value;
							} else { // MoT
								if (spell.formula[i] == 122) { //Splurt
									spell_value = IsBeneficialSpell(spell_id) ? spell_value : abs(spell_value);
								}

								spell_value *= VegasSpellTypeMultipliers::StaminaOverTime;
								spell_value = IsTargetTypeOnlySpell(spell_id) ? spell_value * VegasSpellTypeMultipliers::TargetTypeOnly : spell_value;
								spell_score += spell_value;
								e.sot_score += spell_value;
							}
						}
					}
				}

				break;
			}
			case SE_BindAffinity:
				spell_score += VegasSpellTypeMultipliers::BindAffinity;
				e.bind_affinity_score += VegasSpellTypeMultipliers::BindAffinity;
				break;
			case SE_Gate:
				spell_score += VegasSpellTypeMultipliers::Gate;
				e.gate_score += VegasSpellTypeMultipliers::Gate;
				break;
			case SE_CancelMagic:
			case SE_InvisVsUndead:
			case SE_InvisVsAnimals:
			case SE_Mez:
			case SE_SummonItem:
			case SE_SummonPet:
//case SE_Confuse:
			case SE_DiseaseCounter:
			case SE_PoisonCounter:
//case SE_DetectHostile:
//case SE_DetectMagic:
			case SE_TwinCastBlocker:
			case SE_DivineAura:
			case SE_Destroy:
			case SE_ShadowStep:
			case SE_Berserk:
			case SE_Lycanthropy:
			case SE_Vampirism:
			case SE_ResistFire:
			case SE_ResistCold:
			case SE_ResistPoison:
			case SE_ResistDisease:
			case SE_ResistMagic:
//case SE_DetectTraps:
			case SE_SenseDead:
			case SE_SenseSummoned:
			case SE_SenseAnimals:
			case SE_Rune:
			case SE_TrueNorth:
			case SE_Levitate:
			case SE_Illusion:
			case SE_DamageShield:
//case SE_TransferItem:
			case SE_Identify:
//case SE_ItemID:
			case SE_WipeHateList:
			case SE_SpinTarget:
			case SE_InfraVision:
			case SE_UltraVision:
			case SE_EyeOfZomm:
			case SE_ReclaimPet:
			case SE_TotalHP:
//case SE_CorpseBomb:
			case SE_NecPet:
//case SE_PreserveCorpse:
			case SE_BindSight:
			case SE_FeignDeath:
			case SE_VoiceGraft:
			case SE_Sentinel:
			case SE_LocateCorpse:
			case SE_AbsorbMagicAtt:
//case SE_EnchantLight:
			case SE_Revive:
			case SE_SummonPC:
			case SE_Teleport:
			case SE_TossUp:
			case SE_WeaponProc:
			case SE_MagnifyVision:
			case SE_Succor:
			case SE_ModelSize:
//case SE_Cloak:
			case SE_SummonCorpse:
			case SE_InstantHate:
			case SE_StopRain:
			case SE_NegateIfCombat:
			case SE_Sacrifice:
			case SE_Silence:
			case SE_ManaPool:
			case SE_AttackSpeed2:
			case SE_Root:
			case SE_HealOverTime:
			case SE_CompleteHeal:
			case SE_Fearless:
			case SE_CallPet:
			case SE_Translocate:
			case SE_AntiGate:
			case SE_SummonBSTPet:
			case SE_AlterNPCLevel:
			case SE_Familiar:
			case SE_SummonItemIntoBag:
			case SE_IncreaseArchery:
			case SE_ResistAll:
			case SE_CastingLevel:
			case SE_SummonHorse:
			case SE_ChangeAggro:
			case SE_Hunger:
			case SE_CurseCounter:
			case SE_MagicWeapon:
			case SE_Amplification:
			case SE_AttackSpeed3:
			case SE_HealRate:
			case SE_ReverseDS:
			case SE_ReduceSkill:
			case SE_Screech:
			case SE_ImprovedDamage:
			case SE_ImprovedHeal:
			case SE_SpellResistReduction:
			case SE_IncreaseSpellHaste:
			case SE_IncreaseSpellDuration:
			case SE_IncreaseRange:
			case SE_SpellHateMod:
			case SE_ReduceReagentCost:
			case SE_ReduceManaCost:
			case SE_FcStunTimeMod:
			case SE_LimitMaxLevel:
			case SE_LimitResist:
			case SE_LimitTarget:
			case SE_LimitEffect:
			case SE_LimitSpellType:
			case SE_LimitSpell:
			case SE_LimitMinDur:
			case SE_LimitInstant:
			case SE_LimitMinLevel:
			case SE_LimitCastTimeMin:
			case SE_LimitCastTimeMax:
			case SE_Teleport2:
//case SE_ElectricityResist:
			case SE_PercentalHeal:
			case SE_StackingCommand_Block:
			case SE_StackingCommand_Overwrite:
			case SE_DeathSave:
			case SE_SuspendPet:
			case SE_TemporaryPets:
			case SE_BalanceHP:
			case SE_DispelDetrimental:
			case SE_SpellCritDmgIncrease:
			case SE_IllusionCopy:
			case SE_SpellDamageShield:
			case SE_Reflect:
			case SE_AllStats:
			case SE_MakeDrunk:
			case SE_MitigateSpellDamage:
			case SE_MitigateMeleeDamage:
			case SE_NegateAttacks:
			case SE_AppraiseLDonChest:
			case SE_DisarmLDoNTrap:
			case SE_UnlockLDoNChest:
			case SE_PetPowerIncrease:
			case SE_MeleeMitigation:
			case SE_CriticalHitChance:
			case SE_SpellCritChance:
			case SE_CrippBlowChance:
			case SE_AvoidMeleeChance:
			case SE_RiposteChance:
			case SE_DodgeChance:
			case SE_ParryChance:
			case SE_DualWieldChance:
			case SE_DoubleAttackChance:
			case SE_MeleeLifetap:
			case SE_AllInstrumentMod:
			case SE_ResistSpellChance:
			case SE_ResistFearChance:
			case SE_HundredHands:
			case SE_MeleeSkillCheck:
			case SE_HitChance:
			case SE_DamageModifier:
			case SE_MinDamageModifier:
			case SE_BalanceMana:
			case SE_IncreaseBlockChance:
			case SE_CurrentEndurance:
			case SE_EndurancePool:
			case SE_Amnesia:
			case SE_Hate:
			case SE_SkillAttack:
			case SE_FadingMemories:
			case SE_StunResist:
			case SE_StrikeThrough:
			case SE_SkillDamageTaken:
			case SE_CurrentEnduranceOnce:
			case SE_Taunt:
			case SE_ProcChance:
			case SE_RangedProc:
			case SE_IllusionOther:
			case SE_MassGroupBuff:
			case SE_GroupFearImmunity:
			case SE_Rampage:
			case SE_AETaunt:
			case SE_FleshToBone:
//case SE_PurgePoison:
			case SE_DispelBeneficial:
			case SE_PetShield:
			case SE_AEMelee:
			case SE_FrenziedDevastation:
			case SE_PetMaxHP:
			case SE_MaxHPChange:
			case SE_PetAvoidance:
			case SE_Accuracy:
			case SE_HeadShot:
			case SE_PetCriticalHit:
			case SE_SlayUndead:
			case SE_SkillDamageAmount:
			case SE_Packrat:
			case SE_BlockBehind:
			case SE_DoubleRiposte:
			case SE_GiveDoubleRiposte:
			case SE_GiveDoubleAttack:
			case SE_TwoHandBash:
			case SE_ReduceSkillTimer:
			case SE_ReduceFallDamage:
			case SE_PersistantCasting:
			case SE_ExtendedShielding:
			case SE_StunBashChance:
			case SE_DivineSave:
			case SE_Metabolism:
			case SE_ReduceApplyPoisonTime:
			case SE_ChannelChanceSpells:
//case SE_FreePet:
			case SE_GivePetGroupTarget:
			case SE_IllusionPersistence:
			case SE_FeignedCastOnChance:
//case SE_StringUnbreakable:
			case SE_ImprovedReclaimEnergy:
			case SE_IncreaseChanceMemwipe:
			case SE_CharmBreakChance:
			case SE_RootBreakChance:
			case SE_TrapCircumvention:
			case SE_SetBreathLevel:
			case SE_RaiseSkillCap:
			case SE_SecondaryForte:
			case SE_SecondaryDmgInc:
			case SE_SpellProcChance:
			case SE_ConsumeProjectile:
			case SE_FrontalBackstabChance:
			case SE_FrontalBackstabMinDmg:
			case SE_Blank:
			case SE_ShieldDuration:
			case SE_ShroudofStealth:
			case SE_PetDiscipline:
			case SE_TripleBackstab:
			case SE_CombatStability:
			case SE_AddSingingMod:
			case SE_SongModCap:
			case SE_RaiseStatCap:
			case SE_TradeSkillMastery:
			case SE_HastenedAASkill:
			case SE_MasteryofPast:
			case SE_ExtraAttackChance:
			case SE_AddPetCommand:
			case SE_ReduceTradeskillFail:
			case SE_MaxBindWound:
			case SE_BardSongRange:
			case SE_BaseMovementSpeed:
			case SE_CastingLevel2:
			case SE_CriticalDoTChance:
			case SE_CriticalHealChance:
			case SE_CriticalMend:
			case SE_Ambidexterity:
			case SE_UnfailingDivinity:
			case SE_FinishingBlow:
			case SE_Flurry:
			case SE_PetFlurry:
			case SE_FeignedMinion:
			case SE_ImprovedBindWound:
			case SE_DoubleSpecialAttack:
//case SE_LoHSetHeal:
			case SE_NimbleEvasion:
			case SE_FcDamageAmt:
			case SE_SpellDurationIncByTic:
			case SE_SkillAttackProc:
			case SE_CastOnFadeEffect:
			case SE_IncreaseRunSpeedCap:
			case SE_Purify:
			case SE_StrikeThrough2:
			case SE_FrontalStunResist:
			case SE_CriticalSpellChance:
//case SE_ReduceTimerSpecial:
			case SE_FcSpellVulnerability:
			case SE_FcDamageAmtIncoming:
			case SE_ChangeHeight:
			case SE_WakeTheDead:
			case SE_Doppelganger:
			case SE_ArcheryDamageModifier:
			case SE_FcDamagePctCrit:
			case SE_FcDamageAmtCrit:
			case SE_OffhandRiposteFail:
			case SE_MitigateDamageShield:
			case SE_ArmyOfTheDead:
//case SE_Appraisal:
			case SE_ZoneSuspendMinion:
			case SE_GateCastersBindpoint:
			case SE_ReduceReuseTimer:
			case SE_LimitCombatSkills:
			case SE_Sanctuary:
			case SE_ForageAdditionalItems:
			case SE_Invisibility2:
			case SE_InvisVsUndead2:
			case SE_ImprovedInvisAnimals:
			case SE_ItemHPRegenCapIncrease:
			case SE_ItemManaRegenCapIncrease:
			case SE_CriticalHealOverTime:
			case SE_ShieldBlock:
			case SE_ReduceHate:
			case SE_GateToHomeCity:
			case SE_DefensiveProc:
			case SE_HPToMana:
			case SE_NoBreakAESneak:
			case SE_SpellSlotIncrease:
			case SE_MysticalAttune:
			case SE_DelayDeath:
			case SE_ManaAbsorbPercentDamage:
			case SE_CriticalDamageMob:
			case SE_Salvage:
			case SE_SummonToCorpse:
			case SE_CastOnRuneFadeEffect:
			case SE_BardAEDot:
			case SE_BlockNextSpellFocus:
//case SE_IllusionaryTarget:
			case SE_PercentXPIncrease:
			case SE_SummonAndResAllCorpses:
			case SE_TriggerOnCast:
			case SE_SpellTrigger:
			case SE_ItemAttackCapIncrease:
			case SE_ImmuneFleeing:
			case SE_InterruptCasting:
			case SE_ChannelChanceItems:
			case SE_AssassinateLevel:
			case SE_HeadShotLevel:
			case SE_DoubleRangedAttack:
			case SE_LimitManaMin:
			case SE_ShieldEquipDmgMod:
			case SE_ManaBurn:
			case SE_PersistentEffect:
			case SE_IncreaseTrapCount:
			case SE_AdditionalAura:
//case SE_DeactivateAllTraps:
//case SE_LearnTrap:
//case SE_ChangeTriggerType:
			case SE_FcMute:
//case SE_PassiveSenseTrap:
			case SE_ProcOnKillShot:
			case SE_SpellOnDeath:
			case SE_PotionBeltSlots:
			case SE_BandolierSlots:
			case SE_TripleAttackChance:
			case SE_ProcOnSpellKillShot:
//case SE_GroupShielding:
			case SE_SetBodyType:
//case SE_FactionMod:
			case SE_CorruptionCounter:
			case SE_ResistCorruption:
			case SE_AttackSpeed4:
			case SE_ForageSkill:
			case SE_CastOnFadeEffectAlways:
			case SE_ApplyEffect:
			case SE_DotCritDmgIncrease:
//case SE_Fling:
			case SE_CastOnFadeEffectNPC:
			case SE_SpellEffectResistChance:
			case SE_ShadowStepDirectional:
			case SE_Knockdown:
//case SE_KnockTowardCaster:
			case SE_NegateSpellEffect:
			case SE_SympatheticProc:
			case SE_Leap:
			case SE_LimitSpellGroup:
			case SE_CastOnCurer:
			case SE_CastOnCure:
			case SE_SummonCorpseZone:
			case SE_FcTimerRefresh:
			case SE_FcTimerLockout:
			case SE_LimitManaMax:
			case SE_FcHealAmt:
			case SE_FcHealPctIncoming:
			case SE_FcHealAmtIncoming:
			case SE_FcHealPctCritIncoming:
			case SE_FcHealAmtCrit:
			case SE_PetMeleeMitigation:
			case SE_SwarmPetDuration:
			case SE_FcTwincast:
			case SE_HealGroupFromMana:
			case SE_ManaDrainWithDmg:
			case SE_EndDrainWithDmg:
			case SE_LimitSpellClass:
			case SE_LimitSpellSubclass:
			case SE_TwoHandBluntBlock:
			case SE_CastonNumHitFade:
			case SE_CastonFocusEffect:
			case SE_LimitHPPercent:
			case SE_LimitManaPercent:
			case SE_LimitEndPercent:
			case SE_LimitClass:
			case SE_LimitRace:
			case SE_FcBaseEffects:
			case SE_LimitCastingSkill:
			case SE_FFItemClass:
			case SE_ACv2:
			case SE_ManaRegen_v2:
			case SE_SkillDamageAmount2:
			case SE_AddMeleeProc:
			case SE_FcLimitUse:
			case SE_FcIncreaseNumHits:
			case SE_LimitUseMin:
			case SE_LimitUseType:
			case SE_GravityEffect:
			case SE_Display:
			case SE_IncreaseExtTargetWindow:
			case SE_SkillProcAttempt:
			case SE_LimitToSkill:
			case SE_SkillProcSuccess:
//case SE_PostEffect:
//case SE_PostEffectData:
//case SE_ExpandMaxActiveTrophyBen:
			case SE_CriticalDotDecay:
			case SE_CriticalHealDecay:
			case SE_CriticalRegenDecay:
//case SE_BeneficialCountDownHold:
//case SE_TeleporttoAnchor:
//case SE_TranslocatetoAnchor:
			case SE_Assassinate:
			case SE_FinishingBlowLvl:
			case SE_DistanceRemoval:
			case SE_TriggerOnReqTarget:
			case SE_TriggerOnReqCaster:
			case SE_ImprovedTaunt:
//case SE_AddMercSlot:
			case SE_AStacker:
			case SE_BStacker:
			case SE_CStacker:
			case SE_DStacker:
			case SE_MitigateDotDamage:
			case SE_MeleeThresholdGuard:
			case SE_SpellThresholdGuard:
			case SE_TriggerMeleeThreshold:
			case SE_TriggerSpellThreshold:
			case SE_AddHatePct:
			case SE_AddHateOverTimePct:
			case SE_ResourceTap:
			case SE_FactionModPct:
			case SE_DamageModifier2:
			case SE_Ff_Override_NotFocusable:
			case SE_ImprovedDamage2:
			case SE_FcDamageAmt2:
			case SE_Shield_Target:
			case SE_PC_Pet_Rampage:
			case SE_PC_Pet_AE_Rampage:
			case SE_PC_Pet_Flurry_Chance:
			case SE_DS_Mitigation_Amount:
			case SE_DS_Mitigation_Percentage:
			case SE_Chance_Best_in_Spell_Grp:
			case SE_Trigger_Best_in_Spell_Grp:
			case SE_Double_Melee_Round:
			case SE_Buy_AA_Rank:
			case SE_Double_Backstab_Front:
			case SE_Pet_Crit_Melee_Damage_Pct_Owner:
			case SE_Trigger_Spell_Non_Item:
			case SE_Weapon_Stance:
			case SE_Hatelist_To_Top_Index:
			case SE_Hatelist_To_Tail_Index:
			case SE_Ff_Value_Min:
			case SE_Ff_Value_Max:
			case SE_Fc_Cast_Spell_On_Land:
			case SE_Skill_Base_Damage_Mod:
			case SE_Fc_Spell_Damage_Pct_IncomingPC:
			case SE_Fc_Spell_Damage_Amt_IncomingPC:
			case SE_Ff_CasterClass:
			case SE_Ff_Same_Caster:
//case SE_Extend_Tradeskill_Cap:
//case SE_Defender_Melee_Force_Pct_PC:
			case SE_Worn_Endurance_Regen_Cap:
			case SE_Ff_ReuseTimeMin:
			case SE_Ff_ReuseTimeMax:
			case SE_Ff_Endurance_Min:
			case SE_Ff_Endurance_Max:
			case SE_Pet_Add_Atk:
			case SE_Ff_DurationMax:
			case SE_Critical_Melee_Damage_Mod_Max:
//case SE_Ff_FocusCastProcNoBypass:
			case SE_AddExtraAttackPct_1h_Primary:
			case SE_AddExtraAttackPct_1h_Secondary:
			case SE_Fc_CastTimeMod2:
			case SE_Fc_CastTimeAmt:
			case SE_Fearstun:
			case SE_Melee_Damage_Position_Mod:
			case SE_Melee_Damage_Position_Amt:
			case SE_Damage_Taken_Position_Mod:
			case SE_Damage_Taken_Position_Amt:
			case SE_Fc_Amplify_Mod:
			case SE_Fc_Amplify_Amt:
			case SE_Health_Transfer:
			case SE_Fc_ResistIncoming:
			case SE_Ff_FocusTimerMin:
			case SE_Proc_Timer_Modifier :
//case SE_Mana_Max_Percent:
//case SE_Endurance_Max_Percent:
			case SE_AC_Avoidance_Max_Percent:
			case SE_AC_Mitigation_Max_Percent:
//case SE_Attack_Offense_Max_Percent:
			case SE_Attack_Accuracy_Max_Percent:
//case SE_Luck_Amount:
//case SE_Luck_Percent:
			case SE_Endurance_Absorb_Pct_Damage:
			case SE_Instant_Mana_Pct:
			case SE_Instant_Endurance_Pct:
			case SE_Duration_HP_Pct:
			case SE_Duration_Mana_Pct:
			case SE_Duration_Endurance_Pct:
			default:
				break;
		}
	}

	if (spell.recourse_link && IsValidSpell(spell.recourse_link)) {
		auto recourse_spell = spells[spell.recourse_link];
		auto recourse_score = UpdateFullScore(caster, recourse_spell, spell_level, true);
		LogSpellScoreDetail("{} [{}] has a recourse of {} [{}] which is a {} with a spell score of {}. Increasing spell score from {} to {}.", spell.name, spell_id, recourse_spell.name, recourse_spell.id, recourse_score.spell_type, recourse_score.spell_score, spell_score, spell_score + recourse_score.spell_score); //deleteme

		if (!is_self_only) {
			if (IsPBAESpell(recourse_spell.id)) {
				recourse_score.spell_score *= (0.8f + (std::fmin(2.0f, spell.aoe_range / 100.0f)) * VegasSpellTypeMultipliers::PBAE);
			} else if (IsAnyAESpell(recourse_spell.id)) {
				recourse_score.spell_score *= (0.5f + (std::fmin(1.5f, spell.aoe_range / 100.0f)) * VegasSpellTypeMultipliers::AE);
			} else if (IsGroupSpell(recourse_spell.id)) {
				recourse_score.spell_score *= (std::fmin(1.0f, std::fmax(2.25f, spell.aoe_range / 100.0f)) * VegasSpellTypeMultipliers::Group);
			}
		}

		spell_score += recourse_score.spell_score;
		e.recourse_score = recourse_score.spell_score;
	}

	if (!is_self_only) {
		if (IsPBAESpell(spell_id)) {
			float multiplier = (0.8f + (std::fmax(2.0f, spell.aoe_range / 100.0f)) * VegasSpellTypeMultipliers::PBAE);

			spell_score *= multiplier;
			e.pbae_multiplier = multiplier;
		} else if (IsAnyAESpell(spell_id)) {
			float multiplier = (0.5f + (std::fmax(1.5f, spell.aoe_range / 100.0f)) * VegasSpellTypeMultipliers::AE);

			spell_score *= multiplier;
			e.ae_multiplier = multiplier;
		} else if (IsGroupSpell(spell_id)) {
			float multiplier = (std::fmin(1.0f, std::fmax(2.25f, spell.aoe_range / 100.0f)) *VegasSpellTypeMultipliers::Group);

			spell_score *= multiplier;
			e.group_multiplier = multiplier;
		}
	}

	// spell_score *= (4000 / spell.cast_time); // < 4 second = bonus, > 4 second = penalty
	// spell_score *= 1 + -(spell.recovery_time / 2000); // penalty the longer the recovery time is
	// spell_score *= (4000 / spell.recast_time); // < 4 second = bonus, > 4 second = penalty
	// add resist factors +/- or unresistable

	e.spell_score = spell_score;

	if (bulk_update) {
		return e;
	}
	else {
		VeqSpellScoreRepository::ReplaceOne(database, e);
	}
}
