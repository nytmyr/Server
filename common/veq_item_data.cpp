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

#include "veq_item_data.h"
#include "../zone/client.h"

void SendGearScoreTypePrompts(Client* c) {
	c->Message(Chat::Yellow, "Select an option to view Gear Score types by Name and ID:");
	c->Message(
		Chat::Yellow,
		fmt::format(
			"{}, {}, {}, {}, {}.",
			Saylink::Silent(
				fmt::format("#gearscore gearscoretypes 0-19"), "0-19"
			),
			Saylink::Silent(
				fmt::format("#gearscore gearscoretypes 20-39"), "20-39"
			),
			Saylink::Silent(
				fmt::format("#gearscore gearscoretypes 40-59"), "40-59"
			),
			Saylink::Silent(
				fmt::format("#gearscore gearscoretypes 60-79"), "60-79"
			),
			Saylink::Silent(
				fmt::format("#gearscore gearscoretypes 80+"), "80+"
			)
		).c_str()
	);
}

bool IsValidGearScoreType(uint16 type_id) {
	return EQ::ValueWithin(type_id, GearScoreTypes::START, GearScoreTypes::END);
}

uint16 GetGearScoreTypeByShortname(std::string short_name) {
	for (int i = GearScoreTypes::START; i <= GearScoreTypes::END; ++i) {
		if (!Strings::ToLower(short_name).compare(Strings::ToLower(GetGearScoreTypeByID(i)))) {
			return i;
		}
	}

	return 0;
}

std::string GetGearScoreTypeByID(uint16 type_id) {
	return IsValidGearScoreType(type_id) ? gearscore_type_names[type_id] : "UNKNOWN GEARSCORE TYPE";
}

float GetGearScoreByType(uint16 search_type, const EQ::ItemData* item, uint32 ID, uint8 class_id) {
	switch (search_type) {
		case GearScoreTypes::Min:
			return GetGearScoreMin(ID);
		case GearScoreTypes::Max:
			return GetGearScoreMax(ID);
		case GearScoreTypes::Average:
			return GetGearScoreAverage(ID);
		case GearScoreTypes::Full:
			return GetGearScoreByClass(ID, Class::ALL_CLASSES_BITMASK);
		case GearScoreTypes::ItemTypeWeight:
			return GetItemTypeWeight(item, class_id);
		case GearScoreTypes::SlotWeight:
			return GetSlotWeight(item, class_id);
		case GearScoreTypes::ItemTypeBonus:
			return CalculateItemTypeBonus(item, class_id);
		case GearScoreTypes::ACGearScore:
			return item->AC * CalculateACGearScore(class_id);
		case GearScoreTypes::HPGearScore:
			return item->HP * CalculateHPGearScore(class_id);
		case GearScoreTypes::ManaGearScore:
			return item->Mana * CalculateManaGearScore(class_id);
		case GearScoreTypes::EnduranceGearScore:
			return item->Endur * CalculateEnduranceGearScore(class_id);
		case GearScoreTypes::HPRegenGearScore:
			return item->Regen * CalculateHPRegenGearScore(class_id);
		case GearScoreTypes::ManaRegenGearScore:
			return item->ManaRegen * CalculateManaRegenGearScore(class_id);
		case GearScoreTypes::EndRegenGearScore:
			return item->EnduranceRegen * CalculateEndRegenGearScore(class_id);
		case GearScoreTypes::WeaponGearScore:
			return CalculateWeaponGearScore(item, class_id);
		case GearScoreTypes::DamageGearScore:
			return CalculateDamageGearScore(item, class_id);
		case GearScoreTypes::DelayGearScore:
			return CalculateDelayGearScore(item, class_id);
		case GearScoreTypes::RatioGearScore:
			return CalculateRatioGearScore(item, class_id);
		case GearScoreTypes::ArrowGearScore:
			return CalculateArrowGearScore(item, class_id);
		case GearScoreTypes::HasteGearScore:
			return item->Haste * CalculateHasteGearScore(class_id);
		case GearScoreTypes::AGIGearScore:
			return item->AAgi * CalculateAGIGearScore(class_id);
		case GearScoreTypes::CHAGearScore:
			return item->ACha * CalculateCHAGearScore(class_id);
		case GearScoreTypes::DEXGearScore:
			return item->ADex * CalculateDEXGearScore(class_id);
		case GearScoreTypes::INTGearScore:
			return item->AInt * CalculateINTGearScore(class_id);
		case GearScoreTypes::STAGearScore:
			return item->ASta * CalculateSTAGearScore(class_id);
		case GearScoreTypes::STRGearScore:
			return item->AStr * CalculateSTRGearScore(class_id);
		case GearScoreTypes::WISGearScore:
			return item->AWis * CalculateWISGearScore(class_id);
		case GearScoreTypes::CRGearScore:
			return item->CR * CalculateCRGearScore(class_id);
		case GearScoreTypes::DRGearScore:
			return item->DR * CalculateDRGearScore(class_id);
		case GearScoreTypes::FRGearScore:
			return item->FR * CalculateFRGearScore(class_id);
		case GearScoreTypes::MRGearScore:
			return item->MR * CalculateMRGearScore(class_id);
		case GearScoreTypes::PRGearScore:
			return item->PR * CalculatePRGearScore(class_id);
		case GearScoreTypes::CorruptionGearScore:
			return item->SVCorruption * CalculateCorruptionGearScore(class_id);
		case GearScoreTypes::HeroicAGIGearScore:
			return item->HeroicAgi * CalculateHeroicAGIGearScore(class_id);
		case GearScoreTypes::HeroicCHAGearScore:
			return item->HeroicCha * CalculateHeroicCHAGearScore(class_id);
		case GearScoreTypes::HeroicDEXGearScore:
			return item->HeroicDex * CalculateHeroicDEXGearScore(class_id);
		case GearScoreTypes::HeroicINTGearScore:
			return item->HeroicInt * CalculateHeroicINTGearScore(class_id);
		case GearScoreTypes::HeroicSTAGearScore:
			return item->HeroicSta * CalculateHeroicSTAGearScore(class_id);
		case GearScoreTypes::HeroicSTRGearScore:
			return item->HeroicStr * CalculateHeroicSTRGearScore(class_id);
		case GearScoreTypes::HeroicWISGearScore:
			return item->HeroicWis * CalculateHeroicWISGearScore(class_id);
		case GearScoreTypes::HeroicCRGearScore:
			return item->HeroicCR * CalculateHeroicCRGearScore(class_id);
		case GearScoreTypes::HeroicDRGearScore:
			return item->HeroicDR * CalculateHeroicDRGearScore(class_id);
		case GearScoreTypes::HeroicFRGearScore:
			return item->HeroicFR * CalculateHeroicFRGearScore(class_id);
		case GearScoreTypes::HeroicMRGearScore:
			return item->HeroicMR * CalculateHeroicMRGearScore(class_id);
		case GearScoreTypes::HeroicPRGearScore:
			return item->HeroicPR * CalculateHeroicPRGearScore(class_id);
		case GearScoreTypes::HeroicCorruptionGearScore:
			return item->HeroicSVCorrup * CalculateHeroicCorruptionGearScore(class_id);
		case GearScoreTypes::AccuracyGearScore:
			return item->Accuracy * CalculateAccuracyGearScore(class_id);
		case GearScoreTypes::AttackGearScore:
			return item->Attack * CalculateAttackGearScore(class_id);
		case GearScoreTypes::AvoidanceGearScore:
			return item->Avoidance * CalculateAvoidanceGearScore(class_id);
		case GearScoreTypes::CombatEffectsGearScore:
			return item->CombatEffects * CalculateCombatEffectsGearScore(class_id);
		case GearScoreTypes::DamageShieldGearScore:
			return item->DamageShield * CalculateDamageShieldGearScore(class_id);
		case GearScoreTypes::DoTShieldingGearScore:
			return item->DotShielding * CalculateDoTShieldingGearScore(class_id);
		case GearScoreTypes::ShieldingGearScore:
			return item->Shielding * CalculateShieldingGearScore(class_id);
		case GearScoreTypes::SpellShieldGearScore:
			return item->SpellShield * CalculateSpellShieldGearScore(class_id);
		case GearScoreTypes::StrikethroughGearScore:
			return item->StrikeThrough * CalculateStrikethroughGearScore(class_id);
		case GearScoreTypes::StunResistGearScore:
			return item->StunResist * CalculateStunResistGearScore(class_id);
		case GearScoreTypes::DSMitigationGearScore:
			return item->DSMitigation * CalculateDSMitigationGearScore(class_id);
		case GearScoreTypes::HealAmountGearScore:
			return item->HealAmt * CalculateHealAmountGearScore(class_id);
		case GearScoreTypes::SpellDamageGearScore:
			return item->SpellDmg * CalculateSpellDamageGearScore(class_id);
		case GearScoreTypes::ClairvoyanceGearScore:
			return item->Clairvoyance * CalculateClairvoyanceGearScore(class_id);
		case GearScoreTypes::BackstabDamageGearScore:
			return item->BackstabDmg * CalculateBackstabDamageGearScore(class_id);
		case GearScoreTypes::RecommendedLevelGearScore:
			return item->RecLevel * CalculateRecommendedLevelGearScore(class_id);
		case GearScoreTypes::RequiredLevelGearScore:
			return item->ReqLevel * CalculateRequiredLevelGearScore(class_id);
		case GearScoreTypes::RecommendedSkillGearScore:
			return item->RecSkill * CalculateRecommendedSkillGearScore(class_id);
		case GearScoreTypes::SkillModGearScore:
			return CalculateSkillModGearScore(item, class_id);
		case GearScoreTypes::ExtraSkillDmgGearScore:
			return CalculateExtraSkillDmgGearScore(item, class_id);
		case GearScoreTypes::BaneDmgGearScore:
			return CalculateBaneDmgGearScore(item, class_id);
		case GearScoreTypes::ElementalDmgGearScore:
			return item->ElemDmgAmt * CalculateElementalDmgGearScore(class_id);
		case GearScoreTypes::GearScore:
		case GearScoreTypes::Class:
		default:
			return GetGearScoreByClass(ID, class_id);
	}
}

