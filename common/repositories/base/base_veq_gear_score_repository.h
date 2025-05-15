/**
 * DO NOT MODIFY THIS FILE
 *
 * This repository was automatically generated and is NOT to be modified directly.
 * Any repository modifications are meant to be made to the repository extending the base.
 * Any modifications to base repositories are to be made by the generator only
 *
 * @generator ./utils/scripts/generators/repository-generator.pl
 * @docs https://docs.eqemu.io/developer/repositories
 */

#ifndef EQEMU_BASE_VEQ_GEAR_SCORE_REPOSITORY_H
#define EQEMU_BASE_VEQ_GEAR_SCORE_REPOSITORY_H

#include "../../database.h"
#include "../../strings.h"
#include <ctime>

class BaseVeqGearScoreRepository {
public:
	struct VeqGearScore {
		uint32_t    id;
		std::string name;
		uint8_t     common_drop;
		uint8_t     rare_drop;
		uint8_t     raid_drop;
		float       avg;
		float       high;
		float       low;
		float       classes1;
		float       classes2;
		float       classes3;
		float       classes4;
		float       classes5;
		float       classes6;
		float       classes7;
		float       classes8;
		float       classes9;
		float       classes10;
		float       classes11;
		float       classes12;
		float       classes13;
		float       classes14;
		float       classes15;
		float       classes16;
		float       damage_score;
		float       delay_score;
		float       ratio_score;
		float       weapon_score;
		float       arrow_score;
		float       ac_score;
		float       hp_score;
		float       mana_score;
		float       end_score;
		float       hp_regen_score;
		float       mana_regen_score;
		float       end_regen_score;
		float       haste_score;
		float       agi_score;
		float       cha_score;
		float       dex_score;
		float       int_score;
		float       sta_score;
		float       str_score;
		float       wis_score;
		float       cr_score;
		float       dr_score;
		float       fr_score;
		float       mr_score;
		float       pr_score;
		float       corruption_score;
		float       h_agi_score;
		float       h_cha_score;
		float       h_dex_score;
		float       h_int_score;
		float       h_sta_score;
		float       h_str_score;
		float       h_wis_score;
		float       h_cr_score;
		float       h_dr_score;
		float       h_fr_score;
		float       h_mr_score;
		float       h_pr_score;
		float       h_corruption_score;
		float       accuracy_score;
		float       attack_score;
		float       avoidance_score;
		float       combat_effects_score;
		float       damage_shield_score;
		float       dot_shielding_score;
		float       shielding_score;
		float       spellshield_score;
		float       strikethrough_score;
		float       stunresist_score;
		float       ds_mitigation_score;
		float       heal_amount_score;
		float       spell_damage_score;
		float       clairvoyance_score;
		float       backstab_damage_score;
		float       skillmod_score;
		float       extra_skill_score;
		float       bane_dmg_score;
		float       elemental_dmg_score;
		float       bard_type_score;
		float       proc_effect_score;
		float       worn_effect_score;
		float       focus_effect_score;
		float       click_effect_score;
	};

	static std::string PrimaryKey()
	{
		return std::string("id");
	}

	static std::vector<std::string> Columns()
	{
		return {
			"id",
			"name",
			"common_drop",
			"rare_drop",
			"raid_drop",
			"avg",
			"high",
			"low",
			"classes1",
			"classes2",
			"classes3",
			"classes4",
			"classes5",
			"classes6",
			"classes7",
			"classes8",
			"classes9",
			"classes10",
			"classes11",
			"classes12",
			"classes13",
			"classes14",
			"classes15",
			"classes16",
			"damage_score",
			"delay_score",
			"ratio_score",
			"weapon_score",
			"arrow_score",
			"ac_score",
			"hp_score",
			"mana_score",
			"end_score",
			"hp_regen_score",
			"mana_regen_score",
			"end_regen_score",
			"haste_score",
			"agi_score",
			"cha_score",
			"dex_score",
			"int_score",
			"sta_score",
			"str_score",
			"wis_score",
			"cr_score",
			"dr_score",
			"fr_score",
			"mr_score",
			"pr_score",
			"corruption_score",
			"h_agi_score",
			"h_cha_score",
			"h_dex_score",
			"h_int_score",
			"h_sta_score",
			"h_str_score",
			"h_wis_score",
			"h_cr_score",
			"h_dr_score",
			"h_fr_score",
			"h_mr_score",
			"h_pr_score",
			"h_corruption_score",
			"accuracy_score",
			"attack_score",
			"avoidance_score",
			"combat_effects_score",
			"damage_shield_score",
			"dot_shielding_score",
			"shielding_score",
			"spellshield_score",
			"strikethrough_score",
			"stunresist_score",
			"ds_mitigation_score",
			"heal_amount_score",
			"spell_damage_score",
			"clairvoyance_score",
			"backstab_damage_score",
			"skillmod_score",
			"extra_skill_score",
			"bane_dmg_score",
			"elemental_dmg_score",
			"bard_type_score",
			"proc_effect_score",
			"worn_effect_score",
			"focus_effect_score",
			"click_effect_score",
		};
	}

	static std::vector<std::string> SelectColumns()
	{
		return {
			"id",
			"name",
			"common_drop",
			"rare_drop",
			"raid_drop",
			"avg",
			"high",
			"low",
			"classes1",
			"classes2",
			"classes3",
			"classes4",
			"classes5",
			"classes6",
			"classes7",
			"classes8",
			"classes9",
			"classes10",
			"classes11",
			"classes12",
			"classes13",
			"classes14",
			"classes15",
			"classes16",
			"damage_score",
			"delay_score",
			"ratio_score",
			"weapon_score",
			"arrow_score",
			"ac_score",
			"hp_score",
			"mana_score",
			"end_score",
			"hp_regen_score",
			"mana_regen_score",
			"end_regen_score",
			"haste_score",
			"agi_score",
			"cha_score",
			"dex_score",
			"int_score",
			"sta_score",
			"str_score",
			"wis_score",
			"cr_score",
			"dr_score",
			"fr_score",
			"mr_score",
			"pr_score",
			"corruption_score",
			"h_agi_score",
			"h_cha_score",
			"h_dex_score",
			"h_int_score",
			"h_sta_score",
			"h_str_score",
			"h_wis_score",
			"h_cr_score",
			"h_dr_score",
			"h_fr_score",
			"h_mr_score",
			"h_pr_score",
			"h_corruption_score",
			"accuracy_score",
			"attack_score",
			"avoidance_score",
			"combat_effects_score",
			"damage_shield_score",
			"dot_shielding_score",
			"shielding_score",
			"spellshield_score",
			"strikethrough_score",
			"stunresist_score",
			"ds_mitigation_score",
			"heal_amount_score",
			"spell_damage_score",
			"clairvoyance_score",
			"backstab_damage_score",
			"skillmod_score",
			"extra_skill_score",
			"bane_dmg_score",
			"elemental_dmg_score",
			"bard_type_score",
			"proc_effect_score",
			"worn_effect_score",
			"focus_effect_score",
			"click_effect_score",
		};
	}

	static std::string ColumnsRaw()
	{
		return std::string(Strings::Implode(", ", Columns()));
	}

	static std::string SelectColumnsRaw()
	{
		return std::string(Strings::Implode(", ", SelectColumns()));
	}

	static std::string TableName()
	{
		return std::string("veq_gear_score");
	}

	static std::string BaseSelect()
	{
		return fmt::format(
			"SELECT {} FROM {}",
			SelectColumnsRaw(),
			TableName()
		);
	}

	static std::string BaseInsert()
	{
		return fmt::format(
			"INSERT INTO {} ({}) ",
			TableName(),
			ColumnsRaw()
		);
	}

