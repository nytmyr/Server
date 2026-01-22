/**
 * DO NOT MODIFY THIS FILE
 *
 * This repository was automatically generated and is NOT to be modified directly.
 * Any repository modifications are meant to be made to the repository extending the base.
 * Any modifications to base repositories are to be made by the generator only
 *
 * @generator ./utils/scripts/generators/repository-generator.pl
 * @docs https://docs.eqemu.dev/developer/repositories
 */

#pragma once

#include "common/database.h"
#include "common/strings.h"

#include <ctime>

class BaseValeenItemsRepository {
public:
	struct ValeenItems {
		uint32_t    item_id;
		std::string item_name;
		uint32_t    slot;
		int16_t     faction_required;
		uint8_t     level_required;
		uint8_t     min_status;
		uint8_t     max_status;
		uint16_t    alt_currency_cost;
		int32_t     classes_required;
		int32_t     probability;
		std::string bucket_name;
		std::string bucket_value;
		uint8_t     bucket_comparison;
		int8_t      min_expansion;
		int8_t      max_expansion;
		std::string content_flags;
		std::string content_flags_disabled;
		float       GearScore;
		float       ShardScore;
		uint32_t    MinDropLevel;
		float       difficulty;
		uint32_t    lowest_drop_npc_id;
		uint8_t     raid_only;
		uint8_t     merchant_tier;
	};

	static std::string PrimaryKey()
	{
		return std::string("item_id");
	}

	static std::vector<std::string> Columns()
	{
		return {
			"item_id",
			"item_name",
			"slot",
			"faction_required",
			"level_required",
			"min_status",
			"max_status",
			"alt_currency_cost",
			"classes_required",
			"probability",
			"bucket_name",
			"bucket_value",
			"bucket_comparison",
			"min_expansion",
			"max_expansion",
			"content_flags",
			"content_flags_disabled",
			"GearScore",
			"ShardScore",
			"MinDropLevel",
			"difficulty",
			"lowest_drop_npc_id",
			"raid_only",
			"merchant_tier",
		};
	}