float GetGearScoreMin(uint32 ID) {
	const auto& l = BaseVeqGearScoreRepository::GetWhere(
		database,
		fmt::format(
			"`id` = {}",
			ID
		)
	);

	if (l.empty()) {
		return 0.0f;
	}

	auto e = l.front();

	return e.low;
}

float GetGearScoreMax(uint32 ID) {
	const auto& l = BaseVeqGearScoreRepository::GetWhere(
		database,
		fmt::format(
			"`id` = {}",
			ID
		)
	);

	if (l.empty()) {
		return 0.0f;
	}

	auto e = l.front();

	return e.high;
}

float GetGearScoreAverage(uint32 ID) {
	const auto& l = BaseVeqGearScoreRepository::GetWhere(
		database,
		fmt::format(
			"`id` = {}",
			ID
		)
	);

	if (l.empty()) {
		return 0.0f;
	}

	auto e = l.front();

	return e.avg;
}

float GetGearScoreByClass(uint32 ID, uint8 class_id) {
	const auto& l = BaseVeqGearScoreRepository::GetWhere(
		database,
		fmt::format(
			"`id` = {}",
			ID
		)
	);

	if (l.empty()) {
		return 0.0f;
	}

	auto e = l.front();

	switch (class_id) {
		case Class::Warrior:
			return e.classes1;
		case Class::Cleric:
			return e.classes2;
		case Class::Paladin:
			return e.classes3;
		case Class::Ranger:
			return e.classes4;
		case Class::ShadowKnight:
			return e.classes5;
		case Class::Druid:
			return e.classes6;
		case Class::Monk:
			return e.classes7;
		case Class::Bard:
			return e.classes8;
		case Class::Rogue:
			return e.classes9;
		case Class::Shaman:
			return e.classes10;
		case Class::Necromancer:
			return e.classes11;
		case Class::Wizard:
			return e.classes12;
		case Class::Magician:
			return e.classes13;
		case Class::Enchanter:
			return e.classes14;
		case Class::Beastlord:
			return e.classes15;
		case Class::Berserker:
			return e.classes16;
		default:
			return e.avg;
	}
}

bool IsCommonDrop(uint32 ID) {
	const auto& l = BaseVeqGearScoreRepository::GetWhere(
		database,
		fmt::format(
			"`id` = {}",
			ID
		)
	);

	if (l.empty()) {
		return 0.0f;
	}

	auto e = l.front();

	return e.common_drop;
}

bool IsRareDrop(uint32 ID) {
	const auto& l = BaseVeqGearScoreRepository::GetWhere(
		database,
		fmt::format(
			"`id` = {}",
			ID
		)
	);

	if (l.empty()) {
		return 0.0f;
	}

	auto e = l.front();

	return e.rare_drop;
}

bool IsRaidDrop(uint32 ID) {
	const auto& l = BaseVeqGearScoreRepository::GetWhere(
		database,
		fmt::format(
			"`id` = {}",
			ID
		)
	);

	if (l.empty()) {
		return 0.0f;
	}

	auto e = l.front();

	return e.raid_drop;
}