	static VeqGearScore NewEntity()
	{
		VeqGearScore e{};

		e.id                    = 0;
		e.name                  = "";
		e.common_drop           = 0;
		e.rare_drop             = 0;
		e.raid_drop             = 0;
		e.avg                   = 0;
		e.high                  = 0;
		e.low                   = 0;
		e.classes1              = 0;
		e.classes2              = 0;
		e.classes3              = 0;
		e.classes4              = 0;
		e.classes5              = 0;
		e.classes6              = 0;
		e.classes7              = 0;
		e.classes8              = 0;
		e.classes9              = 0;
		e.classes10             = 0;
		e.classes11             = 0;
		e.classes12             = 0;
		e.classes13             = 0;
		e.classes14             = 0;
		e.classes15             = 0;
		e.classes16             = 0;
		e.damage_score          = 0;
		e.delay_score           = 0;
		e.ratio_score           = 0;
		e.weapon_score          = 0;
		e.arrow_score           = 0;
		e.ac_score              = 0;
		e.hp_score              = 0;
		e.mana_score            = 0;
		e.end_score             = 0;
		e.hp_regen_score        = 0;
		e.mana_regen_score      = 0;
		e.end_regen_score       = 0;
		e.haste_score           = 0;
		e.agi_score             = 0;
		e.cha_score             = 0;
		e.dex_score             = 0;
		e.int_score             = 0;
		e.sta_score             = 0;
		e.str_score             = 0;
		e.wis_score             = 0;
		e.cr_score              = 0;
		e.dr_score              = 0;
		e.fr_score              = 0;
		e.mr_score              = 0;
		e.pr_score              = 0;
		e.corruption_score      = 0;
		e.h_agi_score           = 0;
		e.h_cha_score           = 0;
		e.h_dex_score           = 0;
		e.h_int_score           = 0;
		e.h_sta_score           = 0;
		e.h_str_score           = 0;
		e.h_wis_score           = 0;
		e.h_cr_score            = 0;
		e.h_dr_score            = 0;
		e.h_fr_score            = 0;
		e.h_mr_score            = 0;
		e.h_pr_score            = 0;
		e.h_corruption_score    = 0;
		e.accuracy_score        = 0;
		e.attack_score          = 0;
		e.avoidance_score       = 0;
		e.combat_effects_score  = 0;
		e.damage_shield_score   = 0;
		e.dot_shielding_score   = 0;
		e.shielding_score       = 0;
		e.spellshield_score     = 0;
		e.strikethrough_score   = 0;
		e.stunresist_score      = 0;
		e.ds_mitigation_score   = 0;
		e.heal_amount_score     = 0;
		e.spell_damage_score    = 0;
		e.clairvoyance_score    = 0;
		e.backstab_damage_score = 0;
		e.skillmod_score        = 0;
		e.extra_skill_score     = 0;
		e.bane_dmg_score        = 0;
		e.elemental_dmg_score   = 0;
		e.bard_type_score       = 0;
		e.proc_effect_score     = 0;
		e.worn_effect_score     = 0;
		e.focus_effect_score    = 0;
		e.click_effect_score    = 0;

		return e;
	}

	static VeqGearScore GetVeqGearScore(
		const std::vector<VeqGearScore> &veq_gear_scores,
		int veq_gear_score_id
	)
	{
		for (auto &veq_gear_score : veq_gear_scores) {
			if (veq_gear_score.id == veq_gear_score_id) {
				return veq_gear_score;
			}
		}

		return NewEntity();
	}

	static VeqGearScore FindOne(
		Database& db,
		int veq_gear_score_id
	)
	{
		auto results = db.QueryDatabase(
			fmt::format(
				"{} WHERE {} = {} LIMIT 1",
				BaseSelect(),
				PrimaryKey(),
				veq_gear_score_id
			)
		);

		auto row = results.begin();
		if (results.RowCount() == 1) {
			VeqGearScore e{};

			e.id                    = row[0] ? static_cast<uint32_t>(strtoul(row[0], nullptr, 10)) : 0;
			e.name                  = row[1] ? row[1] : "";
			e.common_drop           = row[2] ? static_cast<uint8_t>(strtoul(row[2], nullptr, 10)) : 0;
			e.rare_drop             = row[3] ? static_cast<uint8_t>(strtoul(row[3], nullptr, 10)) : 0;
			e.raid_drop             = row[4] ? static_cast<uint8_t>(strtoul(row[4], nullptr, 10)) : 0;
			e.avg                   = row[5] ? (strtof(row[5], nullptr) > 0.0f ? strtof(row[5], nullptr) : 0) : 0;
			e.high                  = row[6] ? strtof(row[6], nullptr) : 0;
			e.low                   = row[7] ? strtof(row[7], nullptr) : 0;
			e.classes1              = row[8] ? strtof(row[8], nullptr) : 0;
			e.classes2              = row[9] ? strtof(row[9], nullptr) : 0;
			e.classes3              = row[10] ? strtof(row[10], nullptr) : 0;
			e.classes4              = row[11] ? strtof(row[11], nullptr) : 0;
			e.classes5              = row[12] ? strtof(row[12], nullptr) : 0;
			e.classes6              = row[13] ? strtof(row[13], nullptr) : 0;
			e.classes7              = row[14] ? strtof(row[14], nullptr) : 0;
			e.classes8              = row[15] ? strtof(row[15], nullptr) : 0;
			e.classes9              = row[16] ? strtof(row[16], nullptr) : 0;
			e.classes10             = row[17] ? strtof(row[17], nullptr) : 0;
			e.classes11             = row[18] ? strtof(row[18], nullptr) : 0;
			e.classes12             = row[19] ? strtof(row[19], nullptr) : 0;
			e.classes13             = row[20] ? strtof(row[20], nullptr) : 0;
			e.classes14             = row[21] ? strtof(row[21], nullptr) : 0;
			e.classes15             = row[22] ? strtof(row[22], nullptr) : 0;
			e.classes16             = row[23] ? strtof(row[23], nullptr) : 0;
			e.damage_score          = row[24] ? strtof(row[24], nullptr) : 0;
			e.delay_score           = row[25] ? strtof(row[25], nullptr) : 0;
			e.ratio_score           = row[26] ? strtof(row[26], nullptr) : 0;
			e.weapon_score          = row[27] ? strtof(row[27], nullptr) : 0;
			e.arrow_score           = row[28] ? strtof(row[28], nullptr) : 0;
			e.ac_score              = row[29] ? strtof(row[29], nullptr) : 0;
			e.hp_score              = row[30] ? strtof(row[30], nullptr) : 0;
			e.mana_score            = row[31] ? strtof(row[31], nullptr) : 0;
			e.end_score             = row[32] ? strtof(row[32], nullptr) : 0;
			e.hp_regen_score        = row[33] ? strtof(row[33], nullptr) : 0;
			e.mana_regen_score      = row[34] ? strtof(row[34], nullptr) : 0;
			e.end_regen_score       = row[35] ? strtof(row[35], nullptr) : 0;
			e.haste_score           = row[36] ? strtof(row[36], nullptr) : 0;
			e.agi_score             = row[37] ? strtof(row[37], nullptr) : 0;
			e.cha_score             = row[38] ? strtof(row[38], nullptr) : 0;
			e.dex_score             = row[39] ? strtof(row[39], nullptr) : 0;
			e.int_score             = row[40] ? strtof(row[40], nullptr) : 0;
			e.sta_score             = row[41] ? strtof(row[41], nullptr) : 0;
			e.str_score             = row[42] ? strtof(row[42], nullptr) : 0;
			e.wis_score             = row[43] ? strtof(row[43], nullptr) : 0;
			e.cr_score              = row[44] ? strtof(row[44], nullptr) : 0;
			e.dr_score              = row[45] ? strtof(row[45], nullptr) : 0;
			e.fr_score              = row[46] ? strtof(row[46], nullptr) : 0;
			e.mr_score              = row[47] ? strtof(row[47], nullptr) : 0;
			e.pr_score              = row[48] ? strtof(row[48], nullptr) : 0;
			e.corruption_score      = row[49] ? strtof(row[49], nullptr) : 0;
			e.h_agi_score           = row[50] ? strtof(row[50], nullptr) : 0;
			e.h_cha_score           = row[51] ? strtof(row[51], nullptr) : 0;
			e.h_dex_score           = row[52] ? strtof(row[52], nullptr) : 0;
			e.h_int_score           = row[53] ? strtof(row[53], nullptr) : 0;
			e.h_sta_score           = row[54] ? strtof(row[54], nullptr) : 0;
			e.h_str_score           = row[55] ? strtof(row[55], nullptr) : 0;
			e.h_wis_score           = row[56] ? strtof(row[56], nullptr) : 0;
			e.h_cr_score            = row[57] ? strtof(row[57], nullptr) : 0;
			e.h_dr_score            = row[58] ? strtof(row[58], nullptr) : 0;
			e.h_fr_score            = row[59] ? strtof(row[59], nullptr) : 0;
			e.h_mr_score            = row[60] ? strtof(row[60], nullptr) : 0;
			e.h_pr_score            = row[61] ? strtof(row[61], nullptr) : 0;
			e.h_corruption_score    = row[62] ? strtof(row[62], nullptr) : 0;
			e.accuracy_score        = row[63] ? strtof(row[63], nullptr) : 0;
			e.attack_score          = row[64] ? strtof(row[64], nullptr) : 0;
			e.avoidance_score       = row[65] ? strtof(row[65], nullptr) : 0;
			e.combat_effects_score  = row[66] ? strtof(row[66], nullptr) : 0;
			e.damage_shield_score   = row[67] ? strtof(row[67], nullptr) : 0;
			e.dot_shielding_score   = row[68] ? strtof(row[68], nullptr) : 0;
			e.shielding_score       = row[69] ? strtof(row[69], nullptr) : 0;
			e.spellshield_score     = row[70] ? strtof(row[70], nullptr) : 0;
			e.strikethrough_score   = row[71] ? strtof(row[71], nullptr) : 0;
			e.stunresist_score      = row[72] ? strtof(row[72], nullptr) : 0;
			e.ds_mitigation_score   = row[73] ? strtof(row[73], nullptr) : 0;
			e.heal_amount_score     = row[74] ? strtof(row[74], nullptr) : 0;
			e.spell_damage_score    = row[75] ? strtof(row[75], nullptr) : 0;
			e.clairvoyance_score    = row[76] ? strtof(row[76], nullptr) : 0;
			e.backstab_damage_score = row[77] ? strtof(row[77], nullptr) : 0;
			e.skillmod_score        = row[78] ? strtof(row[78], nullptr) : 0;
			e.extra_skill_score     = row[79] ? strtof(row[79], nullptr) : 0;
			e.bane_dmg_score        = row[80] ? strtof(row[80], nullptr) : 0;
			e.elemental_dmg_score   = row[81] ? strtof(row[81], nullptr) : 0;
			e.bard_type_score       = row[82] ? strtof(row[82], nullptr) : 0;
			e.proc_effect_score     = row[83] ? strtof(row[83], nullptr) : 0;
			e.worn_effect_score     = row[84] ? strtof(row[84], nullptr) : 0;
			e.focus_effect_score    = row[85] ? strtof(row[85], nullptr) : 0;
			e.click_effect_score    = row[86] ? strtof(row[86], nullptr) : 0;

			return e;
		}

		return NewEntity();
	}

