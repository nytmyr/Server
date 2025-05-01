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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  04111-1307  USA
*/

#ifndef COMMON_VEQ_ITEM_DATA_H
#define COMMON_VEQ_ITEM_DATA_H

#include "item_data.h"
#include "repositories/veq_gear_score_repository.h"

void SendGearScoreTypePrompts(Client* c);
bool IsValidGearScoreType(uint16 type_id);
uint16 GetGearScoreTypeByShortname(std::string short_name);
std::string GetGearScoreTypeByID(uint16 type_id);

BaseVeqGearScoreRepository::VeqGearScore UpdateGearScores(const EQ::ItemData* item, bool bulk_update = false);
float GetGearScoreByType(uint16 search_type, const EQ::ItemData* item, uint32 ID, uint8 class_id);
float GetGearScoreMin(uint32 ID);
float GetGearScoreMax(uint32 ID);
float GetGearScoreAverage(uint32 ID);
float GetGearScoreByClass(uint32 ID, uint8 class_id);
bool IsCommonDrop(uint32 ID);
bool IsRareDrop(uint32 ID);
bool IsRaidDrop(uint32 ID);

float GetItemTypeWeight(const EQ::ItemData* item, uint8 class_id);
float GetSlotWeight(const EQ::ItemData* item, uint8 class_id); // TODO Necessary?
float CalculateGearScore(const EQ::ItemData* item, uint8 class_id);
float CalculateItemTypeBonus(const EQ::ItemData* item, uint8 class_id);
float CalculateACGearScore(uint8 class_id);
float CalculateHPGearScore(uint8 class_id);
float CalculateManaGearScore(uint8 class_id);
float CalculateEnduranceGearScore(uint8 class_id);
float CalculateHPRegenGearScore(uint8 class_id);
float CalculateManaRegenGearScore(uint8 class_id);
float CalculateEndRegenGearScore(uint8 class_id);
float CalculateWeaponGearScore(const EQ::ItemData* item, uint8 class_id);
float CalculateDamageGearScore(const EQ::ItemData* item, uint8 class_id); // TODO Necessary?
float CalculateDelayGearScore(const EQ::ItemData* item, uint8 class_id); // TODO Necessary?
float CalculateRatioGearScore(const EQ::ItemData* item, uint8 class_id);
float CalculateArrowGearScore(const EQ::ItemData* item, uint8 class_id);
float CalculateHasteGearScore(uint8 class_id);
float CalculateAGIGearScore(uint8 class_id);
float CalculateCHAGearScore(uint8 class_id);
float CalculateDEXGearScore(uint8 class_id);
float CalculateINTGearScore(uint8 class_id);
float CalculateSTAGearScore(uint8 class_id);
float CalculateSTRGearScore(uint8 class_id);
float CalculateWISGearScore(uint8 class_id);
float CalculateCRGearScore(uint8 class_id);
float CalculateDRGearScore(uint8 class_id);
float CalculateFRGearScore(uint8 class_id);
float CalculateMRGearScore(uint8 class_id);
float CalculatePRGearScore(uint8 class_id);
float CalculateCorruptionGearScore(uint8 class_id);
float CalculateHeroicAGIGearScore(uint8 class_id);
float CalculateHeroicCHAGearScore(uint8 class_id);
float CalculateHeroicDEXGearScore(uint8 class_id);
float CalculateHeroicINTGearScore(uint8 class_id);
float CalculateHeroicSTAGearScore(uint8 class_id);
float CalculateHeroicSTRGearScore(uint8 class_id);
float CalculateHeroicWISGearScore(uint8 class_id);
float CalculateHeroicCRGearScore(uint8 class_id);
float CalculateHeroicDRGearScore(uint8 class_id);
float CalculateHeroicFRGearScore(uint8 class_id);
float CalculateHeroicMRGearScore(uint8 class_id);
float CalculateHeroicPRGearScore(uint8 class_id);
float CalculateHeroicCorruptionGearScore(uint8 class_id);
float CalculateAccuracyGearScore(uint8 class_id);
float CalculateAttackGearScore(uint8 class_id);
float CalculateAvoidanceGearScore(uint8 class_id);
float CalculateCombatEffectsGearScore(uint8 class_id);
float CalculateDamageShieldGearScore(uint8 class_id);
float CalculateDoTShieldingGearScore(uint8 class_id);
float CalculateShieldingGearScore(uint8 class_id);
float CalculateSpellShieldGearScore(uint8 class_id);
float CalculateStrikethroughGearScore(uint8 class_id);
float CalculateStunResistGearScore(uint8 class_id);
float CalculateDSMitigationGearScore(uint8 class_id);
float CalculateHealAmountGearScore(uint8 class_id);
float CalculateSpellDamageGearScore(uint8 class_id);
float CalculateClairvoyanceGearScore(uint8 class_id);
float CalculateBackstabDamageGearScore(uint8 class_id);
float CalculateRecommendedLevelGearScore(uint8 class_id); // TODO Necessary?
float CalculateRequiredLevelGearScore(uint8 class_id); // TODO Necessary?
float CalculateRecommendedSkillGearScore(uint8 class_id); // TODO Necessary?
float CalculateSkillModGearScore(const EQ::ItemData* item, uint8 class_id);
float CalculateExtraSkillDmgGearScore(const EQ::ItemData* item, uint8 class_id);
float CalculateBaneDmgGearScore(const EQ::ItemData* item, uint8 class_id);
float CalculateElementalDmgGearScore(uint8 class_id);

