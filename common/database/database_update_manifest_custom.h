/*	EQEmu: EQEmulator

	Copyright (C) 2001-2026 EQEmu Development Team

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include "common/database/database_update.h"

#include <vector>

std::vector<ManifestEntry> manifest_entries_custom = {
	ManifestEntry{
		.version = 1,
		.description = "2025_10_22_drop_unused_tables",
		.check = "SHOW TABLES LIKE `aa_actions`",
		.condition = "not_empty",
		.match = "",
		.sql = R"(
DROP TABLE IF EXISTS `aa_actions`;
DROP TABLE IF EXISTS `aa_effects`;
DROP TABLE IF EXISTS `aa_required_level_cost`;
DROP TABLE IF EXISTS `aa_timers`;
DROP TABLE IF EXISTS `altadv_vars`;
DROP TABLE IF EXISTS `augment_work_orders`;
DROP TABLE IF EXISTS `botguildmembers_old`;
DROP TABLE IF EXISTS `bot_groups`;
DROP TABLE IF EXISTS `bot_group_members`;
DROP TABLE IF EXISTS `character_backup`;
DROP TABLE IF EXISTS `character_lookup`;
DROP TABLE IF EXISTS `class_skill`;
DROP TABLE IF EXISTS `client_faction_associations`;
DROP TABLE IF EXISTS `client_faction_names`;
DROP TABLE IF EXISTS `client_server_faction_map`;
DROP TABLE IF EXISTS `commands_old`;
DROP TABLE IF EXISTS `custom_faction_mappings`;
DROP TABLE IF EXISTS `faction_list_mod_prefix`;
DROP TABLE IF EXISTS `faction_list_prefix`;
DROP TABLE IF EXISTS `faction_values_prefix`;
DROP TABLE IF EXISTS `fear_hints`;
DROP TABLE IF EXISTS `goallists_backup_9_25_2022`;
DROP TABLE IF EXISTS `inventory_snapshots_v1_bak`;
DROP TABLE IF EXISTS `merc_inventory`;
DROP TABLE IF EXISTS `merc_merchant_entries`;
DROP TABLE IF EXISTS `merc_merchant_templates`;
DROP TABLE IF EXISTS `merc_merchant_template_entries`;
DROP TABLE IF EXISTS `merc_name_types`;
DROP TABLE IF EXISTS `merc_npc_types`;
DROP TABLE IF EXISTS `merc_spell_lists`;
DROP TABLE IF EXISTS `merc_spell_list_entries`;
DROP TABLE IF EXISTS `merc_stance_entries`;
DROP TABLE IF EXISTS `merc_stats`;
DROP TABLE IF EXISTS `merc_subtypes`;
DROP TABLE IF EXISTS `merc_templates`;
DROP TABLE IF EXISTS `merc_types`;
DROP TABLE IF EXISTS `merc_weaponinfo`;
DROP TABLE IF EXISTS `npc_faction_entries_prefix`;
DROP TABLE IF EXISTS `npc_faction_prefix`;
DROP TABLE IF EXISTS `npc_types_metadata`;
DROP TABLE IF EXISTS `p2002_items`;
DROP TABLE IF EXISTS `proximities_backup_9203`;
DROP TABLE IF EXISTS `races`;
DROP TABLE IF EXISTS `spawn2_backup_2023_10_29`;
DROP TABLE IF EXISTS `starting_items_backup_9243`;
DROP TABLE IF EXISTS `tasks_backup_9_25_2022`;
DROP TABLE IF EXISTS `task_activities_backup_9203`;
DROP TABLE IF EXISTS `tool_game_objects`;
DROP TABLE IF EXISTS `tool_gearup_armor_sets`;
DROP TABLE IF EXISTS `veq_zone`;
DROP VIEW IF EXISTS `vwmercnpctypes`;
DROP TABLE IF EXISTS `zoneserver_auth`;
DROP TABLE IF EXISTS `zone_server`;
DROP TABLE IF EXISTS `zone_state_dump`;

DROP VIEW IF EXISTS `vw_gearscore`;
DROP VIEW IF EXISTS `vw_itemdrops`;
DROP VIEW IF EXISTS `vw_itemdrops_bulk`;
DROP VIEW IF EXISTS `vw_itemdrops_diff`;
DROP VIEW IF EXISTS `vw_itemdrops_difftotal`;
DROP VIEW IF EXISTS `vw_itemdrops_min`;
DROP VIEW IF EXISTS `vw_random_item_drop_calculator`;
DROP VIEW IF EXISTS `vw_gearscore`;
DROP VIEW IF EXISTS `vw_gearscore`;
DROP VIEW IF EXISTS `vw_gearscore`;
DROP VIEW IF EXISTS `vw_gearscore`;

)",
		.content_schema_update = false,
	},
	ManifestEntry{
		.version = 2,
		.description = "2025_10_15_correct_raid_members_table",
		.check = "SHOW COLUMNS FROM `raid_members` LIKE 'id'",
		.condition = "empty",
		.match = "",
		.sql = R"(
DROP TABLE IF EXISTS `raid_members`;

CREATE TABLE `raid_members` (
	`id` BIGINT(20) UNSIGNED NOT NULL AUTO_INCREMENT,
	`raidid` INT(4) NOT NULL DEFAULT '0',
	`charid` INT(4) NOT NULL DEFAULT '0',
	`bot_id` INT(4) NOT NULL DEFAULT '0',
	`groupid` INT(4) UNSIGNED NOT NULL DEFAULT '0',
	`_class` TINYINT(4) NOT NULL DEFAULT '0',
	`level` TINYINT(4) NOT NULL DEFAULT '0',
	`name` VARCHAR(64) NOT NULL DEFAULT '' COLLATE 'latin1_swedish_ci',
	`isgroupleader` TINYINT(1) NOT NULL DEFAULT '0',
	`israidleader` TINYINT(1) NOT NULL DEFAULT '0',
	`islooter` TINYINT(1) NOT NULL DEFAULT '0',
	`is_marker` TINYINT(3) UNSIGNED NOT NULL DEFAULT '0',
	`is_assister` TINYINT(3) UNSIGNED NOT NULL DEFAULT '0',
	`note` VARCHAR(64) NOT NULL DEFAULT '' COLLATE 'latin1_swedish_ci',
	PRIMARY KEY (`id`) USING BTREE,
	UNIQUE INDEX `UNIQUE` (`name`) USING BTREE
)
COLLATE='latin1_swedish_ci'
ENGINE=InnoDB
AUTO_INCREMENT=11118
;
)",
		.content_schema_update = false,
	},
	ManifestEntry{
		.version = 3,
		.description = "2025_10_15_update_items_table_column_names",
		.check = "SHOW COLUMNS FROM `items` LIKE 'GearScoreOld'",
		.condition = "not_empty",
		.match = "",
		.sql = R"(
ALTER TABLE `items`
	CHANGE COLUMN `augdistiller` `augdistiller` INT(11) NOT NULL DEFAULT '0' AFTER `delay`,
	CHANGE COLUMN `updated` `updated` DATETIME NULL DEFAULT NULL AFTER `nopet`,
	CHANGE COLUMN `scrollunk1` `scrollunk1` INT(11) NOT NULL DEFAULT '0' AFTER `focusunk7`,
	CHANGE COLUMN `scriptfileid` `scriptfileid` SMALLINT NOT NULL DEFAULT 0 AFTER `ldonsellbackrate`,
	CHANGE COLUMN `powersourcecapacity` `powersourcecapacity` SMALLINT NOT NULL DEFAULT 0 AFTER `expendablearrow`,
	CHANGE COLUMN `bardeffect` `bardeffect` SMALLINT NOT NULL DEFAULT 0 AFTER `powersourcecapacity`,
	CHANGE COLUMN `defaultstacksize` `defaultstacksize` INT(11) NOT NULL AFTER `raid_only`,
	CHANGE COLUMN `defaultstackable` `defaultstackable` INT(11) NOT NULL AFTER `defaultstacksize`,
	CHANGE COLUMN `defaultprice` `defaultprice` INT(11) NOT NULL AFTER `defaultstackable`,
	CHANGE COLUMN `defaultsellrate` `defaultsellrate` INT(11) NOT NULL AFTER `defaultprice`,
	CHANGE COLUMN `defaultnodrop` `defaultnodrop` INT(11) NOT NULL AFTER `defaultsellrate`,
	CHANGE COLUMN `defaultquestitemflag` `defaultquestitemflag` INT(11) NOT NULL AFTER `defaultnodrop`,
	DROP COLUMN `GearScoreOld`,
	DROP COLUMN `GearScoreNoProcClick`;
)",
		.content_schema_update = false,
	},ManifestEntry{
		.version = 4,
		.description = "2025_10_15_migrate_bot_settings",
		.check = "SHOW COLUMNS FROM `bot_data` LIKE 'petsettype_setting'",
		.condition = "not_empty",
		.match = "",
		.sql = R"(
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 6, 0, `petsettype_setting`, 'BaseSetting', 'PetSetTypeSetting' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `petsettype_setting` != 255 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 7, 0, `behind_mob`, 'BaseSetting', 'BehindMob' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `behind_mob` != 0 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 9, 0, `illusion_block`, 'BaseSetting', 'IllusionBlock' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `illusion_block` != 0 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 33, 1, `hold_ae_nukes`, 'SpellHolds', 'AE Nuke' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hold_ae_nukes` != 1 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 34, 1, `hold_ae_rains`, 'SpellHolds', 'AE Rain' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hold_ae_rains` != 1 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 3, 1, `hold_buffs`, 'SpellHolds', 'Buff' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hold_buffs` != 0 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 12, 1, `hold_charms`, 'SpellHolds', 'Charm' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hold_charms` != 0 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 26, 1, `hold_complete_heals`, 'SpellHolds', 'Complete Heal' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hold_complete_heals` != 0 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 15, 1, `hold_cures`, 'SpellHolds', 'Cure' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hold_cures` != 0 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 9, 1, `hold_dispels`, 'SpellHolds', 'Dispel' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hold_dispels` != 0 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 14, 1, `hold_debuffs`, 'SpellHolds', 'Debuff' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hold_debuffs` != 0 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 8, 1, `hold_dots`, 'SpellHolds', 'DoT' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hold_dots` != 0 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 54, 1, `hold_ds`, 'SpellHolds', 'Damage Shield' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hold_ds` != 1 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 4, 1, `hold_escapes`, 'SpellHolds', 'Escape' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hold_escapes` != 0 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 27, 1, `hold_fast_heals`, 'SpellHolds', 'Fast Heal' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hold_fast_heals` != 0 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 29, 1, `hold_group_heals`, 'SpellHolds', 'Group Heal' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hold_group_heals` != 0 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 17, 1, `hold_hateredux`, 'SpellHolds', 'Hate Reduction' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hold_hateredux` != 0 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 32, 1, `hold_hot_heals`, 'SpellHolds', 'HoT Heal' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hold_hot_heals` != 0 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 10, 1, `hold_incombatbuffs`, 'SpellHolds', 'In-Combat Buff' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hold_incombatbuffs` != 0 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 18, 1, `hold_incombatbuffsongs`, 'SpellHolds', 'In-Combat Buff Song' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hold_incombatbuffsongs` != 0 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 6, 1, `hold_lifetaps`, 'SpellHolds', 'Lifetap' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hold_lifetaps` != 0 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 101, 1, `hold_lulls`, 'SpellHolds', 'Lull' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hold_lulls` != 0 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 11, 1, `hold_mez`, 'SpellHolds', 'Mez' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hold_mez` != 0 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 0, 1, `hold_nukes`, 'SpellHolds', 'Nuke' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hold_nukes` != 0 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 19, 1, `hold_outofcombatbuffsongs`, 'SpellHolds', 'Out-of-Combat Buff Song' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hold_outofcombatbuffsongs` != 0 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 47, 1, `hold_pet_buffs`, 'SpellHolds', 'Pet Buff' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hold_pet_buffs` != 0 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 5, 1, `hold_pets`, 'SpellHolds', 'Pet' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hold_pets` != 0 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 20, 1, `hold_precombatbuffs`, 'SpellHolds', 'Pre-Combat Buff' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hold_precombatbuffs` != 0 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 21, 1, `hold_precombatbuffsongs`, 'SpellHolds', 'Pre-Combat Buff Song' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hold_precombatbuffsongs` != 0 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 1, 1, `hold_regular_heals`, 'SpellHolds', 'Regular Heal' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hold_regular_heals` != 0 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 55, 1, `hold_resists`, 'SpellHolds', 'Resist Buff' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hold_resists` != 1 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 16, 1, `hold_rez`, 'SpellHolds', 'Resurrect' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hold_rez` != 0 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 2, 1, `hold_roots`, 'SpellHolds', 'Root' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hold_roots` != 1 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 13, 1, `hold_slows`, 'SpellHolds', 'Slow' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hold_slows` != 0 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 7, 1, `hold_snares`, 'SpellHolds', 'Snare' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hold_snares` != 0 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 12, 2, `buff_delay`, 'SpellDelays', 'Buff' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `buff_delay` != 1 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 26, 2, `complete_heal_delay`, 'SpellDelays', 'Complete Heal' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `complete_heal_delay` != 8000 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 15, 2, `cure_delay`, 'SpellDelays', 'Cure' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `cure_delay` != 1 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 14, 2, `debuff_delay`, 'SpellDelays', 'Debuff' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `debuff_delay` != 12000 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 9, 2, `dispel_delay`, 'SpellDelays', 'Dispel' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `dispel_delay` != 1 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 8, 2, `dot_delay`, 'SpellDelays', 'DoT' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `dot_delay` != 12000 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 4, 2, `escape_delay`, 'SpellDelays', 'Escape' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `escape_delay` != 1 GROUP BY bd.bot_id;

INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 27, 2, `fast_heal_delay`, 'SpellDelays', 'Fast Heal' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `fast_heal_delay` != 2500 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 17, 2, `hate_redux_delay`, 'SpellDelays', 'Hate Reduction' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hate_redux_delay` != 1 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 1, 2, `heal_delay`, 'SpellDelays', 'Regular Heal' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `heal_delay` != 4500 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 32, 2, `hot_heal_delay`, 'SpellDelays', 'HoT Heal' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hot_heal_delay` != 22000 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 10, 2, `incombatbuff_delay`, 'SpellDelays', 'In-Combat Buff' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `incombatbuff_delay` != 1 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 6, 2, `lifetap_delay`, 'SpellDelays', 'Lifetap' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `lifetap_delay` != 1 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 11, 2, `mez_delay`, 'SpellDelays', 'Mez' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `mez_delay` != 1 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 0, 2, `nuke_delay`, 'SpellDelays', 'Nuke' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `nuke_delay` != 8000 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 2, 2, `root_delay`, 'SpellDelays', 'Root' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `root_delay` != 12000 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 13, 2, `slow_delay`, 'SpellDelays', 'Slow' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `slow_delay` != 12000 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 7, 2, `snare_delay`, 'SpellDelays', 'Snare' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `snare_delay` != 12000 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 12, 4, `buff_threshold`, 'SpellMaxThresholds', 'Buff' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `buff_threshold` != 150 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 26, 4, `complete_heal_threshold`, 'SpellMaxThresholds', 'Complete Heal' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `complete_heal_threshold` != 70 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 15, 4, `cure_threshold`, 'SpellMaxThresholds', 'Cure' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `cure_threshold` != 150 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 14, 4, `debuff_threshold`, 'SpellMaxThresholds', 'Debuff' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `debuff_threshold` != 95 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 9, 4, `dispel_threshold`, 'SpellMaxThresholds', 'Dispel' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `dispel_threshold` != 99 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 8, 4, `dot_threshold`, 'SpellMaxThresholds', 'DoT' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `dot_threshold` != 95 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 4, 4, `escape_threshold`, 'SpellMaxThresholds', 'Escape' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `escape_threshold` != 35 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 27, 4, `fast_heal_threshold`, 'SpellMaxThresholds', 'Fast Heal' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `fast_heal_threshold` != 35 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 17, 4, `hate_redux_threshold`, 'SpellMaxThresholds', 'Hate Reduction' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hate_redux_threshold` != 80 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 1, 4, `heal_threshold`, 'SpellMaxThresholds', 'Regular Heal' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `heal_threshold` != 55 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 32, 4, `hot_heal_threshold`, 'SpellMaxThresholds', 'HoT Heal' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hot_heal_threshold` != 85 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 10, 4, `incombatbuff_threshold`, 'SpellMaxThresholds', 'In-Combat Buff' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `incombatbuff_threshold` != 150 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 6, 4, `lifetap_threshold`, 'SpellMaxThresholds', 'Lifetap' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `lifetap_threshold` != 60 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 11, 4, `mez_threshold`, 'SpellMaxThresholds', 'Mez' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `mez_threshold` != 150 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 0, 4, `nuke_threshold`, 'SpellMaxThresholds', 'Nuke' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `nuke_threshold` != 95 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 2, 4, `root_threshold`, 'SpellMaxThresholds', 'Root' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `root_threshold` != 95 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 13, 4, `slow_threshold`, 'SpellMaxThresholds', 'Slow' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `slow_threshold` != 95 GROUP BY bd.bot_id;
)",
		.content_schema_update = false,
	},
	ManifestEntry{
		.version = 5,
		.description = "2025_10_15_migrate_bot_settings_part_two",
		.check = "SHOW COLUMNS FROM `bot_data` LIKE 'snare_threshold'",
		.condition = "not_empty",
		.match = "",
		.sql = R"(
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 7, 4, `snare_threshold`, 'SpellMaxThresholds', 'Snare' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `snare_threshold` != 95 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 12, 3, `buff_min_threshold`, 'SpellMinThresholds', 'Buff' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `buff_min_threshold` != 0 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 15, 3, `cure_min_threshold`, 'SpellMinThresholds', 'Cure' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `cure_min_threshold` != 0 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 14, 3, `debuff_min_threshold`, 'SpellMinThresholds', 'Debuff' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `debuff_min_threshold` != 25 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 9, 3, `dispel_min_threshold`, 'SpellMinThresholds', 'Dispel' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `dispel_min_threshold` != 25 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 8, 3, `dot_min_threshold`, 'SpellMinThresholds', 'DoT' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `dot_min_threshold` != 50 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 4, 3, `escape_min_threshold`, 'SpellMinThresholds', 'Escape' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `escape_min_threshold` != 0 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 17, 3, `hate_redux_min_threshold`, 'SpellMinThresholds', 'Hate Reduction' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `hate_redux_min_threshold` != 0 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 10, 3, `incombatbuff_min_threshold`, 'SpellMinThresholds', 'In-Combat Buff' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `incombatbuff_min_threshold` != 0 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 6, 3, `lifetap_min_threshold`, 'SpellMinThresholds', 'Lifetap' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `lifetap_min_threshold` != 0 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 11, 3, `mez_min_threshold`, 'SpellMinThresholds', 'Mez' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `mez_min_threshold` != 85 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 0, 3, `nuke_min_threshold`, 'SpellMinThresholds', 'Nuke' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `nuke_min_threshold` != 20 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 2, 3, `root_min_threshold`, 'SpellMinThresholds', 'Root' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `root_min_threshold` != 15 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 13, 3, `slow_min_threshold`, 'SpellMinThresholds', 'Slow' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `slow_min_threshold` != 25 GROUP BY bd.bot_id;
INSERT INTO bot_settings SELECT 0, bd.bot_id, bs.stance_id, 7, 3, `snare_min_threshold`, 'SpellMinThresholds', 'Snare' FROM bot_data bd INNER JOIN bot_stances bs ON bd.bot_id = bs.bot_id WHERE `snare_min_threshold` != 0 GROUP BY bd.bot_id;
ALTER TABLE `bot_data`
	DROP COLUMN `petsettype_setting`,
	DROP COLUMN `illusion_block`,
	DROP COLUMN `behind_mob`,
	DROP COLUMN `hold_ae_nukes`,
	DROP COLUMN `hold_ae_rains`,
	DROP COLUMN `hold_buffs`,
	DROP COLUMN `hold_charms`,
	DROP COLUMN `hold_complete_heals`,
	DROP COLUMN `hold_cures`,
	DROP COLUMN `hold_dispels`,
	DROP COLUMN `hold_debuffs`,
	DROP COLUMN `hold_dots`,
	DROP COLUMN `hold_ds`,
	DROP COLUMN `hold_escapes`,
	DROP COLUMN `hold_fast_heals`,
	DROP COLUMN `hold_group_heals`,
	DROP COLUMN `hold_hateredux`,
	DROP COLUMN `hold_heals`,
	DROP COLUMN `hold_hot_heals`,
	DROP COLUMN `hold_incombatbuffs`,
	DROP COLUMN `hold_incombatbuffsongs`,
	DROP COLUMN `hold_lifetaps`,
	DROP COLUMN `hold_lulls`,
	DROP COLUMN `hold_mez`,
	DROP COLUMN `hold_nukes`,
	DROP COLUMN `hold_outofcombatbuffsongs`,
	DROP COLUMN `hold_pet_buffs`,
	DROP COLUMN `hold_pet_heals`,
	DROP COLUMN `hold_pets`,
	DROP COLUMN `hold_precombatbuffs`,
	DROP COLUMN `hold_precombatbuffsongs`,
	DROP COLUMN `hold_regular_heals`,
	DROP COLUMN `hold_resists`,
	DROP COLUMN `hold_rez`,
	DROP COLUMN `hold_roots`,
	DROP COLUMN `hold_slows`,
	DROP COLUMN `hold_snares`,
	DROP COLUMN `buff_delay`,
	DROP COLUMN `complete_heal_delay`,
	DROP COLUMN `cure_delay`,
	DROP COLUMN `debuff_delay`,
	DROP COLUMN `dispel_delay`,
	DROP COLUMN `dot_delay`,
	DROP COLUMN `escape_delay`,
	DROP COLUMN `fast_heal_delay`,
	DROP COLUMN `hate_redux_delay`,
	DROP COLUMN `heal_delay`,
	DROP COLUMN `hot_heal_delay`,
	DROP COLUMN `incombatbuff_delay`,
	DROP COLUMN `lifetap_delay`,
	DROP COLUMN `mez_delay`,
	DROP COLUMN `nuke_delay`,
	DROP COLUMN `root_delay`,
	DROP COLUMN `slow_delay`,
	DROP COLUMN `snare_delay`,
	DROP COLUMN `buff_threshold`,
	DROP COLUMN `complete_heal_threshold`,
	DROP COLUMN `cure_threshold`,
	DROP COLUMN `debuff_threshold`,
	DROP COLUMN `dispel_threshold`,
	DROP COLUMN `dot_threshold`,
	DROP COLUMN `escape_threshold`,
	DROP COLUMN `fast_heal_threshold`,
	DROP COLUMN `hate_redux_threshold`,
	DROP COLUMN `heal_threshold`,
	DROP COLUMN `hot_heal_threshold`,
	DROP COLUMN `incombatbuff_threshold`,
	DROP COLUMN `lifetap_threshold`,
	DROP COLUMN `mez_threshold`,
	DROP COLUMN `nuke_threshold`,
	DROP COLUMN `root_threshold`,
	DROP COLUMN `slow_threshold`,
	DROP COLUMN `snare_threshold`,
	DROP COLUMN `buff_min_threshold`,
	DROP COLUMN `cure_min_threshold`,
	DROP COLUMN `debuff_min_threshold`,
	DROP COLUMN `dispel_min_threshold`,
	DROP COLUMN `dot_min_threshold`,
	DROP COLUMN `escape_min_threshold`,
	DROP COLUMN `hate_redux_min_threshold`,
	DROP COLUMN `incombatbuff_min_threshold`,
	DROP COLUMN `lifetap_min_threshold`,
	DROP COLUMN `mez_min_threshold`,
	DROP COLUMN `nuke_min_threshold`,
	DROP COLUMN `root_min_threshold`,
	DROP COLUMN `slow_min_threshold`,
	DROP COLUMN `snare_min_threshold`;

ALTER TABLE `character_data`
	DROP COLUMN `fast_heal_threshold`,
	DROP COLUMN `heal_threshold`,
	DROP COLUMN `complete_heal_threshold`,
	DROP COLUMN `hot_heal_threshold`,
	DROP COLUMN `fast_heal_delay`,
	DROP COLUMN `heal_delay`,
	DROP COLUMN `complete_heal_delay`,
	DROP COLUMN `hot_heal_delay`,
	DROP COLUMN `cure_delay`,
	DROP COLUMN `cure_min_threshold`,
	DROP COLUMN `cure_threshold`;
)",
		.content_schema_update = false,
	},
	ManifestEntry{
		.version = 6,
		.description = "2025_10_15_correct_bot_spells",
		.check = "SELECT * FROM `bot_spells_entries` WHERE `type` >= 1024",
		.condition = "not_empty",
		.match = "",
		.sql = R"(
UPDATE `bot_spells_entries` SET `type` = 0 WHERE `type` = 1;
UPDATE `bot_spells_entries` SET `type` = 1 WHERE `type` = 2;
UPDATE `bot_spells_entries` SET `type` = 2 WHERE `type` = 4;
UPDATE `bot_spells_entries` SET `type` = 3 WHERE `type` = 8;
UPDATE `bot_spells_entries` SET `type` = 4 WHERE `type` = 16;
UPDATE `bot_spells_entries` SET `type` = 5 WHERE `type` = 32;
UPDATE `bot_spells_entries` SET `type` = 6 WHERE `type` = 64;
UPDATE `bot_spells_entries` SET `type` = 7 WHERE `type` = 128;
UPDATE `bot_spells_entries` SET `type` = 8 WHERE `type` = 256;
UPDATE `bot_spells_entries` SET `type` = 9 WHERE `type` = 512;
UPDATE `bot_spells_entries` SET `type` = 10 WHERE `type` = 1024;
UPDATE `bot_spells_entries` SET `type` = 11 WHERE `type` = 2048;
UPDATE `bot_spells_entries` SET `type` = 12 WHERE `type` = 4096;
UPDATE `bot_spells_entries` SET `type` = 13 WHERE `type` = 8192;
UPDATE `bot_spells_entries` SET `type` = 14 WHERE `type` = 16384;
UPDATE `bot_spells_entries` SET `type` = 15 WHERE `type` = 32768;
UPDATE `bot_spells_entries` SET `type` = 16 WHERE `type` = 65536;
UPDATE `bot_spells_entries` SET `type` = 17 WHERE `type` = 131072;
UPDATE `bot_spells_entries` SET `type` = 18 WHERE `type` = 262144;
UPDATE `bot_spells_entries` SET `type` = 19 WHERE `type` = 524288;
UPDATE `bot_spells_entries` SET `type` = 20 WHERE `type` = 1048576;
UPDATE `bot_spells_entries` SET `type` = 21 WHERE `type` = 2097152;

UPDATE bot_spells_entries b, spells_new s -- Move to Fear
SET b.`type` = 22
WHERE b.spell_id = s.id
AND (
	s.`effectid1` = 23 OR
	s.`effectid2` = 23 OR
	s.`effectid3` = 23 OR
	s.`effectid4` = 23 OR
	s.`effectid5` = 23 OR
	s.`effectid6` = 23 OR
	s.`effectid7` = 23 OR
	s.`effectid8` = 23 OR
	s.`effectid9` = 23 OR
	s.`effectid10` = 23 OR
	s.`effectid11` = 23 OR
	s.`effectid12` = 23
);

UPDATE bot_spells_entries b SET b.type = 24 WHERE b.type = 10 AND b.npc_spells_id = 3005; -- Move SK ICB to Hateline

UPDATE bot_spells_entries b SET b.type = 101 WHERE b.type = 8388608; -- Lull
UPDATE bot_spells_entries b SET b.type = 105 WHERE b.type = 16777216; -- Lev

UPDATE bot_spells_entries b SET b.spell_id = 2521 WHERE b.spell_id = 25205; -- Talisman of the Beast
UPDATE bot_spells_entries b SET b.spell_id = 3391 WHERE b.spell_id = 25201; -- Talisman of Alacrity
UPDATE bot_spells_entries b SET b.spell_id = 3651 WHERE b.spell_id = 25203; -- Wind of Marr
UPDATE bot_spells_entries b SET b.spell_id = 3362 WHERE b.spell_id = 25204; -- Rizlona's Call of Flame
)",
		.content_schema_update = false,
	},
	ManifestEntry{
		.version = 7,
		.description = "2025_10_15_update_bot_data_columns",
		.check = "SHOW TABLES LIKE 'bot_data'",
		.condition = "not_empty",
		.match = "",
		.sql = R"(
ALTER TABLE `bot_data`
CHANGE COLUMN `name` `name` VARCHAR(64) NOT NULL DEFAULT '' COLLATE 'latin1_swedish_ci' AFTER `spells_id`,
CHANGE COLUMN `last_name` `last_name` VARCHAR(64) NOT NULL DEFAULT '' COLLATE 'latin1_swedish_ci' AFTER `name`,
CHANGE COLUMN `title` `title` VARCHAR(32) NOT NULL DEFAULT '' COLLATE 'latin1_swedish_ci' AFTER `last_name`,
CHANGE COLUMN `suffix` `suffix` VARCHAR(32) NOT NULL DEFAULT '' COLLATE 'latin1_spanish_ci' AFTER `title`,
CHANGE COLUMN `beard` `beard` INT(10) NOT NULL DEFAULT 0 AFTER `hair_style`,
COLLATE='latin1_swedish_ci';
)",
		.content_schema_update = false,
	},
	ManifestEntry{
		.version = 8,
		.description = "2025_10_15_update_npc_types_columns",
		.check = "SHOW COLUMNS FROM `npc_types` LIKE 'shard_score'",
		.condition = "not_empty",
		.match = "",
		.sql = R"(
ALTER TABLE `npc_types`
	CHANGE COLUMN `d_melee_texture1` `d_melee_texture1` INT(11) UNSIGNED NOT NULL DEFAULT '0' AFTER `armortint_blue`,
	CHANGE COLUMN `d_melee_texture2` `d_melee_texture2` INT(11) UNSIGNED NOT NULL DEFAULT '0' AFTER `d_melee_texture1`,
	ADD COLUMN `skip_vegas_loot` TINYINT(1) UNSIGNED NOT NULL DEFAULT '0' AFTER `in_zone_id`,
	ADD COLUMN `pass_vegas_loot` TINYINT(1) UNSIGNED NOT NULL DEFAULT '0' AFTER `skip_vegas_loot`,
	ADD COLUMN `raid_not_count_as_raid` TINYINT(1) UNSIGNED NOT NULL DEFAULT '0' AFTER `pass_vegas_loot`,
	ADD COLUMN `non_raid_can_raid_roll` TINYINT(1) UNSIGNED NOT NULL DEFAULT '0' AFTER `raid_not_count_as_raid`,
	DROP COLUMN `shard_score`;
)",
		.content_schema_update = false,
	},
	ManifestEntry{
		.version = 9,
		.description = "2025_10_15_convert_bot_quivers",
		.check = "SELECT * FROM `data_buckets` WHERE `data_buckets`.key LIKE 'BotQuiver-%'",
		.condition = "not_empty",
		.match = "",
		.sql = R"(
INSERT INTO `bot_inventories` (
    `bot_id`,
    `slot_id`,
    `item_id`,
    `inst_charges`,
    `inst_color`,
    `inst_no_drop`,
    `inst_custom_data`,
    `ornament_icon`,
    `ornament_id_file`,
    `ornament_hero_model`,
    `augment_1`,
    `augment_2`,
    `augment_3`,
    `augment_4`,
    `augment_5`,
    `augment_6`
)
SELECT
    CAST(SUBSTRING(`key`, 11) AS UNSIGNED),
    23,  -- Assuming slot_id 23 for quiver slot (adjust if needed based on your schema)
    CAST(`value` AS UNSIGNED),
    0,
    0,
    0,
    '',
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
FROM `data_buckets`
WHERE `data_buckets`.key LIKE 'BotQuiver-%';

DELETE FROM `data_buckets` WHERE `data_buckets`.key LIKE 'BotQuiver-%';
)",
		.content_schema_update = false,
	},
	ManifestEntry{
		.version = 10,
		.description = "2025_10_15_bot_force_cast_spell_types",
		.check = "SELECT * FROM `bot_spells_entries` WHERE `type` = 100", // VegasEQ
		.condition = "empty",
		.match = "",
		.sql = R"(
-- Teleports
INSERT INTO `bot_spells_entries` (`npc_spells_id`, `spell_id`, `type`, `minlevel`, `maxlevel`, `spell_name`) VALUES (3006, 552, 100, 29, 254, 'Circle of Toxxulia'),
	(3006, 2432, 100, 29, 254, 'Circle of the Nexus'),
	(3006, 550, 100, 29, 254, 'Circle of Karana'),
	(3006, 553, 100, 29, 254, 'Circle of Butcher'),
	(3006, 2020, 100, 29, 254, 'Circle of Surefall Glade'),
	(3006, 551, 100, 29, 254, 'Circle of Commons'),
	(3006, 2419, 100, 29, 254, 'Circle of Grimling'),
	(3006, 1434, 100, 34, 254, 'Circle of Iceclad'),
	(3006, 555, 100, 34, 254, 'Circle of Ro'),
	(3006, 1517, 100, 34, 254, 'Circle of the Combines'),
	(3006, 2424, 100, 34, 254, 'Circle of Twilight'),
	(3006, 556, 100, 34, 254, 'Circle of Feerrott'),
	(3006, 554, 100, 34, 254, 'Circle of Lavastorm'),
	(3006, 557, 100, 34, 254, 'Circle of Steamfont'),
	(3006, 558, 100, 39, 254, 'Circle of Misty'),
	(3006, 1438, 100, 39, 254, 'Circle of Great Divide'),
	(3006, 3184, 100, 39, 254, 'Circle of Knowledge'),
	(3006, 2429, 100, 39, 254, 'Circle of Dawnshroud'),
	(3006, 1398, 100, 44, 254, 'Circle of Wakening Lands'),
	(3006, 1440, 100, 44, 254, 'Circle of Cobalt Scar'),
	(3012, 562, 100, 29, 254, 'North Portal'),
	(3012, 2420, 100, 29, 254, 'Grimling Portal'),
	(3012, 563, 100, 29, 254, 'Fay Portal'),
	(3012, 561, 100, 29, 254, 'Tox Portal'),
	(3012, 2944, 100, 29, 254, 'Nexus Portal'),
	(3012, 2425, 100, 34, 254, 'Twilight Portal'),
	(3012, 565, 100, 34, 254, 'Cazic Portal'),
	(3012, 1418, 100, 34, 254, 'Iceclad Portal'),
	(3012, 1516, 100, 34, 254, 'Combine Portal'),
	(3012, 564, 100, 34, 254, 'Nek Portal'),
	(3012, 567, 100, 39, 254, 'Ro Portal'),
	(3012, 1338, 100, 39, 254, 'Translocate: North'),
	(3012, 3180, 100, 39, 254, 'Knowledge Portal'),
	(3012, 568, 100, 39, 254, 'West Portal'),
	(3012, 1336, 100, 39, 254, 'Translocate: Fay'),
	(3012, 2943, 100, 39, 254, 'Translocate: Nexus'),
	(3012, 566, 100, 39, 254, 'Common Portal'),
	(3012, 1339, 100, 39, 254, 'Translocate: Combine'),
	(3012, 1423, 100, 39, 254, 'Great Divide Portal'),
	(3012, 2421, 100, 39, 254, 'Translocate: Grimling'),
	(3012, 2430, 100, 39, 254, 'Dawnshroud Portal'),
	(3012, 1337, 100, 39, 254, 'Translocate: Tox'),
	(3012, 1371, 100, 44, 254, 'Translocate: Nek'),
	(3012, 1425, 100, 44, 254, 'Cobalt Scar Portal'),
	(3012, 1374, 100, 44, 254, 'Translocate: West'),
	(3012, 1372, 100, 44, 254, 'Translocate: Common'),
	(3012, 2426, 100, 44, 254, 'Translocate: Twilight'),
	(3012, 1375, 100, 44, 254, 'Translocate: Cazic'),
	(3012, 1373, 100, 44, 254, 'Translocate: Ro'),
	(3012, 1399, 100, 44, 254, 'Wakening Lands Portal'),
	(3012, 3849, 100, 46, 254, 'Alter Plane: Hate II'),
	(3012, 674, 100, 46, 254, 'Alter Plane: Sky'),
	(3012, 666, 100, 46, 254, 'Alter Plane: Hate'),
	(3012, 2023, 100, 49, 254, 'Translocate: Great Divide'),
	(3012, 2431, 100, 49, 254, 'Translocate: Dawnshroud'),
	(3012, 2024, 100, 49, 254, 'Translocate: Wakening Lands'),
	(3012, 3181, 100, 49, 254, 'Translocate: Knowledge'),
	(3012, 2022, 100, 49, 254, 'Translocate: Iceclad'),
	(3012, 2025, 100, 49, 254, 'Translocate: Cobalt Scar'),

-- Lulls
	(3002, 208, 101, 1, 8, 'Lull'),
	(3002, 501, 101, 9, 18, 'Soothe'),
	(3002, 47, 101, 19, 38, 'Calm'),
	(3002, 45, 101, 39, 64, 'Pacify'),
	(3002, 3197, 101, 65, 254, 'Pacification'),
	(3002, 5274, 101, 70, 254, 'Placate'),
	(3003, 208, 101, 15, 29, 'Lull'),
	(3003, 501, 101, 30, 48, 'Soothe'),
	(3003, 47, 101, 49, 50, 'Calm'),
	(3003, 45, 101, 51, 254, 'Pacify'),
	(3004, 240, 101, 9, 38, 'Lull Animal'),
	(3004, 513, 101, 39, 254, 'Calm Animal'),
	(3004, 3601, 101, 39, 254, 'Harmony of Nature'),
	(3004, 5316, 101, 68, 254, 'Tranquility of the Glade'),
	(3005, 347, 101, 15, 51, 'Numb the Dead'),
	(3005, 448, 101, 52, 254, 'Rest the Dead'),
	(3006, 240, 101, 1, 18, 'Lull Animal'),
	(3006, 513, 101, 19, 254, 'Calm Animal'),
	(3006, 3601, 101, 29, 254, 'Harmony of Nature'),
	(3006, 5347, 101, 67, 254, 'Nature\'s Serenity'),
	(3008, 728, 101, 8, 60, 'Kelin`s Lugubrious Lament'),
	(3008, 3361, 101, 61, 254, 'Silent Song of Quellious'),
	(3008, 5370, 101, 66, 254, 'Luvwen\'s Aria of Serenity'),
	(3011, 347, 101, 4, 23, 'Numb the Dead'),
	(3011, 448, 101, 24, 254, 'Rest the Dead'),
	(3014, 208, 101, 1, 7, 'Lull'),
	(3014, 501, 101, 8, 19, 'Soothe'),
	(3014, 47, 101, 20, 38, 'Calm'),
	(3014, 45, 101, 39, 61, 'Pacify'),
	(3014, 3197, 101, 62, 254, 'Pacification'),
	(3014, 5506, 101, 67, 254, 'Placate'),

-- Succors
	(3006, 2183, 102, 18, 56, 'Lesser Succor'),
	(3006, 1567, 102, 57, 254, 'Succor'),
	(3012, 2184, 102, 18, 56, 'Lesser Evacuate'),
	(3012, 2558, 102, 56, 64, 'Decession'),
	(3012, 1628, 102, 57, 254, 'Evacuate'),
	(3012, 3244, 102, 65, 254, 'Greater Decession'),

-- Bind Affinities
	(3002, 35, 103, 14, 254, 'Bind Affinity'),
	(3006, 35, 103, 14, 254, 'Bind Affinity'),
	(3010, 35, 103, 14, 254, 'Bind Affinity'),
	(3011, 35, 103, 12, 254, 'Bind Affinity'),
	(3012, 35, 103, 12, 254, 'Bind Affinity'),
	(3013, 35, 103, 12, 254, 'Bind Affinity'),
	(3014, 35, 103, 12, 254, 'Bind Affinity'),

-- Identifies
	(3008, 737, 104, 14, 254, 'Lyssa`s Cataloging Libretto'),
	(3011, 305, 104, 20, 254, 'Identify'),
	(3012, 305, 104, 16, 254, 'Identify'),
	(3013, 305, 104, 16, 254, 'Identify'),
	(3014, 305, 104, 16, 254, 'Identify'),

-- Levitates
	(3004, 261, 105, 39, 64, 'Levitate'),
	(3004, 2517, 105, 65, 254, 'Spirit of Eagle'),
	(3006, 261, 105, 14, 50, 'Levitate'),
	(3006, 2894, 105, 51, 53, 'Levitation'),
	(3006, 2517, 105, 54, 254, 'Spirit of Eagle'),
	(3006, 3185, 105, 62, 254, 'Flight of Eagles'),
	(3008, 718, 105, 31, 50, 'Agilmente`s Aria of Eagles'),
	(3008, 1750, 105, 51, 254, 'Selo`s Song of Travel'),
	(3010, 261, 105, 14, 50, 'Levitate'),
	(3010, 2894, 105, 51, 254, 'Levitation'),
	(3011, 457, 105, 44, 254, 'Dead Man Floating'),
	(3011, 1391, 105, 49, 254, 'Dead Men Floating'),
	(3012, 261, 105, 24, 50, 'Levitate'),
	(3012, 2894, 105, 51, 254, 'Levitation'),
	(3014, 261, 105, 16, 50, 'Levitate'),
	(3014, 2894, 105, 51, 254, 'Levitation'),
	(3015, 261, 105, 39, 254, 'Levitate'),

-- Runes
	(3003, 1743, 106, 55, 254, 'Divine Favor'),
	(3008, 714, 106, 41, 254, 'Psalm of Mystic Shielding'),
	(3008, 748, 106, 47, 57, 'Niv`s Melody of Preservation'),
	(3008, 1450, 106, 49, 254, 'Shield of Songs'),
	(3008, 1752, 106, 52, 254, 'Nillipus` March of the Wee'),
	(3008, 1763, 106, 58, 254, 'Niv`s Harmonic'),
	(3012, 2559, 106, 58, 254, 'Spellshield'),
	(3014, 481, 106, 16, 23, 'Rune I'),
	(3014, 21, 106, 20, 23, 'Berserker Strength'),
	(3014, 482, 106, 24, 33, 'Rune II'),
	(3014, 483, 106, 34, 43, 'Rune III'),
	(3014, 648, 106, 39, 43, 'Rampage'),
	(3014, 484, 106, 44, 57, 'Rune IV'),
	(3014, 176, 106, 49, 57, 'Berserker Spirit'),
	(3014, 1689, 106, 52, 57, 'Rune V'),
	(3014, 1713, 106, 58, 60, 'Bedlam'),
	(3014, 3343, 106, 61, 66, 'Rune of Zebuxoruk'),
	(3014, 3351, 106, 63, 66, 'Uproar'),
	(3014, 5504, 106, 67, 254, 'Rune of Salik'),
	(3014, 5514, 106, 69, 254, 'Mayhem'),

-- Water Breathings
	(3004, 86, 107, 22, 254, 'Enduring Breath'),
	(3006, 86, 107, 9, 50, 'Enduring Breath'),
	(3006, 2881, 107, 51, 254, 'Everlasting Breath'),
	(3008, 729, 107, 16, 254, 'Tarew`s Aquatic Ayre'),
	(3010, 86, 107, 14, 50, 'Enduring Breath'),
	(3010, 2881, 107, 51, 254, 'Everlasting Breath'),
	(3011, 457, 107, 44, 254, 'Dead Man Floating'),
	(3011, 1391, 107, 49, 254, 'Dead Men Floating'),
	(3014, 86, 107, 12, 50, 'Enduring Breath'),
	(3014, 2881, 107, 51, 254, 'Everlasting Breath'),
	(3015, 86, 107, 29, 254, 'Enduring Breath'),

-- Size
	(3010, 345, 108, 19, 254, 'Shrink'),
	(3010, 2522, 108, 19, 254, 'Grow'),
	(3015, 345, 108, 30, 254, 'Shrink'),

-- Invis Lines
	(3002, 235, 109, 14, 254, 'Invisibility versus Undead'),
	(3002, 1726, 109, 51, 254, 'Sunskin'),
	(3003, 235, 109, 22, 254, 'Invisibility versus Undead'),
	(3004, 247, 109, 15, 48, 'Camouflage'),
	(3004, 80, 109, 39, 64, 'See Invisible'),
	(3004, 34, 109, 49, 254, 'Superior Camouflage'),
	(3004, 2517, 109, 65, 254, 'Spirit of Eagle'),
	(3005, 235, 109, 9, 254, 'Invisibility versus Undead'),
	(3006, 247, 109, 5, 18, 'Camouflage'),
	(3006, 255, 109, 9, 254, 'Invisibility versus Animals'),
	(3006, 80, 109, 14, 53, 'See Invisible'),
	(3006, 34, 109, 19, 254, 'Superior Camouflage'),
	(3006, 2516, 109, 52, 254, 'Foliage Shield'),
	(3006, 2517, 109, 54, 254, 'Spirit of Eagle'),
	(3006, 3185, 109, 62, 254, 'Flight of Eagles'),
	(3008, 719, 109, 19, 50, 'Shauri`s Sonorous Clouding'),
	(3008, 735, 109, 24, 254, 'Lyssa`s Veracious Concord'),
	(3008, 1750, 109, 51, 254, 'Selo`s Song of Travel'),
	(3010, 79, 109, 9, 55, 'Spirit Sight'),
	(3010, 255, 109, 14, 254, 'Invisibility versus Animals'),
	(3010, 42, 109, 29, 254, 'Invisibility'),
	(3010, 1575, 109, 56, 254, 'Acumen'),
	(3010, 2886, 109, 58, 254, 'Acumen of Dar Khura'),
	(3011, 235, 109, 1, 254, 'Invisibility versus Undead'),
	(3011, 457, 109, 44, 254, 'Dead Man Floating'),
	(3011, 1391, 109, 49, 254, 'Dead Men Floating'),
	(3012, 80, 109, 4, 254, 'See Invisible'),
	(3012, 42, 109, 16, 254, 'Invisibility'),
	(3013, 42, 109, 8, 254, 'Invisibility'),
	(3013, 80, 109, 16, 254, 'See Invisible'),
	(3014, 42, 109, 4, 254, 'Invisibility'),
	(3014, 80, 109, 8, 254, 'See Invisible'),
	(3014, 235, 109, 16, 254, 'Invisibility versus Undead'),
	(3015, 79, 109, 30, 64, 'Spirit Sight'),
	(3015, 42, 109, 49, 254, 'Invisibility'),
	(3015, 1575, 109, 65, 254, 'Acumen'),

-- Movement Speed
	(3004, 278, 110, 30, 64, 'Spirit of Wolf'),
	(3004, 2517, 110, 65, 254, 'Spirit of Eagle'),
	(3006, 278, 110, 14, 53, 'Spirit of Wolf'),
	(3006, 424, 110, 29, 254, 'Scale of Wolf'),
	(3006, 169, 110, 39, 61, 'Pack Spirit'),
	(3006, 3579, 110, 49, 254, 'Share Form of the Great Wolf'),
	(3006, 1554, 110, 53, 254, 'Spirit of Scale'),
	(3006, 2517, 110, 54, 254, 'Spirit of Eagle'),
	(3006, 3185, 110, 62, 254, 'Flight of Eagles'),
	(3008, 717, 110, 5, 48, 'Selo`s Accelerando'),
	(3008, 2605, 110, 49, 50, 'Selo`s Accelerating Chorus'),
	(3008, 1750, 110, 51, 254, 'Selo`s Song of Travel'),
	(3010, 278, 110, 9, 254, 'Spirit of Wolf'),
	(3010, 424, 110, 24, 254, 'Scale of Wolf'),
	(3010, 2524, 110, 39, 254, 'Spirit of Bih`Li'),
	(3010, 1554, 110, 52, 254, 'Spirit of Scale'),
	(3015, 278, 110, 30, 254, 'Spirit of Wolf'),

-- Teleport Bind
	(3012, 1422, 111, 50, 254, 'Translocate'),
	(3012, 1334, 111, 52, 254, 'Translocate: Group'),

-- Summon Corpse
	(3005, 2213, 112, 25, 50, 'Lesser Summon Corpse'),
	(3005, 3, 112, 51, 64, 'Summon Corpse'),
	(3005, 1773, 112, 65, 254, 'Conjure Corpse'),
	(3011, 2213, 112, 12, 38, 'Lesser Summon Corpse'),
	(3011, 3, 112, 39, 56, 'Summon Corpse'),
	(3011, 1773, 112, 57, 254, 'Conjure Corpse'),

-- AE Lulls
	(3002, 1541, 113, 55, 254, 'Wake of Tranquility'),
	(3004, 250, 113, 22, 254, 'Harmony'),
	(3006, 250, 113, 5, 254, 'Harmony');
)",
		.content_schema_update = false,
	},
	ManifestEntry{
		.version = 11,
		.description = "2025_10_15_bot_discipline_spell_types",
		.check = "SELECT * FROM `bot_spells_entries` WHERE `type` = 200", // VegasEQ
		.condition = "empty",
		.match = "",
		.sql = R"(
-- Disciplines
INSERT INTO `bot_spells_entries` (`npc_spells_id`, `spell_id`, `type`, `minlevel`, `maxlevel`, `spell_name`) VALUES (3001, 4585, 200, 30, 254, 'Resistant Discipline'),
	(3001, 4587, 200, 40, 254, 'Fearless Discipline'),
	(3001, 4503, 200, 52, 254, 'Evasive Discipline'),
	(3001, 4672, 200, 53, 56, 'Charge Discipline'),
	(3001, 4514, 200, 54, 254, 'Mighty Strike Discipline'),
	(3001, 4499, 200, 55, 254, 'Defensive Discipline'),
	(3001, 4674, 200, 56, 254, 'Furious Discipline'),
	(3001, 4501, 200, 57, 254, 'Precision Discipline'),
	(3001, 4675, 200, 58, 254, 'Fellstrike Discipline'),
	(3001, 4670, 200, 59, 254, 'Fortitude Discipline'),
	(3001, 4498, 200, 60, 254, 'Aggressive Discipline'),
	(3003, 4585, 200, 51, 254, 'Resistant Discipline'),
	(3003, 4587, 200, 54, 254, 'Fearless Discipline'),
	(3003, 4500, 200, 55, 254, 'Holyforge Discipline'),
	(3003, 4518, 200, 60, 254, 'Sanctification Discipline'),
	(3004, 4585, 200, 51, 254, 'Resistant Discipline'),
	(3004, 4587, 200, 54, 254, 'Fearless Discipline'),
	(3004, 4506, 200, 55, 254, 'Trueshot Discipline'),
	(3004, 4519, 200, 60, 254, 'Weapon Shield Discipline'),
	(3005, 4585, 200, 51, 254, 'Resistant Discipline'),
	(3005, 4587, 200, 54, 254, 'Fearless Discipline'),
	(3005, 4520, 200, 55, 254, 'Unholy Aura Discipline'),
	(3005, 4504, 200, 60, 254, 'Leechcurse Discipline'),
	(3007, 4585, 200, 30, 254, 'Resistant Discipline'),
	(3007, 4587, 200, 40, 254, 'Fearless Discipline'),
	(3007, 4510, 200, 51, 254, 'Stonestance Discipline'),
	(3007, 4511, 200, 52, 254, 'Thunderkick Discipline'),
	(3007, 4509, 200, 53, 254, 'Whirlwind Discipline'),
	(3007, 4502, 200, 54, 254, 'Voiddance Discipline'),
	(3007, 4512, 200, 56, 254, 'Innerflame Discipline'),
	(3007, 4513, 200, 57, 254, 'Hundred Fists Discipline'),
	(3007, 4507, 200, 59, 59, 'Silentfist Discipline'),
	(3007, 4508, 200, 60, 254, 'Ashenhand Discipline'),
	(3008, 4585, 200, 51, 254, 'Resistant Discipline'),
	(3008, 4587, 200, 54, 254, 'Fearless Discipline'),
	(3008, 4516, 200, 55, 254, 'Deftdance Discipline'),
	(3008, 4586, 200, 60, 254, 'Puretone Discipline'),
	(3009, 4585, 200, 30, 254, 'Resistant Discipline'),
	(3009, 4587, 200, 40, 254, 'Fearless Discipline'),
	(3009, 4673, 200, 53, 254, 'Counterattack Discipline'),
	(3009, 4505, 200, 54, 254, 'Deadeye Discipline'),
	(3009, 4515, 200, 55, 254, 'Nimble Discipline'),
	(3009, 4517, 200, 57, 254, 'Kinesthetics Discipline'),
	(3009, 4677, 200, 58, 254, 'Blinding Speed Discipline'),
	(3009, 4676, 200, 59, 254, 'Duelist Discipline'),
	(3015, 4585, 200, 51, 254, 'Resistant Discipline'),
	(3015, 4587, 200, 54, 254, 'Fearless Discipline'),
	(3015, 4671, 200, 55, 254, 'Protective Spirit Discipline'),
	(3015, 4678, 200, 60, 254, 'Bestial Fury Discipline'),
	(3016, 4585, 200, 30, 254, 'Resistant Discipline'),
	(3016, 4587, 200, 40, 254, 'Fearless Discipline'),

-- Disciplines - Aggressive
	(3001, 4672, 201, 53, 56, 'Charge Discipline'),
	(3001, 4514, 201, 54, 254, 'Mighty Strike Discipline'),
	(3001, 4501, 201, 57, 254, 'Precision Discipline'),
	(3001, 4675, 201, 58, 254, 'Fellstrike Discipline'),
	(3001, 4498, 201, 60, 254, 'Aggressive Discipline'),
	(3003, 4500, 201, 55, 254, 'Holyforge Discipline'),
	(3004, 4506, 201, 55, 254, 'Trueshot Discipline'),
	(3005, 4520, 201, 55, 254, 'Unholy Aura Discipline'),
	(3007, 4511, 201, 52, 254, 'Thunderkick Discipline'),
	(3007, 4512, 201, 56, 254, 'Innerflame Discipline'),
	(3007, 4513, 201, 57, 254, 'Hundred Fists Discipline'),
	(3007, 4507, 201, 59, 59, 'Silentfist Discipline'),
	(3007, 4508, 201, 60, 254, 'Ashenhand Discipline'),
	(3008, 4586, 201, 60, 254, 'Puretone Discipline'),
	(3009, 4505, 201, 54, 254, 'Deadeye Discipline'),
	(3009, 4517, 201, 57, 254, 'Kinesthetics Discipline'),
	(3009, 4677, 201, 58, 254, 'Blinding Speed Discipline'),
	(3009, 4676, 201, 59, 254, 'Duelist Discipline'),
	(3015, 4678, 201, 60, 254, 'Bestial Fury Discipline'),

-- Disciplines - Defensive
	(3001, 4585, 202, 30, 254, 'Resistant Discipline'),
	(3001, 4587, 202, 40, 254, 'Fearless Discipline'),
	(3001, 4503, 202, 52, 254, 'Evasive Discipline'),
	(3001, 4499, 202, 55, 254, 'Defensive Discipline'),
	(3001, 4674, 202, 56, 254, 'Furious Discipline'),
	(3001, 4670, 202, 59, 254, 'Fortitude Discipline'),
	(3003, 4585, 202, 51, 254, 'Resistant Discipline'),
	(3003, 4587, 202, 54, 254, 'Fearless Discipline'),
	(3003, 4518, 202, 60, 254, 'Sanctification Discipline'),
	(3004, 4585, 202, 51, 254, 'Resistant Discipline'),
	(3004, 4587, 202, 54, 254, 'Fearless Discipline'),
	(3004, 4519, 202, 60, 254, 'Weapon Shield Discipline'),
	(3005, 4585, 202, 51, 254, 'Resistant Discipline'),
	(3005, 4587, 202, 54, 254, 'Fearless Discipline'),
	(3005, 4504, 202, 60, 254, 'Leechcurse Discipline'),
	(3007, 4585, 202, 30, 254, 'Resistant Discipline'),
	(3007, 4587, 202, 40, 254, 'Fearless Discipline'),
	(3007, 4510, 202, 51, 254, 'Stonestance Discipline'),
	(3007, 4509, 202, 53, 254, 'Whirlwind Discipline'),
	(3007, 4502, 202, 54, 254, 'Voiddance Discipline'),
	(3008, 4585, 202, 51, 254, 'Resistant Discipline'),
	(3008, 4587, 202, 54, 254, 'Fearless Discipline'),
	(3008, 4516, 202, 55, 254, 'Deftdance Discipline'),
	(3009, 4585, 202, 30, 254, 'Resistant Discipline'),
	(3009, 4587, 202, 40, 254, 'Fearless Discipline'),
	(3009, 4673, 202, 53, 254, 'Counterattack Discipline'),
	(3009, 4515, 202, 55, 254, 'Nimble Discipline'),
	(3015, 4585, 202, 51, 254, 'Resistant Discipline'),
	(3015, 4587, 202, 54, 254, 'Fearless Discipline'),
	(3015, 4671, 202, 55, 254, 'Protective Spirit Discipline'),
	(3016, 4585, 202, 30, 254, 'Resistant Discipline'),
	(3016, 4587, 202, 40, 254, 'Fearless Discipline');
)",
		.content_schema_update = false,
	},
	ManifestEntry{
		.version = 12,
		.description = "2025_10_15_create_bot_default_settings",
		.check = "SHOW TABLES LIKE 'bot_default_settings'",
		.condition = "empty",
		.match = "",
		.sql = R"(
DROP TABLE IF EXISTS `bot_default_settings`;

CREATE TABLE `bot_default_settings` (
	`class_id` INT(10) UNSIGNED NOT NULL DEFAULT '0',
	`stance` INT(10) UNSIGNED NOT NULL DEFAULT '0',
	`setting_category` INT(10) UNSIGNED NOT NULL DEFAULT '0',
	`setting_id` INT(10) UNSIGNED NOT NULL DEFAULT '0',
	`value` INT(10) UNSIGNED NOT NULL DEFAULT '0',
	`category_name` TEXT NULL DEFAULT NULL COLLATE 'latin1_swedish_ci',
	`setting_name` TEXT NULL DEFAULT NULL COLLATE 'latin1_swedish_ci',
	PRIMARY KEY (`class_id`, `stance`, `setting_category`, `setting_id`, `value`) USING BTREE
)
COLLATE='latin1_swedish_ci'
ENGINE=InnoDB
;

)",
		.content_schema_update = false,
	},
	ManifestEntry{
		.version = 13,
		.description = "2025_10_15_create_gearscore_and_spellscore_tables",
		.check = "SHOW TABLES LIKE 'veq_gear_score'",
		.condition = "empty",
		.match = "",
		.sql = R"(
DROP TABLE IF EXISTS `veq_gear_score`;

CREATE TABLE `veq_gear_score` (
	`id` MEDIUMINT(8) UNSIGNED NOT NULL DEFAULT '0',
	`name` TEXT NULL DEFAULT NULL COLLATE 'latin1_swedish_ci',
	`common_drop` TINYINT(1) UNSIGNED NOT NULL DEFAULT '0',
	`rare_drop` TINYINT(1) UNSIGNED NOT NULL DEFAULT '0',
	`raid_drop` TINYINT(1) UNSIGNED NOT NULL DEFAULT '0',
	`avg` FLOAT UNSIGNED NOT NULL DEFAULT '0',
	`high` FLOAT NOT NULL DEFAULT '0',
	`low` FLOAT NOT NULL DEFAULT '0',
	`classes1` FLOAT NOT NULL DEFAULT '0',
	`classes2` FLOAT NOT NULL DEFAULT '0',
	`classes3` FLOAT NOT NULL DEFAULT '0',
	`classes4` FLOAT NOT NULL DEFAULT '0',
	`classes5` FLOAT NOT NULL DEFAULT '0',
	`classes6` FLOAT NOT NULL DEFAULT '0',
	`classes7` FLOAT NOT NULL DEFAULT '0',
	`classes8` FLOAT NOT NULL DEFAULT '0',
	`classes9` FLOAT NOT NULL DEFAULT '0',
	`classes10` FLOAT NOT NULL DEFAULT '0',
	`classes11` FLOAT NOT NULL DEFAULT '0',
	`classes12` FLOAT NOT NULL DEFAULT '0',
	`classes13` FLOAT NOT NULL DEFAULT '0',
	`classes14` FLOAT NOT NULL DEFAULT '0',
	`classes15` FLOAT NOT NULL DEFAULT '0',
	`classes16` FLOAT NOT NULL DEFAULT '0',
	`damage_score` FLOAT NOT NULL DEFAULT '0',
	`delay_score` FLOAT NOT NULL DEFAULT '0',
	`ratio_score` FLOAT NOT NULL DEFAULT '0',
	`weapon_score` FLOAT NOT NULL DEFAULT '0',
	`arrow_score` FLOAT NOT NULL DEFAULT '0',
	`ac_score` FLOAT NOT NULL DEFAULT '0',
	`hp_score` FLOAT NOT NULL DEFAULT '0',
	`mana_score` FLOAT NOT NULL DEFAULT '0',
	`end_score` FLOAT NOT NULL DEFAULT '0',
	`hp_regen_score` FLOAT NOT NULL DEFAULT '0',
	`mana_regen_score` FLOAT NOT NULL DEFAULT '0',
	`end_regen_score` FLOAT NOT NULL DEFAULT '0',
	`haste_score` FLOAT NOT NULL DEFAULT '0',
	`agi_score` FLOAT NOT NULL DEFAULT '0',
	`cha_score` FLOAT NOT NULL DEFAULT '0',
	`dex_score` FLOAT NOT NULL DEFAULT '0',
	`int_score` FLOAT NOT NULL DEFAULT '0',
	`sta_score` FLOAT NOT NULL DEFAULT '0',
	`str_score` FLOAT NOT NULL DEFAULT '0',
	`wis_score` FLOAT NOT NULL DEFAULT '0',
	`cr_score` FLOAT NOT NULL DEFAULT '0',
	`dr_score` FLOAT NOT NULL DEFAULT '0',
	`fr_score` FLOAT NOT NULL DEFAULT '0',
	`mr_score` FLOAT NOT NULL DEFAULT '0',
	`pr_score` FLOAT NOT NULL DEFAULT '0',
	`corruption_score` FLOAT NOT NULL DEFAULT '0',
	`h_agi_score` FLOAT NOT NULL DEFAULT '0',
	`h_cha_score` FLOAT NOT NULL DEFAULT '0',
	`h_dex_score` FLOAT NOT NULL DEFAULT '0',
	`h_int_score` FLOAT NOT NULL DEFAULT '0',
	`h_sta_score` FLOAT NOT NULL DEFAULT '0',
	`h_str_score` FLOAT NOT NULL DEFAULT '0',
	`h_wis_score` FLOAT NOT NULL DEFAULT '0',
	`h_cr_score` FLOAT NOT NULL DEFAULT '0',
	`h_dr_score` FLOAT NOT NULL DEFAULT '0',
	`h_fr_score` FLOAT NOT NULL DEFAULT '0',
	`h_mr_score` FLOAT NOT NULL DEFAULT '0',
	`h_pr_score` FLOAT NOT NULL DEFAULT '0',
	`h_corruption_score` FLOAT NOT NULL DEFAULT '0',
	`accuracy_score` FLOAT NOT NULL DEFAULT '0',
	`attack_score` FLOAT NOT NULL DEFAULT '0',
	`avoidance_score` FLOAT NOT NULL DEFAULT '0',
	`combat_effects_score` FLOAT NOT NULL DEFAULT '0',
	`damage_shield_score` FLOAT NOT NULL DEFAULT '0',
	`dot_shielding_score` FLOAT NOT NULL DEFAULT '0',
	`shielding_score` FLOAT NOT NULL DEFAULT '0',
	`spellshield_score` FLOAT NOT NULL DEFAULT '0',
	`strikethrough_score` FLOAT NOT NULL DEFAULT '0',
	`stunresist_score` FLOAT NOT NULL DEFAULT '0',
	`ds_mitigation_score` FLOAT NOT NULL DEFAULT '0',
	`heal_amount_score` FLOAT NOT NULL DEFAULT '0',
	`spell_damage_score` FLOAT NOT NULL DEFAULT '0',
	`clairvoyance_score` FLOAT NOT NULL DEFAULT '0',
	`backstab_damage_score` FLOAT NOT NULL DEFAULT '0',
	`skillmod_score` FLOAT NOT NULL DEFAULT '0',
	`extra_skill_score` FLOAT NOT NULL DEFAULT '0',
	`bane_dmg_score` FLOAT NOT NULL DEFAULT '0',
	`elemental_dmg_score` FLOAT NOT NULL DEFAULT '0',
	`bard_type_score` FLOAT NOT NULL DEFAULT '0',
	`proc_effect_score` FLOAT NOT NULL DEFAULT '0',
	`worn_effect_score` FLOAT NOT NULL DEFAULT '0',
	`focus_effect_score` FLOAT NOT NULL DEFAULT '0',
	`click_effect_score` FLOAT NOT NULL DEFAULT '0',
	UNIQUE INDEX `id` (`id`) USING BTREE,
	INDEX `idx_id` (`id`) USING BTREE,
	INDEX `idx_name` (`name`(3072)) USING BTREE,
	INDEX `idx_veq_gear_score_id_class1` (`id`, `classes1`) USING BTREE,
	INDEX `idx_veq_gear_score_id_class2` (`id`, `classes2`) USING BTREE,
	INDEX `idx_veq_gear_score_id_class3` (`id`, `classes3`) USING BTREE,
	INDEX `idx_veq_gear_score_id_class4` (`id`, `classes4`) USING BTREE,
	INDEX `idx_veq_gear_score_id_class5` (`id`, `classes5`) USING BTREE,
	INDEX `idx_veq_gear_score_id_class6` (`id`, `classes6`) USING BTREE,
	INDEX `idx_veq_gear_score_id_class7` (`id`, `classes7`) USING BTREE,
	INDEX `idx_veq_gear_score_id_class8` (`id`, `classes8`) USING BTREE,
	INDEX `idx_veq_gear_score_id_class9` (`id`, `classes9`) USING BTREE,
	INDEX `idx_veq_gear_score_id_class10` (`id`, `classes10`) USING BTREE,
	INDEX `idx_veq_gear_score_id_class11` (`id`, `classes11`) USING BTREE,
	INDEX `idx_veq_gear_score_id_class12` (`id`, `classes12`) USING BTREE,
	INDEX `idx_veq_gear_score_id_class13` (`id`, `classes13`) USING BTREE,
	INDEX `idx_veq_gear_score_id_class14` (`id`, `classes14`) USING BTREE,
	INDEX `idx_veq_gear_score_id_class15` (`id`, `classes15`) USING BTREE,
	INDEX `idx_veq_gear_score_id_class16` (`id`, `classes16`) USING BTREE
)
COLLATE='latin1_swedish_ci'
ENGINE=InnoDB
;

DROP TABLE IF EXISTS `veq_spell_score`;

CREATE TABLE `veq_spell_score` (
	`id` SMALLINT(5) UNSIGNED NOT NULL DEFAULT '0',
	`name` TEXT NULL DEFAULT NULL COLLATE 'latin1_swedish_ci',
	`lowest_spell_level` TINYINT(3) UNSIGNED NOT NULL DEFAULT '0',
	`lowest_class` TINYINT(3) UNSIGNED NOT NULL DEFAULT '0',
	`spell_score` FLOAT NOT NULL DEFAULT '0',
	`spell_type` TEXT NULL DEFAULT 'Unknown' COLLATE 'latin1_swedish_ci',
	`duration` FLOAT NOT NULL DEFAULT '0',
	`target_type` TINYINT(3) UNSIGNED NOT NULL DEFAULT '0',
	`pbae_multiplier` FLOAT NOT NULL DEFAULT '0',
	`ae_multiplier` FLOAT NOT NULL DEFAULT '0',
	`group_multiplier` FLOAT NOT NULL DEFAULT '0',
	`recourse_score` FLOAT NOT NULL DEFAULT '0',
	`heal_score` FLOAT NOT NULL DEFAULT '0',
	`mana_score` FLOAT NOT NULL DEFAULT '0',
	`stamina_score` FLOAT NOT NULL DEFAULT '0',
	`dd_score` FLOAT NOT NULL DEFAULT '0',
	`ae_rain_score` FLOAT NOT NULL DEFAULT '0',
	`hot_score` FLOAT NOT NULL DEFAULT '0',
	`mana_dd_score` FLOAT NOT NULL DEFAULT '0',
	`stamina_dd_score` FLOAT NOT NULL DEFAULT '0',
	`dot_score` FLOAT NOT NULL DEFAULT '0',
	`mot_score` FLOAT NOT NULL DEFAULT '0',
	`sot_score` FLOAT NOT NULL DEFAULT '0',
	`m_dot_score` FLOAT NOT NULL DEFAULT '0',
	`s_dot_score` FLOAT NOT NULL DEFAULT '0',
	`haste_score` FLOAT NOT NULL DEFAULT '0',
	`slow_score` FLOAT NOT NULL DEFAULT '0',
	`lifetap_score` FLOAT NOT NULL DEFAULT '0',
	`manatap_score` FLOAT NOT NULL DEFAULT '0',
	`staminatap_score` FLOAT NOT NULL DEFAULT '0',
	`ac_buff_score` FLOAT NOT NULL DEFAULT '0',
	`ac_debuff_score` FLOAT NOT NULL DEFAULT '0',
	`atk_buff_score` FLOAT NOT NULL DEFAULT '0',
	`atk_debuff_score` FLOAT NOT NULL DEFAULT '0',
	`movement_speed_score` FLOAT NOT NULL DEFAULT '0',
	`snare_score` FLOAT NOT NULL DEFAULT '0',
	`base_stat_score` FLOAT NOT NULL DEFAULT '0',
	`base_stat_debuff_score` FLOAT NOT NULL DEFAULT '0',
	`invis_score` FLOAT NOT NULL DEFAULT '0',
	`see_invis_score` FLOAT NOT NULL DEFAULT '0',
	`water_breathing_score` FLOAT NOT NULL DEFAULT '0',
	`pacify_score` FLOAT NOT NULL DEFAULT '0',
	`add_faction_score` FLOAT NOT NULL DEFAULT '0',
	`blind_score` FLOAT NOT NULL DEFAULT '0',
	`stun_score` FLOAT NOT NULL DEFAULT '0',
	`charm_score` FLOAT NOT NULL DEFAULT '0',
	`fear_score` FLOAT NOT NULL DEFAULT '0',
	`bind_affinity_score` FLOAT NOT NULL DEFAULT '0',
	`gate_score` FLOAT NOT NULL DEFAULT '0',
	`cancel_magic_score` FLOAT NOT NULL DEFAULT '0',
	`invis_vs_undead_score` FLOAT NOT NULL DEFAULT '0',
	`invis_vs_animals_score` FLOAT NOT NULL DEFAULT '0',
	`mez_score` FLOAT NOT NULL DEFAULT '0',
	`summon_item_score` FLOAT NOT NULL DEFAULT '0',
	`summon_pet_score` FLOAT NOT NULL DEFAULT '0',
	`disease_counter_score` FLOAT NOT NULL DEFAULT '0',
	`poison_counter_score` FLOAT NOT NULL DEFAULT '0',
	UNIQUE INDEX `id` (`id`) USING BTREE
)
COLLATE='latin1_swedish_ci'
ENGINE=InnoDB
;

)",
		.content_schema_update = false,
	},
	ManifestEntry{
		.version = 14,
		.description = "2025_10_19_update_logsys_catergories",
		.check = "SHOW TABLES LIKE 'logsys_categories'",
		.condition = "not_empty",
		.match = "",
		.sql = R"(
UPDATE logsys_categories SET log_to_console = 0, log_to_file = 0, log_to_gmsay = 0, log_to_discord = 0, discord_webhook_id = 0;  -- Disable All
UPDATE logsys_categories SET log_to_console = 1, log_to_file = 1, log_to_gmsay = 1, log_to_discord = 1, discord_webhook_id = 3 WHERE log_category_id = 8;  -- Crash
UPDATE logsys_categories SET log_to_console = 1, log_to_file = 1, log_to_gmsay = 0, log_to_discord = 0, discord_webhook_id = 0 WHERE log_category_id = 9;  -- Debug
UPDATE logsys_categories SET log_to_console = 1, log_to_file = 1, log_to_gmsay = 1, log_to_discord = 1, discord_webhook_id = 2 WHERE log_category_id = 11;  -- Error
UPDATE logsys_categories SET log_to_console = 0, log_to_file = 1, log_to_gmsay = 1, log_to_discord = 0, discord_webhook_id = 5 WHERE log_category_id = 20;  -- Quests
UPDATE logsys_categories SET log_to_console = 1, log_to_file = 1, log_to_gmsay = 1, log_to_discord = 1, discord_webhook_id = 12 WHERE log_category_id = 35;  -- MySQL Error
UPDATE logsys_categories SET log_to_console = 0, log_to_file = 1, log_to_gmsay = 0, log_to_discord = 0, discord_webhook_id = 5 WHERE log_category_id = 38;  -- Quest Debug
UPDATE logsys_categories SET log_to_console = 1, log_to_file = 1, log_to_gmsay = 0, log_to_discord = 0, discord_webhook_id = 0 WHERE log_category_id = 44;  -- Client Login
UPDATE logsys_categories SET log_to_console = 1, log_to_file = 1, log_to_gmsay = 0, log_to_discord = 0, discord_webhook_id = 0 WHERE log_category_id = 53;  -- Info
UPDATE logsys_categories SET log_to_console = 1, log_to_file = 1, log_to_gmsay = 1, log_to_discord = 1, discord_webhook_id = 2 WHERE log_category_id = 54;  -- Warning
UPDATE logsys_categories SET log_to_console = 1, log_to_file = 1, log_to_gmsay = 1, log_to_discord = 1, discord_webhook_id = 11 WHERE log_category_id = 66;  -- HotReload
UPDATE logsys_categories SET log_to_console = 0, log_to_file = 1, log_to_gmsay = 1, log_to_discord = 0, discord_webhook_id = 0 WHERE log_category_id = 70;  -- Expeditions
UPDATE logsys_categories SET log_to_console = 0, log_to_file = 1, log_to_gmsay = 1, log_to_discord = 0, discord_webhook_id = 0 WHERE log_category_id = 71;  -- DynamicZones
UPDATE logsys_categories SET log_to_console = 1, log_to_file = 1, log_to_gmsay = 1, log_to_discord = 1, discord_webhook_id = 4 WHERE log_category_id = 73;  -- Cheat
UPDATE logsys_categories SET log_to_console = 1, log_to_file = 1, log_to_gmsay = 1, log_to_discord = 0, discord_webhook_id = 0 WHERE log_category_id = 76;  -- HTTP
UPDATE logsys_categories SET log_to_console = 1, log_to_file = 3, log_to_gmsay = 0, log_to_discord = 0, discord_webhook_id = 0 WHERE log_category_id = 81;  -- Discord
UPDATE logsys_categories SET log_to_console = 1, log_to_file = 1, log_to_gmsay = 1, log_to_discord = 1, discord_webhook_id = 5 WHERE log_category_id = 87;  -- QuestErrors
UPDATE logsys_categories SET log_to_console = 0, log_to_file = 0, log_to_gmsay = 0, log_to_discord = 3, discord_webhook_id = 24 WHERE log_category_id = 88;  -- PlayerEvents
UPDATE logsys_categories SET log_to_console = 0, log_to_file = 1, log_to_gmsay = 1, log_to_discord = 0, discord_webhook_id = 0 WHERE log_category_id = 104;  -- TestDebug
UPDATE logsys_categories SET log_to_console = 0, log_to_file = 1, log_to_gmsay = 1, log_to_discord = 0, discord_webhook_id = 0 WHERE log_category_id = 107;  -- VegasLoot
)",
		.content_schema_update = false,
	},
	ManifestEntry{
		.version = 15,
		.description = "2026_1_21_add_valeen_merchant_tier",
		.check = "SHOW COLUMNS FROM `npc_types` LIKE 'valeen_merchant_tier'",
		.condition = "empty",
		.match = "",
		.sql = R"(
ALTER TABLE `npc_types`
	ADD COLUMN `valeen_merchant_tier` TINYINT(3) UNSIGNED NOT NULL DEFAULT '0' AFTER `is_valeen_spawned`;
)",
		.content_schema_update = false,
	},
	ManifestEntry{
		.version = 16,
		.description = "2026_3_15_add_ssf_flags",
		.check = "SHOW COLUMNS FROM `character_data` LIKE 'ssf_flag'",
		.condition = "empty",
		.match = "",
		.sql = R"(
ALTER TABLE `character_data`
	ADD COLUMN `ssf_flag` TINYINT(3) UNSIGNED NOT NULL DEFAULT '0' AFTER `illusion_block`;
ALTER TABLE `character_data`
	ADD COLUMN `hardcore_death_time` DATETIME NULL DEFAULT NULL AFTER `ssf_flag`;
ALTER TABLE `character_corpses`
	ADD COLUMN `ssf_flag` TINYINT(3) UNSIGNED NOT NULL DEFAULT '0' AFTER `entity_variables`;
)",
		.content_schema_update = false,
	},
	ManifestEntry{
		.version = 17,
		.description = "2026_3_15_add_ssf_discovered_items",
		.check = "SHOW TABLES LIKE `ssf_discovered_items`",
		.condition = "empty",
		.match = "",
		.sql = R"(
DROP TABLE IF EXISTS `ssf_discovered_items`;

CREATE TABLE `ssf_discovered_items` (
	`item_id` INT(11) UNSIGNED NOT NULL DEFAULT '0',
	`char_name` VARCHAR(64) NOT NULL DEFAULT '' COLLATE 'latin1_swedish_ci',
	`discovered_date` INT(11) UNSIGNED NOT NULL DEFAULT '0',
	`account_status` INT(11) NOT NULL DEFAULT '0',
	`ssf_flag` TINYINT(3) NOT NULL DEFAULT '0',
	PRIMARY KEY (`item_id`) USING BTREE
)
COLLATE='latin1_swedish_ci'
ENGINE=InnoDB
;
)",
		.content_schema_update = false,
	},
	ManifestEntry{
		.version = 18,
		.description = "2026_1_21_add_merchantlist_price",
		.check = "SHOW COLUMNS FROM `merchantlist` LIKE 'price'",
		.condition = "empty",
		.match = "",
		.sql = R"(
ALTER TABLE `merchantlist`
	ADD COLUMN `price` INT(11) NOT NULL DEFAULT -1 AFTER `max_status`;
)",
		.content_schema_update = false,
	},
	ManifestEntry{
		.version = 19,
		.description = "2026_5_5_add_min_max_level_bot_spell_settings",
		.check = "SHOW COLUMNS FROM `bot_spell_settings` LIKE 'min_level'",
		.condition = "empty",
		.match = "",
		.sql = R"(
ALTER TABLE `bot_spell_settings`
	ADD COLUMN `min_level` SMALLINT(5) NOT NULL DEFAULT '0' AFTER `priority`,
	ADD COLUMN `max_level` SMALLINT(5) NOT NULL DEFAULT '0' AFTER `min_level`;

UPDATE bot_spell_settings b, bot_spells_entries bs, bot_data bd
SET b.min_level = bs.minlevel, b.max_level = bs.maxlevel
WHERE b.bot_id = bd.bot_id
AND bd.spells_id = bs.npc_spells_id
AND b.spell_id = bs.spell_id;
)",
		.content_schema_update = false,
	},
// Used for testing
//	ManifestEntry{
//		.version = 9229,
//		.description = "new_database_check_test",
//		.check = "SHOW TABLES LIKE 'new_table'",
//		.condition = "empty",
//		.match = "",
//		.sql = R"(
//CREATE TABLE `new_table`  (
//  `id` int NOT NULL AUTO_INCREMENT,
//  PRIMARY KEY (`id`)
//);
//CREATE TABLE `new_table1`  (
//  `id` int NOT NULL AUTO_INCREMENT,
//  PRIMARY KEY (`id`)
//);
//CREATE TABLE `new_table2`  (
//  `id` int NOT NULL AUTO_INCREMENT,
//  PRIMARY KEY (`id`)
//);
//CREATE TABLE `new_table3`  (
//  `id` int NOT NULL AUTO_INCREMENT,
//  PRIMARY KEY (`id`)
//);
//)",
//	}

};

// see struct definitions for what each field does
// struct ManifestEntry {
// 	int         version{};     // database version of the migration
// 	std::string description{}; // description of the migration ex: "add_new_table" or "add_index_to_table"
// 	std::string check{};       // query that checks against the condition
// 	std::string condition{};   // condition or "match_type" - Possible values [contains|match|missing|empty|not_empty]
// 	std::string match{};       // match field that is not always used, but works in conjunction with "condition" values [missing|match|contains]
// 	std::string sql{};         // the SQL DDL that gets ran when the condition is true
// };