	static int DeleteOne(
		Database& db,
		int veq_gear_score_id
	)
	{
		auto results = db.QueryDatabase(
			fmt::format(
				"DELETE FROM {} WHERE {} = {}",
				TableName(),
				PrimaryKey(),
				veq_gear_score_id
			)
		);

		return (results.Success() ? results.RowsAffected() : 0);
	}

	static int UpdateOne(
		Database& db,
		const VeqGearScore &e
	)
	{
		std::vector<std::string> v;

		auto columns = Columns();

		v.push_back(columns[0] + " = " + std::to_string(e.id));
		v.push_back(columns[1] + " = '" + Strings::Escape(e.name) + "'");
		v.push_back(columns[2] + " = " + std::to_string(e.common_drop));
		v.push_back(columns[3] + " = " + std::to_string(e.rare_drop));
		v.push_back(columns[4] + " = " + std::to_string(e.raid_drop));
		v.push_back(columns[5] + " = " + std::to_string(e.avg));
		v.push_back(columns[6] + " = " + std::to_string(e.high));
		v.push_back(columns[7] + " = " + std::to_string(e.low));
		v.push_back(columns[8] + " = " + std::to_string(e.classes1));
		v.push_back(columns[9] + " = " + std::to_string(e.classes2));
		v.push_back(columns[10] + " = " + std::to_string(e.classes3));
		v.push_back(columns[11] + " = " + std::to_string(e.classes4));
		v.push_back(columns[12] + " = " + std::to_string(e.classes5));
		v.push_back(columns[13] + " = " + std::to_string(e.classes6));
		v.push_back(columns[14] + " = " + std::to_string(e.classes7));
		v.push_back(columns[15] + " = " + std::to_string(e.classes8));
		v.push_back(columns[16] + " = " + std::to_string(e.classes9));
		v.push_back(columns[17] + " = " + std::to_string(e.classes10));
		v.push_back(columns[18] + " = " + std::to_string(e.classes11));
		v.push_back(columns[19] + " = " + std::to_string(e.classes12));
		v.push_back(columns[20] + " = " + std::to_string(e.classes13));
		v.push_back(columns[21] + " = " + std::to_string(e.classes14));
		v.push_back(columns[22] + " = " + std::to_string(e.classes15));
		v.push_back(columns[23] + " = " + std::to_string(e.classes16));
		v.push_back(columns[24] + " = " + std::to_string(e.damage_score));
		v.push_back(columns[25] + " = " + std::to_string(e.delay_score));
		v.push_back(columns[26] + " = " + std::to_string(e.ratio_score));
		v.push_back(columns[27] + " = " + std::to_string(e.weapon_score));
		v.push_back(columns[28] + " = " + std::to_string(e.arrow_score));
		v.push_back(columns[29] + " = " + std::to_string(e.ac_score));
		v.push_back(columns[30] + " = " + std::to_string(e.hp_score));
		v.push_back(columns[31] + " = " + std::to_string(e.mana_score));
		v.push_back(columns[32] + " = " + std::to_string(e.end_score));
		v.push_back(columns[33] + " = " + std::to_string(e.hp_regen_score));
		v.push_back(columns[34] + " = " + std::to_string(e.mana_regen_score));
		v.push_back(columns[35] + " = " + std::to_string(e.end_regen_score));
		v.push_back(columns[36] + " = " + std::to_string(e.haste_score));
		v.push_back(columns[37] + " = " + std::to_string(e.agi_score));
		v.push_back(columns[38] + " = " + std::to_string(e.cha_score));
		v.push_back(columns[39] + " = " + std::to_string(e.dex_score));
		v.push_back(columns[40] + " = " + std::to_string(e.int_score));
		v.push_back(columns[41] + " = " + std::to_string(e.sta_score));
		v.push_back(columns[42] + " = " + std::to_string(e.str_score));
		v.push_back(columns[43] + " = " + std::to_string(e.wis_score));
		v.push_back(columns[44] + " = " + std::to_string(e.cr_score));
		v.push_back(columns[45] + " = " + std::to_string(e.dr_score));
		v.push_back(columns[46] + " = " + std::to_string(e.fr_score));
		v.push_back(columns[47] + " = " + std::to_string(e.mr_score));
		v.push_back(columns[48] + " = " + std::to_string(e.pr_score));
		v.push_back(columns[49] + " = " + std::to_string(e.corruption_score));
		v.push_back(columns[50] + " = " + std::to_string(e.h_agi_score));
		v.push_back(columns[51] + " = " + std::to_string(e.h_cha_score));
		v.push_back(columns[52] + " = " + std::to_string(e.h_dex_score));
		v.push_back(columns[53] + " = " + std::to_string(e.h_int_score));
		v.push_back(columns[54] + " = " + std::to_string(e.h_sta_score));
		v.push_back(columns[55] + " = " + std::to_string(e.h_str_score));
		v.push_back(columns[56] + " = " + std::to_string(e.h_wis_score));
		v.push_back(columns[57] + " = " + std::to_string(e.h_cr_score));
		v.push_back(columns[58] + " = " + std::to_string(e.h_dr_score));
		v.push_back(columns[59] + " = " + std::to_string(e.h_fr_score));
		v.push_back(columns[60] + " = " + std::to_string(e.h_mr_score));
		v.push_back(columns[61] + " = " + std::to_string(e.h_pr_score));
		v.push_back(columns[62] + " = " + std::to_string(e.h_corruption_score));
		v.push_back(columns[63] + " = " + std::to_string(e.accuracy_score));
		v.push_back(columns[64] + " = " + std::to_string(e.attack_score));
		v.push_back(columns[65] + " = " + std::to_string(e.avoidance_score));
		v.push_back(columns[66] + " = " + std::to_string(e.combat_effects_score));
		v.push_back(columns[67] + " = " + std::to_string(e.damage_shield_score));
		v.push_back(columns[68] + " = " + std::to_string(e.dot_shielding_score));
		v.push_back(columns[69] + " = " + std::to_string(e.shielding_score));
		v.push_back(columns[70] + " = " + std::to_string(e.spellshield_score));
		v.push_back(columns[71] + " = " + std::to_string(e.strikethrough_score));
		v.push_back(columns[72] + " = " + std::to_string(e.stunresist_score));
		v.push_back(columns[73] + " = " + std::to_string(e.ds_mitigation_score));
		v.push_back(columns[74] + " = " + std::to_string(e.heal_amount_score));
		v.push_back(columns[75] + " = " + std::to_string(e.spell_damage_score));
		v.push_back(columns[76] + " = " + std::to_string(e.clairvoyance_score));
		v.push_back(columns[77] + " = " + std::to_string(e.backstab_damage_score));
		v.push_back(columns[78] + " = " + std::to_string(e.skillmod_score));
		v.push_back(columns[79] + " = " + std::to_string(e.extra_skill_score));
		v.push_back(columns[80] + " = " + std::to_string(e.bane_dmg_score));
		v.push_back(columns[81] + " = " + std::to_string(e.elemental_dmg_score));
		v.push_back(columns[82] + " = " + std::to_string(e.bard_type_score));
		v.push_back(columns[83] + " = " + std::to_string(e.proc_effect_score));
		v.push_back(columns[84] + " = " + std::to_string(e.worn_effect_score));
		v.push_back(columns[85] + " = " + std::to_string(e.focus_effect_score));
		v.push_back(columns[86] + " = " + std::to_string(e.click_effect_score));

		auto results = db.QueryDatabase(
			fmt::format(
				"UPDATE {} SET {} WHERE {} = {}",
				TableName(),
				Strings::Implode(", ", v),
				PrimaryKey(),
				e.id
			)
		);

		return (results.Success() ? results.RowsAffected() : 0);
	}