BaseVeqGearScoreRepository::VeqGearScore UpdateGearScores(const EQ::ItemData* item, bool bulk_update) {
	auto e = BaseVeqGearScoreRepository::NewEntity();

	e.id = item->ID;
	e.name = item->Name;
	//e.common_drop = ;
	//e.rare_drop = ;
	//e.raid_drop = ;
	e.classes1 = CalculateGearScore(item, Class::Warrior);
	e.classes2 = CalculateGearScore(item, Class::Cleric);
	e.classes3 = CalculateGearScore(item, Class::Paladin);
	e.classes4 = CalculateGearScore(item, Class::Ranger);
	e.classes5 = CalculateGearScore(item, Class::ShadowKnight);
	e.classes6 = CalculateGearScore(item, Class::Druid);
	e.classes7 = CalculateGearScore(item, Class::Monk);
	e.classes8 = CalculateGearScore(item, Class::Bard);
	e.classes9 = CalculateGearScore(item, Class::Rogue);
	e.classes10 = CalculateGearScore(item, Class::Shaman);
	e.classes11 = CalculateGearScore(item, Class::Necromancer);
	e.classes12 = CalculateGearScore(item, Class::Wizard);
	e.classes13 = CalculateGearScore(item, Class::Magician);
	e.classes14 = CalculateGearScore(item, Class::Enchanter);
	e.classes15 = CalculateGearScore(item, Class::Beastlord);
	e.classes16 = CalculateGearScore(item, Class::Berserker);
	e.low = std::min({ e.classes1, e.classes2, e.classes3, e.classes4, e.classes5, e.classes6, e.classes7, e.classes8, e.classes9, e.classes10, e.classes11, e.classes12, e.classes13, e.classes14, e.classes15, e.classes16});
	e.high = std::max({ e.classes1, e.classes2, e.classes3, e.classes4, e.classes5, e.classes6, e.classes7, e.classes8, e.classes9, e.classes10, e.classes11, e.classes12, e.classes13, e.classes14, e.classes15, e.classes16});
	e.avg = (e.classes1 + e.classes2 + e.classes3 + e.classes4 + e.classes5 + e.classes6 + e.classes7 + e.classes8 + e.classes9 + e.classes10 + e.classes11 + e.classes12 + e.classes13 + e.classes14 + e.classes15 + e.classes16) / 16;

	e.ac_score = item->AC * CalculateACGearScore(Class::ALL_CLASSES_BITMASK);
	e.hp_score = item->HP * CalculateHPGearScore(Class::ALL_CLASSES_BITMASK);
	e.mana_score = item->Mana * CalculateManaGearScore(Class::ALL_CLASSES_BITMASK);
	e.end_score = item->Endur * CalculateEnduranceGearScore(Class::ALL_CLASSES_BITMASK);
	e.hp_regen_score = item->Regen * CalculateHPRegenGearScore(Class::ALL_CLASSES_BITMASK);
	e.mana_regen_score = item->ManaRegen * CalculateManaRegenGearScore(Class::Wizard);
	e.end_regen_score = item->EnduranceRegen * CalculateEndRegenGearScore(Class::Berserker);

	e.damage_score = CalculateDamageGearScore(item, Class::ALL_CLASSES_BITMASK);
	e.delay_score = CalculateDelayGearScore(item, Class::ALL_CLASSES_BITMASK);
	e.ratio_score = CalculateRatioGearScore(item, Class::ALL_CLASSES_BITMASK);
	e.weapon_score = CalculateWeaponGearScore(item, Class::Warrior);
	e.arrow_score = CalculateArrowGearScore(item, Class::ALL_CLASSES_BITMASK);

	e.haste_score = item->Haste * CalculateHasteGearScore(Class::Warrior);

	e.agi_score = item->AAgi * CalculateAGIGearScore(Class::ALL_CLASSES_BITMASK);
	e.cha_score = item->ACha * CalculateCHAGearScore(Class::ALL_CLASSES_BITMASK);
	e.dex_score = item->ADex * CalculateDEXGearScore(Class::ALL_CLASSES_BITMASK);
	e.int_score = item->AInt * CalculateINTGearScore(Class::Wizard);
	e.sta_score = item->ASta * CalculateSTAGearScore(Class::ALL_CLASSES_BITMASK);
	e.str_score = item->AStr * CalculateSTRGearScore(Class::ALL_CLASSES_BITMASK);
	e.wis_score = item->AWis * CalculateWISGearScore(Class::Druid);
	e.cr_score = item->CR * CalculateCRGearScore(Class::ALL_CLASSES_BITMASK);
	e.dr_score = item->DR * CalculateDRGearScore(Class::ALL_CLASSES_BITMASK);
	e.fr_score = item->FR * CalculateFRGearScore(Class::ALL_CLASSES_BITMASK);
	e.mr_score = item->MR * CalculateMRGearScore(Class::ALL_CLASSES_BITMASK);
	e.pr_score = item->PR * CalculatePRGearScore(Class::ALL_CLASSES_BITMASK);
	e.corruption_score = item->SVCorruption * CalculateCorruptionGearScore(Class::ALL_CLASSES_BITMASK);
	e.h_agi_score = item->HeroicAgi * CalculateHeroicAGIGearScore(Class::ALL_CLASSES_BITMASK);
	e.h_cha_score = item->HeroicCha * CalculateHeroicCHAGearScore(Class::ALL_CLASSES_BITMASK);
	e.h_dex_score = item->HeroicDex * CalculateHeroicDEXGearScore(Class::ALL_CLASSES_BITMASK);
	e.h_int_score = item->HeroicInt * CalculateHeroicINTGearScore(Class::ALL_CLASSES_BITMASK);
	e.h_sta_score = item->HeroicSta * CalculateHeroicSTAGearScore(Class::ALL_CLASSES_BITMASK);
	e.h_str_score = item->HeroicStr * CalculateHeroicSTRGearScore(Class::ALL_CLASSES_BITMASK);
	e.h_wis_score = item->HeroicWis * CalculateHeroicWISGearScore(Class::ALL_CLASSES_BITMASK);
	e.h_cr_score = item->HeroicCR * CalculateHeroicCRGearScore(Class::ALL_CLASSES_BITMASK);
	e.h_dr_score = item->HeroicDR * CalculateHeroicDRGearScore(Class::ALL_CLASSES_BITMASK);
	e.h_fr_score = item->HeroicFR * CalculateHeroicFRGearScore(Class::ALL_CLASSES_BITMASK);
	e.h_mr_score = item->HeroicMR * CalculateHeroicMRGearScore(Class::ALL_CLASSES_BITMASK);
	e.h_pr_score = item->HeroicPR * CalculateHeroicPRGearScore(Class::ALL_CLASSES_BITMASK);
	e.h_corruption_score = item->HeroicSVCorrup * CalculateHeroicCorruptionGearScore(Class::ALL_CLASSES_BITMASK);
	e.accuracy_score = item->Accuracy * CalculateAccuracyGearScore(Class::ALL_CLASSES_BITMASK);
	e.attack_score = item->Attack * CalculateAttackGearScore(Class::ALL_CLASSES_BITMASK);
	e.avoidance_score = item->Avoidance * CalculateAvoidanceGearScore(Class::ALL_CLASSES_BITMASK);
	e.combat_effects_score = item->CombatEffects * CalculateCombatEffectsGearScore(Class::ALL_CLASSES_BITMASK);
	e.damage_shield_score = item->DamageShield * CalculateDamageShieldGearScore(Class::ALL_CLASSES_BITMASK);
	e.dot_shielding_score = item->DotShielding * CalculateDoTShieldingGearScore(Class::ALL_CLASSES_BITMASK);
	e.shielding_score = item->Shielding * CalculateShieldingGearScore(Class::ALL_CLASSES_BITMASK);
	e.spellshield_score = item->SpellShield * CalculateSpellShieldGearScore(Class::ALL_CLASSES_BITMASK);
	e.strikethrough_score = item->StrikeThrough * CalculateStrikethroughGearScore(Class::ALL_CLASSES_BITMASK);
	e.stunresist_score = item->StunResist * CalculateStunResistGearScore(Class::ALL_CLASSES_BITMASK);
	e.ds_mitigation_score = item->DSMitigation * CalculateDSMitigationGearScore(Class::ALL_CLASSES_BITMASK);
	e.heal_amount_score = item->HealAmt * CalculateHealAmountGearScore(Class::ALL_CLASSES_BITMASK);
	e.spell_damage_score = item->SpellDmg * CalculateSpellDamageGearScore(Class::ALL_CLASSES_BITMASK);
	e.clairvoyance_score = item->Clairvoyance * CalculateClairvoyanceGearScore(Class::ALL_CLASSES_BITMASK);
	e.backstab_damage_score = item->BackstabDmg * CalculateBackstabDamageGearScore(Class::ALL_CLASSES_BITMASK);
	e.skillmod_score = CalculateSkillModGearScore(item, Class::ALL_CLASSES_BITMASK);
	e.extra_skill_score = CalculateExtraSkillDmgGearScore(item, Class::ALL_CLASSES_BITMASK);
	e.bane_dmg_score = CalculateBaneDmgGearScore(item, Class::ALL_CLASSES_BITMASK);
	e.elemental_dmg_score = item->ElemDmgAmt * CalculateElementalDmgGearScore(Class::ALL_CLASSES_BITMASK);

	if (bulk_update) {
		return e;
	}
	else {
		BaseVeqGearScoreRepository::ReplaceOne(database, e);
	}
}

