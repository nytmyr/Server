/**
 * DO NOT MODIFY THIS FILE
 *
 * This repository was automatically generated and is NOT to be modified directly.
 * Any repository modifications are meant to be made to the repository extending the base.
 * Any modifications to base repositories are to be made by the generator only
 *
 * @generator ./utils/scripts/generators/repository-generator.pl
 * @docs https://eqemu.gitbook.io/server/in-development/developer-area/repositories
 */

#ifndef EQEMU_BASE_BOT_DATA_REPOSITORY_H
#define EQEMU_BASE_BOT_DATA_REPOSITORY_H

#include "../../database.h"
#include "../../strings.h"
#include <ctime>

class BaseBotDataRepository {
public:
	struct BotData {
		uint32_t    bot_id;
		uint32_t    owner_id;
		uint32_t    spells_id;
		std::string name;
		std::string last_name;
		std::string title;
		std::string suffix;
		int16_t     zone_id;
		int8_t      gender;
		int16_t     race;
		int8_t      class_;
		uint8_t     level;
		uint32_t    deity;
		uint32_t    creation_day;
		uint32_t    last_spawn;
		uint32_t    time_spawned;
		float       size;
		int32_t     face;
		int32_t     hair_color;
		int32_t     hair_style;
		int32_t     beard;
		int32_t     beard_color;
		int32_t     eye_color_1;
		int32_t     eye_color_2;
		int32_t     drakkin_heritage;
		int32_t     drakkin_tattoo;
		int32_t     drakkin_details;
		int16_t     ac;
		int32_t     atk;
		int32_t     hp;
		int32_t     mana;
		int32_t     str;
		int32_t     sta;
		int32_t     cha;
		int32_t     dex;
		int32_t     int_;
		int32_t     agi;
		int32_t     wis;
		int16_t     fire;
		int16_t     cold;
		int16_t     magic;
		int16_t     poison;
		int16_t     disease;
		int16_t     corruption;
		uint32_t    show_helm;
		uint32_t    follow_distance;
		uint8_t     stop_melee_level;
		int32_t     expansion_bitmask;
		uint8_t     enforce_spell_settings;
		uint8_t     archery_setting;
		uint8_t     petsettype_setting;
		uint8_t		hold_buffs;
		uint8_t		hold_complete_heals;
		uint8_t		hold_cures;
		uint8_t		hold_dots;
		uint8_t		hold_debuffs;
		uint8_t		hold_dispels;
		uint8_t		hold_escapes;
		uint8_t		hold_fast_heals;
		uint8_t		hold_group_heals;
		uint8_t		hold_hateredux;
		uint8_t		hold_heals;
		uint8_t		hold_hot_heals;
		uint8_t		hold_incombatbuffs;
		uint8_t		hold_incombatbuffsongs;
		uint8_t		hold_lifetaps;
		uint8_t		hold_mez;
		uint8_t		hold_nukes;
		uint8_t		hold_outofcombatbuffsongs;
		uint8_t		hold_pet_buffs;
		uint8_t		hold_pet_heals;
		uint8_t		hold_pets;
		uint8_t		hold_precombatbuffs;
		uint8_t		hold_precombatbuffsongs;
		uint8_t		hold_regular_heals;
		uint8_t		hold_roots;
		uint8_t		hold_slows;
		uint8_t		hold_snares;
		uint8_t		auto_ds;
		uint8_t		auto_resist;
		uint8_t		behind_mob;
		uint32_t	caster_range;
		uint32_t	buff_delay;
		uint32_t	complete_heal_delay;
		uint32_t	cure_delay;
		uint32_t	debuff_delay;
		uint32_t	dispel_delay;
		uint32_t	dot_delay;
		uint32_t	escape_delay;
		uint32_t	fast_heal_delay;
		uint32_t	hate_redux_delay;
		uint32_t	heal_delay;
		uint32_t	hot_heal_delay;
		uint32_t	incombatbuff_delay;
		uint32_t	lifetap_delay;
		uint32_t	mez_delay;
		uint32_t	nuke_delay;
		uint32_t	root_delay;
		uint32_t	slow_delay;
		uint32_t	snare_delay;
		uint8_t		buff_threshold;
		uint8_t		complete_heal_threshold;
		uint8_t		cure_threshold;
		uint8_t		debuff_threshold;
		uint8_t		dispel_threshold;
		uint8_t		dot_threshold;
		uint8_t		escape_threshold;
		uint8_t		fast_heal_threshold;
		uint8_t		hate_redux_threshold;
		uint8_t		heal_threshold;
		uint8_t		hot_heal_threshold;
		uint8_t		incombatbuff_threshold;
		uint8_t		lifetap_threshold;
		uint8_t		mez_threshold;
		uint8_t		nuke_threshold;
		uint8_t		root_threshold;
		uint8_t		slow_threshold;
		uint8_t		snare_threshold;
		uint8_t		buff_min_threshold;
		uint8_t		cure_min_threshold;
		uint8_t		debuff_min_threshold;
		uint8_t		dispel_min_threshold;
		uint8_t		dot_min_threshold;
		uint8_t		escape_min_threshold;
		uint8_t		hate_redux_min_threshold;
		uint8_t		incombatbuff_min_threshold;
		uint8_t		lifetap_min_threshold;
		uint8_t		mez_min_threshold;
		uint8_t		nuke_min_threshold;
		uint8_t		root_min_threshold;
		uint8_t		slow_min_threshold;
		uint8_t		snare_min_threshold;
	};

	static std::string PrimaryKey()
	{
		return std::string("bot_id");
	}

	static std::vector<std::string> Columns()
	{
		return {
			"bot_id",
			"owner_id",
			"spells_id",
			"name",
			"last_name",
			"title",
			"suffix",
			"zone_id",
			"gender",
			"race",
			"`class`",
			"level",
			"deity",
			"creation_day",
			"last_spawn",
			"time_spawned",
			"size",
			"face",
			"hair_color",
			"hair_style",
			"beard",
			"beard_color",
			"eye_color_1",
			"eye_color_2",
			"drakkin_heritage",
			"drakkin_tattoo",
			"drakkin_details",
			"ac",
			"atk",
			"hp",
			"mana",
			"str",
			"sta",
			"cha",
			"dex",
			"`int`",
			"agi",
			"wis",
			"fire",
			"cold",
			"magic",
			"poison",
			"disease",
			"corruption",
			"show_helm",
			"follow_distance",
			"stop_melee_level",
			"expansion_bitmask",
			"enforce_spell_settings",
			"archery_setting",
			"petsettype_setting",
			"hold_buffs",
			"hold_complete_heals",
			"hold_cures",
			"hold_dots",
			"hold_debuffs",
			"hold_dispels",
			"hold_escapes",
			"hold_fast_heals",
			"hold_group_heals",
			"hold_hateredux",
			"hold_heals",
			"hold_hot_heals",
			"hold_incombatbuffs",
			"hold_incombatbuffsongs",
			"hold_lifetaps",
			"hold_mez",
			"hold_nukes",
			"hold_outofcombatbuffsongs",
			"hold_pet_buffs",
			"hold_pet_heals",
			"hold_pets",
			"hold_precombatbuffs",
			"hold_precombatbuffsongs",
			"hold_regular_heals",
			"hold_roots",
			"hold_slows",
			"hold_snares",
			"auto_ds",
			"auto_resist",
			"behind_mob",
			"caster_range",
			"buff_delay",
			"complete_heal_delay",
			"cure_delay",
			"debuff_delay",
			"dispel_delay",
			"dot_delay",
			"escape_delay",
			"fast_heal_delay",
			"hate_redux_delay",
			"heal_delay",
			"hot_heal_delay",
			"incombatbuff_delay",
			"lifetap_delay",
			"mez_delay",
			"nuke_delay",
			"root_delay",
			"slow_delay",
			"snare_delay",
			"buff_threshold",
			"complete_heal_threshold",
			"cure_threshold",
			"debuff_threshold",
			"dispel_threshold",
			"dot_threshold",
			"escape_threshold",
			"fast_heal_threshold",
			"hate_redux_threshold",
			"heal_threshold",
			"hot_heal_threshold",
			"incombatbuff_threshold",
			"lifetap_threshold",
			"mez_threshold",
			"nuke_threshold",
			"root_threshold",
			"slow_threshold",
			"snare_threshold",
			"buff_min_threshold",
			"cure_min_threshold",
			"debuff_min_threshold",
			"dispel_min_threshold",
			"dot_min_threshold",
			"escape_min_threshold",
			"hate_redux_min_threshold",
			"incombatbuff_min_threshold",
			"lifetap_min_threshold",
			"mez_min_threshold",
			"nuke_min_threshold",
			"root_min_threshold",
			"slow_min_threshold",
			"snare_min_threshold",
		};
	}