	static VeqGearScore InsertOne(
		Database& db,
		VeqGearScore e
	)
	{
		std::vector<std::string> v;

		v.push_back(std::to_string(e.id));
		v.push_back("'" + Strings::Escape(e.name) + "'");
		v.push_back(std::to_string(e.common_drop));
		v.push_back(std::to_string(e.rare_drop));
		v.push_back(std::to_string(e.raid_drop));
		v.push_back(std::to_string(e.avg));
		v.push_back(std::to_string(e.high));
		v.push_back(std::to_string(e.low));
		v.push_back(std::to_string(e.classes1));
		v.push_back(std::to_string(e.classes2));
		v.push_back(std::to_string(e.classes3));
		v.push_back(std::to_string(e.classes4));
		v.push_back(std::to_string(e.classes5));
		v.push_back(std::to_string(e.classes6));
		v.push_back(std::to_string(e.classes7));
		v.push_back(std::to_string(e.classes8));
		v.push_back(std::to_string(e.classes9));
		v.push_back(std::to_string(e.classes10));
		v.push_back(std::to_string(e.classes11));
		v.push_back(std::to_string(e.classes12));
		v.push_back(std::to_string(e.classes13));
		v.push_back(std::to_string(e.classes14));
		v.push_back(std::to_string(e.classes15));
		v.push_back(std::to_string(e.classes16));
		v.push_back(std::to_string(e.damage_score));
		v.push_back(std::to_string(e.delay_score));
		v.push_back(std::to_string(e.ratio_score));
		v.push_back(std::to_string(e.weapon_score));
		v.push_back(std::to_string(e.arrow_score));
		v.push_back(std::to_string(e.ac_score));
		v.push_back(std::to_string(e.hp_score));
		v.push_back(std::to_string(e.mana_score));
		v.push_back(std::to_string(e.end_score));
		v.push_back(std::to_string(e.hp_regen_score));
		v.push_back(std::to_string(e.mana_regen_score));
		v.push_back(std::to_string(e.end_regen_score));
		v.push_back(std::to_string(e.haste_score));
		v.push_back(std::to_string(e.agi_score));
		v.push_back(std::to_string(e.cha_score));
		v.push_back(std::to_string(e.dex_score));
		v.push_back(std::to_string(e.int_score));
		v.push_back(std::to_string(e.sta_score));
		v.push_back(std::to_string(e.str_score));
		v.push_back(std::to_string(e.wis_score));
		v.push_back(std::to_string(e.cr_score));
		v.push_back(std::to_string(e.dr_score));
		v.push_back(std::to_string(e.fr_score));
		v.push_back(std::to_string(e.mr_score));
		v.push_back(std::to_string(e.pr_score));
		v.push_back(std::to_string(e.corruption_score));
		v.push_back(std::to_string(e.h_agi_score));
		v.push_back(std::to_string(e.h_cha_score));
		v.push_back(std::to_string(e.h_dex_score));
		v.push_back(std::to_string(e.h_int_score));
		v.push_back(std::to_string(e.h_sta_score));
		v.push_back(std::to_string(e.h_str_score));
		v.push_back(std::to_string(e.h_wis_score));
		v.push_back(std::to_string(e.h_cr_score));
		v.push_back(std::to_string(e.h_dr_score));
		v.push_back(std::to_string(e.h_fr_score));
		v.push_back(std::to_string(e.h_mr_score));
		v.push_back(std::to_string(e.h_pr_score));
		v.push_back(std::to_string(e.h_corruption_score));
		v.push_back(std::to_string(e.accuracy_score));
		v.push_back(std::to_string(e.attack_score));
		v.push_back(std::to_string(e.avoidance_score));
		v.push_back(std::to_string(e.combat_effects_score));
		v.push_back(std::to_string(e.damage_shield_score));
		v.push_back(std::to_string(e.dot_shielding_score));
		v.push_back(std::to_string(e.shielding_score));
		v.push_back(std::to_string(e.spellshield_score));
		v.push_back(std::to_string(e.strikethrough_score));
		v.push_back(std::to_string(e.stunresist_score));
		v.push_back(std::to_string(e.ds_mitigation_score));
		v.push_back(std::to_string(e.heal_amount_score));
		v.push_back(std::to_string(e.spell_damage_score));
		v.push_back(std::to_string(e.clairvoyance_score));
		v.push_back(std::to_string(e.backstab_damage_score));
		v.push_back(std::to_string(e.skillmod_score));
		v.push_back(std::to_string(e.extra_skill_score));
		v.push_back(std::to_string(e.bane_dmg_score));
		v.push_back(std::to_string(e.elemental_dmg_score));
		v.push_back(std::to_string(e.bard_type_score));
		v.push_back(std::to_string(e.proc_effect_score));
		v.push_back(std::to_string(e.worn_effect_score));
		v.push_back(std::to_string(e.focus_effect_score));
		v.push_back(std::to_string(e.click_effect_score));

		auto results = db.QueryDatabase(
			fmt::format(
				"{} VALUES ({})",
				BaseInsert(),
				Strings::Implode(",", v)
			)
		);

		if (results.Success()) {
			e.id = results.LastInsertedID();
			return e;
		}

		e = NewEntity();

		return e;
	}

