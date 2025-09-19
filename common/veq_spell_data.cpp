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
			spell_level = RuleI(VegasNPCs, MaxNPCLevel); // NPC only spell
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
				effect_id == SpellEffect::CurrentHPOnce ||
				(effect_id == SpellEffect::CurrentHP && spell.buff_duration == 0 && spell.aoe_duration == 0)
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
			effect_id == SpellEffect::CurrentHP &&
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
			effect_id == SpellEffect::AttackSpeed &&
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
			effect_id == SpellEffect::CurrentHP &&
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
				effect_id == SpellEffect::CurrentHPOnce ||
				(effect_id == SpellEffect::CurrentHP && spell.buff_duration == 0 && spell.aoe_duration == 0)
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
								  effect_id == SpellEffect::CurrentHPOnce ||
								  (effect_id == SpellEffect::CurrentHP && spell.buff_duration == 0 && spell.aoe_duration == 0)
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
			effect_id == SpellEffect::HealOverTime &&
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
			effect_id == SpellEffect::AttackSpeed &&
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
			spell_level = RuleI(VegasNPCs, MaxNPCLevel); // NPC only spell
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
			case SpellEffect::CurrentHP:
			case SpellEffect::CurrentHPOnce: { // DDs, Instant Heals, DoTs, HoTs, Lifetaps
				bool is_instant_spell = spell.base_value[i] != 0 && spell.buff_duration == 0 && spell.aoe_duration == 0;

				if (effect_id == SpellEffect::CurrentHPOnce || is_instant_spell) {
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
			case SpellEffect::ArmorClass: {
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
			case SpellEffect::ATK: {
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
			case SpellEffect::MovementSpeed: {
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
			case SpellEffect::STR:
			case SpellEffect::DEX:
			case SpellEffect::AGI:
			case SpellEffect::STA:
			case SpellEffect::INT:
			case SpellEffect::WIS:
			case SpellEffect::CHA: {
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
			case SpellEffect::AttackSpeed: {
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
			case SpellEffect::Invisibility: {
				duration = std::fmax(1.0f, std::fmin(duration, 50.0f)); // Only score up to 5 minutes.
				LogSpellScoreDetail("Checking {} [{}] as an {} --- effect value of {} for {} ticks [{}].", spell.name, spell_id, "Invis", spell_value, duration, Strings::SecondsToTime(duration * 6)); //deleteme
				spell_value += (spell_value * (duration / 100));
				spell_value *= VegasSpellTypeMultipliers::Invisibility;
				spell_score += spell_value;
				e.invis_score += spell_value;

				break;
			}
			case SpellEffect::SeeInvis: {
				duration = std::fmax(1.0f, std::fmin(duration, 50.0f)); // Only score up to 5 minutes.
				LogSpellScoreDetail("Checking {} [{}] as an {} --- effect value of {} for {} ticks [{}].", spell.name, spell_id, "See Invis", spell_value, duration, Strings::SecondsToTime(duration * 6)); //deleteme
				spell_value += (spell_value * (duration / 100));
				spell_value *= VegasSpellTypeMultipliers::SeeInvis;
				spell_score += spell_value;
				e.see_invis_score += spell_value;

				break;
			}
			case SpellEffect::WaterBreathing: {
				duration = std::fmax(1.0f, std::fmin(duration, 50.0f)); // Only score up to 5 minutes.
				LogSpellScoreDetail("Checking {} [{}] as an {} --- effect value of {} for {} ticks [{}].", spell.name, spell_id, "Water Breathing", spell_value, duration, Strings::SecondsToTime(duration * 6)); //deleteme
				spell_value += (spell_value * (duration / 100));
				spell_value *= VegasSpellTypeMultipliers::WaterBreathing;
				spell_score += spell_value;
				e.water_breathing_score += spell_value;

				break;
			}
			case SpellEffect::CurrentMana:
			case SpellEffect::CurrentManaOnce: {
				bool is_instant_spell = spell.base_value[i] != 0 && spell.buff_duration == 0 && spell.aoe_duration == 0;

				if (effect_id == SpellEffect::CurrentHPOnce || is_instant_spell) {
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
			//case SpellEffect::NPCFrenzy:
			//case SpellEffect::NPCAwareness:
			//case SpellEffect::ChangeFrenzyRad:
			//case SpellEffect::Harmony:
			case SpellEffect::Lull: {
				spell_value = 0.0f;
				float level_cap_multiplier = 1.0f;
				auto frenzy_effect_index = GetSpellEffectIndex(spell_id, SpellEffect::ChangeFrenzyRad);
				auto harmony_effect_index = GetSpellEffectIndex(spell_id, SpellEffect::Harmony);

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
			case SpellEffect::AddFaction: {
				LogSpellScoreDetail("Checking {} [{}] as an {} --- effect value of {}.", spell.name, spell_id, "Add Faction", spell_value); //deleteme
				spell_value *= VegasSpellTypeMultipliers::AddFaction;
				spell_value = IsTargetTypeOnlySpell(spell_id) ? spell_value * VegasSpellTypeMultipliers::TargetTypeOnly : spell_value;
				spell_score += spell_value;
				e.add_faction_score += spell_value;

				break;
			}
			case SpellEffect::Blind: {
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
			case SpellEffect::Stun: {
				spell_value = VegasSpellTypeMultipliers::StunBase;
				duration = std::fmin(30.0f, spell.base_value[i] / 1000.0f); // Only score up to 30 seconds
				auto max_level = spell.max_value[i] == 0 ? RuleI(VegasNPCs, MaxNPCLevel) : spell.max_value[i];
				float level_cap_multiplier = max_level / RuleI(VegasNPCs, MaxNPCLevel);
				LogSpellScoreDetail("Checking {} [{}] as a {} --- effect value of {} for {} seconds up to level {} and level_cap_multiplier of {}.", spell.name, spell_id, "Stun", spell_value, duration, spell.max_value[i], 1.0f + level_cap_multiplier); //deleteme
				spell_value += (VegasSpellTypeMultipliers::StunMultiplier * duration);
				spell_value = IsTargetTypeOnlySpell(spell_id) ? spell_value * VegasSpellTypeMultipliers::TargetTypeOnly : spell_value;
				spell_value *= level_cap_multiplier;
				spell_score += spell_value;
				e.stun_score += spell_value;

				break;
			}
			case SpellEffect::Charm: {
				spell_value = VegasSpellTypeMultipliers::Charm;

				duration = std::fmax(1.0f, std::fmin(duration, 50.0f)); // Only score up to 5 minutes.
				auto max_level = spell.max_value[i] == 0 ? RuleI(VegasNPCs, MaxNPCLevel) : spell.max_value[i];
				float level_cap_multiplier = max_level / RuleI(VegasNPCs, MaxNPCLevel);
				LogSpellScoreDetail("Checking {} [{}] as a {} --- effect value of {} for {} seconds up to level {} and level_cap_multiplier of {}.", spell.name, spell_id, "Charm", spell_value, duration, spell.max_value[i], 1.0f + level_cap_multiplier); //deleteme
				spell_value += (VegasSpellTypeMultipliers::Charm * duration);
				spell_value = IsTargetTypeOnlySpell(spell_id) ? spell_value * VegasSpellTypeMultipliers::TargetTypeOnly : spell_value;
				spell_value *= level_cap_multiplier;
				spell_score += spell_value;
				e.charm_score += spell_value;

				break;
			}
			case SpellEffect::Fear: {
				spell_value = VegasSpellTypeMultipliers::Fear;

				duration = std::fmax(1.0f, std::fmin(duration, 50.0f)); // Only score up to 5 minutes.
				auto max_level = spell.max_value[i] == 0 ? RuleI(VegasNPCs, MaxNPCLevel) : spell.max_value[i];
				float level_cap_multiplier = max_level / RuleI(VegasNPCs, MaxNPCLevel);
				LogSpellScoreDetail("Checking {} [{}] as a {} --- effect value of {} for {} seconds up to level {} and level_cap_multiplier of {}.", spell.name, spell_id, "Fear", spell_value, duration, spell.max_value[i], 1.0f + level_cap_multiplier); //deleteme
				spell_value += (VegasSpellTypeMultipliers::Fear * duration);
				spell_value = IsTargetTypeOnlySpell(spell_id) ? spell_value * VegasSpellTypeMultipliers::TargetTypeOnly : spell_value;
				spell_value *= level_cap_multiplier;
				spell_score += spell_value;
				e.fear_score += spell_value;

				break;
			}
			case SpellEffect::Stamina: {
				LogSpellScoreDetail("Checking {} [{}] as a {} --- effect value of {}.", spell.name, spell_id, "Stamina", spell_value); //deleteme
				spell_score += spell_value;
				e.stamina_score += spell_value;
				break;

				bool is_instant_spell = spell.base_value[i] != 0 && spell.buff_duration == 0 && spell.aoe_duration == 0;

				if (effect_id == SpellEffect::CurrentHPOnce || is_instant_spell) {
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
			case SpellEffect::BindAffinity:
				spell_score += VegasSpellTypeMultipliers::BindAffinity;
				e.bind_affinity_score += VegasSpellTypeMultipliers::BindAffinity;
				break;
			case SpellEffect::Gate:
				spell_score += VegasSpellTypeMultipliers::Gate;
				e.gate_score += VegasSpellTypeMultipliers::Gate;
				break;
			case SpellEffect::CancelMagic:
			case SpellEffect::InvisVsUndead:
			case SpellEffect::InvisVsAnimals:
			case SpellEffect::Mez:
			case SpellEffect::SummonItem:
			case SpellEffect::SummonPet:
//case SpellEffect::Confuse:
			case SpellEffect::DiseaseCounter:
			case SpellEffect::PoisonCounter:
//case SpellEffect::DetectHostile:
//case SpellEffect::DetectMagic:
			case SpellEffect::TwinCastBlocker:
			case SpellEffect::DivineAura:
			case SpellEffect::Destroy:
			case SpellEffect::ShadowStep:
			case SpellEffect::Berserk:
			case SpellEffect::Lycanthropy:
			case SpellEffect::Vampirism:
			case SpellEffect::ResistFire:
			case SpellEffect::ResistCold:
			case SpellEffect::ResistPoison:
			case SpellEffect::ResistDisease:
			case SpellEffect::ResistMagic:
//case SpellEffect::DetectTraps:
			case SpellEffect::SenseDead:
			case SpellEffect::SenseSummoned:
			case SpellEffect::SenseAnimals:
			case SpellEffect::Rune:
			case SpellEffect::TrueNorth:
			case SpellEffect::Levitate:
			case SpellEffect::Illusion:
			case SpellEffect::DamageShield:
//case SpellEffect::TransferItem:
			case SpellEffect::Identify:
//case SpellEffect::ItemID:
			case SpellEffect::WipeHateList:
			case SpellEffect::SpinTarget:
			case SpellEffect::InfraVision:
			case SpellEffect::UltraVision:
			case SpellEffect::EyeOfZomm:
			case SpellEffect::ReclaimPet:
			case SpellEffect::TotalHP:
//case SpellEffect::CorpseBomb:
			case SpellEffect::NecPet:
//case SpellEffect::PreserveCorpse:
			case SpellEffect::BindSight:
			case SpellEffect::FeignDeath:
			case SpellEffect::VoiceGraft:
			case SpellEffect::Sentinel:
			case SpellEffect::LocateCorpse:
			case SpellEffect::AbsorbMagicAtt:
//case SpellEffect::EnchantLight:
			case SpellEffect::Revive:
			case SpellEffect::SummonPC:
			case SpellEffect::Teleport:
			case SpellEffect::TossUp:
			case SpellEffect::WeaponProc:
			case SpellEffect::MagnifyVision:
			case SpellEffect::Succor:
			case SpellEffect::ModelSize:
//case SpellEffect::Cloak:
			case SpellEffect::SummonCorpse:
			case SpellEffect::InstantHate:
			case SpellEffect::StopRain:
			case SpellEffect::NegateIfCombat:
			case SpellEffect::Sacrifice:
			case SpellEffect::Silence:
			case SpellEffect::ManaPool:
			case SpellEffect::AttackSpeed2:
			case SpellEffect::Root:
			case SpellEffect::HealOverTime:
			case SpellEffect::CompleteHeal:
			case SpellEffect::Fearless:
			case SpellEffect::CallPet:
			case SpellEffect::Translocate:
			case SpellEffect::AntiGate:
			case SpellEffect::SummonBSTPet:
			case SpellEffect::AlterNPCLevel:
			case SpellEffect::Familiar:
			case SpellEffect::SummonItemIntoBag:
			case SpellEffect::IncreaseArchery:
			case SpellEffect::ResistAll:
			case SpellEffect::CastingLevel:
			case SpellEffect::SummonHorse:
			case SpellEffect::ChangeAggro:
			case SpellEffect::Hunger:
			case SpellEffect::CurseCounter:
			case SpellEffect::MagicWeapon:
			case SpellEffect::Amplification:
			case SpellEffect::AttackSpeed3:
			case SpellEffect::HealRate:
			case SpellEffect::ReverseDS:
			case SpellEffect::ReduceSkill:
			case SpellEffect::Screech:
			case SpellEffect::ImprovedDamage:
			case SpellEffect::ImprovedHeal:
			case SpellEffect::SpellResistReduction:
			case SpellEffect::IncreaseSpellHaste:
			case SpellEffect::IncreaseSpellDuration:
			case SpellEffect::IncreaseRange:
			case SpellEffect::SpellHateMod:
			case SpellEffect::ReduceReagentCost:
			case SpellEffect::ReduceManaCost:
			case SpellEffect::FcStunTimeMod:
			case SpellEffect::LimitMaxLevel:
			case SpellEffect::LimitResist:
			case SpellEffect::LimitTarget:
			case SpellEffect::LimitEffect:
			case SpellEffect::LimitSpellType:
			case SpellEffect::LimitSpell:
			case SpellEffect::LimitMinDur:
			case SpellEffect::LimitInstant:
			case SpellEffect::LimitMinLevel:
			case SpellEffect::LimitCastTimeMin:
			case SpellEffect::LimitCastTimeMax:
			case SpellEffect::Teleport2:
//case SpellEffect::ElectricityResist:
			case SpellEffect::PercentalHeal:
			case SpellEffect::StackingCommand_Block:
			case SpellEffect::StackingCommand_Overwrite:
			case SpellEffect::DeathSave:
			case SpellEffect::SuspendPet:
			case SpellEffect::TemporaryPets:
			case SpellEffect::BalanceHP:
			case SpellEffect::DispelDetrimental:
			case SpellEffect::SpellCritDmgIncrease:
			case SpellEffect::IllusionCopy:
			case SpellEffect::SpellDamageShield:
			case SpellEffect::Reflect:
			case SpellEffect::AllStats:
			case SpellEffect::MakeDrunk:
			case SpellEffect::MitigateSpellDamage:
			case SpellEffect::MitigateMeleeDamage:
			case SpellEffect::NegateAttacks:
			case SpellEffect::AppraiseLDonChest:
			case SpellEffect::DisarmLDoNTrap:
			case SpellEffect::UnlockLDoNChest:
			case SpellEffect::PetPowerIncrease:
			case SpellEffect::MeleeMitigation:
			case SpellEffect::CriticalHitChance:
			case SpellEffect::SpellCritChance:
			case SpellEffect::CrippBlowChance:
			case SpellEffect::AvoidMeleeChance:
			case SpellEffect::RiposteChance:
			case SpellEffect::DodgeChance:
			case SpellEffect::ParryChance:
			case SpellEffect::DualWieldChance:
			case SpellEffect::DoubleAttackChance:
			case SpellEffect::MeleeLifetap:
			case SpellEffect::AllInstrumentMod:
			case SpellEffect::ResistSpellChance:
			case SpellEffect::ResistFearChance:
			case SpellEffect::HundredHands:
			case SpellEffect::MeleeSkillCheck:
			case SpellEffect::HitChance:
			case SpellEffect::DamageModifier:
			case SpellEffect::MinDamageModifier:
			case SpellEffect::BalanceMana:
			case SpellEffect::IncreaseBlockChance:
			case SpellEffect::CurrentEndurance:
			case SpellEffect::EndurancePool:
			case SpellEffect::Amnesia:
			case SpellEffect::Hate:
			case SpellEffect::SkillAttack:
			case SpellEffect::FadingMemories:
			case SpellEffect::StunResist:
			case SpellEffect::StrikeThrough:
			case SpellEffect::SkillDamageTaken:
			case SpellEffect::CurrentEnduranceOnce:
			case SpellEffect::Taunt:
			case SpellEffect::ProcChance:
			case SpellEffect::RangedProc:
			case SpellEffect::IllusionOther:
			case SpellEffect::MassGroupBuff:
			case SpellEffect::GroupFearImmunity:
			case SpellEffect::Rampage:
			case SpellEffect::AETaunt:
			case SpellEffect::FleshToBone:
//case SpellEffect::PurgePoison:
			case SpellEffect::DispelBeneficial:
			case SpellEffect::PetShield:
			case SpellEffect::AEMelee:
			case SpellEffect::FrenziedDevastation:
			case SpellEffect::PetMaxHP:
			case SpellEffect::MaxHPChange:
			case SpellEffect::PetAvoidance:
			case SpellEffect::Accuracy:
			case SpellEffect::HeadShot:
			case SpellEffect::PetCriticalHit:
			case SpellEffect::SlayUndead:
			case SpellEffect::SkillDamageAmount:
			case SpellEffect::Packrat:
			case SpellEffect::BlockBehind:
			case SpellEffect::DoubleRiposte:
			case SpellEffect::GiveDoubleRiposte:
			case SpellEffect::GiveDoubleAttack:
			case SpellEffect::TwoHandBash:
			case SpellEffect::ReduceSkillTimer:
			case SpellEffect::ReduceFallDamage:
			case SpellEffect::PersistantCasting:
			case SpellEffect::ExtendedShielding:
			case SpellEffect::StunBashChance:
			case SpellEffect::DivineSave:
			case SpellEffect::Metabolism:
			case SpellEffect::ReduceApplyPoisonTime:
			case SpellEffect::ChannelChanceSpells:
//case SpellEffect::FreePet:
			case SpellEffect::GivePetGroupTarget:
			case SpellEffect::IllusionPersistence:
			case SpellEffect::FeignedCastOnChance:
//case SpellEffect::StringUnbreakable:
			case SpellEffect::ImprovedReclaimEnergy:
			case SpellEffect::IncreaseChanceMemwipe:
			case SpellEffect::CharmBreakChance:
			case SpellEffect::RootBreakChance:
			case SpellEffect::TrapCircumvention:
			case SpellEffect::SetBreathLevel:
			case SpellEffect::RaiseSkillCap:
			case SpellEffect::SecondaryForte:
			case SpellEffect::SecondaryDmgInc:
			case SpellEffect::SpellProcChance:
			case SpellEffect::ConsumeProjectile:
			case SpellEffect::FrontalBackstabChance:
			case SpellEffect::FrontalBackstabMinDmg:
			case SpellEffect::Blank:
			case SpellEffect::ShieldDuration:
			case SpellEffect::ShroudofStealth:
			case SpellEffect::PetDiscipline:
			case SpellEffect::TripleBackstab:
			case SpellEffect::CombatStability:
			case SpellEffect::AddSingingMod:
			case SpellEffect::SongModCap:
			case SpellEffect::RaiseStatCap:
			case SpellEffect::TradeSkillMastery:
			case SpellEffect::HastenedAASkill:
			case SpellEffect::MasteryofPast:
			case SpellEffect::ExtraAttackChance:
			case SpellEffect::AddPetCommand:
			case SpellEffect::ReduceTradeskillFail:
			case SpellEffect::MaxBindWound:
			case SpellEffect::BardSongRange:
			case SpellEffect::BaseMovementSpeed:
			case SpellEffect::CastingLevel2:
			case SpellEffect::CriticalDoTChance:
			case SpellEffect::CriticalHealChance:
			case SpellEffect::CriticalMend:
			case SpellEffect::Ambidexterity:
			case SpellEffect::UnfailingDivinity:
			case SpellEffect::FinishingBlow:
			case SpellEffect::Flurry:
			case SpellEffect::PetFlurry:
			case SpellEffect::FeignedMinion:
			case SpellEffect::ImprovedBindWound:
			case SpellEffect::DoubleSpecialAttack:
//case SpellEffect::LoHSetHeal:
			case SpellEffect::NimbleEvasion:
			case SpellEffect::FcDamageAmt:
			case SpellEffect::SpellDurationIncByTic:
			case SpellEffect::SkillAttackProc:
			case SpellEffect::CastOnFadeEffect:
			case SpellEffect::IncreaseRunSpeedCap:
			case SpellEffect::Purify:
			case SpellEffect::StrikeThrough2:
			case SpellEffect::FrontalStunResist:
			case SpellEffect::CriticalSpellChance:
//case SpellEffect::ReduceTimerSpecial:
			case SpellEffect::FcSpellVulnerability:
			case SpellEffect::FcDamageAmtIncoming:
			case SpellEffect::ChangeHeight:
			case SpellEffect::WakeTheDead:
			case SpellEffect::Doppelganger:
			case SpellEffect::ArcheryDamageModifier:
			case SpellEffect::FcDamagePctCrit:
			case SpellEffect::FcDamageAmtCrit:
			case SpellEffect::OffhandRiposteFail:
			case SpellEffect::MitigateDamageShield:
			case SpellEffect::ArmyOfTheDead:
//case SpellEffect::Appraisal:
			case SpellEffect::ZoneSuspendMinion:
			case SpellEffect::GateCastersBindpoint:
			case SpellEffect::ReduceReuseTimer:
			case SpellEffect::LimitCombatSkills:
			case SpellEffect::Sanctuary:
			case SpellEffect::ForageAdditionalItems:
			case SpellEffect::Invisibility2:
			case SpellEffect::InvisVsUndead2:
			case SpellEffect::ImprovedInvisAnimals:
			case SpellEffect::ItemHPRegenCapIncrease:
			case SpellEffect::ItemManaRegenCapIncrease:
			case SpellEffect::CriticalHealOverTime:
			case SpellEffect::ShieldBlock:
			case SpellEffect::ReduceHate:
			case SpellEffect::GateToHomeCity:
			case SpellEffect::DefensiveProc:
			case SpellEffect::HPToMana:
			case SpellEffect::NoBreakAESneak:
			case SpellEffect::SpellSlotIncrease:
			case SpellEffect::MysticalAttune:
			case SpellEffect::DelayDeath:
			case SpellEffect::ManaAbsorbPercentDamage:
			case SpellEffect::CriticalDamageMob:
			case SpellEffect::Salvage:
			case SpellEffect::SummonToCorpse:
			case SpellEffect::CastOnRuneFadeEffect:
			case SpellEffect::BardAEDot:
			case SpellEffect::BlockNextSpellFocus:
//case SpellEffect::IllusionaryTarget:
			case SpellEffect::PercentXPIncrease:
			case SpellEffect::SummonAndResAllCorpses:
			case SpellEffect::TriggerOnCast:
			case SpellEffect::SpellTrigger:
			case SpellEffect::ItemAttackCapIncrease:
			case SpellEffect::ImmuneFleeing:
			case SpellEffect::InterruptCasting:
			case SpellEffect::ChannelChanceItems:
			case SpellEffect::AssassinateLevel:
			case SpellEffect::HeadShotLevel:
			case SpellEffect::DoubleRangedAttack:
			case SpellEffect::LimitManaMin:
			case SpellEffect::ShieldEquipDmgMod:
			case SpellEffect::ManaBurn:
			case SpellEffect::PersistentEffect:
			case SpellEffect::IncreaseTrapCount:
			case SpellEffect::AdditionalAura:
//case SpellEffect::DeactivateAllTraps:
//case SpellEffect::LearnTrap:
//case SpellEffect::ChangeTriggerType:
			case SpellEffect::FcMute:
//case SpellEffect::PassiveSenseTrap:
			case SpellEffect::ProcOnKillShot:
			case SpellEffect::SpellOnDeath:
			case SpellEffect::PotionBeltSlots:
			case SpellEffect::BandolierSlots:
			case SpellEffect::TripleAttackChance:
			case SpellEffect::ProcOnSpellKillShot:
//case SpellEffect::GroupShielding:
			case SpellEffect::SetBodyType:
//case SpellEffect::FactionMod:
			case SpellEffect::CorruptionCounter:
			case SpellEffect::ResistCorruption:
			case SpellEffect::AttackSpeed4:
			case SpellEffect::ForageSkill:
			case SpellEffect::CastOnFadeEffectAlways:
			case SpellEffect::ApplyEffect:
			case SpellEffect::DotCritDmgIncrease:
//case SpellEffect::Fling:
			case SpellEffect::CastOnFadeEffectNPC:
			case SpellEffect::SpellEffectResistChance:
			case SpellEffect::ShadowStepDirectional:
			case SpellEffect::Knockdown:
//case SpellEffect::KnockTowardCaster:
			case SpellEffect::NegateSpellEffect:
			case SpellEffect::SympatheticProc:
			case SpellEffect::Leap:
			case SpellEffect::LimitSpellGroup:
			case SpellEffect::CastOnCurer:
			case SpellEffect::CastOnCure:
			case SpellEffect::SummonCorpseZone:
			case SpellEffect::FcTimerRefresh:
			case SpellEffect::FcTimerLockout:
			case SpellEffect::LimitManaMax:
			case SpellEffect::FcHealAmt:
			case SpellEffect::FcHealPctIncoming:
			case SpellEffect::FcHealAmtIncoming:
			case SpellEffect::FcHealPctCritIncoming:
			case SpellEffect::FcHealAmtCrit:
			case SpellEffect::PetMeleeMitigation:
			case SpellEffect::SwarmPetDuration:
			case SpellEffect::FcTwincast:
			case SpellEffect::HealGroupFromMana:
			case SpellEffect::ManaDrainWithDmg:
			case SpellEffect::EndDrainWithDmg:
			case SpellEffect::LimitSpellClass:
			case SpellEffect::LimitSpellSubclass:
			case SpellEffect::TwoHandBluntBlock:
			case SpellEffect::CastonNumHitFade:
			case SpellEffect::CastonFocusEffect:
			case SpellEffect::LimitHPPercent:
			case SpellEffect::LimitManaPercent:
			case SpellEffect::LimitEndPercent:
			case SpellEffect::LimitClass:
			case SpellEffect::LimitRace:
			case SpellEffect::FcBaseEffects:
			case SpellEffect::LimitCastingSkill:
			case SpellEffect::FFItemClass:
			case SpellEffect::ACv2:
			case SpellEffect::ManaRegen_v2:
			case SpellEffect::SkillDamageAmount2:
			case SpellEffect::AddMeleeProc:
			case SpellEffect::FcLimitUse:
			case SpellEffect::FcIncreaseNumHits:
			case SpellEffect::LimitUseMin:
			case SpellEffect::LimitUseType:
			case SpellEffect::GravityEffect:
			case SpellEffect::Display:
			case SpellEffect::IncreaseExtTargetWindow:
			case SpellEffect::SkillProcAttempt:
			case SpellEffect::LimitToSkill:
			case SpellEffect::SkillProcSuccess:
//case SpellEffect::PostEffect:
//case SpellEffect::PostEffectData:
//case SpellEffect::ExpandMaxActiveTrophyBen:
			case SpellEffect::CriticalDotDecay:
			case SpellEffect::CriticalHealDecay:
			case SpellEffect::CriticalRegenDecay:
//case SpellEffect::BeneficialCountDownHold:
//case SpellEffect::TeleporttoAnchor:
//case SpellEffect::TranslocatetoAnchor:
			case SpellEffect::Assassinate:
			case SpellEffect::FinishingBlowLvl:
			case SpellEffect::DistanceRemoval:
			case SpellEffect::TriggerOnReqTarget:
			case SpellEffect::TriggerOnReqCaster:
			case SpellEffect::ImprovedTaunt:
//case SpellEffect::AddMercSlot:
			case SpellEffect::AStacker:
			case SpellEffect::BStacker:
			case SpellEffect::CStacker:
			case SpellEffect::DStacker:
			case SpellEffect::MitigateDotDamage:
			case SpellEffect::MeleeThresholdGuard:
			case SpellEffect::SpellThresholdGuard:
			case SpellEffect::TriggerMeleeThreshold:
			case SpellEffect::TriggerSpellThreshold:
			case SpellEffect::AddHatePct:
			case SpellEffect::AddHateOverTimePct:
			case SpellEffect::ResourceTap:
			case SpellEffect::FactionModPct:
			case SpellEffect::DamageModifier2:
			case SpellEffect::Ff_Override_NotFocusable:
			case SpellEffect::ImprovedDamage2:
			case SpellEffect::FcDamageAmt2:
			case SpellEffect::Shield_Target:
			case SpellEffect::PC_Pet_Rampage:
			case SpellEffect::PC_Pet_AE_Rampage:
			case SpellEffect::PC_Pet_Flurry_Chance:
			case SpellEffect::DS_Mitigation_Amount:
			case SpellEffect::DS_Mitigation_Percentage:
			case SpellEffect::Chance_Best_in_Spell_Grp:
			case SpellEffect::Trigger_Best_in_Spell_Grp:
			case SpellEffect::Double_Melee_Round:
			case SpellEffect::Buy_AA_Rank:
			case SpellEffect::Double_Backstab_Front:
			case SpellEffect::Pet_Crit_Melee_Damage_Pct_Owner:
			case SpellEffect::Trigger_Spell_Non_Item:
			case SpellEffect::Weapon_Stance:
			case SpellEffect::Hatelist_To_Top_Index:
			case SpellEffect::Hatelist_To_Tail_Index:
			case SpellEffect::Ff_Value_Min:
			case SpellEffect::Ff_Value_Max:
			case SpellEffect::Fc_Cast_Spell_On_Land:
			case SpellEffect::Skill_Base_Damage_Mod:
			case SpellEffect::Fc_Spell_Damage_Pct_IncomingPC:
			case SpellEffect::Fc_Spell_Damage_Amt_IncomingPC:
			case SpellEffect::Ff_CasterClass:
			case SpellEffect::Ff_Same_Caster:
//case SpellEffect::Extend_Tradeskill_Cap:
//case SpellEffect::Defender_Melee_Force_Pct_PC:
			case SpellEffect::Worn_Endurance_Regen_Cap:
			case SpellEffect::Ff_ReuseTimeMin:
			case SpellEffect::Ff_ReuseTimeMax:
			case SpellEffect::Ff_Endurance_Min:
			case SpellEffect::Ff_Endurance_Max:
			case SpellEffect::Pet_Add_Atk:
			case SpellEffect::Ff_DurationMax:
			case SpellEffect::Critical_Melee_Damage_Mod_Max:
//case SpellEffect::Ff_FocusCastProcNoBypass:
			case SpellEffect::AddExtraAttackPct_1h_Primary:
			case SpellEffect::AddExtraAttackPct_1h_Secondary:
			case SpellEffect::Fc_CastTimeMod2:
			case SpellEffect::Fc_CastTimeAmt:
			case SpellEffect::Fearstun:
			case SpellEffect::Melee_Damage_Position_Mod:
			case SpellEffect::Melee_Damage_Position_Amt:
			case SpellEffect::Damage_Taken_Position_Mod:
			case SpellEffect::Damage_Taken_Position_Amt:
			case SpellEffect::Fc_Amplify_Mod:
			case SpellEffect::Fc_Amplify_Amt:
			case SpellEffect::Health_Transfer:
			case SpellEffect::Fc_ResistIncoming:
			case SpellEffect::Ff_FocusTimerMin:
			case SpellEffect::Proc_Timer_Modifier :
//case SpellEffect::Mana_Max_Percent:
//case SpellEffect::Endurance_Max_Percent:
			case SpellEffect::AC_Avoidance_Max_Percent:
			case SpellEffect::AC_Mitigation_Max_Percent:
//case SpellEffect::Attack_Offense_Max_Percent:
			case SpellEffect::Attack_Accuracy_Max_Percent:
//case SpellEffect::Luck_Amount:
//case SpellEffect::Luck_Percent:
			case SpellEffect::Endurance_Absorb_Pct_Damage:
			case SpellEffect::Instant_Mana_Pct:
			case SpellEffect::Instant_Endurance_Pct:
			case SpellEffect::Duration_HP_Pct:
			case SpellEffect::Duration_Mana_Pct:
			case SpellEffect::Duration_Endurance_Pct:
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
