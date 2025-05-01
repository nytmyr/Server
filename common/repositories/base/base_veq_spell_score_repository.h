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
		int32_t     id;
		std::string name;
		int32_t     spell_level;
		float       spell_score;
		std::string spell_type;
		float       dd_score;
		float       dot_score;
		float       slow_score;
		float       ae_rain_score;
		float       lifetap_score;
		float       heal_score;
		float       hot_score;
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
			"spell_level",
			"spell_score",
			"spell_type",
			"dd_score",
			"dot_score",
			"slow_score",
			"ae_rain_score",
			"lifetap_score",
			"heal_score",
			"hot_score",
		};
	}

	static std::vector<std::string> SelectColumns()
	{
		return {
			"id",
			"name",
			"spell_level",
			"spell_score",
			"spell_type",
			"dd_score",
			"dot_score",
			"slow_score",
			"ae_rain_score",
			"lifetap_score",
			"heal_score",
			"hot_score",
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

		e.id            = 0;
		e.name          = "";
		e.spell_level   = 0;
		e.spell_score   = 0;
		e.spell_type    = "Unknown";
		e.dd_score      = 0;
		e.dot_score     = 0;
		e.slow_score    = 0;
		e.ae_rain_score = 0;
		e.lifetap_score = 0;
		e.heal_score    = 0;
		e.hot_score     = 0;

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

			e.id            = row[0] ? static_cast<int32_t>(atoi(row[0])) : 0;
			e.name          = row[1] ? row[1] : "";
			e.spell_level   = row[2] ? static_cast<int32_t>(atoi(row[2])) : 0;
			e.spell_score   = row[3] ? strtof(row[3], nullptr) : 0;
			e.spell_type    = row[4] ? row[4] : "Unknown";
			e.dd_score      = row[5] ? strtof(row[5], nullptr) : 0;
			e.dot_score     = row[6] ? strtof(row[6], nullptr) : 0;
			e.slow_score    = row[7] ? strtof(row[7], nullptr) : 0;
			e.ae_rain_score = row[8] ? strtof(row[8], nullptr) : 0;
			e.lifetap_score = row[9] ? strtof(row[9], nullptr) : 0;
			e.heal_score    = row[10] ? strtof(row[10], nullptr) : 0;
			e.hot_score     = row[11] ? strtof(row[11], nullptr) : 0;

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
		v.push_back(columns[2] + " = " + std::to_string(e.spell_level));
		v.push_back(columns[3] + " = " + std::to_string(e.spell_score));
		v.push_back(columns[4] + " = '" + Strings::Escape(e.spell_type) + "'");
		v.push_back(columns[5] + " = " + std::to_string(e.dd_score));
		v.push_back(columns[6] + " = " + std::to_string(e.dot_score));
		v.push_back(columns[7] + " = " + std::to_string(e.slow_score));
		v.push_back(columns[8] + " = " + std::to_string(e.ae_rain_score));
		v.push_back(columns[9] + " = " + std::to_string(e.lifetap_score));
		v.push_back(columns[10] + " = " + std::to_string(e.heal_score));
		v.push_back(columns[11] + " = " + std::to_string(e.hot_score));

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
		v.push_back(std::to_string(e.spell_level));
		v.push_back(std::to_string(e.spell_score));
		v.push_back("'" + Strings::Escape(e.spell_type) + "'");
		v.push_back(std::to_string(e.dd_score));
		v.push_back(std::to_string(e.dot_score));
		v.push_back(std::to_string(e.slow_score));
		v.push_back(std::to_string(e.ae_rain_score));
		v.push_back(std::to_string(e.lifetap_score));
		v.push_back(std::to_string(e.heal_score));
		v.push_back(std::to_string(e.hot_score));

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
			v.push_back(std::to_string(e.spell_level));
			v.push_back(std::to_string(e.spell_score));
			v.push_back("'" + Strings::Escape(e.spell_type) + "'");
			v.push_back(std::to_string(e.dd_score));
			v.push_back(std::to_string(e.dot_score));
			v.push_back(std::to_string(e.slow_score));
			v.push_back(std::to_string(e.ae_rain_score));
			v.push_back(std::to_string(e.lifetap_score));
			v.push_back(std::to_string(e.heal_score));
			v.push_back(std::to_string(e.hot_score));

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

			e.id            = row[0] ? static_cast<int32_t>(atoi(row[0])) : 0;
			e.name          = row[1] ? row[1] : "";
			e.spell_level   = row[2] ? static_cast<int32_t>(atoi(row[2])) : 0;
			e.spell_score   = row[3] ? strtof(row[3], nullptr) : 0;
			e.spell_type    = row[4] ? row[4] : "Unknown";
			e.dd_score      = row[5] ? strtof(row[5], nullptr) : 0;
			e.dot_score     = row[6] ? strtof(row[6], nullptr) : 0;
			e.slow_score    = row[7] ? strtof(row[7], nullptr) : 0;
			e.ae_rain_score = row[8] ? strtof(row[8], nullptr) : 0;
			e.lifetap_score = row[9] ? strtof(row[9], nullptr) : 0;
			e.heal_score    = row[10] ? strtof(row[10], nullptr) : 0;
			e.hot_score     = row[11] ? strtof(row[11], nullptr) : 0;

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

			e.id            = row[0] ? static_cast<int32_t>(atoi(row[0])) : 0;
			e.name          = row[1] ? row[1] : "";
			e.spell_level   = row[2] ? static_cast<int32_t>(atoi(row[2])) : 0;
			e.spell_score   = row[3] ? strtof(row[3], nullptr) : 0;
			e.spell_type    = row[4] ? row[4] : "Unknown";
			e.dd_score      = row[5] ? strtof(row[5], nullptr) : 0;
			e.dot_score     = row[6] ? strtof(row[6], nullptr) : 0;
			e.slow_score    = row[7] ? strtof(row[7], nullptr) : 0;
			e.ae_rain_score = row[8] ? strtof(row[8], nullptr) : 0;
			e.lifetap_score = row[9] ? strtof(row[9], nullptr) : 0;
			e.heal_score    = row[10] ? strtof(row[10], nullptr) : 0;
			e.hot_score     = row[11] ? strtof(row[11], nullptr) : 0;

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
		v.push_back(std::to_string(e.spell_level));
		v.push_back(std::to_string(e.spell_score));
		v.push_back("'" + Strings::Escape(e.spell_type) + "'");
		v.push_back(std::to_string(e.dd_score));
		v.push_back(std::to_string(e.dot_score));
		v.push_back(std::to_string(e.slow_score));
		v.push_back(std::to_string(e.ae_rain_score));
		v.push_back(std::to_string(e.lifetap_score));
		v.push_back(std::to_string(e.heal_score));
		v.push_back(std::to_string(e.hot_score));

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
			v.push_back(std::to_string(e.spell_level));
			v.push_back(std::to_string(e.spell_score));
			v.push_back("'" + Strings::Escape(e.spell_type) + "'");
			v.push_back(std::to_string(e.dd_score));
			v.push_back(std::to_string(e.dot_score));
			v.push_back(std::to_string(e.slow_score));
			v.push_back(std::to_string(e.ae_rain_score));
			v.push_back(std::to_string(e.lifetap_score));
			v.push_back(std::to_string(e.heal_score));
			v.push_back(std::to_string(e.hot_score));

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