	static int InsertMany(
		Database& db,
		const std::vector<VeqGearScore> &entries
	)
	{
		std::vector<std::string> insert_chunks;

		for (auto &e: entries) {
			std::vector<std::string> v;

			v.push_back(std::to_string(e.id));
			v.push_back("'" + Strings::Escape(e.name) + "'");
			v.push_back(std::to_string(e.common_drop));
			v.push_back(std::to_string(e.rare_drop));
			v.push_back(std::to_string(e.raid_drop));
			v.push_back(std::to_string(e.avg));
			v.push_back(std::to_string(e.high));
			v.push_back(std::to_string(e.low));
			v.push_back(std::to_string(e.classes1));
			v.push_back(std::to_string(e.classes2));
			v.push_back(std::to_string(e.classes3));
			v.push_back(std::to_string(e.classes4));
			v.push_back(std::to_string(e.classes5));
			v.push_back(std::to_string(e.classes6));
			v.push_back(std::to_string(e.classes7));
			v.push_back(std::to_string(e.classes8));
			v.push_back(std::to_string(e.classes9));
			v.push_back(std::to_string(e.classes10));
			v.push_back(std::to_string(e.classes11));
			v.push_back(std::to_string(e.classes12));
			v.push_back(std::to_string(e.classes13));
			v.push_back(std::to_string(e.classes14));
			v.push_back(std::to_string(e.classes15));
			v.push_back(std::to_string(e.classes16));
			v.push_back(std::to_string(e.damage_score));
			v.push_back(std::to_string(e.delay_score));
			v.push_back(std::to_string(e.ratio_score));
			v.push_back(std::to_string(e.weapon_score));
			v.push_back(std::to_string(e.arrow_score));
			v.push_back(std::to_string(e.ac_score));
			v.push_back(std::to_string(e.hp_score));
			v.push_back(std::to_string(e.mana_score));
			v.push_back(std::to_string(e.end_score));
			v.push_back(std::to_string(e.hp_regen_score));
			v.push_back(std::to_string(e.mana_regen_score));
			v.push_back(std::to_string(e.end_regen_score));
			v.push_back(std::to_string(e.haste_score));
			v.push_back(std::to_string(e.agi_score));
			v.push_back(std::to_string(e.cha_score));
			v.push_back(std::to_string(e.dex_score));
			v.push_back(std::to_string(e.int_score));
			v.push_back(std::to_string(e.sta_score));
			v.push_back(std::to_string(e.str_score));
			v.push_back(std::to_string(e.wis_score));
			v.push_back(std::to_string(e.cr_score));
			v.push_back(std::to_string(e.dr_score));
			v.push_back(std::to_string(e.fr_score));
			v.push_back(std::to_string(e.mr_score));
			v.push_back(std::to_string(e.pr_score));
			v.push_back(std::to_string(e.corruption_score));
			v.push_back(std::to_string(e.h_agi_score));
			v.push_back(std::to_string(e.h_cha_score));
			v.push_back(std::to_string(e.h_dex_score));
			v.push_back(std::to_string(e.h_int_score));
			v.push_back(std::to_string(e.h_sta_score));
			v.push_back(std::to_string(e.h_str_score));
			v.push_back(std::to_string(e.h_wis_score));
			v.push_back(std::to_string(e.h_cr_score));
			v.push_back(std::to_string(e.h_dr_score));
			v.push_back(std::to_string(e.h_fr_score));
			v.push_back(std::to_string(e.h_mr_score));
			v.push_back(std::to_string(e.h_pr_score));
			v.push_back(std::to_string(e.h_corruption_score));
			v.push_back(std::to_string(e.accuracy_score));
			v.push_back(std::to_string(e.attack_score));
			v.push_back(std::to_string(e.avoidance_score));
			v.push_back(std::to_string(e.combat_effects_score));
			v.push_back(std::to_string(e.damage_shield_score));
			v.push_back(std::to_string(e.dot_shielding_score));
			v.push_back(std::to_string(e.shielding_score));
			v.push_back(std::to_string(e.spellshield_score));
			v.push_back(std::to_string(e.strikethrough_score));
			v.push_back(std::to_string(e.stunresist_score));
			v.push_back(std::to_string(e.ds_mitigation_score));
			v.push_back(std::to_string(e.heal_amount_score));
			v.push_back(std::to_string(e.spell_damage_score));
			v.push_back(std::to_string(e.clairvoyance_score));
			v.push_back(std::to_string(e.backstab_damage_score));
			v.push_back(std::to_string(e.skillmod_score));
			v.push_back(std::to_string(e.extra_skill_score));
			v.push_back(std::to_string(e.bane_dmg_score));
			v.push_back(std::to_string(e.elemental_dmg_score));
			v.push_back(std::to_string(e.bard_type_score));
			v.push_back(std::to_string(e.proc_effect_score));
			v.push_back(std::to_string(e.worn_effect_score));
			v.push_back(std::to_string(e.focus_effect_score));
			v.push_back(std::to_string(e.click_effect_score));

			insert_chunks.push_back("(" + Strings::Implode(",", v) + ")");
		}

		std::vector<std::string> v;

		auto results = db.QueryDatabase(
			fmt::format(
				"{} VALUES {}",
				BaseInsert(),
				Strings::Implode(",", insert_chunks)
			)
		);

		return (results.Success() ? results.RowsAffected() : 0);
	}