	static std::vector<std::string> SelectColumns()
	{
		return {
			"bot_id",
			"owner_id",
			"spells_id",
			"name",
			"last_name",
			"title",
			"suffix",
			"zone_id",
			"gender",
			"race",
			"`class`",
			"level",
			"deity",
			"creation_day",
			"last_spawn",
			"time_spawned",
			"size",
			"face",
			"hair_color",
			"hair_style",
			"beard",
			"beard_color",
			"eye_color_1",
			"eye_color_2",
			"drakkin_heritage",
			"drakkin_tattoo",
			"drakkin_details",
			"ac",
			"atk",
			"hp",
			"mana",
			"str",
			"sta",
			"cha",
			"dex",
			"`int`",
			"agi",
			"wis",
			"fire",
			"cold",
			"magic",
			"poison",
			"disease",
			"corruption",
			"show_helm",
			"follow_distance",
			"stop_melee_level",
			"expansion_bitmask",
			"enforce_spell_settings",
			"archery_setting",
			"petsettype_setting",
			"hold_buffs",
			"hold_complete_heals",
			"hold_cures",
			"hold_dots",
			"hold_debuffs",
			"hold_dispels",
			"hold_escapes",
			"hold_fast_heals",
			"hold_group_heals",
			"hold_hateredux",
			"hold_heals",
			"hold_hot_heals",
			"hold_incombatbuffs",
			"hold_incombatbuffsongs",
			"hold_lifetaps",
			"hold_mez",
			"hold_nukes",
			"hold_outofcombatbuffsongs",
			"hold_pet_buffs",
			"hold_pet_heals",
			"hold_pets",
			"hold_precombatbuffs",
			"hold_precombatbuffsongs",
			"hold_regular_heals",
			"hold_roots",
			"hold_slows",
			"hold_snares",
			"auto_ds",
			"auto_resist",
			"behind_mob",
			"caster_range",
			"buff_delay",
			"complete_heal_delay",
			"cure_delay",
			"debuff_delay",
			"dispel_delay",
			"dot_delay",
			"escape_delay",
			"fast_heal_delay",
			"hate_redux_delay",
			"heal_delay",
			"hot_heal_delay",
			"incombatbuff_delay",
			"lifetap_delay",
			"mez_delay",
			"nuke_delay",
			"root_delay",
			"slow_delay",
			"snare_delay",
			"buff_threshold",
			"complete_heal_threshold",
			"cure_threshold",
			"debuff_threshold",
			"dispel_threshold",
			"dot_threshold",
			"escape_threshold",
			"fast_heal_threshold",
			"hate_redux_threshold",
			"heal_threshold",
			"hot_heal_threshold",
			"incombatbuff_threshold",
			"lifetap_threshold",
			"mez_threshold",
			"nuke_threshold",
			"root_threshold",
			"slow_threshold",
			"snare_threshold",
			"buff_min_threshold",
			"cure_min_threshold",
			"debuff_min_threshold",
			"dispel_min_threshold",
			"dot_min_threshold",
			"escape_min_threshold",
			"hate_redux_min_threshold",
			"incombatbuff_min_threshold",
			"lifetap_min_threshold",
			"mez_min_threshold",
			"nuke_min_threshold",
			"root_min_threshold",
			"slow_min_threshold",
			"snare_min_threshold",
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
		return std::string("bot_data");
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

	static BotData NewEntity()
	{
		BotData e{};

		e.bot_id					 = 0;
		e.owner_id					 = 0;
		e.spells_id					 = 0;
		e.name						 = "";
		e.last_name					 = "";
		e.title						 = "";
		e.suffix					 = "";
		e.zone_id					 = 0;
		e.gender					 = 0;
		e.race						 = 0;
		e.class_					 = 0;
		e.level						 = 0;
		e.deity						 = 0;
		e.creation_day				 = 0;
		e.last_spawn				 = 0;
		e.time_spawned				 = 0;
		e.size						 = 0;
		e.face						 = 1;
		e.hair_color				 = 1;
		e.hair_style				 = 1;
		e.beard						 = 0;
		e.beard_color				 = 1;
		e.eye_color_1				 = 1;
		e.eye_color_2				 = 1;
		e.drakkin_heritage			 = 0;
		e.drakkin_tattoo			 = 0;
		e.drakkin_details			 = 0;
		e.ac						 = 0;
		e.atk						 = 0;
		e.hp						 = 0;
		e.mana						 = 0;
		e.str						 = 75;
		e.sta						 = 75;
		e.cha						 = 75;
		e.dex						 = 75;
		e.int_						 = 75;
		e.agi						 = 75;
		e.wis						 = 75;
		e.fire						 = 0;
		e.cold						 = 0;
		e.magic						 = 0;
		e.poison					 = 0;
		e.disease					 = 0;
		e.corruption				 = 0;
		e.show_helm					 = 0;
		e.follow_distance			 = 200;
		e.stop_melee_level			 = 255;
		e.expansion_bitmask			 = -1;
		e.enforce_spell_settings	 = 0;
		e.archery_setting			 = 0;
		e.petsettype_setting		 = 255;
		e.hold_buffs				 = 0;
		e.hold_complete_heals		 = 0;
		e.hold_cures				 = 0;
		e.hold_dots					 = 0;
		e.hold_debuffs				 = 0;
		e.hold_dispels				 = 1;
		e.hold_escapes				 = 0;
		e.hold_fast_heals			 = 0;
		e.hold_group_heals			 = 0;
		e.hold_hateredux			 = 0;
		e.hold_heals				 = 0;
		e.hold_hot_heals			 = 0;
		e.hold_incombatbuffs		 = 0;
		e.hold_incombatbuffsongs	 = 0;
		e.hold_lifetaps				 = 0;
		e.hold_mez					 = 0;
		e.hold_nukes				 = 0;
		e.hold_outofcombatbuffsongs	 = 0;
		e.hold_pet_buffs			 = 0;
		e.hold_pet_heals			 = 0;
		e.hold_pets					 = 0;
		e.hold_precombatbuffs		 = 0;
		e.hold_precombatbuffsongs	 = 0;
		e.hold_regular_heals		 = 0;
		e.hold_roots				 = 1;
		e.hold_slows				 = 0;
		e.hold_snares				 = 0;
		e.auto_ds					 = 1;
		e.auto_resist				 = 1;
		e.behind_mob				 = 0;
		e.caster_range				 = 90;
		e.buff_delay				 = 1;
		e.complete_heal_delay		 = 8000;
		e.cure_delay				 = 1;
		e.debuff_delay				 = 12000;
		e.dispel_delay				 = 1;
		e.dot_delay					 = 12000;
		e.escape_delay				 = 1;
		e.fast_heal_delay			 = 2500;
		e.hate_redux_delay			 = 1;
		e.heal_delay				 = 4500;
		e.hot_heal_delay			 = 22000;
		e.incombatbuff_delay		 = 1;
		e.lifetap_delay				 = 1;
		e.mez_delay					 = 1;
		e.nuke_delay				 = 8000;
		e.root_delay				 = 12000;
		e.slow_delay				 = 12000;
		e.snare_delay				 = 12000;
		e.buff_threshold			 = 150;
		e.complete_heal_threshold	 = 70;
		e.cure_threshold			 = 150;
		e.debuff_threshold			 = 95;
		e.dispel_threshold			 = 99;
		e.dot_threshold				 = 95;
		e.escape_threshold			 = 35;
		e.fast_heal_threshold		 = 35;
		e.hate_redux_threshold		 = 80;
		e.heal_threshold			 = 55;
		e.hot_heal_threshold		 = 85;
		e.incombatbuff_threshold	 = 150;
		e.lifetap_threshold			 = 60;
		e.mez_threshold				 = 150;
		e.nuke_threshold			 = 95;
		e.root_threshold			 = 95;
		e.slow_threshold			 = 95;
		e.snare_threshold			 = 95;
		e.buff_min_threshold		 = 0;
		e.cure_min_threshold		 = 0;
		e.debuff_min_threshold		 = 25;
		e.dispel_min_threshold		 = 25;
		e.dot_min_threshold			 = 50;
		e.escape_min_threshold		 = 0;
		e.hate_redux_min_threshold	 = 0;
		e.incombatbuff_min_threshold = 0;
		e.lifetap_min_threshold		 = 0;
		e.mez_min_threshold			 = 85;
		e.nuke_min_threshold		 = 20;
		e.root_min_threshold		 = 15;
		e.slow_min_threshold		 = 25;
		e.snare_min_threshold		 = 0;

		return e;
	}

	static BotData GetBotData(
		const std::vector<BotData> &bot_datas,
		int bot_data_id
	)
	{
		for (auto &bot_data : bot_datas) {
			if (bot_data.bot_id == bot_data_id) {
				return bot_data;
			}
		}

		return NewEntity();
	}

	static BotData FindOne(
		Database& db,
		int bot_data_id
	)
	{
		auto results = db.QueryDatabase(
			fmt::format(
				"{} WHERE {} = {} LIMIT 1",
				BaseSelect(),
				PrimaryKey(),
				bot_data_id
			)
		);

		auto row = results.begin();
		if (results.RowCount() == 1) {
			BotData e{};

			e.bot_id						= static_cast<uint32_t>(strtoul(row[0], nullptr, 10));
			e.owner_id						= static_cast<uint32_t>(strtoul(row[1], nullptr, 10));
			e.spells_id						= static_cast<uint32_t>(strtoul(row[2], nullptr, 10));
			e.name							= row[3] ? row[3] : "";
			e.last_name						= row[4] ? row[4] : "";
			e.title							= row[5] ? row[5] : "";
			e.suffix						= row[6] ? row[6] : "";
			e.zone_id						= static_cast<int16_t>(atoi(row[7]));
			e.gender						= static_cast<int8_t>(atoi(row[8]));
			e.race							= static_cast<int16_t>(atoi(row[9]));
			e.class_						= static_cast<int8_t>(atoi(row[10]));
			e.level							= static_cast<uint8_t>(strtoul(row[11], nullptr, 10));
			e.deity							= static_cast<uint32_t>(strtoul(row[12], nullptr, 10));
			e.creation_day					= static_cast<uint32_t>(strtoul(row[13], nullptr, 10));
			e.last_spawn					= static_cast<uint32_t>(strtoul(row[14], nullptr, 10));
			e.time_spawned					= static_cast<uint32_t>(strtoul(row[15], nullptr, 10));
			e.size							= strtof(row[16], nullptr);
			e.face							= static_cast<int32_t>(atoi(row[17]));
			e.hair_color					= static_cast<int32_t>(atoi(row[18]));
			e.hair_style					= static_cast<int32_t>(atoi(row[19]));
			e.beard							= static_cast<int32_t>(atoi(row[20]));
			e.beard_color					= static_cast<int32_t>(atoi(row[21]));
			e.eye_color_1					= static_cast<int32_t>(atoi(row[22]));
			e.eye_color_2					= static_cast<int32_t>(atoi(row[23]));
			e.drakkin_heritage				= static_cast<int32_t>(atoi(row[24]));
			e.drakkin_tattoo				= static_cast<int32_t>(atoi(row[25]));
			e.drakkin_details				= static_cast<int32_t>(atoi(row[26]));
			e.ac							= static_cast<int16_t>(atoi(row[27]));
			e.atk							= static_cast<int32_t>(atoi(row[28]));
			e.hp							= static_cast<int32_t>(atoi(row[29]));
			e.mana							= static_cast<int32_t>(atoi(row[30]));
			e.str							= static_cast<int32_t>(atoi(row[31]));
			e.sta							= static_cast<int32_t>(atoi(row[32]));
			e.cha							= static_cast<int32_t>(atoi(row[33]));
			e.dex							= static_cast<int32_t>(atoi(row[34]));
			e.int_							= static_cast<int32_t>(atoi(row[35]));
			e.agi							= static_cast<int32_t>(atoi(row[36]));
			e.wis							= static_cast<int32_t>(atoi(row[37]));
			e.fire							= static_cast<int16_t>(atoi(row[38]));
			e.cold							= static_cast<int16_t>(atoi(row[39]));
			e.magic							= static_cast<int16_t>(atoi(row[40]));
			e.poison						= static_cast<int16_t>(atoi(row[41]));
			e.disease						= static_cast<int16_t>(atoi(row[42]));
			e.corruption					= static_cast<int16_t>(atoi(row[43]));
			e.show_helm						= static_cast<uint32_t>(strtoul(row[44], nullptr, 10));
			e.follow_distance				= static_cast<uint32_t>(strtoul(row[45], nullptr, 10));
			e.stop_melee_level				= static_cast<uint8_t>(strtoul(row[46], nullptr, 10));
			e.expansion_bitmask				= static_cast<int32_t>(atoi(row[47]));
			e.enforce_spell_settings		= static_cast<uint8_t>(strtoul(row[48], nullptr, 10));
			e.archery_setting				= static_cast<uint8_t>(strtoul(row[49], nullptr, 10));
			e.petsettype_setting		    = static_cast<uint8_t>(strtoul(row[50], nullptr, 10));
			e.hold_buffs					= static_cast<uint8_t>(strtoul(row[51], nullptr, 10));
			e.hold_complete_heals		    = static_cast<uint8_t>(strtoul(row[52], nullptr, 10));
			e.hold_cures					= static_cast<uint8_t>(strtoul(row[53], nullptr, 10));
			e.hold_dots						= static_cast<uint8_t>(strtoul(row[54], nullptr, 10));
			e.hold_debuffs					= static_cast<uint8_t>(strtoul(row[55], nullptr, 10));
			e.hold_dispels					= static_cast<uint8_t>(strtoul(row[56], nullptr, 10));
			e.hold_escapes					= static_cast<uint8_t>(strtoul(row[57], nullptr, 10));
			e.hold_fast_heals			    = static_cast<uint8_t>(strtoul(row[58], nullptr, 10));
			e.hold_group_heals				= static_cast<uint8_t>(strtoul(row[59], nullptr, 10));
			e.hold_hateredux				= static_cast<uint8_t>(strtoul(row[60], nullptr, 10));
			e.hold_heals					= static_cast<uint8_t>(strtoul(row[61], nullptr, 10));
			e.hold_hot_heals				= static_cast<uint8_t>(strtoul(row[62], nullptr, 10));
			e.hold_incombatbuffs			= static_cast<uint8_t>(strtoul(row[63], nullptr, 10));
			e.hold_incombatbuffsongs		= static_cast<uint8_t>(strtoul(row[64], nullptr, 10));
			e.hold_lifetaps					= static_cast<uint8_t>(strtoul(row[65], nullptr, 10));
			e.hold_mez						= static_cast<uint8_t>(strtoul(row[66], nullptr, 10));
			e.hold_nukes					= static_cast<uint8_t>(strtoul(row[67], nullptr, 10));
			e.hold_outofcombatbuffsongs		= static_cast<uint8_t>(strtoul(row[68], nullptr, 10));
			e.hold_pet_buffs				= static_cast<uint8_t>(strtoul(row[69], nullptr, 10));
			e.hold_pet_heals				= static_cast<uint8_t>(strtoul(row[70], nullptr, 10));
			e.hold_pets						= static_cast<uint8_t>(strtoul(row[71], nullptr, 10));
			e.hold_precombatbuffs			= static_cast<uint8_t>(strtoul(row[72], nullptr, 10));
			e.hold_precombatbuffsongs		= static_cast<uint8_t>(strtoul(row[73], nullptr, 10));
			e.hold_regular_heals			= static_cast<uint8_t>(strtoul(row[74], nullptr, 10));
			e.hold_roots					= static_cast<uint8_t>(strtoul(row[75], nullptr, 10));
			e.hold_slows					= static_cast<uint8_t>(strtoul(row[76], nullptr, 10));
			e.hold_snares					= static_cast<uint8_t>(strtoul(row[77], nullptr, 10));
			e.auto_ds						= static_cast<uint8_t>(strtoul(row[78], nullptr, 10));
			e.auto_resist					= static_cast<uint8_t>(strtoul(row[79], nullptr, 10));
			e.behind_mob					= static_cast<uint8_t>(strtoul(row[80], nullptr, 10));
			e.caster_range					= static_cast<uint32_t>(strtoul(row[81], nullptr, 10));
			e.buff_delay					= static_cast<uint32_t>(strtoul(row[82], nullptr, 10));
			e.complete_heal_delay			= static_cast<uint32_t>(strtoul(row[83], nullptr, 10));
			e.cure_delay					= static_cast<uint32_t>(strtoul(row[84], nullptr, 10));
			e.debuff_delay					= static_cast<uint32_t>(strtoul(row[85], nullptr, 10));
			e.dispel_delay					= static_cast<uint32_t>(strtoul(row[86], nullptr, 10));
			e.dot_delay						= static_cast<uint32_t>(strtoul(row[87], nullptr, 10));
			e.escape_delay					= static_cast<uint32_t>(strtoul(row[88], nullptr, 10));
			e.fast_heal_delay				= static_cast<uint32_t>(strtoul(row[89], nullptr, 10));
			e.hate_redux_delay				= static_cast<uint32_t>(strtoul(row[90], nullptr, 10));
			e.heal_delay					= static_cast<uint32_t>(strtoul(row[91], nullptr, 10));
			e.hot_heal_delay				= static_cast<uint32_t>(strtoul(row[92], nullptr, 10));
			e.incombatbuff_delay			= static_cast<uint32_t>(strtoul(row[93], nullptr, 10));
			e.lifetap_delay					= static_cast<uint32_t>(strtoul(row[94], nullptr, 10));
			e.mez_delay						= static_cast<uint32_t>(strtoul(row[95], nullptr, 10));
			e.nuke_delay					= static_cast<uint32_t>(strtoul(row[96], nullptr, 10));
			e.root_delay					= static_cast<uint32_t>(strtoul(row[97], nullptr, 10));
			e.slow_delay					= static_cast<uint32_t>(strtoul(row[98], nullptr, 10));
			e.snare_delay					= static_cast<uint32_t>(strtoul(row[99], nullptr, 10));
			e.buff_threshold				= static_cast<uint8_t>(strtoul(row[100], nullptr, 10));
			e.complete_heal_threshold		= static_cast<uint8_t>(strtoul(row[101], nullptr, 10));
			e.cure_threshold				= static_cast<uint8_t>(strtoul(row[102], nullptr, 10));
			e.debuff_threshold				= static_cast<uint8_t>(strtoul(row[103], nullptr, 10));
			e.dispel_threshold				= static_cast<uint8_t>(strtoul(row[104], nullptr, 10));
			e.dot_threshold					= static_cast<uint8_t>(strtoul(row[105], nullptr, 10));
			e.escape_threshold				= static_cast<uint8_t>(strtoul(row[106], nullptr, 10));
			e.fast_heal_threshold			= static_cast<uint8_t>(strtoul(row[107], nullptr, 10));
			e.hate_redux_threshold			= static_cast<uint8_t>(strtoul(row[108], nullptr, 10));
			e.heal_threshold				= static_cast<uint8_t>(strtoul(row[109], nullptr, 10));
			e.hot_heal_threshold			= static_cast<uint8_t>(strtoul(row[110], nullptr, 10));
			e.incombatbuff_threshold		= static_cast<uint8_t>(strtoul(row[111], nullptr, 10));
			e.lifetap_threshold				= static_cast<uint8_t>(strtoul(row[112], nullptr, 10));
			e.mez_threshold					= static_cast<uint8_t>(strtoul(row[113], nullptr, 10));
			e.nuke_threshold				= static_cast<uint8_t>(strtoul(row[114], nullptr, 10));
			e.root_threshold				= static_cast<uint8_t>(strtoul(row[115], nullptr, 10));
			e.slow_threshold				= static_cast<uint8_t>(strtoul(row[116], nullptr, 10));
			e.snare_threshold				= static_cast<uint8_t>(strtoul(row[117], nullptr, 10));
			e.buff_min_threshold			= static_cast<uint8_t>(strtoul(row[118], nullptr, 10));
			e.cure_min_threshold			= static_cast<uint8_t>(strtoul(row[119], nullptr, 10));
			e.debuff_min_threshold			= static_cast<uint8_t>(strtoul(row[120], nullptr, 10));
			e.dispel_min_threshold			= static_cast<uint8_t>(strtoul(row[121], nullptr, 10));
			e.dot_min_threshold				= static_cast<uint8_t>(strtoul(row[122], nullptr, 10));
			e.escape_min_threshold			= static_cast<uint8_t>(strtoul(row[123], nullptr, 10));
			e.hate_redux_min_threshold		= static_cast<uint8_t>(strtoul(row[124], nullptr, 10));
			e.incombatbuff_min_threshold	= static_cast<uint8_t>(strtoul(row[125], nullptr, 10));
			e.lifetap_min_threshold			= static_cast<uint8_t>(strtoul(row[126], nullptr, 10));
			e.mez_min_threshold				= static_cast<uint8_t>(strtoul(row[127], nullptr, 10));
			e.nuke_min_threshold			= static_cast<uint8_t>(strtoul(row[128], nullptr, 10));
			e.root_min_threshold			= static_cast<uint8_t>(strtoul(row[129], nullptr, 10));
			e.slow_min_threshold			= static_cast<uint8_t>(strtoul(row[130], nullptr, 10));
			e.snare_min_threshold			= static_cast<uint8_t>(strtoul(row[131], nullptr, 10));

			return e;
		}

		return NewEntity();
	}

	static int DeleteOne(
		Database& db,
		int bot_data_id
	)
	{
		auto results = db.QueryDatabase(
			fmt::format(
				"DELETE FROM {} WHERE {} = {}",
				TableName(),
				PrimaryKey(),
				bot_data_id
			)
		);

		return (results.Success() ? results.RowsAffected() : 0);
	}

	static int UpdateOne(
		Database& db,
		const BotData &e
	)
	{
		std::vector<std::string> v;

		auto columns = Columns();

		v.push_back(columns[1] + " = " + std::to_string(e.owner_id));
		v.push_back(columns[2] + " = " + std::to_string(e.spells_id));
		v.push_back(columns[3] + " = '" + Strings::Escape(e.name) + "'");
		v.push_back(columns[4] + " = '" + Strings::Escape(e.last_name) + "'");
		v.push_back(columns[5] + " = '" + Strings::Escape(e.title) + "'");
		v.push_back(columns[6] + " = '" + Strings::Escape(e.suffix) + "'");
		v.push_back(columns[7] + " = " + std::to_string(e.zone_id));
		v.push_back(columns[8] + " = " + std::to_string(e.gender));
		v.push_back(columns[9] + " = " + std::to_string(e.race));
		v.push_back(columns[10] + " = " + std::to_string(e.class_));
		v.push_back(columns[11] + " = " + std::to_string(e.level));
		v.push_back(columns[12] + " = " + std::to_string(e.deity));
		v.push_back(columns[13] + " = " + std::to_string(e.creation_day));
		v.push_back(columns[14] + " = " + std::to_string(e.last_spawn));
		v.push_back(columns[15] + " = " + std::to_string(e.time_spawned));
		v.push_back(columns[16] + " = " + std::to_string(e.size));
		v.push_back(columns[17] + " = " + std::to_string(e.face));
		v.push_back(columns[18] + " = " + std::to_string(e.hair_color));
		v.push_back(columns[19] + " = " + std::to_string(e.hair_style));
		v.push_back(columns[20] + " = " + std::to_string(e.beard));
		v.push_back(columns[21] + " = " + std::to_string(e.beard_color));
		v.push_back(columns[22] + " = " + std::to_string(e.eye_color_1));
		v.push_back(columns[23] + " = " + std::to_string(e.eye_color_2));
		v.push_back(columns[24] + " = " + std::to_string(e.drakkin_heritage));
		v.push_back(columns[25] + " = " + std::to_string(e.drakkin_tattoo));
		v.push_back(columns[26] + " = " + std::to_string(e.drakkin_details));
		v.push_back(columns[27] + " = " + std::to_string(e.ac));
		v.push_back(columns[28] + " = " + std::to_string(e.atk));
		v.push_back(columns[29] + " = " + std::to_string(e.hp));
		v.push_back(columns[30] + " = " + std::to_string(e.mana));
		v.push_back(columns[31] + " = " + std::to_string(e.str));
		v.push_back(columns[32] + " = " + std::to_string(e.sta));
		v.push_back(columns[33] + " = " + std::to_string(e.cha));
		v.push_back(columns[34] + " = " + std::to_string(e.dex));
		v.push_back(columns[35] + " = " + std::to_string(e.int_));
		v.push_back(columns[36] + " = " + std::to_string(e.agi));
		v.push_back(columns[37] + " = " + std::to_string(e.wis));
		v.push_back(columns[38] + " = " + std::to_string(e.fire));
		v.push_back(columns[39] + " = " + std::to_string(e.cold));
		v.push_back(columns[40] + " = " + std::to_string(e.magic));
		v.push_back(columns[41] + " = " + std::to_string(e.poison));
		v.push_back(columns[42] + " = " + std::to_string(e.disease));
		v.push_back(columns[43] + " = " + std::to_string(e.corruption));
		v.push_back(columns[44] + " = " + std::to_string(e.show_helm));
		v.push_back(columns[45] + " = " + std::to_string(e.follow_distance));
		v.push_back(columns[46] + " = " + std::to_string(e.stop_melee_level));
		v.push_back(columns[47] + " = " + std::to_string(e.expansion_bitmask));
		v.push_back(columns[48] + " = " + std::to_string(e.enforce_spell_settings));
		v.push_back(columns[49] + " = " + std::to_string(e.archery_setting));
		v.push_back(columns[50] + " = " + std::to_string(e.petsettype_setting));
		v.push_back(columns[51] + " = " + std::to_string(e.hold_buffs));
		v.push_back(columns[52] + " = " + std::to_string(e.hold_complete_heals));
		v.push_back(columns[53] + " = " + std::to_string(e.hold_cures));
		v.push_back(columns[54] + " = " + std::to_string(e.hold_dots));
		v.push_back(columns[55] + " = " + std::to_string(e.hold_debuffs));
		v.push_back(columns[56] + " = " + std::to_string(e.hold_dispels));
		v.push_back(columns[57] + " = " + std::to_string(e.hold_group_heals));
		v.push_back(columns[58] + " = " + std::to_string(e.hold_fast_heals));
		v.push_back(columns[59] + " = " + std::to_string(e.hold_escapes));
		v.push_back(columns[60] + " = " + std::to_string(e.hold_hateredux));
		v.push_back(columns[61] + " = " + std::to_string(e.hold_heals));
		v.push_back(columns[62] + " = " + std::to_string(e.hold_hot_heals));
		v.push_back(columns[63] + " = " + std::to_string(e.hold_incombatbuffs));
		v.push_back(columns[64] + " = " + std::to_string(e.hold_incombatbuffsongs));
		v.push_back(columns[65] + " = " + std::to_string(e.hold_lifetaps));
		v.push_back(columns[66] + " = " + std::to_string(e.hold_mez));
		v.push_back(columns[67] + " = " + std::to_string(e.hold_nukes));
		v.push_back(columns[68] + " = " + std::to_string(e.hold_outofcombatbuffsongs));
		v.push_back(columns[69] + " = " + std::to_string(e.hold_pet_buffs));
		v.push_back(columns[70] + " = " + std::to_string(e.hold_pet_heals));
		v.push_back(columns[71] + " = " + std::to_string(e.hold_pets));
		v.push_back(columns[72] + " = " + std::to_string(e.hold_precombatbuffs));
		v.push_back(columns[73] + " = " + std::to_string(e.hold_precombatbuffsongs));
		v.push_back(columns[74] + " = " + std::to_string(e.hold_regular_heals));
		v.push_back(columns[75] + " = " + std::to_string(e.hold_roots));
		v.push_back(columns[76] + " = " + std::to_string(e.hold_slows));
		v.push_back(columns[77] + " = " + std::to_string(e.hold_snares));
		v.push_back(columns[78] + " = " + std::to_string(e.auto_ds));
		v.push_back(columns[79] + " = " + std::to_string(e.auto_resist));
		v.push_back(columns[80] + " = " + std::to_string(e.behind_mob));
		v.push_back(columns[81] + " = " + std::to_string(e.caster_range));
		v.push_back(columns[82] + " = " + std::to_string(e.buff_delay));
		v.push_back(columns[83] + " = " + std::to_string(e.complete_heal_delay));
		v.push_back(columns[84] + " = " + std::to_string(e.cure_delay));
		v.push_back(columns[85] + " = " + std::to_string(e.debuff_delay));
		v.push_back(columns[86] + " = " + std::to_string(e.dispel_delay));
		v.push_back(columns[87] + " = " + std::to_string(e.dot_delay));
		v.push_back(columns[88] + " = " + std::to_string(e.escape_delay));
		v.push_back(columns[89] + " = " + std::to_string(e.fast_heal_delay));
		v.push_back(columns[90] + " = " + std::to_string(e.hate_redux_delay));
		v.push_back(columns[91] + " = " + std::to_string(e.heal_delay));
		v.push_back(columns[92] + " = " + std::to_string(e.hot_heal_delay));
		v.push_back(columns[93] + " = " + std::to_string(e.incombatbuff_delay));
		v.push_back(columns[94] + " = " + std::to_string(e.lifetap_delay));
		v.push_back(columns[95] + " = " + std::to_string(e.mez_delay));
		v.push_back(columns[96] + " = " + std::to_string(e.nuke_delay));
		v.push_back(columns[97] + " = " + std::to_string(e.root_delay));
		v.push_back(columns[98] + " = " + std::to_string(e.slow_delay));
		v.push_back(columns[99] + " = " + std::to_string(e.snare_delay));
		v.push_back(columns[100] + " = " + std::to_string(e.buff_threshold));
		v.push_back(columns[101] + " = " + std::to_string(e.complete_heal_threshold));
		v.push_back(columns[102] + " = " + std::to_string(e.cure_threshold));
		v.push_back(columns[103] + " = " + std::to_string(e.debuff_threshold));
		v.push_back(columns[104] + " = " + std::to_string(e.dispel_threshold));
		v.push_back(columns[105] + " = " + std::to_string(e.dot_threshold));
		v.push_back(columns[106] + " = " + std::to_string(e.escape_threshold));
		v.push_back(columns[107] + " = " + std::to_string(e.fast_heal_threshold));
		v.push_back(columns[108] + " = " + std::to_string(e.hate_redux_threshold));
		v.push_back(columns[109] + " = " + std::to_string(e.heal_threshold));
		v.push_back(columns[110] + " = " + std::to_string(e.hot_heal_threshold));
		v.push_back(columns[111] + " = " + std::to_string(e.incombatbuff_threshold));
		v.push_back(columns[112] + " = " + std::to_string(e.lifetap_threshold));
		v.push_back(columns[113] + " = " + std::to_string(e.mez_threshold));
		v.push_back(columns[114] + " = " + std::to_string(e.nuke_threshold));
		v.push_back(columns[115] + " = " + std::to_string(e.root_threshold));
		v.push_back(columns[116] + " = " + std::to_string(e.slow_threshold));
		v.push_back(columns[117] + " = " + std::to_string(e.snare_threshold));
		v.push_back(columns[118] + " = " + std::to_string(e.buff_min_threshold));
		v.push_back(columns[119] + " = " + std::to_string(e.cure_min_threshold));
		v.push_back(columns[120] + " = " + std::to_string(e.debuff_min_threshold));
		v.push_back(columns[121] + " = " + std::to_string(e.dispel_min_threshold));
		v.push_back(columns[122] + " = " + std::to_string(e.dot_min_threshold));
		v.push_back(columns[123] + " = " + std::to_string(e.escape_min_threshold));
		v.push_back(columns[124] + " = " + std::to_string(e.hate_redux_min_threshold));
		v.push_back(columns[125] + " = " + std::to_string(e.incombatbuff_min_threshold));
		v.push_back(columns[126] + " = " + std::to_string(e.lifetap_min_threshold));
		v.push_back(columns[127] + " = " + std::to_string(e.mez_min_threshold));
		v.push_back(columns[128] + " = " + std::to_string(e.nuke_min_threshold));
		v.push_back(columns[129] + " = " + std::to_string(e.root_min_threshold));
		v.push_back(columns[130] + " = " + std::to_string(e.slow_min_threshold));
		v.push_back(columns[131] + " = " + std::to_string(e.snare_min_threshold));

		auto results = db.QueryDatabase(
			fmt::format(
				"UPDATE {} SET {} WHERE {} = {}",
				TableName(),
				Strings::Implode(", ", v),
				PrimaryKey(),
				e.bot_id
			)
		);

		return (results.Success() ? results.RowsAffected() : 0);
	}

	static BotData InsertOne(
		Database& db,
		BotData e
	)
	{
		std::vector<std::string> v;

		v.push_back(std::to_string(e.bot_id));
		v.push_back(std::to_string(e.owner_id));
		v.push_back(std::to_string(e.spells_id));
		v.push_back("'" + Strings::Escape(e.name) + "'");
		v.push_back("'" + Strings::Escape(e.last_name) + "'");
		v.push_back("'" + Strings::Escape(e.title) + "'");
		v.push_back("'" + Strings::Escape(e.suffix) + "'");
		v.push_back(std::to_string(e.zone_id));
		v.push_back(std::to_string(e.gender));
		v.push_back(std::to_string(e.race));
		v.push_back(std::to_string(e.class_));
		v.push_back(std::to_string(e.level));
		v.push_back(std::to_string(e.deity));
		v.push_back(std::to_string(e.creation_day));
		v.push_back(std::to_string(e.last_spawn));
		v.push_back(std::to_string(e.time_spawned));
		v.push_back(std::to_string(e.size));
		v.push_back(std::to_string(e.face));
		v.push_back(std::to_string(e.hair_color));
		v.push_back(std::to_string(e.hair_style));
		v.push_back(std::to_string(e.beard));
		v.push_back(std::to_string(e.beard_color));
		v.push_back(std::to_string(e.eye_color_1));
		v.push_back(std::to_string(e.eye_color_2));
		v.push_back(std::to_string(e.drakkin_heritage));
		v.push_back(std::to_string(e.drakkin_tattoo));
		v.push_back(std::to_string(e.drakkin_details));
		v.push_back(std::to_string(e.ac));
		v.push_back(std::to_string(e.atk));
		v.push_back(std::to_string(e.hp));
		v.push_back(std::to_string(e.mana));
		v.push_back(std::to_string(e.str));
		v.push_back(std::to_string(e.sta));
		v.push_back(std::to_string(e.cha));
		v.push_back(std::to_string(e.dex));
		v.push_back(std::to_string(e.int_));
		v.push_back(std::to_string(e.agi));
		v.push_back(std::to_string(e.wis));
		v.push_back(std::to_string(e.fire));
		v.push_back(std::to_string(e.cold));
		v.push_back(std::to_string(e.magic));
		v.push_back(std::to_string(e.poison));
		v.push_back(std::to_string(e.disease));
		v.push_back(std::to_string(e.corruption));
		v.push_back(std::to_string(e.show_helm));
		v.push_back(std::to_string(e.follow_distance));
		v.push_back(std::to_string(e.stop_melee_level));
		v.push_back(std::to_string(e.expansion_bitmask));
		v.push_back(std::to_string(e.enforce_spell_settings));
		v.push_back(std::to_string(e.archery_setting));
		v.push_back(std::to_string(e.petsettype_setting));
		v.push_back(std::to_string(e.hold_buffs));
		v.push_back(std::to_string(e.hold_complete_heals));
		v.push_back(std::to_string(e.hold_cures));
		v.push_back(std::to_string(e.hold_dots));
		v.push_back(std::to_string(e.hold_debuffs));
		v.push_back(std::to_string(e.hold_dispels));
		v.push_back(std::to_string(e.hold_escapes));
		v.push_back(std::to_string(e.hold_fast_heals));
		v.push_back(std::to_string(e.hold_group_heals));
		v.push_back(std::to_string(e.hold_hateredux));
		v.push_back(std::to_string(e.hold_heals));
		v.push_back(std::to_string(e.hold_hot_heals));
		v.push_back(std::to_string(e.hold_incombatbuffs));
		v.push_back(std::to_string(e.hold_incombatbuffsongs));
		v.push_back(std::to_string(e.hold_lifetaps));
		v.push_back(std::to_string(e.hold_mez));
		v.push_back(std::to_string(e.hold_nukes));
		v.push_back(std::to_string(e.hold_outofcombatbuffsongs));
		v.push_back(std::to_string(e.hold_pet_buffs));
		v.push_back(std::to_string(e.hold_pet_heals));
		v.push_back(std::to_string(e.hold_pets));
		v.push_back(std::to_string(e.hold_precombatbuffs));
		v.push_back(std::to_string(e.hold_precombatbuffsongs));
		v.push_back(std::to_string(e.hold_regular_heals));
		v.push_back(std::to_string(e.hold_roots));
		v.push_back(std::to_string(e.hold_slows));
		v.push_back(std::to_string(e.hold_snares));
		v.push_back(std::to_string(e.auto_ds));
		v.push_back(std::to_string(e.auto_resist));
		v.push_back(std::to_string(e.behind_mob));
		v.push_back(std::to_string(e.caster_range));
		v.push_back(std::to_string(e.buff_delay));
		v.push_back(std::to_string(e.complete_heal_delay));
		v.push_back(std::to_string(e.cure_delay));
		v.push_back(std::to_string(e.debuff_delay));
		v.push_back(std::to_string(e.dispel_delay));
		v.push_back(std::to_string(e.dot_delay));
		v.push_back(std::to_string(e.escape_delay));
		v.push_back(std::to_string(e.fast_heal_delay));
		v.push_back(std::to_string(e.hate_redux_delay));
		v.push_back(std::to_string(e.heal_delay));
		v.push_back(std::to_string(e.hot_heal_delay));
		v.push_back(std::to_string(e.incombatbuff_delay));
		v.push_back(std::to_string(e.lifetap_delay));
		v.push_back(std::to_string(e.mez_delay));
		v.push_back(std::to_string(e.nuke_delay));
		v.push_back(std::to_string(e.root_delay));
		v.push_back(std::to_string(e.slow_delay));
		v.push_back(std::to_string(e.snare_delay));
		v.push_back(std::to_string(e.buff_threshold));
		v.push_back(std::to_string(e.complete_heal_threshold));
		v.push_back(std::to_string(e.cure_threshold));
		v.push_back(std::to_string(e.debuff_threshold));
		v.push_back(std::to_string(e.dispel_threshold));
		v.push_back(std::to_string(e.dot_threshold));
		v.push_back(std::to_string(e.escape_threshold));
		v.push_back(std::to_string(e.fast_heal_threshold));
		v.push_back(std::to_string(e.hate_redux_threshold));
		v.push_back(std::to_string(e.heal_threshold));
		v.push_back(std::to_string(e.hot_heal_threshold));
		v.push_back(std::to_string(e.incombatbuff_threshold));
		v.push_back(std::to_string(e.lifetap_threshold));
		v.push_back(std::to_string(e.mez_threshold));
		v.push_back(std::to_string(e.nuke_threshold));
		v.push_back(std::to_string(e.root_threshold));
		v.push_back(std::to_string(e.slow_threshold));
		v.push_back(std::to_string(e.snare_threshold));
		v.push_back(std::to_string(e.buff_min_threshold));
		v.push_back(std::to_string(e.cure_min_threshold));
		v.push_back(std::to_string(e.debuff_min_threshold));
		v.push_back(std::to_string(e.dispel_min_threshold));
		v.push_back(std::to_string(e.dot_min_threshold));
		v.push_back(std::to_string(e.escape_min_threshold));
		v.push_back(std::to_string(e.hate_redux_min_threshold));
		v.push_back(std::to_string(e.incombatbuff_min_threshold));
		v.push_back(std::to_string(e.lifetap_min_threshold));
		v.push_back(std::to_string(e.mez_min_threshold));
		v.push_back(std::to_string(e.nuke_min_threshold));
		v.push_back(std::to_string(e.root_min_threshold));
		v.push_back(std::to_string(e.slow_min_threshold));
		v.push_back(std::to_string(e.snare_min_threshold));

		auto results = db.QueryDatabase(
			fmt::format(
				"{} VALUES ({})",
				BaseInsert(),
				Strings::Implode(",", v)
			)
		);

		if (results.Success()) {
			e.bot_id = results.LastInsertedID();
			return e;
		}

		e = NewEntity();

		return e;
	}

	static int InsertMany(
		Database& db,
		const std::vector<BotData> &entries
	)
	{
		std::vector<std::string> insert_chunks;

		for (auto &e: entries) {
			std::vector<std::string> v;

			v.push_back(std::to_string(e.bot_id));
			v.push_back(std::to_string(e.owner_id));
			v.push_back(std::to_string(e.spells_id));
			v.push_back("'" + Strings::Escape(e.name) + "'");
			v.push_back("'" + Strings::Escape(e.last_name) + "'");
			v.push_back("'" + Strings::Escape(e.title) + "'");
			v.push_back("'" + Strings::Escape(e.suffix) + "'");
			v.push_back(std::to_string(e.zone_id));
			v.push_back(std::to_string(e.gender));
			v.push_back(std::to_string(e.race));
			v.push_back(std::to_string(e.class_));
			v.push_back(std::to_string(e.level));
			v.push_back(std::to_string(e.deity));
			v.push_back(std::to_string(e.creation_day));
			v.push_back(std::to_string(e.last_spawn));
			v.push_back(std::to_string(e.time_spawned));
			v.push_back(std::to_string(e.size));
			v.push_back(std::to_string(e.face));
			v.push_back(std::to_string(e.hair_color));
			v.push_back(std::to_string(e.hair_style));
			v.push_back(std::to_string(e.beard));
			v.push_back(std::to_string(e.beard_color));
			v.push_back(std::to_string(e.eye_color_1));
			v.push_back(std::to_string(e.eye_color_2));
			v.push_back(std::to_string(e.drakkin_heritage));
			v.push_back(std::to_string(e.drakkin_tattoo));
			v.push_back(std::to_string(e.drakkin_details));
			v.push_back(std::to_string(e.ac));
			v.push_back(std::to_string(e.atk));
			v.push_back(std::to_string(e.hp));
			v.push_back(std::to_string(e.mana));
			v.push_back(std::to_string(e.str));
			v.push_back(std::to_string(e.sta));
			v.push_back(std::to_string(e.cha));
			v.push_back(std::to_string(e.dex));
			v.push_back(std::to_string(e.int_));
			v.push_back(std::to_string(e.agi));
			v.push_back(std::to_string(e.wis));
			v.push_back(std::to_string(e.fire));
			v.push_back(std::to_string(e.cold));
			v.push_back(std::to_string(e.magic));
			v.push_back(std::to_string(e.poison));
			v.push_back(std::to_string(e.disease));
			v.push_back(std::to_string(e.corruption));
			v.push_back(std::to_string(e.show_helm));
			v.push_back(std::to_string(e.follow_distance));
			v.push_back(std::to_string(e.stop_melee_level));
			v.push_back(std::to_string(e.expansion_bitmask));
			v.push_back(std::to_string(e.enforce_spell_settings));
			v.push_back(std::to_string(e.archery_setting));
			v.push_back(std::to_string(e.petsettype_setting));
			v.push_back(std::to_string(e.hold_buffs));
			v.push_back(std::to_string(e.hold_complete_heals));
			v.push_back(std::to_string(e.hold_cures));
			v.push_back(std::to_string(e.hold_dots));
			v.push_back(std::to_string(e.hold_debuffs));
			v.push_back(std::to_string(e.hold_dispels));
			v.push_back(std::to_string(e.hold_escapes));
			v.push_back(std::to_string(e.hold_fast_heals));
			v.push_back(std::to_string(e.hold_group_heals));
			v.push_back(std::to_string(e.hold_hateredux));
			v.push_back(std::to_string(e.hold_heals));
			v.push_back(std::to_string(e.hold_hot_heals));
			v.push_back(std::to_string(e.hold_incombatbuffs));
			v.push_back(std::to_string(e.hold_incombatbuffsongs));
			v.push_back(std::to_string(e.hold_lifetaps));
			v.push_back(std::to_string(e.hold_mez));
			v.push_back(std::to_string(e.hold_nukes));
			v.push_back(std::to_string(e.hold_outofcombatbuffsongs));
			v.push_back(std::to_string(e.hold_pet_buffs));
			v.push_back(std::to_string(e.hold_pet_heals));
			v.push_back(std::to_string(e.hold_pets));
			v.push_back(std::to_string(e.hold_precombatbuffs));
			v.push_back(std::to_string(e.hold_precombatbuffsongs));
			v.push_back(std::to_string(e.hold_regular_heals));
			v.push_back(std::to_string(e.hold_roots));
			v.push_back(std::to_string(e.hold_slows));
			v.push_back(std::to_string(e.hold_snares));
			v.push_back(std::to_string(e.auto_ds));
			v.push_back(std::to_string(e.auto_resist));
			v.push_back(std::to_string(e.behind_mob));
			v.push_back(std::to_string(e.caster_range));
			v.push_back(std::to_string(e.buff_delay));
			v.push_back(std::to_string(e.complete_heal_delay));
			v.push_back(std::to_string(e.cure_delay));
			v.push_back(std::to_string(e.debuff_delay));
			v.push_back(std::to_string(e.dispel_delay));
			v.push_back(std::to_string(e.dot_delay));
			v.push_back(std::to_string(e.escape_delay));
			v.push_back(std::to_string(e.fast_heal_delay));
			v.push_back(std::to_string(e.hate_redux_delay));
			v.push_back(std::to_string(e.heal_delay));
			v.push_back(std::to_string(e.hot_heal_delay));
			v.push_back(std::to_string(e.incombatbuff_delay));
			v.push_back(std::to_string(e.lifetap_delay));
			v.push_back(std::to_string(e.mez_delay));
			v.push_back(std::to_string(e.nuke_delay));
			v.push_back(std::to_string(e.root_delay));
			v.push_back(std::to_string(e.slow_delay));
			v.push_back(std::to_string(e.snare_delay));
			v.push_back(std::to_string(e.buff_threshold));
			v.push_back(std::to_string(e.complete_heal_threshold));
			v.push_back(std::to_string(e.cure_threshold));
			v.push_back(std::to_string(e.debuff_threshold));
			v.push_back(std::to_string(e.dispel_threshold));
			v.push_back(std::to_string(e.dot_threshold));
			v.push_back(std::to_string(e.escape_threshold));
			v.push_back(std::to_string(e.fast_heal_threshold));
			v.push_back(std::to_string(e.hate_redux_threshold));
			v.push_back(std::to_string(e.heal_threshold));
			v.push_back(std::to_string(e.hot_heal_threshold));
			v.push_back(std::to_string(e.incombatbuff_threshold));
			v.push_back(std::to_string(e.lifetap_threshold));
			v.push_back(std::to_string(e.mez_threshold));
			v.push_back(std::to_string(e.nuke_threshold));
			v.push_back(std::to_string(e.root_threshold));
			v.push_back(std::to_string(e.slow_threshold));
			v.push_back(std::to_string(e.snare_threshold));
			v.push_back(std::to_string(e.buff_min_threshold));
			v.push_back(std::to_string(e.cure_min_threshold));
			v.push_back(std::to_string(e.debuff_min_threshold));
			v.push_back(std::to_string(e.dispel_min_threshold));
			v.push_back(std::to_string(e.dot_min_threshold));
			v.push_back(std::to_string(e.escape_min_threshold));
			v.push_back(std::to_string(e.hate_redux_min_threshold));
			v.push_back(std::to_string(e.incombatbuff_min_threshold));
			v.push_back(std::to_string(e.lifetap_min_threshold));
			v.push_back(std::to_string(e.mez_min_threshold));
			v.push_back(std::to_string(e.nuke_min_threshold));
			v.push_back(std::to_string(e.root_min_threshold));
			v.push_back(std::to_string(e.slow_min_threshold));
			v.push_back(std::to_string(e.snare_min_threshold));

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

	static std::vector<BotData> All(Database& db)
	{
		std::vector<BotData> all_entries;

		auto results = db.QueryDatabase(
			fmt::format(
				"{}",
				BaseSelect()
			)
		);

		all_entries.reserve(results.RowCount());

		for (auto row = results.begin(); row != results.end(); ++row) {
			BotData e{};

			e.bot_id						= static_cast<uint32_t>(strtoul(row[0], nullptr, 10));
			e.owner_id						= static_cast<uint32_t>(strtoul(row[1], nullptr, 10));
			e.spells_id						= static_cast<uint32_t>(strtoul(row[2], nullptr, 10));
			e.name							= row[3] ? row[3] : "";
			e.last_name						= row[4] ? row[4] : "";
			e.title							= row[5] ? row[5] : "";
			e.suffix						= row[6] ? row[6] : "";
			e.zone_id						= static_cast<int16_t>(atoi(row[7]));
			e.gender						= static_cast<int8_t>(atoi(row[8]));
			e.race							= static_cast<int16_t>(atoi(row[9]));
			e.class_						= static_cast<int8_t>(atoi(row[10]));
			e.level							= static_cast<uint8_t>(strtoul(row[11], nullptr, 10));
			e.deity							= static_cast<uint32_t>(strtoul(row[12], nullptr, 10));
			e.creation_day					= static_cast<uint32_t>(strtoul(row[13], nullptr, 10));
			e.last_spawn					= static_cast<uint32_t>(strtoul(row[14], nullptr, 10));
			e.time_spawned					= static_cast<uint32_t>(strtoul(row[15], nullptr, 10));
			e.size							= strtof(row[16], nullptr);
			e.face							= static_cast<int32_t>(atoi(row[17]));
			e.hair_color					= static_cast<int32_t>(atoi(row[18]));
			e.hair_style					= static_cast<int32_t>(atoi(row[19]));
			e.beard							= static_cast<int32_t>(atoi(row[20]));
			e.beard_color					= static_cast<int32_t>(atoi(row[21]));
			e.eye_color_1					= static_cast<int32_t>(atoi(row[22]));
			e.eye_color_2					= static_cast<int32_t>(atoi(row[23]));
			e.drakkin_heritage				= static_cast<int32_t>(atoi(row[24]));
			e.drakkin_tattoo				= static_cast<int32_t>(atoi(row[25]));
			e.drakkin_details				= static_cast<int32_t>(atoi(row[26]));
			e.ac							= static_cast<int16_t>(atoi(row[27]));
			e.atk							= static_cast<int32_t>(atoi(row[28]));
			e.hp							= static_cast<int32_t>(atoi(row[29]));
			e.mana							= static_cast<int32_t>(atoi(row[30]));
			e.str							= static_cast<int32_t>(atoi(row[31]));
			e.sta							= static_cast<int32_t>(atoi(row[32]));
			e.cha							= static_cast<int32_t>(atoi(row[33]));
			e.dex							= static_cast<int32_t>(atoi(row[34]));
			e.int_							= static_cast<int32_t>(atoi(row[35]));
			e.agi							= static_cast<int32_t>(atoi(row[36]));
			e.wis							= static_cast<int32_t>(atoi(row[37]));
			e.fire							= static_cast<int16_t>(atoi(row[38]));
			e.cold							= static_cast<int16_t>(atoi(row[39]));
			e.magic							= static_cast<int16_t>(atoi(row[40]));
			e.poison						= static_cast<int16_t>(atoi(row[41]));
			e.disease						= static_cast<int16_t>(atoi(row[42]));
			e.corruption					= static_cast<int16_t>(atoi(row[43]));
			e.show_helm						= static_cast<uint32_t>(strtoul(row[44], nullptr, 10));
			e.follow_distance				= static_cast<uint32_t>(strtoul(row[45], nullptr, 10));
			e.stop_melee_level				= static_cast<uint8_t>(strtoul(row[46], nullptr, 10));
			e.expansion_bitmask				= static_cast<int32_t>(atoi(row[47]));
			e.enforce_spell_settings		= static_cast<uint8_t>(strtoul(row[48], nullptr, 10));
			e.archery_setting				= static_cast<uint8_t>(strtoul(row[49], nullptr, 10));
			e.petsettype_setting		    = static_cast<uint8_t>(strtoul(row[50], nullptr, 10));
			e.hold_buffs					= static_cast<uint8_t>(strtoul(row[51], nullptr, 10));
			e.hold_complete_heals		    = static_cast<uint8_t>(strtoul(row[52], nullptr, 10));
			e.hold_cures					= static_cast<uint8_t>(strtoul(row[53], nullptr, 10));
			e.hold_dots						= static_cast<uint8_t>(strtoul(row[54], nullptr, 10));
			e.hold_debuffs					= static_cast<uint8_t>(strtoul(row[55], nullptr, 10));
			e.hold_dispels					= static_cast<uint8_t>(strtoul(row[56], nullptr, 10));
			e.hold_escapes					= static_cast<uint8_t>(strtoul(row[57], nullptr, 10));
			e.hold_fast_heals			    = static_cast<uint8_t>(strtoul(row[58], nullptr, 10));
			e.hold_group_heals				= static_cast<uint8_t>(strtoul(row[59], nullptr, 10));
			e.hold_hateredux				= static_cast<uint8_t>(strtoul(row[60], nullptr, 10));
			e.hold_heals					= static_cast<uint8_t>(strtoul(row[61], nullptr, 10));
			e.hold_hot_heals				= static_cast<uint8_t>(strtoul(row[62], nullptr, 10));
			e.hold_incombatbuffs			= static_cast<uint8_t>(strtoul(row[63], nullptr, 10));
			e.hold_incombatbuffsongs		= static_cast<uint8_t>(strtoul(row[64], nullptr, 10));
			e.hold_lifetaps					= static_cast<uint8_t>(strtoul(row[65], nullptr, 10));
			e.hold_mez						= static_cast<uint8_t>(strtoul(row[66], nullptr, 10));
			e.hold_nukes					= static_cast<uint8_t>(strtoul(row[67], nullptr, 10));
			e.hold_outofcombatbuffsongs		= static_cast<uint8_t>(strtoul(row[68], nullptr, 10));
			e.hold_pet_buffs				= static_cast<uint8_t>(strtoul(row[69], nullptr, 10));
			e.hold_pet_heals				= static_cast<uint8_t>(strtoul(row[70], nullptr, 10));
			e.hold_pets						= static_cast<uint8_t>(strtoul(row[71], nullptr, 10));
			e.hold_precombatbuffs			= static_cast<uint8_t>(strtoul(row[72], nullptr, 10));
			e.hold_precombatbuffsongs		= static_cast<uint8_t>(strtoul(row[73], nullptr, 10));
			e.hold_regular_heals			= static_cast<uint8_t>(strtoul(row[74], nullptr, 10));
			e.hold_roots					= static_cast<uint8_t>(strtoul(row[75], nullptr, 10));
			e.hold_slows					= static_cast<uint8_t>(strtoul(row[76], nullptr, 10));
			e.hold_snares					= static_cast<uint8_t>(strtoul(row[77], nullptr, 10));
			e.auto_ds						= static_cast<uint8_t>(strtoul(row[78], nullptr, 10));
			e.auto_resist					= static_cast<uint8_t>(strtoul(row[79], nullptr, 10));
			e.behind_mob					= static_cast<uint8_t>(strtoul(row[80], nullptr, 10));
			e.caster_range					= static_cast<uint32_t>(strtoul(row[81], nullptr, 10));
			e.buff_delay					= static_cast<uint32_t>(strtoul(row[82], nullptr, 10));
			e.complete_heal_delay			= static_cast<uint32_t>(strtoul(row[83], nullptr, 10));
			e.cure_delay					= static_cast<uint32_t>(strtoul(row[84], nullptr, 10));
			e.debuff_delay					= static_cast<uint32_t>(strtoul(row[85], nullptr, 10));
			e.dispel_delay					= static_cast<uint32_t>(strtoul(row[86], nullptr, 10));
			e.dot_delay						= static_cast<uint32_t>(strtoul(row[87], nullptr, 10));
			e.escape_delay					= static_cast<uint32_t>(strtoul(row[88], nullptr, 10));
			e.fast_heal_delay				= static_cast<uint32_t>(strtoul(row[89], nullptr, 10));
			e.hate_redux_delay				= static_cast<uint32_t>(strtoul(row[90], nullptr, 10));
			e.heal_delay					= static_cast<uint32_t>(strtoul(row[91], nullptr, 10));
			e.hot_heal_delay				= static_cast<uint32_t>(strtoul(row[92], nullptr, 10));
			e.incombatbuff_delay			= static_cast<uint32_t>(strtoul(row[93], nullptr, 10));
			e.lifetap_delay					= static_cast<uint32_t>(strtoul(row[94], nullptr, 10));
			e.mez_delay						= static_cast<uint32_t>(strtoul(row[95], nullptr, 10));
			e.nuke_delay					= static_cast<uint32_t>(strtoul(row[96], nullptr, 10));
			e.root_delay					= static_cast<uint32_t>(strtoul(row[97], nullptr, 10));
			e.slow_delay					= static_cast<uint32_t>(strtoul(row[98], nullptr, 10));
			e.snare_delay					= static_cast<uint32_t>(strtoul(row[99], nullptr, 10));
			e.buff_threshold				= static_cast<uint8_t>(strtoul(row[100], nullptr, 10));
			e.complete_heal_threshold		= static_cast<uint8_t>(strtoul(row[101], nullptr, 10));
			e.cure_threshold				= static_cast<uint8_t>(strtoul(row[102], nullptr, 10));
			e.debuff_threshold				= static_cast<uint8_t>(strtoul(row[103], nullptr, 10));
			e.dispel_threshold				= static_cast<uint8_t>(strtoul(row[104], nullptr, 10));
			e.dot_threshold					= static_cast<uint8_t>(strtoul(row[105], nullptr, 10));
			e.escape_threshold				= static_cast<uint8_t>(strtoul(row[106], nullptr, 10));
			e.fast_heal_threshold			= static_cast<uint8_t>(strtoul(row[107], nullptr, 10));
			e.hate_redux_threshold			= static_cast<uint8_t>(strtoul(row[108], nullptr, 10));
			e.heal_threshold				= static_cast<uint8_t>(strtoul(row[109], nullptr, 10));
			e.hot_heal_threshold			= static_cast<uint8_t>(strtoul(row[110], nullptr, 10));
			e.incombatbuff_threshold		= static_cast<uint8_t>(strtoul(row[111], nullptr, 10));
			e.lifetap_threshold				= static_cast<uint8_t>(strtoul(row[112], nullptr, 10));
			e.mez_threshold					= static_cast<uint8_t>(strtoul(row[113], nullptr, 10));
			e.nuke_threshold				= static_cast<uint8_t>(strtoul(row[114], nullptr, 10));
			e.root_threshold				= static_cast<uint8_t>(strtoul(row[115], nullptr, 10));
			e.slow_threshold				= static_cast<uint8_t>(strtoul(row[116], nullptr, 10));
			e.snare_threshold				= static_cast<uint8_t>(strtoul(row[117], nullptr, 10));
			e.buff_min_threshold			= static_cast<uint8_t>(strtoul(row[118], nullptr, 10));
			e.cure_min_threshold			= static_cast<uint8_t>(strtoul(row[119], nullptr, 10));
			e.debuff_min_threshold			= static_cast<uint8_t>(strtoul(row[120], nullptr, 10));
			e.dispel_min_threshold			= static_cast<uint8_t>(strtoul(row[121], nullptr, 10));
			e.dot_min_threshold				= static_cast<uint8_t>(strtoul(row[122], nullptr, 10));
			e.escape_min_threshold			= static_cast<uint8_t>(strtoul(row[123], nullptr, 10));
			e.hate_redux_min_threshold		= static_cast<uint8_t>(strtoul(row[124], nullptr, 10));
			e.incombatbuff_min_threshold	= static_cast<uint8_t>(strtoul(row[125], nullptr, 10));
			e.lifetap_min_threshold			= static_cast<uint8_t>(strtoul(row[126], nullptr, 10));
			e.mez_min_threshold				= static_cast<uint8_t>(strtoul(row[127], nullptr, 10));
			e.nuke_min_threshold			= static_cast<uint8_t>(strtoul(row[128], nullptr, 10));
			e.root_min_threshold			= static_cast<uint8_t>(strtoul(row[129], nullptr, 10));
			e.slow_min_threshold			= static_cast<uint8_t>(strtoul(row[130], nullptr, 10));
			e.snare_min_threshold			= static_cast<uint8_t>(strtoul(row[131], nullptr, 10));

			all_entries.push_back(e);
		}

		return all_entries;
	}

	static std::vector<BotData> GetWhere(Database& db, const std::string &where_filter)
	{
		std::vector<BotData> all_entries;

		auto results = db.QueryDatabase(
			fmt::format(
				"{} WHERE {}",
				BaseSelect(),
				where_filter
			)
		);

		all_entries.reserve(results.RowCount());

		for (auto row = results.begin(); row != results.end(); ++row) {
			BotData e{};

			e.bot_id						= static_cast<uint32_t>(strtoul(row[0], nullptr, 10));
			e.owner_id						= static_cast<uint32_t>(strtoul(row[1], nullptr, 10));
			e.spells_id						= static_cast<uint32_t>(strtoul(row[2], nullptr, 10));
			e.name							= row[3] ? row[3] : "";
			e.last_name						= row[4] ? row[4] : "";
			e.title							= row[5] ? row[5] : "";
			e.suffix						= row[6] ? row[6] : "";
			e.zone_id						= static_cast<int16_t>(atoi(row[7]));
			e.gender						= static_cast<int8_t>(atoi(row[8]));
			e.race							= static_cast<int16_t>(atoi(row[9]));
			e.class_						= static_cast<int8_t>(atoi(row[10]));
			e.level							= static_cast<uint8_t>(strtoul(row[11], nullptr, 10));
			e.deity							= static_cast<uint32_t>(strtoul(row[12], nullptr, 10));
			e.creation_day					= static_cast<uint32_t>(strtoul(row[13], nullptr, 10));
			e.last_spawn					= static_cast<uint32_t>(strtoul(row[14], nullptr, 10));
			e.time_spawned					= static_cast<uint32_t>(strtoul(row[15], nullptr, 10));
			e.size							= strtof(row[16], nullptr);
			e.face							= static_cast<int32_t>(atoi(row[17]));
			e.hair_color					= static_cast<int32_t>(atoi(row[18]));
			e.hair_style					= static_cast<int32_t>(atoi(row[19]));
			e.beard							= static_cast<int32_t>(atoi(row[20]));
			e.beard_color					= static_cast<int32_t>(atoi(row[21]));
			e.eye_color_1					= static_cast<int32_t>(atoi(row[22]));
			e.eye_color_2					= static_cast<int32_t>(atoi(row[23]));
			e.drakkin_heritage				= static_cast<int32_t>(atoi(row[24]));
			e.drakkin_tattoo				= static_cast<int32_t>(atoi(row[25]));
			e.drakkin_details				= static_cast<int32_t>(atoi(row[26]));
			e.ac							= static_cast<int16_t>(atoi(row[27]));
			e.atk							= static_cast<int32_t>(atoi(row[28]));
			e.hp							= static_cast<int32_t>(atoi(row[29]));
			e.mana							= static_cast<int32_t>(atoi(row[30]));
			e.str							= static_cast<int32_t>(atoi(row[31]));
			e.sta							= static_cast<int32_t>(atoi(row[32]));
			e.cha							= static_cast<int32_t>(atoi(row[33]));
			e.dex							= static_cast<int32_t>(atoi(row[34]));
			e.int_							= static_cast<int32_t>(atoi(row[35]));
			e.agi							= static_cast<int32_t>(atoi(row[36]));
			e.wis							= static_cast<int32_t>(atoi(row[37]));
			e.fire							= static_cast<int16_t>(atoi(row[38]));
			e.cold							= static_cast<int16_t>(atoi(row[39]));
			e.magic							= static_cast<int16_t>(atoi(row[40]));
			e.poison						= static_cast<int16_t>(atoi(row[41]));
			e.disease						= static_cast<int16_t>(atoi(row[42]));
			e.corruption					= static_cast<int16_t>(atoi(row[43]));
			e.show_helm						= static_cast<uint32_t>(strtoul(row[44], nullptr, 10));
			e.follow_distance				= static_cast<uint32_t>(strtoul(row[45], nullptr, 10));
			e.stop_melee_level				= static_cast<uint8_t>(strtoul(row[46], nullptr, 10));
			e.expansion_bitmask				= static_cast<int32_t>(atoi(row[47]));
			e.enforce_spell_settings		= static_cast<uint8_t>(strtoul(row[48], nullptr, 10));
			e.archery_setting				= static_cast<uint8_t>(strtoul(row[49], nullptr, 10));
			e.petsettype_setting		    = static_cast<uint8_t>(strtoul(row[50], nullptr, 10));
			e.hold_buffs					= static_cast<uint8_t>(strtoul(row[51], nullptr, 10));
			e.hold_complete_heals		    = static_cast<uint8_t>(strtoul(row[52], nullptr, 10));
			e.hold_cures					= static_cast<uint8_t>(strtoul(row[53], nullptr, 10));
			e.hold_dots						= static_cast<uint8_t>(strtoul(row[54], nullptr, 10));
			e.hold_debuffs					= static_cast<uint8_t>(strtoul(row[55], nullptr, 10));
			e.hold_dispels					= static_cast<uint8_t>(strtoul(row[56], nullptr, 10));
			e.hold_escapes					= static_cast<uint8_t>(strtoul(row[57], nullptr, 10));
			e.hold_fast_heals			    = static_cast<uint8_t>(strtoul(row[58], nullptr, 10));
			e.hold_group_heals				= static_cast<uint8_t>(strtoul(row[59], nullptr, 10));
			e.hold_hateredux				= static_cast<uint8_t>(strtoul(row[60], nullptr, 10));
			e.hold_heals					= static_cast<uint8_t>(strtoul(row[61], nullptr, 10));
			e.hold_hot_heals				= static_cast<uint8_t>(strtoul(row[62], nullptr, 10));
			e.hold_incombatbuffs			= static_cast<uint8_t>(strtoul(row[63], nullptr, 10));
			e.hold_incombatbuffsongs		= static_cast<uint8_t>(strtoul(row[64], nullptr, 10));
			e.hold_lifetaps					= static_cast<uint8_t>(strtoul(row[65], nullptr, 10));
			e.hold_mez						= static_cast<uint8_t>(strtoul(row[66], nullptr, 10));
			e.hold_nukes					= static_cast<uint8_t>(strtoul(row[67], nullptr, 10));
			e.hold_outofcombatbuffsongs		= static_cast<uint8_t>(strtoul(row[68], nullptr, 10));
			e.hold_pet_buffs				= static_cast<uint8_t>(strtoul(row[69], nullptr, 10));
			e.hold_pet_heals				= static_cast<uint8_t>(strtoul(row[70], nullptr, 10));
			e.hold_pets						= static_cast<uint8_t>(strtoul(row[71], nullptr, 10));
			e.hold_precombatbuffs			= static_cast<uint8_t>(strtoul(row[72], nullptr, 10));
			e.hold_precombatbuffsongs		= static_cast<uint8_t>(strtoul(row[73], nullptr, 10));
			e.hold_regular_heals			= static_cast<uint8_t>(strtoul(row[74], nullptr, 10));
			e.hold_roots					= static_cast<uint8_t>(strtoul(row[75], nullptr, 10));
			e.hold_slows					= static_cast<uint8_t>(strtoul(row[76], nullptr, 10));
			e.hold_snares					= static_cast<uint8_t>(strtoul(row[77], nullptr, 10));
			e.auto_ds						= static_cast<uint8_t>(strtoul(row[78], nullptr, 10));
			e.auto_resist					= static_cast<uint8_t>(strtoul(row[79], nullptr, 10));
			e.behind_mob					= static_cast<uint8_t>(strtoul(row[80], nullptr, 10));
			e.caster_range					= static_cast<uint32_t>(strtoul(row[81], nullptr, 10));
			e.buff_delay					= static_cast<uint32_t>(strtoul(row[82], nullptr, 10));
			e.complete_heal_delay			= static_cast<uint32_t>(strtoul(row[83], nullptr, 10));
			e.cure_delay					= static_cast<uint32_t>(strtoul(row[84], nullptr, 10));
			e.debuff_delay					= static_cast<uint32_t>(strtoul(row[85], nullptr, 10));
			e.dispel_delay					= static_cast<uint32_t>(strtoul(row[86], nullptr, 10));
			e.dot_delay						= static_cast<uint32_t>(strtoul(row[87], nullptr, 10));
			e.escape_delay					= static_cast<uint32_t>(strtoul(row[88], nullptr, 10));
			e.fast_heal_delay				= static_cast<uint32_t>(strtoul(row[89], nullptr, 10));
			e.hate_redux_delay				= static_cast<uint32_t>(strtoul(row[90], nullptr, 10));
			e.heal_delay					= static_cast<uint32_t>(strtoul(row[91], nullptr, 10));
			e.hot_heal_delay				= static_cast<uint32_t>(strtoul(row[92], nullptr, 10));
			e.incombatbuff_delay			= static_cast<uint32_t>(strtoul(row[93], nullptr, 10));
			e.lifetap_delay					= static_cast<uint32_t>(strtoul(row[94], nullptr, 10));
			e.mez_delay						= static_cast<uint32_t>(strtoul(row[95], nullptr, 10));
			e.nuke_delay					= static_cast<uint32_t>(strtoul(row[96], nullptr, 10));
			e.root_delay					= static_cast<uint32_t>(strtoul(row[97], nullptr, 10));
			e.slow_delay					= static_cast<uint32_t>(strtoul(row[98], nullptr, 10));
			e.snare_delay					= static_cast<uint32_t>(strtoul(row[99], nullptr, 10));
			e.buff_threshold				= static_cast<uint8_t>(strtoul(row[100], nullptr, 10));
			e.complete_heal_threshold		= static_cast<uint8_t>(strtoul(row[101], nullptr, 10));
			e.cure_threshold				= static_cast<uint8_t>(strtoul(row[102], nullptr, 10));
			e.debuff_threshold				= static_cast<uint8_t>(strtoul(row[103], nullptr, 10));
			e.dispel_threshold				= static_cast<uint8_t>(strtoul(row[104], nullptr, 10));
			e.dot_threshold					= static_cast<uint8_t>(strtoul(row[105], nullptr, 10));
			e.escape_threshold				= static_cast<uint8_t>(strtoul(row[106], nullptr, 10));
			e.fast_heal_threshold			= static_cast<uint8_t>(strtoul(row[107], nullptr, 10));
			e.hate_redux_threshold			= static_cast<uint8_t>(strtoul(row[108], nullptr, 10));
			e.heal_threshold				= static_cast<uint8_t>(strtoul(row[109], nullptr, 10));
			e.hot_heal_threshold			= static_cast<uint8_t>(strtoul(row[110], nullptr, 10));
			e.incombatbuff_threshold		= static_cast<uint8_t>(strtoul(row[111], nullptr, 10));
			e.lifetap_threshold				= static_cast<uint8_t>(strtoul(row[112], nullptr, 10));
			e.mez_threshold					= static_cast<uint8_t>(strtoul(row[113], nullptr, 10));
			e.nuke_threshold				= static_cast<uint8_t>(strtoul(row[114], nullptr, 10));
			e.root_threshold				= static_cast<uint8_t>(strtoul(row[115], nullptr, 10));
			e.slow_threshold				= static_cast<uint8_t>(strtoul(row[116], nullptr, 10));
			e.snare_threshold				= static_cast<uint8_t>(strtoul(row[117], nullptr, 10));
			e.buff_min_threshold			= static_cast<uint8_t>(strtoul(row[118], nullptr, 10));
			e.cure_min_threshold			= static_cast<uint8_t>(strtoul(row[119], nullptr, 10));
			e.debuff_min_threshold			= static_cast<uint8_t>(strtoul(row[120], nullptr, 10));
			e.dispel_min_threshold			= static_cast<uint8_t>(strtoul(row[121], nullptr, 10));
			e.dot_min_threshold				= static_cast<uint8_t>(strtoul(row[122], nullptr, 10));
			e.escape_min_threshold			= static_cast<uint8_t>(strtoul(row[123], nullptr, 10));
			e.hate_redux_min_threshold		= static_cast<uint8_t>(strtoul(row[124], nullptr, 10));
			e.incombatbuff_min_threshold	= static_cast<uint8_t>(strtoul(row[125], nullptr, 10));
			e.lifetap_min_threshold			= static_cast<uint8_t>(strtoul(row[126], nullptr, 10));
			e.mez_min_threshold				= static_cast<uint8_t>(strtoul(row[127], nullptr, 10));
			e.nuke_min_threshold			= static_cast<uint8_t>(strtoul(row[128], nullptr, 10));
			e.root_min_threshold			= static_cast<uint8_t>(strtoul(row[129], nullptr, 10));
			e.slow_min_threshold			= static_cast<uint8_t>(strtoul(row[130], nullptr, 10));
			e.snare_min_threshold			= static_cast<uint8_t>(strtoul(row[131], nullptr, 10));

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

};

#endif //EQEMU_BASE_BOT_DATA_REPOSITORY_H