float CalculateGearScore(const EQ::ItemData* item, uint8 class_id) {
	// TODO if ever added to character specific equipment, augments can be calculated with EQ::ItemInstance:: | inst->GetItemArmorClass(true)
	if (!item) {
		LogTestDebug("Returning due to !item"); //deleteme
		return 0.0f;
	}

	if (!(item->Classes & GetPlayerClassBit(GetPlayerClassValue(class_id)))) {
		return 0.0f;
	}

	float gear_score = 0.0f;

	gear_score += CalculateItemTypeBonus(item, class_id);
	gear_score += item->AC * CalculateACGearScore(class_id);
	gear_score += item->HP * CalculateHPGearScore(class_id);
	gear_score += item->Mana * CalculateManaGearScore(class_id);
	gear_score += item->Endur * CalculateEnduranceGearScore(class_id);
	gear_score += item->Regen * CalculateHPRegenGearScore(class_id);
	gear_score += item->ManaRegen * CalculateManaRegenGearScore(class_id);
	gear_score += item->EnduranceRegen * CalculateEndRegenGearScore(class_id);

	gear_score += CalculateWeaponGearScore(item, class_id);
	gear_score += CalculateArrowGearScore(item, class_id);

	gear_score += item->Haste * CalculateHasteGearScore(class_id);

	gear_score += item->AAgi * CalculateAGIGearScore(class_id);
	gear_score += item->ACha * CalculateCHAGearScore(class_id);
	gear_score += item->ADex * CalculateDEXGearScore(class_id);
	gear_score += item->AInt * CalculateINTGearScore(class_id);
	gear_score += item->ASta * CalculateSTAGearScore(class_id);
	gear_score += item->AStr * CalculateSTRGearScore(class_id);
	gear_score += item->AWis * CalculateWISGearScore(class_id);
	gear_score += item->CR * CalculateCRGearScore(class_id);
	gear_score += item->DR * CalculateDRGearScore(class_id);
	gear_score += item->FR * CalculateFRGearScore(class_id);
	gear_score += item->MR * CalculateMRGearScore(class_id);
	gear_score += item->PR * CalculatePRGearScore(class_id);
	gear_score += item->SVCorruption * CalculateCorruptionGearScore(class_id);
	gear_score += item->HeroicAgi * CalculateHeroicAGIGearScore(class_id);
	gear_score += item->HeroicCha * CalculateHeroicCHAGearScore(class_id);
	gear_score += item->HeroicDex * CalculateHeroicDEXGearScore(class_id);
	gear_score += item->HeroicInt * CalculateHeroicINTGearScore(class_id);
	gear_score += item->HeroicSta * CalculateHeroicSTAGearScore(class_id);
	gear_score += item->HeroicStr * CalculateHeroicSTRGearScore(class_id);
	gear_score += item->HeroicWis * CalculateHeroicWISGearScore(class_id);
	gear_score += item->HeroicCR * CalculateHeroicCRGearScore(class_id);
	gear_score += item->HeroicDR * CalculateHeroicDRGearScore(class_id);
	gear_score += item->HeroicFR * CalculateHeroicFRGearScore(class_id);
	gear_score += item->HeroicMR * CalculateHeroicMRGearScore(class_id);
	gear_score += item->HeroicPR * CalculateHeroicPRGearScore(class_id);
	gear_score += item->HeroicSVCorrup * CalculateHeroicCorruptionGearScore(class_id);
	gear_score += item->Accuracy * CalculateAccuracyGearScore(class_id);
	gear_score += item->Attack * CalculateAttackGearScore(class_id);
	gear_score += item->Avoidance * CalculateAvoidanceGearScore(class_id);
	gear_score += item->CombatEffects * CalculateCombatEffectsGearScore(class_id);
	gear_score += item->DamageShield * CalculateDamageShieldGearScore(class_id);
	gear_score += item->DotShielding * CalculateDoTShieldingGearScore(class_id);
	gear_score += item->Shielding * CalculateShieldingGearScore(class_id);
	gear_score += item->SpellShield * CalculateSpellShieldGearScore(class_id);
	gear_score += item->StrikeThrough * CalculateStrikethroughGearScore(class_id);
	gear_score += item->StunResist * CalculateStunResistGearScore(class_id);
	gear_score += item->DSMitigation * CalculateDSMitigationGearScore(class_id);
	gear_score += item->HealAmt * CalculateHealAmountGearScore(class_id);
	gear_score += item->SpellDmg * CalculateSpellDamageGearScore(class_id);
	gear_score += item->Clairvoyance * CalculateClairvoyanceGearScore(class_id);
	gear_score += item->BackstabDmg * CalculateBackstabDamageGearScore(class_id);
	gear_score += item->RecLevel * CalculateRecommendedLevelGearScore(class_id);
	gear_score += item->ReqLevel * CalculateRequiredLevelGearScore(class_id);
	gear_score += item->RecSkill * CalculateRecommendedSkillGearScore(class_id);
	gear_score += CalculateSkillModGearScore(item, class_id);
	gear_score += CalculateExtraSkillDmgGearScore(item, class_id);
	gear_score += CalculateBaneDmgGearScore(item, class_id);
	gear_score += item->ElemDmgAmt * CalculateElementalDmgGearScore(class_id);

	gear_score *= GetItemTypeWeight(item, class_id);
	gear_score *= GetSlotWeight(item, class_id);

	return gear_score;
}

float GetItemTypeWeight(const EQ::ItemData* item, uint8 class_id) {
	if (!item) {
		LogTestDebug("Returning due to !item"); //deleteme
		return 0.0f;
	}

	uint8 item_type = item->ItemType;

	switch (item_type) {
		case EQ::item::ItemType2HSlash:
		case EQ::item::ItemType2HBlunt:
		case EQ::item::ItemType2HPiercing:
			return 1.25f;
		default:
			return 1.0f;
	}
}

float GetSlotWeight(const EQ::ItemData* item, uint8 class_id) {
	//TODO add bitmask check in order of slot hierarchy
	return 1.0f;

	if (!item) {
		LogTestDebug("Returning due to !item"); //deleteme
		return 0.0f;
	}

	int16 slot_id = item->Slots;

	switch (slot_id) {
		case EQ::invslot::slotCharm:
		case EQ::invslot::slotEar1:
		case EQ::invslot::slotEar2:
		case EQ::invslot::slotHead:
		case EQ::invslot::slotFace:
		case EQ::invslot::slotNeck:
		case EQ::invslot::slotShoulders:
		case EQ::invslot::slotArms:
		case EQ::invslot::slotBack:
		case EQ::invslot::slotWrist1:
		case EQ::invslot::slotWrist2:
		case EQ::invslot::slotHands:
		case EQ::invslot::slotPrimary:
		case EQ::invslot::slotSecondary:
		case EQ::invslot::slotRange:
		case EQ::invslot::slotFinger1:
		case EQ::invslot::slotFinger2:
		case EQ::invslot::slotChest:
		case EQ::invslot::slotLegs:
		case EQ::invslot::slotFeet:
		case EQ::invslot::slotWaist:
		case EQ::invslot::slotPowerSource:
		case EQ::invslot::slotAmmo:
		default:
			return 1.0f;
	}
}