	static std::vector<VeqGearScore> All(Database& db)
	{
		std::vector<VeqGearScore> all_entries;

		auto results = db.QueryDatabase(
			fmt::format(
				"{}",
				BaseSelect()
			)
		);

		all_entries.reserve(results.RowCount());

		for (auto row = results.begin(); row != results.end(); ++row) {
			VeqGearScore e{};

			e.id                    = row[0] ? static_cast<uint32_t>(strtoul(row[0], nullptr, 10)) : 0;
			e.name                  = row[1] ? row[1] : "";
			e.common_drop           = row[2] ? static_cast<uint8_t>(strtoul(row[2], nullptr, 10)) : 0;
			e.rare_drop             = row[3] ? static_cast<uint8_t>(strtoul(row[3], nullptr, 10)) : 0;
			e.raid_drop             = row[4] ? static_cast<uint8_t>(strtoul(row[4], nullptr, 10)) : 0;
			e.avg                   = row[5] ? (strtof(row[5], nullptr) > 0.0f ? strtof(row[5], nullptr) : 0) : 0;
			e.high                  = row[6] ? strtof(row[6], nullptr) : 0;
			e.low                   = row[7] ? strtof(row[7], nullptr) : 0;
			e.classes1              = row[8] ? strtof(row[8], nullptr) : 0;
			e.classes2              = row[9] ? strtof(row[9], nullptr) : 0;
			e.classes3              = row[10] ? strtof(row[10], nullptr) : 0;
			e.classes4              = row[11] ? strtof(row[11], nullptr) : 0;
			e.classes5              = row[12] ? strtof(row[12], nullptr) : 0;
			e.classes6              = row[13] ? strtof(row[13], nullptr) : 0;
			e.classes7              = row[14] ? strtof(row[14], nullptr) : 0;
			e.classes8              = row[15] ? strtof(row[15], nullptr) : 0;
			e.classes9              = row[16] ? strtof(row[16], nullptr) : 0;
			e.classes10             = row[17] ? strtof(row[17], nullptr) : 0;
			e.classes11             = row[18] ? strtof(row[18], nullptr) : 0;
			e.classes12             = row[19] ? strtof(row[19], nullptr) : 0;
			e.classes13             = row[20] ? strtof(row[20], nullptr) : 0;
			e.classes14             = row[21] ? strtof(row[21], nullptr) : 0;
			e.classes15             = row[22] ? strtof(row[22], nullptr) : 0;
			e.classes16             = row[23] ? strtof(row[23], nullptr) : 0;
			e.damage_score          = row[24] ? strtof(row[24], nullptr) : 0;
			e.delay_score           = row[25] ? strtof(row[25], nullptr) : 0;
			e.ratio_score           = row[26] ? strtof(row[26], nullptr) : 0;
			e.weapon_score          = row[27] ? strtof(row[27], nullptr) : 0;
			e.arrow_score           = row[28] ? strtof(row[28], nullptr) : 0;
			e.ac_score              = row[29] ? strtof(row[29], nullptr) : 0;
			e.hp_score              = row[30] ? strtof(row[30], nullptr) : 0;
			e.mana_score            = row[31] ? strtof(row[31], nullptr) : 0;
			e.end_score             = row[32] ? strtof(row[32], nullptr) : 0;
			e.hp_regen_score        = row[33] ? strtof(row[33], nullptr) : 0;
			e.mana_regen_score      = row[34] ? strtof(row[34], nullptr) : 0;
			e.end_regen_score       = row[35] ? strtof(row[35], nullptr) : 0;
			e.haste_score           = row[36] ? strtof(row[36], nullptr) : 0;
			e.agi_score             = row[37] ? strtof(row[37], nullptr) : 0;
			e.cha_score             = row[38] ? strtof(row[38], nullptr) : 0;
			e.dex_score             = row[39] ? strtof(row[39], nullptr) : 0;
			e.int_score             = row[40] ? strtof(row[40], nullptr) : 0;
			e.sta_score             = row[41] ? strtof(row[41], nullptr) : 0;
			e.str_score             = row[42] ? strtof(row[42], nullptr) : 0;
			e.wis_score             = row[43] ? strtof(row[43], nullptr) : 0;
			e.cr_score              = row[44] ? strtof(row[44], nullptr) : 0;
			e.dr_score              = row[45] ? strtof(row[45], nullptr) : 0;
			e.fr_score              = row[46] ? strtof(row[46], nullptr) : 0;
			e.mr_score              = row[47] ? strtof(row[47], nullptr) : 0;
			e.pr_score              = row[48] ? strtof(row[48], nullptr) : 0;
			e.corruption_score      = row[49] ? strtof(row[49], nullptr) : 0;
			e.h_agi_score           = row[50] ? strtof(row[50], nullptr) : 0;
			e.h_cha_score           = row[51] ? strtof(row[51], nullptr) : 0;
			e.h_dex_score           = row[52] ? strtof(row[52], nullptr) : 0;
			e.h_int_score           = row[53] ? strtof(row[53], nullptr) : 0;
			e.h_sta_score           = row[54] ? strtof(row[54], nullptr) : 0;
			e.h_str_score           = row[55] ? strtof(row[55], nullptr) : 0;
			e.h_wis_score           = row[56] ? strtof(row[56], nullptr) : 0;
			e.h_cr_score            = row[57] ? strtof(row[57], nullptr) : 0;
			e.h_dr_score            = row[58] ? strtof(row[58], nullptr) : 0;
			e.h_fr_score            = row[59] ? strtof(row[59], nullptr) : 0;
			e.h_mr_score            = row[60] ? strtof(row[60], nullptr) : 0;
			e.h_pr_score            = row[61] ? strtof(row[61], nullptr) : 0;
			e.h_corruption_score    = row[62] ? strtof(row[62], nullptr) : 0;
			e.accuracy_score        = row[63] ? strtof(row[63], nullptr) : 0;
			e.attack_score          = row[64] ? strtof(row[64], nullptr) : 0;
			e.avoidance_score       = row[65] ? strtof(row[65], nullptr) : 0;
			e.combat_effects_score  = row[66] ? strtof(row[66], nullptr) : 0;
			e.damage_shield_score   = row[67] ? strtof(row[67], nullptr) : 0;
			e.dot_shielding_score   = row[68] ? strtof(row[68], nullptr) : 0;
			e.shielding_score       = row[69] ? strtof(row[69], nullptr) : 0;
			e.spellshield_score     = row[70] ? strtof(row[70], nullptr) : 0;
			e.strikethrough_score   = row[71] ? strtof(row[71], nullptr) : 0;
			e.stunresist_score      = row[72] ? strtof(row[72], nullptr) : 0;
			e.ds_mitigation_score   = row[73] ? strtof(row[73], nullptr) : 0;
			e.heal_amount_score     = row[74] ? strtof(row[74], nullptr) : 0;
			e.spell_damage_score    = row[75] ? strtof(row[75], nullptr) : 0;
			e.clairvoyance_score    = row[76] ? strtof(row[76], nullptr) : 0;
			e.backstab_damage_score = row[77] ? strtof(row[77], nullptr) : 0;
			e.skillmod_score        = row[78] ? strtof(row[78], nullptr) : 0;
			e.extra_skill_score     = row[79] ? strtof(row[79], nullptr) : 0;
			e.bane_dmg_score        = row[80] ? strtof(row[80], nullptr) : 0;
			e.elemental_dmg_score   = row[81] ? strtof(row[81], nullptr) : 0;
			e.bard_type_score       = row[82] ? strtof(row[82], nullptr) : 0;
			e.proc_effect_score     = row[83] ? strtof(row[83], nullptr) : 0;
			e.worn_effect_score     = row[84] ? strtof(row[84], nullptr) : 0;
			e.focus_effect_score    = row[85] ? strtof(row[85], nullptr) : 0;
			e.click_effect_score    = row[86] ? strtof(row[86], nullptr) : 0;

			all_entries.push_back(e);
		}

		return all_entries;
	}