namespace VEQ
{
	//	    bardtype,
	//	    bardvalue,
	//		bardeffect,
	//		bardeffecttype,
	//		bardlevel2,
	//		bardlevel,
	//		procrate,
	//		proceffect,
	//		proctype,
	//		proclevel2,
	//		proclevel,
	//		worneffect,
	//		worntype,
	//		wornlevel2,
	//		wornlevel,
	//		focuseffect,
	//		focustype,
	//		focuslevel2,
	//		focuslevel,
	//		clickeffect,
	//		maxcharges,
	//		clicktype,
	//		clicklevel,
	//		clicklevel2,
	//	    casttime,
	//	    casttime_,
	//		recastdelay,
	//		recasttype,

	//	    loregroup,
	//	    magic,
	//	    nodrop,
	//	    norent,
	//	    summonedflag,
	//	    attuneable,
	//	    notransfer,
	//	    powersourcecapacity,
	//	    heirloom,
	//	    epicitem
    //    };
    //} /*Items*/
}

namespace GearScoreTypes {
	constexpr uint16 Class                             = 1;
	constexpr uint16 Min                               = 2;
	constexpr uint16 Max                               = 3;
	constexpr uint16 Average                           = 4;
	constexpr uint16 Full                              = 5;
	constexpr uint16 ItemTypeWeight                    = 6;
	constexpr uint16 SlotWeight                        = 7;
	constexpr uint16 GearScore                         = 8;
	constexpr uint16 ItemTypeBonus                     = 9;
	constexpr uint16 ACGearScore                       = 10;
	constexpr uint16 HPGearScore                       = 11;
	constexpr uint16 ManaGearScore                     = 12;
	constexpr uint16 EnduranceGearScore                = 13;
	constexpr uint16 HPRegenGearScore                  = 14;
	constexpr uint16 ManaRegenGearScore                = 15;
	constexpr uint16 EndRegenGearScore                 = 16;
	constexpr uint16 WeaponGearScore                   = 17;
	constexpr uint16 DamageGearScore                   = 18;
	constexpr uint16 DelayGearScore                    = 19;
	constexpr uint16 RatioGearScore                    = 20;
	constexpr uint16 ArrowGearScore                    = 21;
	constexpr uint16 HasteGearScore                    = 22;
	constexpr uint16 AGIGearScore                      = 23;
	constexpr uint16 CHAGearScore                      = 24;
	constexpr uint16 DEXGearScore                      = 25;
	constexpr uint16 INTGearScore                      = 26;
	constexpr uint16 STAGearScore                      = 27;
	constexpr uint16 STRGearScore                      = 28;
	constexpr uint16 WISGearScore                      = 29;
	constexpr uint16 CRGearScore                       = 30;
	constexpr uint16 DRGearScore                       = 31;
	constexpr uint16 FRGearScore                       = 32;
	constexpr uint16 MRGearScore                       = 33;
	constexpr uint16 PRGearScore                       = 34;
	constexpr uint16 CorruptionGearScore               = 35;
	constexpr uint16 HeroicAGIGearScore                = 36;
	constexpr uint16 HeroicCHAGearScore                = 37;
	constexpr uint16 HeroicDEXGearScore                = 38;
	constexpr uint16 HeroicINTGearScore                = 39;
	constexpr uint16 HeroicSTAGearScore                = 40;
	constexpr uint16 HeroicSTRGearScore                = 41;
	constexpr uint16 HeroicWISGearScore                = 42;
	constexpr uint16 HeroicCRGearScore                 = 43;
	constexpr uint16 HeroicDRGearScore                 = 44;
	constexpr uint16 HeroicFRGearScore                 = 45;
	constexpr uint16 HeroicMRGearScore                 = 46;
	constexpr uint16 HeroicPRGearScore                 = 47;
	constexpr uint16 HeroicCorruptionGearScore         = 48;
	constexpr uint16 AccuracyGearScore                 = 49;
	constexpr uint16 AttackGearScore                   = 50;
	constexpr uint16 AvoidanceGearScore                = 51;
	constexpr uint16 CombatEffectsGearScore            = 52;
	constexpr uint16 DamageShieldGearScore             = 53;
	constexpr uint16 DoTShieldingGearScore             = 54;
	constexpr uint16 ShieldingGearScore                = 55;
	constexpr uint16 SpellShieldGearScore              = 56;
	constexpr uint16 StrikethroughGearScore            = 57;
	constexpr uint16 StunResistGearScore               = 58;
	constexpr uint16 DSMitigationGearScore             = 59;
	constexpr uint16 HealAmountGearScore               = 60;
	constexpr uint16 SpellDamageGearScore              = 61;
	constexpr uint16 ClairvoyanceGearScore             = 62;
	constexpr uint16 BackstabDamageGearScore           = 63;
	constexpr uint16 RecommendedLevelGearScore         = 64;
	constexpr uint16 RequiredLevelGearScore            = 65;
	constexpr uint16 RecommendedSkillGearScore         = 66;
	constexpr uint16 SkillModGearScore                 = 67;
	constexpr uint16 ExtraSkillDmgGearScore            = 68;
	constexpr uint16 BaneDmgGearScore                  = 69;
	constexpr uint16 ElementalDmgGearScore             = 70;