float CalculateItemTypeBonus(const EQ::ItemData* item, uint8 class_id) {
	if (!item) {
		LogTestDebug("Returning due to !item"); //deleteme
		return 0.0f;
	}

	float bonus = 0.0f;

	if (item->ItemType == EQ::item::ItemTypeShield) {
		switch (class_id) {
			case Class::Warrior:
			case Class::Paladin:
			case Class::ShadowKnight:
			case Class::Cleric:
				bonus += (item->AC * CalculateACGearScore(class_id));

				break;
			case Class::Shaman:
			case Class::Druid:
			case Class::Necromancer:
			case Class::Wizard:
			case Class::Magician:
			case Class::Enchanter:
				bonus += 0.75 * (item->AC * CalculateACGearScore(class_id));

				break;
			case Class::Ranger:
			case Class::Monk:
			case Class::Bard:
			case Class::Rogue:
			case Class::Beastlord:
			case Class::Berserker:
				break;
			default:
				bonus += (item->AC * CalculateACGearScore(class_id));

				break;
		}
	}

	return bonus;
}

float CalculateACGearScore(uint8 class_id) {
	switch (class_id) {
		case Class::Warrior:
			return 6.0f;
		case Class::Paladin:
		case Class::ShadowKnight:
			return 5.5f;
		case Class::Ranger:
			return 5.0f;
		case Class::Cleric:
		case Class::Monk:
		case Class::Bard:
			return 4.5f;
		case Class::Rogue:
		case Class::Shaman:
		case Class::Beastlord:
		case Class::Berserker:
			return 4.0f;
		case Class::Druid:
			return 3.5f;
		case Class::Necromancer:
		case Class::Wizard:
		case Class::Magician:
		case Class::Enchanter:
			return 2.5f;
		default:
			return 6.0f;
	}
}

float CalculateHPGearScore(uint8 class_id) {
	switch (class_id) {
		case Class::Warrior:
		case Class::Paladin:
		case Class::ShadowKnight:
			return 1.0f;
		case Class::Berserker:
		case Class::Monk:
			return 0.75f;
		case Class::Beastlord:
		case Class::Ranger:
		case Class::Rogue:
		case Class::Cleric:
		case Class::Bard:
			return 0.5f;
		case Class::Druid:
		case Class::Shaman:
		case Class::Necromancer:
		case Class::Wizard:
		case Class::Magician:
		case Class::Enchanter:
			return 0.35f;
		default:
			return 1.0f;
	}
}

float CalculateManaGearScore(uint8 class_id) {
	switch (class_id) {
		case Class::Cleric:
		case Class::Druid:
		case Class::Shaman:
		case Class::Necromancer:
		case Class::Wizard:
		case Class::Magician:
		case Class::Enchanter:
			return 1.0f;
		case Class::Paladin:
		case Class::ShadowKnight:
		case Class::Beastlord:
		case Class::Ranger:
			return 0.50f;
		case Class::Bard:
			return 0.15f;
		case Class::Berserker:
		case Class::Monk:
		case Class::Rogue:
		case Class::Warrior:
			return 0.0f;
		default:
			return 1.0f;
	}
}

float CalculateEnduranceGearScore(uint8 class_id) {
	switch (class_id) {
		case Class::Berserker:
			return 1.0f;
		case Class::Paladin:
		case Class::ShadowKnight:
		case Class::Beastlord:
		case Class::Ranger:
		case Class::Monk:
		case Class::Rogue:
		case Class::Warrior:
		case Class::Bard:
			return 0.1f;
		case Class::Cleric:
		case Class::Druid:
		case Class::Shaman:
		case Class::Necromancer:
		case Class::Wizard:
		case Class::Magician:
		case Class::Enchanter:
			return 0.0f;
		default:
			return 1.0f;
	}
}

float CalculateHPRegenGearScore(uint8 class_id) {
	return 25.0f;
}

float CalculateManaRegenGearScore(uint8 class_id) {
	return IsNonSpellFighterClass(class_id) ? 0.0f : 25.0f;
}

float CalculateEndRegenGearScore(uint8 class_id) {
	return class_id == Class::Berserker ? 25.0f : IsFighterClass(class_id) ? 5.0f : 1.0f;
}

float CalculateWeaponGearScore(const EQ::ItemData* item, uint8 class_id) {
	if (!item) {
		LogTestDebug("Returning due to !item"); //deleteme
		return 0.0f;
	}

	float gear_score = 0.0f;

	if (item->Damage > 0 && item->Delay > 0) {
		gear_score += item->Damage * CalculateDamageGearScore(item, class_id);
		gear_score += item->Delay * CalculateDelayGearScore(item, class_id);
		gear_score += (static_cast<float>(item->Damage) / item->Delay) * CalculateRatioGearScore(item, class_id);
		gear_score *= item->Damage;

		switch (item->ItemType) {
			case EQ::item::ItemType2HSlash:
			case EQ::item::ItemType2HBlunt:
			case EQ::item::ItemType2HPiercing:
				gear_score *= 0.75f;

				break;
			case EQ::item::ItemTypeBow:
			case EQ::item::ItemTypeSmallThrowing:
			case EQ::item::ItemTypeLargeThrowing:
				gear_score *= (0.50f + (0.15f * (static_cast<float>(item->Range) / 250)));

				break;
			default:
				break;
		}
	}

	return IsFighterClass(class_id) ? gear_score : (gear_score / 2);
}

float CalculateDamageGearScore(const EQ::ItemData* item, uint8 class_id) {
	if (!item) {
		LogTestDebug("Returning due to !item"); //deleteme
		return 0.0f;
	}

	//TODO currently unused
	return 0.0f;
}

float CalculateDelayGearScore(const EQ::ItemData* item, uint8 class_id) {
	if (!item) {
		LogTestDebug("Returning due to !item"); //deleteme
		return 0.0f;
	}

	//TODO currently unused
	return 0.0f;
}

float CalculateRatioGearScore(const EQ::ItemData* item, uint8 class_id) {
	if (!item) {
		LogTestDebug("Returning due to !item"); //deleteme
		return 0.0f;
	}

	switch (item->ItemType) {
		case EQ::item::ItemType2HSlash:
		case EQ::item::ItemType2HBlunt:
		case EQ::item::ItemType2HPiercing:
			return 4.0f;
		default:
			return 8.0f;
	}
}

float CalculateArrowGearScore(const EQ::ItemData* item, uint8 class_id) {
	if (!item) {
		LogTestDebug("Returning due to !item"); //deleteme
		return 0.0f;
	}

	if (item->ItemType != EQ::item::ItemTypeArrow) {
		return 0.0f;
	}

	return item->Damage * (10 * (static_cast<float>(item->Range) / 150));
}

float CalculateHasteGearScore(uint8 class_id) {
	return IsFighterClass(class_id) ? 4.0f : 2.0f; // TODO update logic
}

float CalculateAGIGearScore(uint8 class_id) {
	switch (class_id) {
		case Class::Warrior:
		case Class::Paladin:
		case Class::ShadowKnight:
		case Class::Monk:
			return 3.0f;
		case Class::Berserker:
		case Class::Beastlord:
		case Class::Rogue:
			return 2.0f;
		case Class::Ranger:
		case Class::Bard:
			return 1.5f;
		case Class::Cleric:
		case Class::Druid:
		case Class::Shaman:
		case Class::Necromancer:
		case Class::Wizard:
		case Class::Magician:
		case Class::Enchanter:
			return 1.0f;
		default:
			return 3.0f;
	}
}