	static std::vector<VeqGearScore> GetWhere(Database& db, const std::string &where_filter)
	{
		std::vector<VeqGearScore> all_entries;

		auto results = db.QueryDatabase(
			fmt::format(
				"{} WHERE {}",
				BaseSelect(),
				where_filter
			)
		);

		all_entries.reserve(results.RowCount());

		for (auto row = results.begin(); row != results.end(); ++row) {
			VeqGearScore e{};

			e.id                    = row[0] ? static_cast<uint32_t>(strtoul(row[0], nullptr, 10)) : 0;
			e.name                  = row[1] ? row[1] : "";
			e.common_drop           = row[2] ? static_cast<uint8_t>(strtoul(row[2], nullptr, 10)) : 0;
			e.rare_drop             = row[3] ? static_cast<uint8_t>(strtoul(row[3], nullptr, 10)) : 0;
			e.raid_drop             = row[4] ? static_cast<uint8_t>(strtoul(row[4], nullptr, 10)) : 0;
			e.avg                   = row[5] ? (strtof(row[5], nullptr) > 0.0f ? strtof(row[5], nullptr) : 0) : 0;
			e.high                  = row[6] ? strtof(row[6], nullptr) : 0;
			e.low                   = row[7] ? strtof(row[7], nullptr) : 0;
			e.classes1              = row[8] ? strtof(row[8], nullptr) : 0;
			e.classes2              = row[9] ? strtof(row[9], nullptr) : 0;
			e.classes3              = row[10] ? strtof(row[10], nullptr) : 0;
			e.classes4              = row[11] ? strtof(row[11], nullptr) : 0;
			e.classes5              = row[12] ? strtof(row[12], nullptr) : 0;
			e.classes6              = row[13] ? strtof(row[13], nullptr) : 0;
			e.classes7              = row[14] ? strtof(row[14], nullptr) : 0;
			e.classes8              = row[15] ? strtof(row[15], nullptr) : 0;
			e.classes9              = row[16] ? strtof(row[16], nullptr) : 0;
			e.classes10             = row[17] ? strtof(row[17], nullptr) : 0;
			e.classes11             = row[18] ? strtof(row[18], nullptr) : 0;
			e.classes12             = row[19] ? strtof(row[19], nullptr) : 0;
			e.classes13             = row[20] ? strtof(row[20], nullptr) : 0;
			e.classes14             = row[21] ? strtof(row[21], nullptr) : 0;
			e.classes15             = row[22] ? strtof(row[22], nullptr) : 0;
			e.classes16             = row[23] ? strtof(row[23], nullptr) : 0;
			e.damage_score          = row[24] ? strtof(row[24], nullptr) : 0;
			e.delay_score           = row[25] ? strtof(row[25], nullptr) : 0;
			e.ratio_score           = row[26] ? strtof(row[26], nullptr) : 0;
			e.weapon_score          = row[27] ? strtof(row[27], nullptr) : 0;
			e.arrow_score           = row[28] ? strtof(row[28], nullptr) : 0;
			e.ac_score              = row[29] ? strtof(row[29], nullptr) : 0;
			e.hp_score              = row[30] ? strtof(row[30], nullptr) : 0;
			e.mana_score            = row[31] ? strtof(row[31], nullptr) : 0;
			e.end_score             = row[32] ? strtof(row[32], nullptr) : 0;
			e.hp_regen_score        = row[33] ? strtof(row[33], nullptr) : 0;
			e.mana_regen_score      = row[34] ? strtof(row[34], nullptr) : 0;
			e.end_regen_score       = row[35] ? strtof(row[35], nullptr) : 0;
			e.haste_score           = row[36] ? strtof(row[36], nullptr) : 0;
			e.agi_score             = row[37] ? strtof(row[37], nullptr) : 0;
			e.cha_score             = row[38] ? strtof(row[38], nullptr) : 0;
			e.dex_score             = row[39] ? strtof(row[39], nullptr) : 0;
			e.int_score             = row[40] ? strtof(row[40], nullptr) : 0;
			e.sta_score             = row[41] ? strtof(row[41], nullptr) : 0;
			e.str_score             = row[42] ? strtof(row[42], nullptr) : 0;
			e.wis_score             = row[43] ? strtof(row[43], nullptr) : 0;
			e.cr_score              = row[44] ? strtof(row[44], nullptr) : 0;
			e.dr_score              = row[45] ? strtof(row[45], nullptr) : 0;
			e.fr_score              = row[46] ? strtof(row[46], nullptr) : 0;
			e.mr_score              = row[47] ? strtof(row[47], nullptr) : 0;
			e.pr_score              = row[48] ? strtof(row[48], nullptr) : 0;
			e.corruption_score      = row[49] ? strtof(row[49], nullptr) : 0;
			e.h_agi_score           = row[50] ? strtof(row[50], nullptr) : 0;
			e.h_cha_score           = row[51] ? strtof(row[51], nullptr) : 0;
			e.h_dex_score           = row[52] ? strtof(row[52], nullptr) : 0;
			e.h_int_score           = row[53] ? strtof(row[53], nullptr) : 0;
			e.h_sta_score           = row[54] ? strtof(row[54], nullptr) : 0;
			e.h_str_score           = row[55] ? strtof(row[55], nullptr) : 0;
			e.h_wis_score           = row[56] ? strtof(row[56], nullptr) : 0;
			e.h_cr_score            = row[57] ? strtof(row[57], nullptr) : 0;
			e.h_dr_score            = row[58] ? strtof(row[58], nullptr) : 0;
			e.h_fr_score            = row[59] ? strtof(row[59], nullptr) : 0;
			e.h_mr_score            = row[60] ? strtof(row[60], nullptr) : 0;
			e.h_pr_score            = row[61] ? strtof(row[61], nullptr) : 0;
			e.h_corruption_score    = row[62] ? strtof(row[62], nullptr) : 0;
			e.accuracy_score        = row[63] ? strtof(row[63], nullptr) : 0;
			e.attack_score          = row[64] ? strtof(row[64], nullptr) : 0;
			e.avoidance_score       = row[65] ? strtof(row[65], nullptr) : 0;
			e.combat_effects_score  = row[66] ? strtof(row[66], nullptr) : 0;
			e.damage_shield_score   = row[67] ? strtof(row[67], nullptr) : 0;
			e.dot_shielding_score   = row[68] ? strtof(row[68], nullptr) : 0;
			e.shielding_score       = row[69] ? strtof(row[69], nullptr) : 0;
			e.spellshield_score     = row[70] ? strtof(row[70], nullptr) : 0;
			e.strikethrough_score   = row[71] ? strtof(row[71], nullptr) : 0;
			e.stunresist_score      = row[72] ? strtof(row[72], nullptr) : 0;
			e.ds_mitigation_score   = row[73] ? strtof(row[73], nullptr) : 0;
			e.heal_amount_score     = row[74] ? strtof(row[74], nullptr) : 0;
			e.spell_damage_score    = row[75] ? strtof(row[75], nullptr) : 0;
			e.clairvoyance_score    = row[76] ? strtof(row[76], nullptr) : 0;
			e.backstab_damage_score = row[77] ? strtof(row[77], nullptr) : 0;
			e.skillmod_score        = row[78] ? strtof(row[78], nullptr) : 0;
			e.extra_skill_score     = row[79] ? strtof(row[79], nullptr) : 0;
			e.bane_dmg_score        = row[80] ? strtof(row[80], nullptr) : 0;
			e.elemental_dmg_score   = row[81] ? strtof(row[81], nullptr) : 0;
			e.bard_type_score       = row[82] ? strtof(row[82], nullptr) : 0;
			e.proc_effect_score     = row[83] ? strtof(row[83], nullptr) : 0;
			e.worn_effect_score     = row[84] ? strtof(row[84], nullptr) : 0;
			e.focus_effect_score    = row[85] ? strtof(row[85], nullptr) : 0;
			e.click_effect_score    = row[86] ? strtof(row[86], nullptr) : 0;

			all_entries.push_back(e);
		}

		return all_entries;
	}

	static int DeleteWhere(Database& db, const std::string &where_filter)
	{
		auto results = db.QueryDatabase(
			fmt::format(
				"DELETE FROM {} WHERE {}",
				TableName(),
				where_filter
			)
		);

		return (results.Success() ? results.RowsAffected() : 0);
	}

	static int Truncate(Database& db)
	{
		auto results = db.QueryDatabase(
			fmt::format(
				"TRUNCATE TABLE {}",
				TableName()
			)
		);

		return (results.Success() ? results.RowsAffected() : 0);
	}

	static int64 GetMaxId(Database& db)
	{
		auto results = db.QueryDatabase(
			fmt::format(
				"SELECT COALESCE(MAX({}), 0) FROM {}",
				PrimaryKey(),
				TableName()
			)
		);

		return (results.Success() && results.begin()[0] ? strtoll(results.begin()[0], nullptr, 10) : 0);
	}

	static int64 Count(Database& db, const std::string &where_filter = "")
	{
		auto results = db.QueryDatabase(
			fmt::format(
				"SELECT COUNT(*) FROM {} {}",
				TableName(),
				(where_filter.empty() ? "" : "WHERE " + where_filter)
			)
		);

		return (results.Success() && results.begin()[0] ? strtoll(results.begin()[0], nullptr, 10) : 0);
	}

	static std::string BaseReplace()
	{
		return fmt::format(
			"REPLACE INTO {} ({}) ",
			TableName(),
			ColumnsRaw()
		);
	}

