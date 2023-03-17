UPDATE `npc_scale_global_base` SET ac = 0 WHERE ac IS NULL;
UPDATE `npc_scale_global_base` SET hp = 0 WHERE hp IS NULL;
UPDATE `npc_scale_global_base` SET accuracy = 0 WHERE accuracy IS NULL;
UPDATE `npc_scale_global_base` SET slow_mitigation = 0 WHERE slow_mitigation IS NULL;
UPDATE `npc_scale_global_base` SET attack = 0 WHERE attack IS NULL;
UPDATE `npc_scale_global_base` SET strength = 0 WHERE strength IS NULL;
UPDATE `npc_scale_global_base` SET stamina = 0 WHERE stamina IS NULL;
UPDATE `npc_scale_global_base` SET dexterity = 0 WHERE dexterity IS NULL;
UPDATE `npc_scale_global_base` SET agility = 0 WHERE agility IS NULL;
UPDATE `npc_scale_global_base` SET intelligence = 0 WHERE intelligence IS NULL;
UPDATE `npc_scale_global_base` SET wisdom = 0 WHERE wisdom IS NULL;
UPDATE `npc_scale_global_base` SET charisma = 0 WHERE charisma IS NULL;
UPDATE `npc_scale_global_base` SET magic_resist = 0 WHERE magic_resist IS NULL;
UPDATE `npc_scale_global_base` SET cold_resist = 0 WHERE cold_resist IS NULL;
UPDATE `npc_scale_global_base` SET fire_resist = 0 WHERE fire_resist IS NULL;
UPDATE `npc_scale_global_base` SET poison_resist = 0 WHERE poison_resist IS NULL;
UPDATE `npc_scale_global_base` SET disease_resist = 0 WHERE disease_resist IS NULL;
UPDATE `npc_scale_global_base` SET corruption_resist = 0 WHERE corruption_resist IS NULL;
UPDATE `npc_scale_global_base` SET physical_resist = 0 WHERE physical_resist IS NULL;
UPDATE `npc_scale_global_base` SET min_dmg = 0 WHERE min_dmg IS NULL;
UPDATE `npc_scale_global_base` SET max_dmg = 0 WHERE max_dmg IS NULL;
UPDATE `npc_scale_global_base` SET hp_regen_rate = 0 WHERE hp_regen_rate IS NULL;
UPDATE `npc_scale_global_base` SET attack_delay = 0 WHERE attack_delay IS NULL;
UPDATE `npc_scale_global_base` SET physical_resist = 0 WHERE physical_resist IS NULL;
UPDATE `npc_scale_global_base` SET spell_scale = 100 WHERE spell_scale IS NULL;
UPDATE `npc_scale_global_base` SET heal_scale = 100 WHERE heal_scale IS NULL;
UPDATE `npc_scale_global_base` SET special_abilities = '' WHERE special_abilities IS NULL;
ALTER TABLE `npc_scale_global_base`
    MODIFY COLUMN `ac` int(11) NOT NULL DEFAULT 0 AFTER `instance_version_list`,
    MODIFY COLUMN `hp` int(11) NOT NULL DEFAULT 0 AFTER `ac`,
    MODIFY COLUMN `accuracy` int(11) NOT NULL DEFAULT 0 AFTER `hp`,
    MODIFY COLUMN `slow_mitigation` int(11) NOT NULL DEFAULT 0 AFTER `accuracy`,
    MODIFY COLUMN `attack` int(11) NOT NULL DEFAULT 0 AFTER `slow_mitigation`,
    MODIFY COLUMN `strength` int(11) NOT NULL DEFAULT 0 AFTER `attack`,
    MODIFY COLUMN `stamina` int(11) NOT NULL DEFAULT 0 AFTER `strength`,
    MODIFY COLUMN `dexterity` int(11) NOT NULL DEFAULT 0 AFTER `stamina`,
    MODIFY COLUMN `agility` int(11) NOT NULL DEFAULT 0 AFTER `dexterity`,
    MODIFY COLUMN `intelligence` int(11) NOT NULL DEFAULT 0 AFTER `agility`,
    MODIFY COLUMN `wisdom` int(11) NOT NULL DEFAULT 0 AFTER `intelligence`,
    MODIFY COLUMN `charisma` int(11) NOT NULL DEFAULT 0 AFTER `wisdom`,
    MODIFY COLUMN `magic_resist` int(11) NOT NULL DEFAULT 0 AFTER `charisma`,
    MODIFY COLUMN `cold_resist` int(11) NOT NULL DEFAULT 0 AFTER `magic_resist`,
    MODIFY COLUMN `fire_resist` int(11) NOT NULL DEFAULT 0 AFTER `cold_resist`,
    MODIFY COLUMN `poison_resist` int(11) NOT NULL DEFAULT 0 AFTER `fire_resist`,
    MODIFY COLUMN `disease_resist` int(11) NOT NULL DEFAULT 0 AFTER `poison_resist`,
    MODIFY COLUMN `corruption_resist` int(11) NOT NULL DEFAULT 0 AFTER `disease_resist`,
    MODIFY COLUMN `physical_resist` int(11) NOT NULL DEFAULT 0 AFTER `corruption_resist`,
    MODIFY COLUMN `min_dmg` int(11) NOT NULL DEFAULT 0 AFTER `physical_resist`,
    MODIFY COLUMN `max_dmg` int(11) NOT NULL DEFAULT 0 AFTER `min_dmg`,
    MODIFY COLUMN `hp_regen_rate` int(11) NOT NULL DEFAULT 0 AFTER `max_dmg`,
    MODIFY COLUMN `attack_delay` int(11) NOT NULL DEFAULT 0 AFTER `hp_regen_rate`,
    MODIFY COLUMN `spell_scale` int(11) NOT NULL DEFAULT 100 AFTER `attack_delay`,
    MODIFY COLUMN `heal_scale` int(11) NOT NULL DEFAULT 100 AFTER `spell_scale`,
    MODIFY COLUMN `special_abilities` text CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT '' AFTER `heal_scale`,
    ADD COLUMN `heroic_strikethrough` int(11) NOT NULL DEFAULT 0 AFTER `heal_scale`;