float CalculateCHAGearScore(uint8 class_id) {
	switch (class_id) {
		case Class::Bard:
		case Class::Enchanter:
			return 3.0f;
		case Class::Druid:
		case Class::Shaman:
		case Class::Necromancer:
		case Class::Wizard:
		case Class::Magician:
			return 2.0f;
		case Class::Cleric:
		case Class::Beastlord:
		case Class::Ranger:
			return 1.5f;
		case Class::Paladin:
		case Class::ShadowKnight:
			return 1.00f;
		case Class::Warrior:
		case Class::Monk:
		case Class::Berserker:
		case Class::Rogue:
			return 0.50f;
		default:
			return 3.0f;
	}
}

float CalculateDEXGearScore(uint8 class_id) {
	switch (class_id) {
		case Class::Ranger:
			return 3.0f;
		case Class::Rogue:
			return 2.5f;
		case Class::Monk:
		case Class::Berserker:
		case Class::Beastlord:
		case Class::Paladin:
		case Class::ShadowKnight:
		case Class::Warrior:
		case Class::Bard:
			return 2.0f;
		case Class::Cleric:
			return 1.5f;
		case Class::Druid:
		case Class::Shaman:
		case Class::Necromancer:
		case Class::Wizard:
		case Class::Magician:
		case Class::Enchanter:
			return 1.0f;
		default:
			return 3.0f;
	}
}

float CalculateINTGearScore(uint8 class_id) {
	if (IsINTCasterClass(class_id)) {
		return 3.0f;
	}
	else {
		return 1.0f;
	}
}

float CalculateSTAGearScore(uint8 class_id) {
	switch (class_id) {
		case Class::Warrior:
		case Class::Paladin:
		case Class::ShadowKnight:
			return 3.0f;
		case Class::Berserker:
		case Class::Monk:
			return 2.0f;
		case Class::Beastlord:
		case Class::Ranger:
		case Class::Rogue:
		case Class::Cleric:
		case Class::Bard:
			return 1.5f;
		case Class::Druid:
		case Class::Shaman:
		case Class::Necromancer:
		case Class::Wizard:
		case Class::Magician:
		case Class::Enchanter:
			return 1.0f;
		default:
			return 3.0f;
	}
}

float CalculateSTRGearScore(uint8 class_id) {
	switch (class_id) {
		case Class::Warrior:
		case Class::Paladin:
		case Class::ShadowKnight:
		case Class::Berserker:
		case Class::Monk:
		case Class::Beastlord:
		case Class::Rogue:
			return 3.0f;
		case Class::Cleric:
			return 2.0f;
		case Class::Bard:
			return 1.5f;
		case Class::Ranger: // Wait for parse on dex vs str for this
		case Class::Druid:
		case Class::Shaman:
		case Class::Necromancer:
		case Class::Wizard:
		case Class::Magician:
		case Class::Enchanter:
			return 1.0f;
		default:
			return 3.0f;
	}
}

float CalculateWISGearScore(uint8 class_id) {
	if (IsWISCasterClass(class_id)) {
		return 3.0f;
	}
	else {
		return 1.0f;
	}
}

float CalculateCRGearScore(uint8 class_id) {
	return 3.0f;
}

float CalculateDRGearScore(uint8 class_id) {
	return 3.0f;
}

float CalculateFRGearScore(uint8 class_id) {
	return 3.0f;
}

float CalculateMRGearScore(uint8 class_id) {
	return 3.0f;
}

float CalculatePRGearScore(uint8 class_id) {
	return 3.0f;
}

float CalculateCorruptionGearScore(uint8 class_id) {
	return 6.0f;
}

float CalculateHeroicAGIGearScore(uint8 class_id) {
	switch (class_id) {
		case Class::Warrior:
		case Class::Paladin:
		case Class::ShadowKnight:
		case Class::Monk:
			return 10.0f;
		case Class::Berserker:
		case Class::Beastlord:
		case Class::Rogue:
			return 7.5f;
		case Class::Ranger:
		case Class::Bard:
			return 5.0f;
		case Class::Cleric:
		case Class::Druid:
		case Class::Shaman:
		case Class::Necromancer:
		case Class::Wizard:
		case Class::Magician:
		case Class::Enchanter:
			return 3.0f;
		default:
			return 10.0f;
	}
}

float CalculateHeroicCHAGearScore(uint8 class_id) {
	return 2.5f;
}

float CalculateHeroicDEXGearScore(uint8 class_id) {
	switch (class_id) {
		case Class::Ranger:
			return 10.0f;
		case Class::Rogue:
			return 8.0f;
		case Class::Monk:
		case Class::Berserker:
		case Class::Beastlord:
		case Class::Paladin:
		case Class::ShadowKnight:
		case Class::Warrior:
		case Class::Bard:
			return 5.0f;
		case Class::Cleric:
		case Class::Druid:
		case Class::Shaman:
		case Class::Necromancer:
		case Class::Wizard:
		case Class::Magician:
		case Class::Enchanter:
			return 1.0f;
		default:
			return 10.0f;
	}
}

float CalculateHeroicINTGearScore(uint8 class_id) {
	switch (class_id) {
		case Class::Necromancer:
		case Class::Wizard:
		case Class::Magician:
		case Class::Enchanter:
			return 10.0f;
		case Class::Ranger:
		case Class::Beastlord:
		case Class::Paladin:
		case Class::ShadowKnight:
		case Class::Bard:
		case Class::Cleric:
		case Class::Druid:
		case Class::Shaman:
			return 3.0f;
		case Class::Warrior:
		case Class::Rogue:
		case Class::Monk:
		case Class::Berserker:
			return 0.0f;
		default:
			return 10.0f;
	}
}

float CalculateHeroicSTAGearScore(uint8 class_id) {
	switch (class_id) {
		case Class::Warrior:
		case Class::Paladin:
		case Class::ShadowKnight:
			return 10.0f;
		case Class::Berserker:
		case Class::Monk:
			return 8.0f;
		case Class::Beastlord:
		case Class::Ranger:
		case Class::Rogue:
		case Class::Cleric:
		case Class::Bard:
			return 6.0f;
		case Class::Druid:
		case Class::Shaman:
		case Class::Necromancer:
		case Class::Wizard:
		case Class::Magician:
		case Class::Enchanter:
			return 4.0f;
		default:
			return 10.0f;
	}
}

float CalculateHeroicSTRGearScore(uint8 class_id) {
	switch (class_id) {
		case Class::Warrior:
		case Class::Paladin:
		case Class::ShadowKnight:
		case Class::Berserker:
		case Class::Monk:
		case Class::Beastlord:
		case Class::Rogue:
			return 10.0f;
		case Class::Cleric:
			return 7.0f;
		case Class::Bard:
			return 5.0f;
		case Class::Ranger: // Wait for parse on dex vs str for this
		case Class::Druid:
		case Class::Shaman:
		case Class::Necromancer:
		case Class::Wizard:
		case Class::Magician:
		case Class::Enchanter:
			return 2.0f;
		default:
			return 10.0f;
	}
}

float CalculateHeroicWISGearScore(uint8 class_id) {
	switch (class_id) {
		case Class::Cleric:
		case Class::Druid:
		case Class::Shaman:
			return 10.0f;
		case Class::Ranger:
		case Class::Beastlord:
		case Class::Paladin:
			return 5.0f;
		case Class::Bard:
			return 3.0f;
		case Class::ShadowKnight:
		case Class::Necromancer:
			return 2.0f;
		case Class::Magician:
			return 1.0f;
		case Class::Wizard:
		case Class::Enchanter:
		case Class::Warrior:
		case Class::Rogue:
		case Class::Monk:
		case Class::Berserker:
			return 0.0f;
		default:
			return 10.0f;
	}
}

float CalculateHeroicCRGearScore(uint8 class_id) {
	return 10.0f;
}