	static int ReplaceOne(
		Database& db,
		const VeqGearScore &e
	)
	{
		std::vector<std::string> v;

		v.push_back(std::to_string(e.id));
		v.push_back("'" + Strings::Escape(e.name) + "'");
		v.push_back(std::to_string(e.common_drop));
		v.push_back(std::to_string(e.rare_drop));
		v.push_back(std::to_string(e.raid_drop));
		v.push_back(std::to_string(e.avg));
		v.push_back(std::to_string(e.high));
		v.push_back(std::to_string(e.low));
		v.push_back(std::to_string(e.classes1));
		v.push_back(std::to_string(e.classes2));
		v.push_back(std::to_string(e.classes3));
		v.push_back(std::to_string(e.classes4));
		v.push_back(std::to_string(e.classes5));
		v.push_back(std::to_string(e.classes6));
		v.push_back(std::to_string(e.classes7));
		v.push_back(std::to_string(e.classes8));
		v.push_back(std::to_string(e.classes9));
		v.push_back(std::to_string(e.classes10));
		v.push_back(std::to_string(e.classes11));
		v.push_back(std::to_string(e.classes12));
		v.push_back(std::to_string(e.classes13));
		v.push_back(std::to_string(e.classes14));
		v.push_back(std::to_string(e.classes15));
		v.push_back(std::to_string(e.classes16));
		v.push_back(std::to_string(e.damage_score));
		v.push_back(std::to_string(e.delay_score));
		v.push_back(std::to_string(e.ratio_score));
		v.push_back(std::to_string(e.weapon_score));
		v.push_back(std::to_string(e.arrow_score));
		v.push_back(std::to_string(e.ac_score));
		v.push_back(std::to_string(e.hp_score));
		v.push_back(std::to_string(e.mana_score));
		v.push_back(std::to_string(e.end_score));
		v.push_back(std::to_string(e.hp_regen_score));
		v.push_back(std::to_string(e.mana_regen_score));
		v.push_back(std::to_string(e.end_regen_score));
		v.push_back(std::to_string(e.haste_score));
		v.push_back(std::to_string(e.agi_score));
		v.push_back(std::to_string(e.cha_score));
		v.push_back(std::to_string(e.dex_score));
		v.push_back(std::to_string(e.int_score));
		v.push_back(std::to_string(e.sta_score));
		v.push_back(std::to_string(e.str_score));
		v.push_back(std::to_string(e.wis_score));
		v.push_back(std::to_string(e.cr_score));
		v.push_back(std::to_string(e.dr_score));
		v.push_back(std::to_string(e.fr_score));
		v.push_back(std::to_string(e.mr_score));
		v.push_back(std::to_string(e.pr_score));
		v.push_back(std::to_string(e.corruption_score));
		v.push_back(std::to_string(e.h_agi_score));
		v.push_back(std::to_string(e.h_cha_score));
		v.push_back(std::to_string(e.h_dex_score));
		v.push_back(std::to_string(e.h_int_score));
		v.push_back(std::to_string(e.h_sta_score));
		v.push_back(std::to_string(e.h_str_score));
		v.push_back(std::to_string(e.h_wis_score));
		v.push_back(std::to_string(e.h_cr_score));
		v.push_back(std::to_string(e.h_dr_score));
		v.push_back(std::to_string(e.h_fr_score));
		v.push_back(std::to_string(e.h_mr_score));
		v.push_back(std::to_string(e.h_pr_score));
		v.push_back(std::to_string(e.h_corruption_score));
		v.push_back(std::to_string(e.accuracy_score));
		v.push_back(std::to_string(e.attack_score));
		v.push_back(std::to_string(e.avoidance_score));
		v.push_back(std::to_string(e.combat_effects_score));
		v.push_back(std::to_string(e.damage_shield_score));
		v.push_back(std::to_string(e.dot_shielding_score));
		v.push_back(std::to_string(e.shielding_score));
		v.push_back(std::to_string(e.spellshield_score));
		v.push_back(std::to_string(e.strikethrough_score));
		v.push_back(std::to_string(e.stunresist_score));
		v.push_back(std::to_string(e.ds_mitigation_score));
		v.push_back(std::to_string(e.heal_amount_score));
		v.push_back(std::to_string(e.spell_damage_score));
		v.push_back(std::to_string(e.clairvoyance_score));
		v.push_back(std::to_string(e.backstab_damage_score));
		v.push_back(std::to_string(e.skillmod_score));
		v.push_back(std::to_string(e.extra_skill_score));
		v.push_back(std::to_string(e.bane_dmg_score));
		v.push_back(std::to_string(e.elemental_dmg_score));
		v.push_back(std::to_string(e.bard_type_score));
		v.push_back(std::to_string(e.proc_effect_score));
		v.push_back(std::to_string(e.worn_effect_score));
		v.push_back(std::to_string(e.focus_effect_score));
		v.push_back(std::to_string(e.click_effect_score));

		auto results = db.QueryDatabase(
			fmt::format(
				"{} VALUES ({})",
				BaseReplace(),
				Strings::Implode(",", v)
			)
		);

		return (results.Success() ? results.RowsAffected() : 0);
	}

	static int ReplaceMany(
		Database& db,
		const std::vector<VeqGearScore> &entries
	)
	{
		std::vector<std::string> insert_chunks;

		for (auto &e: entries) {
			std::vector<std::string> v;

			v.push_back(std::to_string(e.id));
			v.push_back("'" + Strings::Escape(e.name) + "'");
			v.push_back(std::to_string(e.common_drop));
			v.push_back(std::to_string(e.rare_drop));
			v.push_back(std::to_string(e.raid_drop));
			v.push_back(std::to_string(e.avg));
			v.push_back(std::to_string(e.high));
			v.push_back(std::to_string(e.low));
			v.push_back(std::to_string(e.classes1));
			v.push_back(std::to_string(e.classes2));
			v.push_back(std::to_string(e.classes3));
			v.push_back(std::to_string(e.classes4));
			v.push_back(std::to_string(e.classes5));
			v.push_back(std::to_string(e.classes6));
			v.push_back(std::to_string(e.classes7));
			v.push_back(std::to_string(e.classes8));
			v.push_back(std::to_string(e.classes9));
			v.push_back(std::to_string(e.classes10));
			v.push_back(std::to_string(e.classes11));
			v.push_back(std::to_string(e.classes12));
			v.push_back(std::to_string(e.classes13));
			v.push_back(std::to_string(e.classes14));
			v.push_back(std::to_string(e.classes15));
			v.push_back(std::to_string(e.classes16));
			v.push_back(std::to_string(e.damage_score));
			v.push_back(std::to_string(e.delay_score));
			v.push_back(std::to_string(e.ratio_score));
			v.push_back(std::to_string(e.weapon_score));
			v.push_back(std::to_string(e.arrow_score));
			v.push_back(std::to_string(e.ac_score));
			v.push_back(std::to_string(e.hp_score));
			v.push_back(std::to_string(e.mana_score));
			v.push_back(std::to_string(e.end_score));
			v.push_back(std::to_string(e.hp_regen_score));
			v.push_back(std::to_string(e.mana_regen_score));
			v.push_back(std::to_string(e.end_regen_score));
			v.push_back(std::to_string(e.haste_score));
			v.push_back(std::to_string(e.agi_score));
			v.push_back(std::to_string(e.cha_score));
			v.push_back(std::to_string(e.dex_score));
			v.push_back(std::to_string(e.int_score));
			v.push_back(std::to_string(e.sta_score));
			v.push_back(std::to_string(e.str_score));
			v.push_back(std::to_string(e.wis_score));
			v.push_back(std::to_string(e.cr_score));
			v.push_back(std::to_string(e.dr_score));
			v.push_back(std::to_string(e.fr_score));
			v.push_back(std::to_string(e.mr_score));
			v.push_back(std::to_string(e.pr_score));
			v.push_back(std::to_string(e.corruption_score));
			v.push_back(std::to_string(e.h_agi_score));
			v.push_back(std::to_string(e.h_cha_score));
			v.push_back(std::to_string(e.h_dex_score));
			v.push_back(std::to_string(e.h_int_score));
			v.push_back(std::to_string(e.h_sta_score));
			v.push_back(std::to_string(e.h_str_score));
			v.push_back(std::to_string(e.h_wis_score));
			v.push_back(std::to_string(e.h_cr_score));
			v.push_back(std::to_string(e.h_dr_score));
			v.push_back(std::to_string(e.h_fr_score));
			v.push_back(std::to_string(e.h_mr_score));
			v.push_back(std::to_string(e.h_pr_score));
			v.push_back(std::to_string(e.h_corruption_score));
			v.push_back(std::to_string(e.accuracy_score));
			v.push_back(std::to_string(e.attack_score));
			v.push_back(std::to_string(e.avoidance_score));
			v.push_back(std::to_string(e.combat_effects_score));
			v.push_back(std::to_string(e.damage_shield_score));
			v.push_back(std::to_string(e.dot_shielding_score));
			v.push_back(std::to_string(e.shielding_score));
			v.push_back(std::to_string(e.spellshield_score));
			v.push_back(std::to_string(e.strikethrough_score));
			v.push_back(std::to_string(e.stunresist_score));
			v.push_back(std::to_string(e.ds_mitigation_score));
			v.push_back(std::to_string(e.heal_amount_score));
			v.push_back(std::to_string(e.spell_damage_score));
			v.push_back(std::to_string(e.clairvoyance_score));
			v.push_back(std::to_string(e.backstab_damage_score));
			v.push_back(std::to_string(e.skillmod_score));
			v.push_back(std::to_string(e.extra_skill_score));
			v.push_back(std::to_string(e.bane_dmg_score));
			v.push_back(std::to_string(e.elemental_dmg_score));
			v.push_back(std::to_string(e.bard_type_score));
			v.push_back(std::to_string(e.proc_effect_score));
			v.push_back(std::to_string(e.worn_effect_score));
			v.push_back(std::to_string(e.focus_effect_score));
			v.push_back(std::to_string(e.click_effect_score));

			insert_chunks.push_back("(" + Strings::Implode(",", v) + ")");
		}

		std::vector<std::string> v;

		auto results = db.QueryDatabase(
			fmt::format(
				"{} VALUES {}",
				BaseReplace(),
				Strings::Implode(",", insert_chunks)
			)
		);

		return (results.Success() ? results.RowsAffected() : 0);
	}
};

#endif //EQEMU_BASE_VEQ_GEAR_SCORE_REPOSITORY_H