	constexpr uint16 START                             = GearScoreTypes::Class;
	constexpr uint16 END                               = GearScoreTypes::ElementalDmgGearScore;

	constexpr uint16 GM_START                      	   = GearScoreTypes::ItemTypeWeight;
	constexpr uint16 GM_END                            = GearScoreTypes::END;
	constexpr uint16 PLAYER_START                      = GearScoreTypes::Class;
	constexpr uint16 PLAYER_END                        = GearScoreTypes::Full;
}

static std::map<uint16, std::string> gearscore_type_names = {
	{ GearScoreTypes::Class, "Class" },
	{ GearScoreTypes::Min, "Min" },
	{ GearScoreTypes::Max, "Max" },
	{ GearScoreTypes::Average, "Average" },
	{ GearScoreTypes::Full, "Full" },
	{ GearScoreTypes::ItemTypeWeight, "ItemTypeWeight" },
	{ GearScoreTypes::SlotWeight, "SlotWeight" },
	{ GearScoreTypes::GearScore, "Gear" },
	{ GearScoreTypes::ItemTypeBonus, "ItemTypeBonus" },
	{ GearScoreTypes::ACGearScore, "AC" },
	{ GearScoreTypes::HPGearScore, "HP" },
	{ GearScoreTypes::ManaGearScore, "Mana" },
	{ GearScoreTypes::EnduranceGearScore, "Endurance" },
	{ GearScoreTypes::HPRegenGearScore, "HPRegen" },
	{ GearScoreTypes::ManaRegenGearScore, "ManaRegen" },
	{ GearScoreTypes::EndRegenGearScore, "EndRegen" },
	{ GearScoreTypes::WeaponGearScore, "Weapon" },
	{ GearScoreTypes::DamageGearScore, "Damage" },
	{ GearScoreTypes::DelayGearScore, "Delay" },
	{ GearScoreTypes::RatioGearScore, "Ratio" },
	{ GearScoreTypes::ArrowGearScore, "Arrow" },
	{ GearScoreTypes::HasteGearScore, "Haste" },
	{ GearScoreTypes::AGIGearScore, "AGI" },
	{ GearScoreTypes::CHAGearScore, "CHA" },
	{ GearScoreTypes::DEXGearScore, "DEX" },
	{ GearScoreTypes::INTGearScore, "INT" },
	{ GearScoreTypes::STAGearScore, "STA" },
	{ GearScoreTypes::STRGearScore, "STR" },
	{ GearScoreTypes::WISGearScore, "WIS" },
	{ GearScoreTypes::CRGearScore, "CR" },
	{ GearScoreTypes::DRGearScore, "DR" },
	{ GearScoreTypes::FRGearScore, "FR" },
	{ GearScoreTypes::MRGearScore, "MR" },
	{ GearScoreTypes::PRGearScore, "PR" },
	{ GearScoreTypes::CorruptionGearScore, "Corruption" },
	{ GearScoreTypes::HeroicAGIGearScore, "HeroicAGI" },
	{ GearScoreTypes::HeroicCHAGearScore, "HeroicCHA" },
	{ GearScoreTypes::HeroicDEXGearScore, "HeroicDEX" },
	{ GearScoreTypes::HeroicINTGearScore, "HeroicINT" },
	{ GearScoreTypes::HeroicSTAGearScore, "HeroicSTA" },
	{ GearScoreTypes::HeroicSTRGearScore, "HeroicSTR" },
	{ GearScoreTypes::HeroicWISGearScore, "HeroicWIS" },
	{ GearScoreTypes::HeroicCRGearScore, "HeroicCR" },
	{ GearScoreTypes::HeroicDRGearScore, "HeroicDR" },
	{ GearScoreTypes::HeroicFRGearScore, "HeroicFR" },
	{ GearScoreTypes::HeroicMRGearScore, "HeroicMR" },
	{ GearScoreTypes::HeroicPRGearScore, "HeroicPR" },
	{ GearScoreTypes::HeroicCorruptionGearScore, "HeroicCorruption" },
	{ GearScoreTypes::AccuracyGearScore, "Accuracy" },
	{ GearScoreTypes::AttackGearScore, "Attack" },
	{ GearScoreTypes::AvoidanceGearScore, "Avoidance" },
	{ GearScoreTypes::CombatEffectsGearScore, "CombatEffects" },
	{ GearScoreTypes::DamageShieldGearScore, "DamageShield" },
	{ GearScoreTypes::DoTShieldingGearScore, "DoTShielding" },
	{ GearScoreTypes::ShieldingGearScore, "Shielding" },
	{ GearScoreTypes::SpellShieldGearScore, "SpellShield" },
	{ GearScoreTypes::StrikethroughGearScore, "Strikethrough" },
	{ GearScoreTypes::StunResistGearScore, "StunResist" },
	{ GearScoreTypes::DSMitigationGearScore, "DSMitigation" },
	{ GearScoreTypes::HealAmountGearScore, "HealAmount" },
	{ GearScoreTypes::SpellDamageGearScore, "SpellDamage" },
	{ GearScoreTypes::ClairvoyanceGearScore, "Clairvoyance" },
	{ GearScoreTypes::BackstabDamageGearScore, "BackstabDamage" },
	{ GearScoreTypes::RecommendedLevelGearScore, "RecommendedLevel" },
	{ GearScoreTypes::RequiredLevelGearScore, "RequiredLevel" },
	{ GearScoreTypes::RecommendedSkillGearScore, "RecommendedSkill" },
	{ GearScoreTypes::SkillModGearScore, "SkillMod" },
	{ GearScoreTypes::ExtraSkillDmgGearScore, "ExtraSkillDmg" },
	{ GearScoreTypes::BaneDmgGearScore, "BaneDmg" },
	{ GearScoreTypes::ElementalDmgGearScore, "ElementalDmg" }
};

#endif /*COMMON_VEQ_ITEM_DATA_H*/