float CalculateHeroicDRGearScore(uint8 class_id) {
	return 10.0f;
}

float CalculateHeroicFRGearScore(uint8 class_id) {
	return 10.0f;
}

float CalculateHeroicMRGearScore(uint8 class_id) {
	return 10.0f;
}

float CalculateHeroicPRGearScore(uint8 class_id) {
	return 10.0f;
}

float CalculateHeroicCorruptionGearScore(uint8 class_id) {
	return 20.0f;
}

float CalculateAccuracyGearScore(uint8 class_id) {
	return IsFighterClass(class_id) ? 10.0f : 3.0f;
}

float CalculateAttackGearScore(uint8 class_id) {
	return IsFighterClass(class_id) ? 10.0f : 3.0f;
}

float CalculateAvoidanceGearScore(uint8 class_id) {
	switch (class_id) {
		case Class::Warrior:
		case Class::Paladin:
		case Class::ShadowKnight:
		case Class::Monk:
			return 10.0f;
		case Class::Berserker:
		case Class::Beastlord:
		case Class::Rogue:
			return 7.0f;
		case Class::Ranger:
		case Class::Bard:
			return 5.5f;
		case Class::Cleric:
		case Class::Druid:
		case Class::Shaman:
		case Class::Necromancer:
		case Class::Wizard:
		case Class::Magician:
		case Class::Enchanter:
			return 3.0f;
		default:
			return 10.0f;
	}
}

float CalculateCombatEffectsGearScore(uint8 class_id) {
	return IsFighterClass(class_id) ? 10.0f : 3.0f;
}

float CalculateDamageShieldGearScore(uint8 class_id) {
	return 10.0f;
}

float CalculateDoTShieldingGearScore(uint8 class_id) {
	return 10.0f;
}

float CalculateShieldingGearScore(uint8 class_id) {
	return IsFighterClass(class_id) ? 10.0f : 3.0f;
}

float CalculateSpellShieldGearScore(uint8 class_id) {
	return 10.0f;
}

float CalculateStrikethroughGearScore(uint8 class_id) {
	return IsFighterClass(class_id) ? 10.0f : 3.0f;
}

float CalculateStunResistGearScore(uint8 class_id) {
	switch (class_id) {
		case Class::Warrior:
		case Class::Paladin:
		case Class::ShadowKnight:
			return 10.0f;
		case Class::Monk:
		case Class::Berserker:
		case Class::Beastlord:
		case Class::Rogue:
		case Class::Ranger:
			return 7.0f;
		case Class::Bard:
			return 5.0f;
		case Class::Cleric:
		case Class::Druid:
		case Class::Shaman:
		case Class::Necromancer:
		case Class::Wizard:
		case Class::Magician:
		case Class::Enchanter:
			return 3.0f;
		default:
			return 10.0f;
	}
}

float CalculateDSMitigationGearScore(uint8 class_id) {
	return IsFighterClass(class_id) ? 10.0f : 3.0f;
}

float CalculateHealAmountGearScore(uint8 class_id) {
	switch (class_id) {
		case Class::Cleric:
			return 10.0f;
		case Class::Druid:
		case Class::Shaman:
			return 7.0f;
		case Class::Paladin:
		case Class::Beastlord:
		case Class::Ranger:
			return 4.0f;
		case Class::Bard:
		case Class::ShadowKnight:
		case Class::Magician:
		case Class::Necromancer:
			return 2.0f;
		case Class::Warrior:
		case Class::Monk:
		case Class::Rogue:
		case Class::Berserker:
		case Class::Wizard:
		case Class::Enchanter:
			return 0.0f;
		default:
			return 10.0f;
	}
}

float CalculateSpellDamageGearScore(uint8 class_id) {
	return IsCasterClass(class_id) ? 10.0f : 3.0f;
}

float CalculateClairvoyanceGearScore(uint8 class_id) {
	return IsNonSpellFighterClass(class_id) ? 0.0f : 10.0f;
}

float CalculateBackstabDamageGearScore(uint8 class_id) {
	return class_id == Class::Rogue ? 10.0f : 0.0f;
}

float CalculateRecommendedLevelGearScore(uint8 class_id) {
	//TODO currently unused
	//CalcRecommendedLevelBonus
	return 0.0f;
}

float CalculateRequiredLevelGearScore(uint8 class_id) {
	//TODO currently unused
	return 0.0f;
}

float CalculateRecommendedSkillGearScore(uint8 class_id) {
	//TODO currently unused
	return 0.0f;
}

float CalculateSkillModGearScore(const EQ::ItemData* item, uint8 class_id) {
	if (!item) {
		LogTestDebug("Returning due to !item"); //deleteme
		return 0.0f;
	}

	auto skill_mod_value = item->SkillModValue;
	auto skill_mod_max = item->SkillModMax; //TODO currently unused
	float gear_score = 0.0f;

	switch (item->SkillModType) {
		case EQ::skills::SkillHandtoHand:
		case EQ::skills::Skill1HBlunt:
		case EQ::skills::Skill1HSlashing:
		case EQ::skills::Skill1HPiercing:
		case EQ::skills::Skill2HBlunt:
		case EQ::skills::Skill2HSlashing:
		case EQ::skills::Skill2HPiercing:
		case EQ::skills::SkillArchery:
			return gear_score += skill_mod_value * 20.0f;
		case EQ::skills::SkillOffense:
		case EQ::skills::SkillDoubleAttack:
		case EQ::skills::SkillTripleAttack:
		case EQ::skills::SkillDualWield:
			return gear_score += skill_mod_value * 20.0f;
		case EQ::skills::SkillTaunt:
		case EQ::skills::SkillBash:
			return gear_score += skill_mod_value * 20.0f;
		case EQ::skills::SkillDefense:
		case EQ::skills::SkillDodge:
		case EQ::skills::SkillBlock:
		case EQ::skills::SkillParry:
		case EQ::skills::SkillRiposte:
		case EQ::skills::SkillFeignDeath:
		case EQ::skills::SkillMend:
			return gear_score += skill_mod_value * 20.0f;
		case EQ::skills::SkillSinging:
		case EQ::skills::SkillBrassInstruments:
		case EQ::skills::SkillStringedInstruments:
		case EQ::skills::SkillWindInstruments:
		case EQ::skills::SkillPercussionInstruments:
			return gear_score += skill_mod_value * 20.0f;
		case EQ::skills::SkillAbjuration:
		case EQ::skills::SkillAlteration:
		case EQ::skills::SkillDivination:
		case EQ::skills::SkillConjuration:
		case EQ::skills::SkillEvocation:
		case EQ::skills::SkillChanneling:
		case EQ::skills::SkillSpecializeAbjure:
		case EQ::skills::SkillSpecializeAlteration:
		case EQ::skills::SkillSpecializeConjuration:
		case EQ::skills::SkillSpecializeDivination:
		case EQ::skills::SkillSpecializeEvocation:
			return gear_score += skill_mod_value * 20.0f;
		case EQ::skills::SkillMakePoison:
		case EQ::skills::SkillTinkering:
		case EQ::skills::SkillResearch:
		case EQ::skills::SkillAlchemy:
		case EQ::skills::SkillBaking:
		case EQ::skills::SkillTailoring:
		case EQ::skills::SkillBlacksmithing:
		case EQ::skills::SkillFletching:
		case EQ::skills::SkillBrewing:
		case EQ::skills::SkillJewelryMaking:
		case EQ::skills::SkillPottery:
			return gear_score += skill_mod_value * 20.0f;
		case EQ::skills::SkillThrowing:
		case EQ::skills::SkillBackstab:
			return gear_score += skill_mod_value * 15.0f;
		case EQ::skills::SkillKick:
		case EQ::skills::SkillRoundKick:
		case EQ::skills::SkillDragonPunch: // also SkillTailRake
		case EQ::skills::SkillEagleStrike:
		case EQ::skills::SkillFlyingKick:
		case EQ::skills::SkillTigerClaw:
		case EQ::skills::SkillBerserking:
		case EQ::skills::SkillFrenzy:
		case EQ::skills::SkillMeditate:
			return gear_score += skill_mod_value * 10.0f;
		case EQ::skills::SkillIntimidation:
		case EQ::skills::SkillDisarm:
		case EQ::skills::SkillDisarmTraps:
		case EQ::skills::SkillBindWound:
			return gear_score += skill_mod_value * 10.0f;
		case EQ::skills::SkillSafeFall:
		case EQ::skills::SkillPickLock:
		case EQ::skills::SkillForage:
		case EQ::skills::SkillSneak:
		case EQ::skills::SkillHide:
		case EQ::skills::SkillTracking:
		case EQ::skills::SkillBegging:
		case EQ::skills::SkillPickPockets:
		case EQ::skills::SkillSenseTraps:
		case EQ::skills::SkillRemoveTraps:
			return gear_score += skill_mod_value * 5.0f;
		case EQ::skills::SkillApplyPoison:
		case EQ::skills::SkillSenseHeading:
		case EQ::skills::SkillSwimming:
		case EQ::skills::SkillAlcoholTolerance:
		case EQ::skills::SkillFishing:
			return gear_score += skill_mod_value * 2.5f;
		default:
			return 0.0f;
	}
}