	static std::vector<std::string> SelectColumns()
	{
		return {
			"item_id",
			"item_name",
			"slot",
			"faction_required",
			"level_required",
			"min_status",
			"max_status",
			"alt_currency_cost",
			"classes_required",
			"probability",
			"bucket_name",
			"bucket_value",
			"bucket_comparison",
			"min_expansion",
			"max_expansion",
			"content_flags",
			"content_flags_disabled",
			"GearScore",
			"ShardScore",
			"MinDropLevel",
			"difficulty",
			"lowest_drop_npc_id",
			"raid_only",
			"merchant_tier",
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
		return std::string("valeen_items");
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

	static ValeenItems NewEntity()
	{
		ValeenItems e{};

		e.item_id                = 0;
		e.item_name              = "";
		e.slot                   = 0;
		e.faction_required       = -100;
		e.level_required         = 0;
		e.min_status             = 0;
		e.max_status             = 255;
		e.alt_currency_cost      = 0;
		e.classes_required       = 65535;
		e.probability            = 100;
		e.bucket_name            = "";
		e.bucket_value           = "";
		e.bucket_comparison      = 0;
		e.min_expansion          = -1;
		e.max_expansion          = -1;
		e.content_flags          = "";
		e.content_flags_disabled = "";
		e.GearScore              = 0;
		e.ShardScore             = 0;
		e.MinDropLevel           = 0;
		e.difficulty             = 0;
		e.lowest_drop_npc_id     = 0;
		e.raid_only              = 0;
		e.merchant_tier          = 0;

		return e;
	}

	static ValeenItems GetValeenItems(
		const std::vector<ValeenItems> &valeen_itemss,
		int valeen_items_id
	)
	{
		for (auto &valeen_items : valeen_itemss) {
			if (valeen_items.item_id == valeen_items_id) {
				return valeen_items;
			}
		}

		return NewEntity();
	}

	static ValeenItems FindOne(
		Database& db,
		int valeen_items_id
	)
	{
		auto results = db.QueryDatabase(
			fmt::format(
				"{} WHERE {} = {} LIMIT 1",
				BaseSelect(),
				PrimaryKey(),
				valeen_items_id
			)
		);

		auto row = results.begin();
		if (results.RowCount() == 1) {
			ValeenItems e{};

			e.item_id                = row[0] ? static_cast<uint32_t>(strtoul(row[0], nullptr, 10)) : 0;
			e.item_name              = row[1] ? row[1] : "";
			e.slot                   = row[2] ? static_cast<uint32_t>(strtoul(row[2], nullptr, 10)) : 0;
			e.faction_required       = row[3] ? static_cast<int16_t>(atoi(row[3])) : -100;
			e.level_required         = row[4] ? static_cast<uint8_t>(strtoul(row[4], nullptr, 10)) : 0;
			e.min_status             = row[5] ? static_cast<uint8_t>(strtoul(row[5], nullptr, 10)) : 0;
			e.max_status             = row[6] ? static_cast<uint8_t>(strtoul(row[6], nullptr, 10)) : 255;
			e.alt_currency_cost      = row[7] ? static_cast<uint16_t>(strtoul(row[7], nullptr, 10)) : 0;
			e.classes_required       = row[8] ? static_cast<int32_t>(atoi(row[8])) : 65535;
			e.probability            = row[9] ? static_cast<int32_t>(atoi(row[9])) : 100;
			e.bucket_name            = row[10] ? row[10] : "";
			e.bucket_value           = row[11] ? row[11] : "";
			e.bucket_comparison      = row[12] ? static_cast<uint8_t>(strtoul(row[12], nullptr, 10)) : 0;
			e.min_expansion          = row[13] ? static_cast<int8_t>(atoi(row[13])) : -1;
			e.max_expansion          = row[14] ? static_cast<int8_t>(atoi(row[14])) : -1;
			e.content_flags          = row[15] ? row[15] : "";
			e.content_flags_disabled = row[16] ? row[16] : "";
			e.GearScore              = row[17] ? (strtof(row[17], nullptr) > 0.0f ? strtof(row[17], nullptr) : 0) : 0;
			e.ShardScore             = row[18] ? (strtof(row[18], nullptr) > 0.0f ? strtof(row[18], nullptr) : 0) : 0;
			e.MinDropLevel           = row[19] ? static_cast<uint32_t>(strtoul(row[19], nullptr, 10)) : 0;
			e.difficulty             = row[20] ? (strtof(row[20], nullptr) > 0.0f ? strtof(row[20], nullptr) : 0) : 0;
			e.lowest_drop_npc_id     = row[21] ? static_cast<uint32_t>(strtoul(row[21], nullptr, 10)) : 0;
			e.raid_only              = row[22] ? static_cast<uint8_t>(strtoul(row[22], nullptr, 10)) : 0;
			e.merchant_tier          = row[23] ? static_cast<uint8_t>(strtoul(row[23], nullptr, 10)) : 0;

			return e;
		}

		return NewEntity();
	}

	static int DeleteOne(
		Database& db,
		int valeen_items_id
	)
	{
		auto results = db.QueryDatabase(
			fmt::format(
				"DELETE FROM {} WHERE {} = {}",
				TableName(),
				PrimaryKey(),
				valeen_items_id
			)
		);

		return (results.Success() ? results.RowsAffected() : 0);
	}

	static int UpdateOne(
		Database& db,
		const ValeenItems &e
	)
	{
		std::vector<std::string> v;

		auto columns = Columns();

		v.push_back(columns[0] + " = " + std::to_string(e.item_id));
		v.push_back(columns[1] + " = '" + Strings::Escape(e.item_name) + "'");
		v.push_back(columns[2] + " = " + std::to_string(e.slot));
		v.push_back(columns[3] + " = " + std::to_string(e.faction_required));
		v.push_back(columns[4] + " = " + std::to_string(e.level_required));
		v.push_back(columns[5] + " = " + std::to_string(e.min_status));
		v.push_back(columns[6] + " = " + std::to_string(e.max_status));
		v.push_back(columns[7] + " = " + std::to_string(e.alt_currency_cost));
		v.push_back(columns[8] + " = " + std::to_string(e.classes_required));
		v.push_back(columns[9] + " = " + std::to_string(e.probability));
		v.push_back(columns[10] + " = '" + Strings::Escape(e.bucket_name) + "'");
		v.push_back(columns[11] + " = '" + Strings::Escape(e.bucket_value) + "'");
		v.push_back(columns[12] + " = " + std::to_string(e.bucket_comparison));
		v.push_back(columns[13] + " = " + std::to_string(e.min_expansion));
		v.push_back(columns[14] + " = " + std::to_string(e.max_expansion));
		v.push_back(columns[15] + " = '" + Strings::Escape(e.content_flags) + "'");
		v.push_back(columns[16] + " = '" + Strings::Escape(e.content_flags_disabled) + "'");
		v.push_back(columns[17] + " = " + std::to_string(e.GearScore));
		v.push_back(columns[18] + " = " + std::to_string(e.ShardScore));
		v.push_back(columns[19] + " = " + std::to_string(e.MinDropLevel));
		v.push_back(columns[20] + " = " + std::to_string(e.difficulty));
		v.push_back(columns[21] + " = " + std::to_string(e.lowest_drop_npc_id));
		v.push_back(columns[22] + " = " + std::to_string(e.raid_only));
		v.push_back(columns[23] + " = " + std::to_string(e.merchant_tier));

		auto results = db.QueryDatabase(
			fmt::format(
				"UPDATE {} SET {} WHERE {} = {}",
				TableName(),
				Strings::Implode(", ", v),
				PrimaryKey(),
				e.item_id
			)
		);

		return (results.Success() ? results.RowsAffected() : 0);
	}

	static ValeenItems InsertOne(
		Database& db,
		ValeenItems e
	)
	{
		std::vector<std::string> v;

		v.push_back(std::to_string(e.item_id));
		v.push_back("'" + Strings::Escape(e.item_name) + "'");
		v.push_back(std::to_string(e.slot));
		v.push_back(std::to_string(e.faction_required));
		v.push_back(std::to_string(e.level_required));
		v.push_back(std::to_string(e.min_status));
		v.push_back(std::to_string(e.max_status));
		v.push_back(std::to_string(e.alt_currency_cost));
		v.push_back(std::to_string(e.classes_required));
		v.push_back(std::to_string(e.probability));
		v.push_back("'" + Strings::Escape(e.bucket_name) + "'");
		v.push_back("'" + Strings::Escape(e.bucket_value) + "'");
		v.push_back(std::to_string(e.bucket_comparison));
		v.push_back(std::to_string(e.min_expansion));
		v.push_back(std::to_string(e.max_expansion));
		v.push_back("'" + Strings::Escape(e.content_flags) + "'");
		v.push_back("'" + Strings::Escape(e.content_flags_disabled) + "'");
		v.push_back(std::to_string(e.GearScore));
		v.push_back(std::to_string(e.ShardScore));
		v.push_back(std::to_string(e.MinDropLevel));
		v.push_back(std::to_string(e.difficulty));
		v.push_back(std::to_string(e.lowest_drop_npc_id));
		v.push_back(std::to_string(e.raid_only));
		v.push_back(std::to_string(e.merchant_tier));

		auto results = db.QueryDatabase(
			fmt::format(
				"{} VALUES ({})",
				BaseInsert(),
				Strings::Implode(",", v)
			)
		);

		if (results.Success()) {
			e.item_id = results.LastInsertedID();
			return e;
		}

		e = NewEntity();

		return e;
	}

	static int InsertMany(
		Database& db,
		const std::vector<ValeenItems> &entries
	)
	{
		std::vector<std::string> insert_chunks;

		for (auto &e: entries) {
			std::vector<std::string> v;

			v.push_back(std::to_string(e.item_id));
			v.push_back("'" + Strings::Escape(e.item_name) + "'");
			v.push_back(std::to_string(e.slot));
			v.push_back(std::to_string(e.faction_required));
			v.push_back(std::to_string(e.level_required));
			v.push_back(std::to_string(e.min_status));
			v.push_back(std::to_string(e.max_status));
			v.push_back(std::to_string(e.alt_currency_cost));
			v.push_back(std::to_string(e.classes_required));
			v.push_back(std::to_string(e.probability));
			v.push_back("'" + Strings::Escape(e.bucket_name) + "'");
			v.push_back("'" + Strings::Escape(e.bucket_value) + "'");
			v.push_back(std::to_string(e.bucket_comparison));
			v.push_back(std::to_string(e.min_expansion));
			v.push_back(std::to_string(e.max_expansion));
			v.push_back("'" + Strings::Escape(e.content_flags) + "'");
			v.push_back("'" + Strings::Escape(e.content_flags_disabled) + "'");
			v.push_back(std::to_string(e.GearScore));
			v.push_back(std::to_string(e.ShardScore));
			v.push_back(std::to_string(e.MinDropLevel));
			v.push_back(std::to_string(e.difficulty));
			v.push_back(std::to_string(e.lowest_drop_npc_id));
			v.push_back(std::to_string(e.raid_only));
			v.push_back(std::to_string(e.merchant_tier));

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

	static std::vector<ValeenItems> All(Database& db)
	{
		std::vector<ValeenItems> all_entries;

		auto results = db.QueryDatabase(
			fmt::format(
				"{}",
				BaseSelect()
			)
		);

		all_entries.reserve(results.RowCount());

		for (auto row = results.begin(); row != results.end(); ++row) {
			ValeenItems e{};

			e.item_id                = row[0] ? static_cast<uint32_t>(strtoul(row[0], nullptr, 10)) : 0;
			e.item_name              = row[1] ? row[1] : "";
			e.slot                   = row[2] ? static_cast<uint32_t>(strtoul(row[2], nullptr, 10)) : 0;
			e.faction_required       = row[3] ? static_cast<int16_t>(atoi(row[3])) : -100;
			e.level_required         = row[4] ? static_cast<uint8_t>(strtoul(row[4], nullptr, 10)) : 0;
			e.min_status             = row[5] ? static_cast<uint8_t>(strtoul(row[5], nullptr, 10)) : 0;
			e.max_status             = row[6] ? static_cast<uint8_t>(strtoul(row[6], nullptr, 10)) : 255;
			e.alt_currency_cost      = row[7] ? static_cast<uint16_t>(strtoul(row[7], nullptr, 10)) : 0;
			e.classes_required       = row[8] ? static_cast<int32_t>(atoi(row[8])) : 65535;
			e.probability            = row[9] ? static_cast<int32_t>(atoi(row[9])) : 100;
			e.bucket_name            = row[10] ? row[10] : "";
			e.bucket_value           = row[11] ? row[11] : "";
			e.bucket_comparison      = row[12] ? static_cast<uint8_t>(strtoul(row[12], nullptr, 10)) : 0;
			e.min_expansion          = row[13] ? static_cast<int8_t>(atoi(row[13])) : -1;
			e.max_expansion          = row[14] ? static_cast<int8_t>(atoi(row[14])) : -1;
			e.content_flags          = row[15] ? row[15] : "";
			e.content_flags_disabled = row[16] ? row[16] : "";
			e.GearScore              = row[17] ? (strtof(row[17], nullptr) > 0.0f ? strtof(row[17], nullptr) : 0) : 0;
			e.ShardScore             = row[18] ? (strtof(row[18], nullptr) > 0.0f ? strtof(row[18], nullptr) : 0) : 0;
			e.MinDropLevel           = row[19] ? static_cast<uint32_t>(strtoul(row[19], nullptr, 10)) : 0;
			e.difficulty             = row[20] ? (strtof(row[20], nullptr) > 0.0f ? strtof(row[20], nullptr) : 0) : 0;
			e.lowest_drop_npc_id     = row[21] ? static_cast<uint32_t>(strtoul(row[21], nullptr, 10)) : 0;
			e.raid_only              = row[22] ? static_cast<uint8_t>(strtoul(row[22], nullptr, 10)) : 0;
			e.merchant_tier          = row[23] ? static_cast<uint8_t>(strtoul(row[23], nullptr, 10)) : 0;

			all_entries.push_back(e);
		}

		return all_entries;
	}

	static std::vector<ValeenItems> GetWhere(Database& db, const std::string &where_filter)
	{
		std::vector<ValeenItems> all_entries;

		auto results = db.QueryDatabase(
			fmt::format(
				"{} WHERE {}",
				BaseSelect(),
				where_filter
			)
		);

		all_entries.reserve(results.RowCount());

		for (auto row = results.begin(); row != results.end(); ++row) {
			ValeenItems e{};

			e.item_id                = row[0] ? static_cast<uint32_t>(strtoul(row[0], nullptr, 10)) : 0;
			e.item_name              = row[1] ? row[1] : "";
			e.slot                   = row[2] ? static_cast<uint32_t>(strtoul(row[2], nullptr, 10)) : 0;
			e.faction_required       = row[3] ? static_cast<int16_t>(atoi(row[3])) : -100;
			e.level_required         = row[4] ? static_cast<uint8_t>(strtoul(row[4], nullptr, 10)) : 0;
			e.min_status             = row[5] ? static_cast<uint8_t>(strtoul(row[5], nullptr, 10)) : 0;
			e.max_status             = row[6] ? static_cast<uint8_t>(strtoul(row[6], nullptr, 10)) : 255;
			e.alt_currency_cost      = row[7] ? static_cast<uint16_t>(strtoul(row[7], nullptr, 10)) : 0;
			e.classes_required       = row[8] ? static_cast<int32_t>(atoi(row[8])) : 65535;
			e.probability            = row[9] ? static_cast<int32_t>(atoi(row[9])) : 100;
			e.bucket_name            = row[10] ? row[10] : "";
			e.bucket_value           = row[11] ? row[11] : "";
			e.bucket_comparison      = row[12] ? static_cast<uint8_t>(strtoul(row[12], nullptr, 10)) : 0;
			e.min_expansion          = row[13] ? static_cast<int8_t>(atoi(row[13])) : -1;
			e.max_expansion          = row[14] ? static_cast<int8_t>(atoi(row[14])) : -1;
			e.content_flags          = row[15] ? row[15] : "";
			e.content_flags_disabled = row[16] ? row[16] : "";
			e.GearScore              = row[17] ? (strtof(row[17], nullptr) > 0.0f ? strtof(row[17], nullptr) : 0) : 0;
			e.ShardScore             = row[18] ? (strtof(row[18], nullptr) > 0.0f ? strtof(row[18], nullptr) : 0) : 0;
			e.MinDropLevel           = row[19] ? static_cast<uint32_t>(strtoul(row[19], nullptr, 10)) : 0;
			e.difficulty             = row[20] ? (strtof(row[20], nullptr) > 0.0f ? strtof(row[20], nullptr) : 0) : 0;
			e.lowest_drop_npc_id     = row[21] ? static_cast<uint32_t>(strtoul(row[21], nullptr, 10)) : 0;
			e.raid_only              = row[22] ? static_cast<uint8_t>(strtoul(row[22], nullptr, 10)) : 0;
			e.merchant_tier          = row[23] ? static_cast<uint8_t>(strtoul(row[23], nullptr, 10)) : 0;

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
		const ValeenItems &e
	)
	{
		std::vector<std::string> v;

		v.push_back(std::to_string(e.item_id));
		v.push_back("'" + Strings::Escape(e.item_name) + "'");
		v.push_back(std::to_string(e.slot));
		v.push_back(std::to_string(e.faction_required));
		v.push_back(std::to_string(e.level_required));
		v.push_back(std::to_string(e.min_status));
		v.push_back(std::to_string(e.max_status));
		v.push_back(std::to_string(e.alt_currency_cost));
		v.push_back(std::to_string(e.classes_required));
		v.push_back(std::to_string(e.probability));
		v.push_back("'" + Strings::Escape(e.bucket_name) + "'");
		v.push_back("'" + Strings::Escape(e.bucket_value) + "'");
		v.push_back(std::to_string(e.bucket_comparison));
		v.push_back(std::to_string(e.min_expansion));
		v.push_back(std::to_string(e.max_expansion));
		v.push_back("'" + Strings::Escape(e.content_flags) + "'");
		v.push_back("'" + Strings::Escape(e.content_flags_disabled) + "'");
		v.push_back(std::to_string(e.GearScore));
		v.push_back(std::to_string(e.ShardScore));
		v.push_back(std::to_string(e.MinDropLevel));
		v.push_back(std::to_string(e.difficulty));
		v.push_back(std::to_string(e.lowest_drop_npc_id));
		v.push_back(std::to_string(e.raid_only));
		v.push_back(std::to_string(e.merchant_tier));

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
		const std::vector<ValeenItems> &entries
	)
	{
		std::vector<std::string> insert_chunks;

		for (auto &e: entries) {
			std::vector<std::string> v;

			v.push_back(std::to_string(e.item_id));
			v.push_back("'" + Strings::Escape(e.item_name) + "'");
			v.push_back(std::to_string(e.slot));
			v.push_back(std::to_string(e.faction_required));
			v.push_back(std::to_string(e.level_required));
			v.push_back(std::to_string(e.min_status));
			v.push_back(std::to_string(e.max_status));
			v.push_back(std::to_string(e.alt_currency_cost));
			v.push_back(std::to_string(e.classes_required));
			v.push_back(std::to_string(e.probability));
			v.push_back("'" + Strings::Escape(e.bucket_name) + "'");
			v.push_back("'" + Strings::Escape(e.bucket_value) + "'");
			v.push_back(std::to_string(e.bucket_comparison));
			v.push_back(std::to_string(e.min_expansion));
			v.push_back(std::to_string(e.max_expansion));
			v.push_back("'" + Strings::Escape(e.content_flags) + "'");
			v.push_back("'" + Strings::Escape(e.content_flags_disabled) + "'");
			v.push_back(std::to_string(e.GearScore));
			v.push_back(std::to_string(e.ShardScore));
			v.push_back(std::to_string(e.MinDropLevel));
			v.push_back(std::to_string(e.difficulty));
			v.push_back(std::to_string(e.lowest_drop_npc_id));
			v.push_back(std::to_string(e.raid_only));
			v.push_back(std::to_string(e.merchant_tier));

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
