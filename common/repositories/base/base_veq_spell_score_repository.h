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

#ifndef EQEMU_BASE_VEQ_SPELL_SCORE_REPOSITORY_H
#define EQEMU_BASE_VEQ_SPELL_SCORE_REPOSITORY_H

#include "../../database.h"
#include "../../strings.h"
#include <ctime>

class BaseVeqSpellScoreRepository {
public:
	struct VeqSpellScore {
		uint16_t    id;
		std::string name;
		uint8_t     lowest_spell_level;
		uint8_t     lowest_class;
		float       spell_score;
		std::string spell_type;
		float       duration;
		uint8_t     target_type;
		float       pbae_multiplier;
		float       ae_multiplier;
		float       group_multiplier;
		float       recourse_score;
		float       heal_score;
		float       mana_score;
		float       stamina_score;
		float       dd_score;
		float       ae_rain_score;
		float       hot_score;
		float       mana_dd_score;
		float       stamina_dd_score;
		float       dot_score;
		float       mot_score;
		float       sot_score;
		float       m_dot_score;
		float       s_dot_score;
		float       haste_score;
		float       slow_score;
		float       lifetap_score;
		float       manatap_score;
		float       staminatap_score;
		float       ac_buff_score;
		float       ac_debuff_score;
		float       atk_buff_score;
		float       atk_debuff_score;
		float       movement_speed_score;
		float       snare_score;
		float       base_stat_score;
		float       base_stat_debuff_score;
		float       invis_score;
		float       see_invis_score;
		float       water_breathing_score;
		float       pacify_score;
		float       add_faction_score;
		float       blind_score;
		float       stun_score;
		float       charm_score;
		float       fear_score;
		float       bind_affinity_score;
		float       gate_score;
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
			"lowest_spell_level",
			"lowest_class",
			"spell_score",
			"spell_type",
			"duration",
			"target_type",
			"pbae_multiplier",
			"ae_multiplier",
			"group_multiplier",
			"recourse_score",
			"heal_score",
			"mana_score",
			"stamina_score",
			"dd_score",
			"ae_rain_score",
			"hot_score",
			"mana_dd_score",
			"stamina_dd_score",
			"dot_score",
			"mot_score",
			"sot_score",
			"m_dot_score",
			"s_dot_score",
			"haste_score",
			"slow_score",
			"lifetap_score",
			"manatap_score",
			"staminatap_score",
			"ac_buff_score",
			"ac_debuff_score",
			"atk_buff_score",
			"atk_debuff_score",
			"movement_speed_score",
			"snare_score",
			"base_stat_score",
			"base_stat_debuff_score",
			"invis_score",
			"see_invis_score",
			"water_breathing_score",
			"pacify_score",
			"add_faction_score",
			"blind_score",
			"stun_score",
			"charm_score",
			"fear_score",
			"bind_affinity_score",
			"gate_score",
		};
	}

	static std::vector<std::string> SelectColumns()
	{
		return {
			"id",
			"name",
			"lowest_spell_level",
			"lowest_class",
			"spell_score",
			"spell_type",
			"duration",
			"target_type",
			"pbae_multiplier",
			"ae_multiplier",
			"group_multiplier",
			"recourse_score",
			"heal_score",
			"mana_score",
			"stamina_score",
			"dd_score",
			"ae_rain_score",
			"hot_score",
			"mana_dd_score",
			"stamina_dd_score",
			"dot_score",
			"mot_score",
			"sot_score",
			"m_dot_score",
			"s_dot_score",
			"haste_score",
			"slow_score",
			"lifetap_score",
			"manatap_score",
			"staminatap_score",
			"ac_buff_score",
			"ac_debuff_score",
			"atk_buff_score",
			"atk_debuff_score",
			"movement_speed_score",
			"snare_score",
			"base_stat_score",
			"base_stat_debuff_score",
			"invis_score",
			"see_invis_score",
			"water_breathing_score",
			"pacify_score",
			"add_faction_score",
			"blind_score",
			"stun_score",
			"charm_score",
			"fear_score",
			"bind_affinity_score",
			"gate_score",
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
		return std::string("veq_spell_score");
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

	static VeqSpellScore NewEntity()
	{
		VeqSpellScore e{};

		e.id                     = 0;
		e.name                   = "";
		e.lowest_spell_level     = 0;
		e.lowest_class           = 0;
		e.spell_score            = 0;
		e.spell_type             = "Unknown";
		e.duration               = 0;
		e.target_type            = 0;
		e.pbae_multiplier        = 0;
		e.ae_multiplier          = 0;
		e.group_multiplier       = 0;
		e.recourse_score         = 0;
		e.heal_score             = 0;
		e.mana_score             = 0;
		e.stamina_score          = 0;
		e.dd_score               = 0;
		e.ae_rain_score          = 0;
		e.hot_score              = 0;
		e.mana_dd_score          = 0;
		e.stamina_dd_score       = 0;
		e.dot_score              = 0;
		e.mot_score              = 0;
		e.sot_score              = 0;
		e.m_dot_score            = 0;
		e.s_dot_score            = 0;
		e.haste_score            = 0;
		e.slow_score             = 0;
		e.lifetap_score          = 0;
		e.manatap_score          = 0;
		e.staminatap_score       = 0;
		e.ac_buff_score          = 0;
		e.ac_debuff_score        = 0;
		e.atk_buff_score         = 0;
		e.atk_debuff_score       = 0;
		e.movement_speed_score   = 0;
		e.snare_score            = 0;
		e.base_stat_score        = 0;
		e.base_stat_debuff_score = 0;
		e.invis_score            = 0;
		e.see_invis_score        = 0;
		e.water_breathing_score  = 0;
		e.pacify_score           = 0;
		e.add_faction_score      = 0;
		e.blind_score            = 0;
		e.stun_score             = 0;
		e.charm_score            = 0;
		e.fear_score             = 0;
		e.bind_affinity_score    = 0;
		e.gate_score             = 0;

		return e;
	}

	static VeqSpellScore GetVeqSpellScore(
		const std::vector<VeqSpellScore> &veq_spell_scores,
		int veq_spell_score_id
	)
	{
		for (auto &veq_spell_score : veq_spell_scores) {
			if (veq_spell_score.id == veq_spell_score_id) {
				return veq_spell_score;
			}
		}

		return NewEntity();
	}

	static VeqSpellScore FindOne(
		Database& db,
		int veq_spell_score_id
	)
	{
		auto results = db.QueryDatabase(
			fmt::format(
				"{} WHERE {} = {} LIMIT 1",
				BaseSelect(),
				PrimaryKey(),
				veq_spell_score_id
			)
		);

		auto row = results.begin();
		if (results.RowCount() == 1) {
			VeqSpellScore e{};

			e.id                     = row[0] ? static_cast<uint16_t>(strtoul(row[0], nullptr, 10)) : 0;
			e.name                   = row[1] ? row[1] : "";
			e.lowest_spell_level     = row[2] ? static_cast<uint8_t>(strtoul(row[2], nullptr, 10)) : 0;
			e.lowest_class           = row[3] ? static_cast<uint8_t>(strtoul(row[3], nullptr, 10)) : 0;
			e.spell_score            = row[4] ? strtof(row[4], nullptr) : 0;
			e.spell_type             = row[5] ? row[5] : "Unknown";
			e.duration               = row[6] ? strtof(row[6], nullptr) : 0;
			e.target_type            = row[7] ? static_cast<uint8_t>(strtoul(row[7], nullptr, 10)) : 0;
			e.pbae_multiplier        = row[8] ? strtof(row[8], nullptr) : 0;
			e.ae_multiplier          = row[9] ? strtof(row[9], nullptr) : 0;
			e.group_multiplier       = row[10] ? strtof(row[10], nullptr) : 0;
			e.recourse_score         = row[11] ? strtof(row[11], nullptr) : 0;
			e.heal_score             = row[12] ? strtof(row[12], nullptr) : 0;
			e.mana_score             = row[13] ? strtof(row[13], nullptr) : 0;
			e.stamina_score          = row[14] ? strtof(row[14], nullptr) : 0;
			e.dd_score               = row[15] ? strtof(row[15], nullptr) : 0;
			e.ae_rain_score          = row[16] ? strtof(row[16], nullptr) : 0;
			e.hot_score              = row[17] ? strtof(row[17], nullptr) : 0;
			e.mana_dd_score          = row[18] ? strtof(row[18], nullptr) : 0;
			e.stamina_dd_score       = row[19] ? strtof(row[19], nullptr) : 0;
			e.dot_score              = row[20] ? strtof(row[20], nullptr) : 0;
			e.mot_score              = row[21] ? strtof(row[21], nullptr) : 0;
			e.sot_score              = row[22] ? strtof(row[22], nullptr) : 0;
			e.m_dot_score            = row[23] ? strtof(row[23], nullptr) : 0;
			e.s_dot_score            = row[24] ? strtof(row[24], nullptr) : 0;
			e.haste_score            = row[25] ? strtof(row[25], nullptr) : 0;
			e.slow_score             = row[26] ? strtof(row[26], nullptr) : 0;
			e.lifetap_score          = row[27] ? strtof(row[27], nullptr) : 0;
			e.manatap_score          = row[28] ? strtof(row[28], nullptr) : 0;
			e.staminatap_score       = row[29] ? strtof(row[29], nullptr) : 0;
			e.ac_buff_score          = row[30] ? strtof(row[30], nullptr) : 0;
			e.ac_debuff_score        = row[31] ? strtof(row[31], nullptr) : 0;
			e.atk_buff_score         = row[32] ? strtof(row[32], nullptr) : 0;
			e.atk_debuff_score       = row[33] ? strtof(row[33], nullptr) : 0;
			e.movement_speed_score   = row[34] ? strtof(row[34], nullptr) : 0;
			e.snare_score            = row[35] ? strtof(row[35], nullptr) : 0;
			e.base_stat_score        = row[36] ? strtof(row[36], nullptr) : 0;
			e.base_stat_debuff_score = row[37] ? strtof(row[37], nullptr) : 0;
			e.invis_score            = row[38] ? strtof(row[38], nullptr) : 0;
			e.see_invis_score        = row[39] ? strtof(row[39], nullptr) : 0;
			e.water_breathing_score  = row[40] ? strtof(row[40], nullptr) : 0;
			e.pacify_score           = row[41] ? strtof(row[41], nullptr) : 0;
			e.add_faction_score      = row[42] ? strtof(row[42], nullptr) : 0;
			e.blind_score            = row[43] ? strtof(row[43], nullptr) : 0;
			e.stun_score             = row[44] ? strtof(row[44], nullptr) : 0;
			e.charm_score            = row[45] ? strtof(row[45], nullptr) : 0;
			e.fear_score             = row[46] ? strtof(row[46], nullptr) : 0;
			e.bind_affinity_score    = row[47] ? strtof(row[47], nullptr) : 0;
			e.gate_score             = row[48] ? strtof(row[48], nullptr) : 0;

			return e;
		}

		return NewEntity();
	}

	static int DeleteOne(
		Database& db,
		int veq_spell_score_id
	)
	{
		auto results = db.QueryDatabase(
			fmt::format(
				"DELETE FROM {} WHERE {} = {}",
				TableName(),
				PrimaryKey(),
				veq_spell_score_id
			)
		);

		return (results.Success() ? results.RowsAffected() : 0);
	}

	static int UpdateOne(
		Database& db,
		const VeqSpellScore &e
	)
	{
		std::vector<std::string> v;

		auto columns = Columns();

		v.push_back(columns[0] + " = " + std::to_string(e.id));
		v.push_back(columns[1] + " = '" + Strings::Escape(e.name) + "'");
		v.push_back(columns[2] + " = " + std::to_string(e.lowest_spell_level));
		v.push_back(columns[3] + " = " + std::to_string(e.lowest_class));
		v.push_back(columns[4] + " = " + std::to_string(e.spell_score));
		v.push_back(columns[5] + " = '" + Strings::Escape(e.spell_type) + "'");
		v.push_back(columns[6] + " = " + std::to_string(e.duration));
		v.push_back(columns[7] + " = " + std::to_string(e.target_type));
		v.push_back(columns[8] + " = " + std::to_string(e.pbae_multiplier));
		v.push_back(columns[9] + " = " + std::to_string(e.ae_multiplier));
		v.push_back(columns[10] + " = " + std::to_string(e.group_multiplier));
		v.push_back(columns[11] + " = " + std::to_string(e.recourse_score));
		v.push_back(columns[12] + " = " + std::to_string(e.heal_score));
		v.push_back(columns[13] + " = " + std::to_string(e.mana_score));
		v.push_back(columns[14] + " = " + std::to_string(e.stamina_score));
		v.push_back(columns[15] + " = " + std::to_string(e.dd_score));
		v.push_back(columns[16] + " = " + std::to_string(e.ae_rain_score));
		v.push_back(columns[17] + " = " + std::to_string(e.hot_score));
		v.push_back(columns[18] + " = " + std::to_string(e.mana_dd_score));
		v.push_back(columns[19] + " = " + std::to_string(e.stamina_dd_score));
		v.push_back(columns[20] + " = " + std::to_string(e.dot_score));
		v.push_back(columns[21] + " = " + std::to_string(e.mot_score));
		v.push_back(columns[22] + " = " + std::to_string(e.sot_score));
		v.push_back(columns[23] + " = " + std::to_string(e.m_dot_score));
		v.push_back(columns[24] + " = " + std::to_string(e.s_dot_score));
		v.push_back(columns[25] + " = " + std::to_string(e.haste_score));
		v.push_back(columns[26] + " = " + std::to_string(e.slow_score));
		v.push_back(columns[27] + " = " + std::to_string(e.lifetap_score));
		v.push_back(columns[28] + " = " + std::to_string(e.manatap_score));
		v.push_back(columns[29] + " = " + std::to_string(e.staminatap_score));
		v.push_back(columns[30] + " = " + std::to_string(e.ac_buff_score));
		v.push_back(columns[31] + " = " + std::to_string(e.ac_debuff_score));
		v.push_back(columns[32] + " = " + std::to_string(e.atk_buff_score));
		v.push_back(columns[33] + " = " + std::to_string(e.atk_debuff_score));
		v.push_back(columns[34] + " = " + std::to_string(e.movement_speed_score));
		v.push_back(columns[35] + " = " + std::to_string(e.snare_score));
		v.push_back(columns[36] + " = " + std::to_string(e.base_stat_score));
		v.push_back(columns[37] + " = " + std::to_string(e.base_stat_debuff_score));
		v.push_back(columns[38] + " = " + std::to_string(e.invis_score));
		v.push_back(columns[39] + " = " + std::to_string(e.see_invis_score));
		v.push_back(columns[40] + " = " + std::to_string(e.water_breathing_score));
		v.push_back(columns[41] + " = " + std::to_string(e.pacify_score));
		v.push_back(columns[42] + " = " + std::to_string(e.add_faction_score));
		v.push_back(columns[43] + " = " + std::to_string(e.blind_score));
		v.push_back(columns[44] + " = " + std::to_string(e.stun_score));
		v.push_back(columns[45] + " = " + std::to_string(e.charm_score));
		v.push_back(columns[46] + " = " + std::to_string(e.fear_score));
		v.push_back(columns[47] + " = " + std::to_string(e.bind_affinity_score));
		v.push_back(columns[48] + " = " + std::to_string(e.gate_score));

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

	static VeqSpellScore InsertOne(
		Database& db,
		VeqSpellScore e
	)
	{
		std::vector<std::string> v;

		v.push_back(std::to_string(e.id));
		v.push_back("'" + Strings::Escape(e.name) + "'");
		v.push_back(std::to_string(e.lowest_spell_level));
		v.push_back(std::to_string(e.lowest_class));
		v.push_back(std::to_string(e.spell_score));
		v.push_back("'" + Strings::Escape(e.spell_type) + "'");
		v.push_back(std::to_string(e.duration));
		v.push_back(std::to_string(e.target_type));
		v.push_back(std::to_string(e.pbae_multiplier));
		v.push_back(std::to_string(e.ae_multiplier));
		v.push_back(std::to_string(e.group_multiplier));
		v.push_back(std::to_string(e.recourse_score));
		v.push_back(std::to_string(e.heal_score));
		v.push_back(std::to_string(e.mana_score));
		v.push_back(std::to_string(e.stamina_score));
		v.push_back(std::to_string(e.dd_score));
		v.push_back(std::to_string(e.ae_rain_score));
		v.push_back(std::to_string(e.hot_score));
		v.push_back(std::to_string(e.mana_dd_score));
		v.push_back(std::to_string(e.stamina_dd_score));
		v.push_back(std::to_string(e.dot_score));
		v.push_back(std::to_string(e.mot_score));
		v.push_back(std::to_string(e.sot_score));
		v.push_back(std::to_string(e.m_dot_score));
		v.push_back(std::to_string(e.s_dot_score));
		v.push_back(std::to_string(e.haste_score));
		v.push_back(std::to_string(e.slow_score));
		v.push_back(std::to_string(e.lifetap_score));
		v.push_back(std::to_string(e.manatap_score));
		v.push_back(std::to_string(e.staminatap_score));
		v.push_back(std::to_string(e.ac_buff_score));
		v.push_back(std::to_string(e.ac_debuff_score));
		v.push_back(std::to_string(e.atk_buff_score));
		v.push_back(std::to_string(e.atk_debuff_score));
		v.push_back(std::to_string(e.movement_speed_score));
		v.push_back(std::to_string(e.snare_score));
		v.push_back(std::to_string(e.base_stat_score));
		v.push_back(std::to_string(e.base_stat_debuff_score));
		v.push_back(std::to_string(e.invis_score));
		v.push_back(std::to_string(e.see_invis_score));
		v.push_back(std::to_string(e.water_breathing_score));
		v.push_back(std::to_string(e.pacify_score));
		v.push_back(std::to_string(e.add_faction_score));
		v.push_back(std::to_string(e.blind_score));
		v.push_back(std::to_string(e.stun_score));
		v.push_back(std::to_string(e.charm_score));
		v.push_back(std::to_string(e.fear_score));
		v.push_back(std::to_string(e.bind_affinity_score));
		v.push_back(std::to_string(e.gate_score));

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
		const std::vector<VeqSpellScore> &entries
	)
	{
		std::vector<std::string> insert_chunks;

		for (auto &e: entries) {
			std::vector<std::string> v;

			v.push_back(std::to_string(e.id));
			v.push_back("'" + Strings::Escape(e.name) + "'");
			v.push_back(std::to_string(e.lowest_spell_level));
			v.push_back(std::to_string(e.lowest_class));
			v.push_back(std::to_string(e.spell_score));
			v.push_back("'" + Strings::Escape(e.spell_type) + "'");
			v.push_back(std::to_string(e.duration));
			v.push_back(std::to_string(e.target_type));
			v.push_back(std::to_string(e.pbae_multiplier));
			v.push_back(std::to_string(e.ae_multiplier));
			v.push_back(std::to_string(e.group_multiplier));
			v.push_back(std::to_string(e.recourse_score));
			v.push_back(std::to_string(e.heal_score));
			v.push_back(std::to_string(e.mana_score));
			v.push_back(std::to_string(e.stamina_score));
			v.push_back(std::to_string(e.dd_score));
			v.push_back(std::to_string(e.ae_rain_score));
			v.push_back(std::to_string(e.hot_score));
			v.push_back(std::to_string(e.mana_dd_score));
			v.push_back(std::to_string(e.stamina_dd_score));
			v.push_back(std::to_string(e.dot_score));
			v.push_back(std::to_string(e.mot_score));
			v.push_back(std::to_string(e.sot_score));
			v.push_back(std::to_string(e.m_dot_score));
			v.push_back(std::to_string(e.s_dot_score));
			v.push_back(std::to_string(e.haste_score));
			v.push_back(std::to_string(e.slow_score));
			v.push_back(std::to_string(e.lifetap_score));
			v.push_back(std::to_string(e.manatap_score));
			v.push_back(std::to_string(e.staminatap_score));
			v.push_back(std::to_string(e.ac_buff_score));
			v.push_back(std::to_string(e.ac_debuff_score));
			v.push_back(std::to_string(e.atk_buff_score));
			v.push_back(std::to_string(e.atk_debuff_score));
			v.push_back(std::to_string(e.movement_speed_score));
			v.push_back(std::to_string(e.snare_score));
			v.push_back(std::to_string(e.base_stat_score));
			v.push_back(std::to_string(e.base_stat_debuff_score));
			v.push_back(std::to_string(e.invis_score));
			v.push_back(std::to_string(e.see_invis_score));
			v.push_back(std::to_string(e.water_breathing_score));
			v.push_back(std::to_string(e.pacify_score));
			v.push_back(std::to_string(e.add_faction_score));
			v.push_back(std::to_string(e.blind_score));
			v.push_back(std::to_string(e.stun_score));
			v.push_back(std::to_string(e.charm_score));
			v.push_back(std::to_string(e.fear_score));
			v.push_back(std::to_string(e.bind_affinity_score));
			v.push_back(std::to_string(e.gate_score));

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

	static std::vector<VeqSpellScore> All(Database& db)
	{
		std::vector<VeqSpellScore> all_entries;

		auto results = db.QueryDatabase(
			fmt::format(
				"{}",
				BaseSelect()
			)
		);

		all_entries.reserve(results.RowCount());

		for (auto row = results.begin(); row != results.end(); ++row) {
			VeqSpellScore e{};

			e.id                     = row[0] ? static_cast<uint16_t>(strtoul(row[0], nullptr, 10)) : 0;
			e.name                   = row[1] ? row[1] : "";
			e.lowest_spell_level     = row[2] ? static_cast<uint8_t>(strtoul(row[2], nullptr, 10)) : 0;
			e.lowest_class           = row[3] ? static_cast<uint8_t>(strtoul(row[3], nullptr, 10)) : 0;
			e.spell_score            = row[4] ? strtof(row[4], nullptr) : 0;
			e.spell_type             = row[5] ? row[5] : "Unknown";
			e.duration               = row[6] ? strtof(row[6], nullptr) : 0;
			e.target_type            = row[7] ? static_cast<uint8_t>(strtoul(row[7], nullptr, 10)) : 0;
			e.pbae_multiplier        = row[8] ? strtof(row[8], nullptr) : 0;
			e.ae_multiplier          = row[9] ? strtof(row[9], nullptr) : 0;
			e.group_multiplier       = row[10] ? strtof(row[10], nullptr) : 0;
			e.recourse_score         = row[11] ? strtof(row[11], nullptr) : 0;
			e.heal_score             = row[12] ? strtof(row[12], nullptr) : 0;
			e.mana_score             = row[13] ? strtof(row[13], nullptr) : 0;
			e.stamina_score          = row[14] ? strtof(row[14], nullptr) : 0;
			e.dd_score               = row[15] ? strtof(row[15], nullptr) : 0;
			e.ae_rain_score          = row[16] ? strtof(row[16], nullptr) : 0;
			e.hot_score              = row[17] ? strtof(row[17], nullptr) : 0;
			e.mana_dd_score          = row[18] ? strtof(row[18], nullptr) : 0;
			e.stamina_dd_score       = row[19] ? strtof(row[19], nullptr) : 0;
			e.dot_score              = row[20] ? strtof(row[20], nullptr) : 0;
			e.mot_score              = row[21] ? strtof(row[21], nullptr) : 0;
			e.sot_score              = row[22] ? strtof(row[22], nullptr) : 0;
			e.m_dot_score            = row[23] ? strtof(row[23], nullptr) : 0;
			e.s_dot_score            = row[24] ? strtof(row[24], nullptr) : 0;
			e.haste_score            = row[25] ? strtof(row[25], nullptr) : 0;
			e.slow_score             = row[26] ? strtof(row[26], nullptr) : 0;
			e.lifetap_score          = row[27] ? strtof(row[27], nullptr) : 0;
			e.manatap_score          = row[28] ? strtof(row[28], nullptr) : 0;
			e.staminatap_score       = row[29] ? strtof(row[29], nullptr) : 0;
			e.ac_buff_score          = row[30] ? strtof(row[30], nullptr) : 0;
			e.ac_debuff_score        = row[31] ? strtof(row[31], nullptr) : 0;
			e.atk_buff_score         = row[32] ? strtof(row[32], nullptr) : 0;
			e.atk_debuff_score       = row[33] ? strtof(row[33], nullptr) : 0;
			e.movement_speed_score   = row[34] ? strtof(row[34], nullptr) : 0;
			e.snare_score            = row[35] ? strtof(row[35], nullptr) : 0;
			e.base_stat_score        = row[36] ? strtof(row[36], nullptr) : 0;
			e.base_stat_debuff_score = row[37] ? strtof(row[37], nullptr) : 0;
			e.invis_score            = row[38] ? strtof(row[38], nullptr) : 0;
			e.see_invis_score        = row[39] ? strtof(row[39], nullptr) : 0;
			e.water_breathing_score  = row[40] ? strtof(row[40], nullptr) : 0;
			e.pacify_score           = row[41] ? strtof(row[41], nullptr) : 0;
			e.add_faction_score      = row[42] ? strtof(row[42], nullptr) : 0;
			e.blind_score            = row[43] ? strtof(row[43], nullptr) : 0;
			e.stun_score             = row[44] ? strtof(row[44], nullptr) : 0;
			e.charm_score            = row[45] ? strtof(row[45], nullptr) : 0;
			e.fear_score             = row[46] ? strtof(row[46], nullptr) : 0;
			e.bind_affinity_score    = row[47] ? strtof(row[47], nullptr) : 0;
			e.gate_score             = row[48] ? strtof(row[48], nullptr) : 0;

			all_entries.push_back(e);
		}

		return all_entries;
	}

	static std::vector<VeqSpellScore> GetWhere(Database& db, const std::string &where_filter)
	{
		std::vector<VeqSpellScore> all_entries;

		auto results = db.QueryDatabase(
			fmt::format(
				"{} WHERE {}",
				BaseSelect(),
				where_filter
			)
		);

		all_entries.reserve(results.RowCount());

		for (auto row = results.begin(); row != results.end(); ++row) {
			VeqSpellScore e{};

			e.id                     = row[0] ? static_cast<uint16_t>(strtoul(row[0], nullptr, 10)) : 0;
			e.name                   = row[1] ? row[1] : "";
			e.lowest_spell_level     = row[2] ? static_cast<uint8_t>(strtoul(row[2], nullptr, 10)) : 0;
			e.lowest_class           = row[3] ? static_cast<uint8_t>(strtoul(row[3], nullptr, 10)) : 0;
			e.spell_score            = row[4] ? strtof(row[4], nullptr) : 0;
			e.spell_type             = row[5] ? row[5] : "Unknown";
			e.duration               = row[6] ? strtof(row[6], nullptr) : 0;
			e.target_type            = row[7] ? static_cast<uint8_t>(strtoul(row[7], nullptr, 10)) : 0;
			e.pbae_multiplier        = row[8] ? strtof(row[8], nullptr) : 0;
			e.ae_multiplier          = row[9] ? strtof(row[9], nullptr) : 0;
			e.group_multiplier       = row[10] ? strtof(row[10], nullptr) : 0;
			e.recourse_score         = row[11] ? strtof(row[11], nullptr) : 0;
			e.heal_score             = row[12] ? strtof(row[12], nullptr) : 0;
			e.mana_score             = row[13] ? strtof(row[13], nullptr) : 0;
			e.stamina_score          = row[14] ? strtof(row[14], nullptr) : 0;
			e.dd_score               = row[15] ? strtof(row[15], nullptr) : 0;
			e.ae_rain_score          = row[16] ? strtof(row[16], nullptr) : 0;
			e.hot_score              = row[17] ? strtof(row[17], nullptr) : 0;
			e.mana_dd_score          = row[18] ? strtof(row[18], nullptr) : 0;
			e.stamina_dd_score       = row[19] ? strtof(row[19], nullptr) : 0;
			e.dot_score              = row[20] ? strtof(row[20], nullptr) : 0;
			e.mot_score              = row[21] ? strtof(row[21], nullptr) : 0;
			e.sot_score              = row[22] ? strtof(row[22], nullptr) : 0;
			e.m_dot_score            = row[23] ? strtof(row[23], nullptr) : 0;
			e.s_dot_score            = row[24] ? strtof(row[24], nullptr) : 0;
			e.haste_score            = row[25] ? strtof(row[25], nullptr) : 0;
			e.slow_score             = row[26] ? strtof(row[26], nullptr) : 0;
			e.lifetap_score          = row[27] ? strtof(row[27], nullptr) : 0;
			e.manatap_score          = row[28] ? strtof(row[28], nullptr) : 0;
			e.staminatap_score       = row[29] ? strtof(row[29], nullptr) : 0;
			e.ac_buff_score          = row[30] ? strtof(row[30], nullptr) : 0;
			e.ac_debuff_score        = row[31] ? strtof(row[31], nullptr) : 0;
			e.atk_buff_score         = row[32] ? strtof(row[32], nullptr) : 0;
			e.atk_debuff_score       = row[33] ? strtof(row[33], nullptr) : 0;
			e.movement_speed_score   = row[34] ? strtof(row[34], nullptr) : 0;
			e.snare_score            = row[35] ? strtof(row[35], nullptr) : 0;
			e.base_stat_score        = row[36] ? strtof(row[36], nullptr) : 0;
			e.base_stat_debuff_score = row[37] ? strtof(row[37], nullptr) : 0;
			e.invis_score            = row[38] ? strtof(row[38], nullptr) : 0;
			e.see_invis_score        = row[39] ? strtof(row[39], nullptr) : 0;
			e.water_breathing_score  = row[40] ? strtof(row[40], nullptr) : 0;
			e.pacify_score           = row[41] ? strtof(row[41], nullptr) : 0;
			e.add_faction_score      = row[42] ? strtof(row[42], nullptr) : 0;
			e.blind_score            = row[43] ? strtof(row[43], nullptr) : 0;
			e.stun_score             = row[44] ? strtof(row[44], nullptr) : 0;
			e.charm_score            = row[45] ? strtof(row[45], nullptr) : 0;
			e.fear_score             = row[46] ? strtof(row[46], nullptr) : 0;
			e.bind_affinity_score    = row[47] ? strtof(row[47], nullptr) : 0;
			e.gate_score             = row[48] ? strtof(row[48], nullptr) : 0;

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
		const VeqSpellScore &e
	)
	{
		std::vector<std::string> v;

		v.push_back(std::to_string(e.id));
		v.push_back("'" + Strings::Escape(e.name) + "'");
		v.push_back(std::to_string(e.lowest_spell_level));
		v.push_back(std::to_string(e.lowest_class));
		v.push_back(std::to_string(e.spell_score));
		v.push_back("'" + Strings::Escape(e.spell_type) + "'");
		v.push_back(std::to_string(e.duration));
		v.push_back(std::to_string(e.target_type));
		v.push_back(std::to_string(e.pbae_multiplier));
		v.push_back(std::to_string(e.ae_multiplier));
		v.push_back(std::to_string(e.group_multiplier));
		v.push_back(std::to_string(e.recourse_score));
		v.push_back(std::to_string(e.heal_score));
		v.push_back(std::to_string(e.mana_score));
		v.push_back(std::to_string(e.stamina_score));
		v.push_back(std::to_string(e.dd_score));
		v.push_back(std::to_string(e.ae_rain_score));
		v.push_back(std::to_string(e.hot_score));
		v.push_back(std::to_string(e.mana_dd_score));
		v.push_back(std::to_string(e.stamina_dd_score));
		v.push_back(std::to_string(e.dot_score));
		v.push_back(std::to_string(e.mot_score));
		v.push_back(std::to_string(e.sot_score));
		v.push_back(std::to_string(e.m_dot_score));
		v.push_back(std::to_string(e.s_dot_score));
		v.push_back(std::to_string(e.haste_score));
		v.push_back(std::to_string(e.slow_score));
		v.push_back(std::to_string(e.lifetap_score));
		v.push_back(std::to_string(e.manatap_score));
		v.push_back(std::to_string(e.staminatap_score));
		v.push_back(std::to_string(e.ac_buff_score));
		v.push_back(std::to_string(e.ac_debuff_score));
		v.push_back(std::to_string(e.atk_buff_score));
		v.push_back(std::to_string(e.atk_debuff_score));
		v.push_back(std::to_string(e.movement_speed_score));
		v.push_back(std::to_string(e.snare_score));
		v.push_back(std::to_string(e.base_stat_score));
		v.push_back(std::to_string(e.base_stat_debuff_score));
		v.push_back(std::to_string(e.invis_score));
		v.push_back(std::to_string(e.see_invis_score));
		v.push_back(std::to_string(e.water_breathing_score));
		v.push_back(std::to_string(e.pacify_score));
		v.push_back(std::to_string(e.add_faction_score));
		v.push_back(std::to_string(e.blind_score));
		v.push_back(std::to_string(e.stun_score));
		v.push_back(std::to_string(e.charm_score));
		v.push_back(std::to_string(e.fear_score));
		v.push_back(std::to_string(e.bind_affinity_score));
		v.push_back(std::to_string(e.gate_score));

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
		const std::vector<VeqSpellScore> &entries
	)
	{
		std::vector<std::string> insert_chunks;

		for (auto &e: entries) {
			std::vector<std::string> v;

			v.push_back(std::to_string(e.id));
			v.push_back("'" + Strings::Escape(e.name) + "'");
			v.push_back(std::to_string(e.lowest_spell_level));
			v.push_back(std::to_string(e.lowest_class));
			v.push_back(std::to_string(e.spell_score));
			v.push_back("'" + Strings::Escape(e.spell_type) + "'");
			v.push_back(std::to_string(e.duration));
			v.push_back(std::to_string(e.target_type));
			v.push_back(std::to_string(e.pbae_multiplier));
			v.push_back(std::to_string(e.ae_multiplier));
			v.push_back(std::to_string(e.group_multiplier));
			v.push_back(std::to_string(e.recourse_score));
			v.push_back(std::to_string(e.heal_score));
			v.push_back(std::to_string(e.mana_score));
			v.push_back(std::to_string(e.stamina_score));
			v.push_back(std::to_string(e.dd_score));
			v.push_back(std::to_string(e.ae_rain_score));
			v.push_back(std::to_string(e.hot_score));
			v.push_back(std::to_string(e.mana_dd_score));
			v.push_back(std::to_string(e.stamina_dd_score));
			v.push_back(std::to_string(e.dot_score));
			v.push_back(std::to_string(e.mot_score));
			v.push_back(std::to_string(e.sot_score));
			v.push_back(std::to_string(e.m_dot_score));
			v.push_back(std::to_string(e.s_dot_score));
			v.push_back(std::to_string(e.haste_score));
			v.push_back(std::to_string(e.slow_score));
			v.push_back(std::to_string(e.lifetap_score));
			v.push_back(std::to_string(e.manatap_score));
			v.push_back(std::to_string(e.staminatap_score));
			v.push_back(std::to_string(e.ac_buff_score));
			v.push_back(std::to_string(e.ac_debuff_score));
			v.push_back(std::to_string(e.atk_buff_score));
			v.push_back(std::to_string(e.atk_debuff_score));
			v.push_back(std::to_string(e.movement_speed_score));
			v.push_back(std::to_string(e.snare_score));
			v.push_back(std::to_string(e.base_stat_score));
			v.push_back(std::to_string(e.base_stat_debuff_score));
			v.push_back(std::to_string(e.invis_score));
			v.push_back(std::to_string(e.see_invis_score));
			v.push_back(std::to_string(e.water_breathing_score));
			v.push_back(std::to_string(e.pacify_score));
			v.push_back(std::to_string(e.add_faction_score));
			v.push_back(std::to_string(e.blind_score));
			v.push_back(std::to_string(e.stun_score));
			v.push_back(std::to_string(e.charm_score));
			v.push_back(std::to_string(e.fear_score));
			v.push_back(std::to_string(e.bind_affinity_score));
			v.push_back(std::to_string(e.gate_score));

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

#endif //EQEMU_BASE_VEQ_SPELL_SCORE_REPOSITORY_H