float CalculateExtraSkillDmgGearScore(const EQ::ItemData* item, uint8 class_id) {
	if (!item) {
		LogTestDebug("Returning due to !item"); //deleteme
		return 0.0f;
	}

	auto skill_mod_value = item->ExtraDmgAmt;
	float gear_score = 0.0f;

	switch (item->ExtraDmgSkill) {
		case EQ::skills::SkillHandtoHand:
		case EQ::skills::Skill1HBlunt:
		case EQ::skills::Skill1HSlashing:
		case EQ::skills::Skill1HPiercing:
		case EQ::skills::Skill2HBlunt:
		case EQ::skills::Skill2HSlashing:
		case EQ::skills::Skill2HPiercing:
		case EQ::skills::SkillArchery:
			return gear_score += skill_mod_value * 5.0f;
		case EQ::skills::SkillOffense:
		case EQ::skills::SkillDoubleAttack:
		case EQ::skills::SkillTripleAttack:
		case EQ::skills::SkillDualWield:
			return gear_score += skill_mod_value * 5.0f;
		case EQ::skills::SkillTaunt:
		case EQ::skills::SkillBash:
			return gear_score += skill_mod_value * 5.0f;
		case EQ::skills::SkillDefense:
		case EQ::skills::SkillDodge:
		case EQ::skills::SkillBlock:
		case EQ::skills::SkillParry:
		case EQ::skills::SkillRiposte:
		case EQ::skills::SkillFeignDeath:
		case EQ::skills::SkillMend:
			return gear_score += skill_mod_value * 5.0f;
		case EQ::skills::SkillSinging:
		case EQ::skills::SkillBrassInstruments:
		case EQ::skills::SkillStringedInstruments:
		case EQ::skills::SkillWindInstruments:
		case EQ::skills::SkillPercussionInstruments:
			return gear_score += skill_mod_value * 5.0f;
		case EQ::skills::SkillAbjuration:
		case EQ::skills::SkillAlteration:
		case EQ::skills::SkillDivination:
		case EQ::skills::SkillConjuration:
		case EQ::skills::SkillEvocation:
		case EQ::skills::SkillChanneling:
		case EQ::skills::SkillSpecializeAbjure:
		case EQ::skills::SkillSpecializeAlteration:
		case EQ::skills::SkillSpecializeConjuration:
		case EQ::skills::SkillSpecializeDivination:
		case EQ::skills::SkillSpecializeEvocation:
			return gear_score += skill_mod_value * 5.0f;
		case EQ::skills::SkillMakePoison:
		case EQ::skills::SkillTinkering:
		case EQ::skills::SkillResearch:
		case EQ::skills::SkillAlchemy:
		case EQ::skills::SkillBaking:
		case EQ::skills::SkillTailoring:
		case EQ::skills::SkillBlacksmithing:
		case EQ::skills::SkillFletching:
		case EQ::skills::SkillBrewing:
		case EQ::skills::SkillJewelryMaking:
		case EQ::skills::SkillPottery:
			return gear_score += skill_mod_value * 5.0f;
		case EQ::skills::SkillThrowing:
		case EQ::skills::SkillBackstab:
			return gear_score += skill_mod_value * 4.0f;
		case EQ::skills::SkillKick:
		case EQ::skills::SkillRoundKick:
		case EQ::skills::SkillDragonPunch: // also SkillTailRake
		case EQ::skills::SkillEagleStrike:
		case EQ::skills::SkillFlyingKick:
		case EQ::skills::SkillTigerClaw:
		case EQ::skills::SkillBerserking:
		case EQ::skills::SkillFrenzy:
		case EQ::skills::SkillMeditate:
			return gear_score += skill_mod_value * 3.0f;
		case EQ::skills::SkillIntimidation:
		case EQ::skills::SkillDisarm:
		case EQ::skills::SkillDisarmTraps:
		case EQ::skills::SkillBindWound:
			return gear_score += skill_mod_value * 3.0f;
		case EQ::skills::SkillSafeFall:
		case EQ::skills::SkillPickLock:
		case EQ::skills::SkillForage:
		case EQ::skills::SkillSneak:
		case EQ::skills::SkillHide:
		case EQ::skills::SkillTracking:
		case EQ::skills::SkillBegging:
		case EQ::skills::SkillPickPockets:
		case EQ::skills::SkillSenseTraps:
		case EQ::skills::SkillRemoveTraps:
			return gear_score += skill_mod_value * 2.0f;
		case EQ::skills::SkillApplyPoison:
		case EQ::skills::SkillSenseHeading:
		case EQ::skills::SkillSwimming:
		case EQ::skills::SkillAlcoholTolerance:
		case EQ::skills::SkillFishing:
			return gear_score += skill_mod_value * 1.0f;
		default:
			return 0.0f;
	}
}

float CalculateBaneDmgGearScore(const EQ::ItemData* item, uint8 class_id) {
	if (!item) {
		LogTestDebug("Returning due to !item"); //deleteme
		return 0.0f;
	}

	auto skill_mod_value = item->ExtraDmgAmt;
	float gear_score = 0.0f;

	if (item->BaneDmgAmt > 0 && item->BaneDmgBody > 0) {
		gear_score += item->BaneDmgAmt * 4;
	}

	if (item->BaneDmgRaceAmt > 0 && item->BaneDmgRace > 0) {
		gear_score += item->BaneDmgAmt * 4;
	}

	switch (item->ItemType) {
		case EQ::item::ItemType2HSlash:
		case EQ::item::ItemType2HBlunt:
		case EQ::item::ItemType2HPiercing:
			gear_score *= 0.75f;

			break;
		case EQ::item::ItemTypeBow:
			gear_score *= (0.50f + (0.15f * (static_cast<float>(item->Range) / 250)));

			break;
		default:
			break;
	}

	return gear_score;
}

float CalculateElementalDmgGearScore(uint8 class_id) {
	return 4.0f;
}