/*	EQEMu: Everquest Server Emulator
	Copyright (C) 2001-2016 EQEMu Development Team (http://eqemulator.org)

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
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "bot.h"
#include "../common/data_verification.h"
#include "../common/repositories/bot_spells_entries_repository.h"
#include "../common/repositories/npc_spells_repository.h"

bool Bot::AICastSpell(Mob* tar, uint8 iChance, uint32 iSpellTypes) {

	// Bot AI
	Raid* raid = entity_list.GetRaidByBotName(GetName());

	if (!tar) {
		return false;
	}

	if (tar->IsFamiliar()) {
		return false;
	}

	if (!AI_HasSpells()) {
		return false;
	}

	if (iChance < 100 && zone->random.Int(0, 100) > iChance) {
		return false;
	}

	if (tar->GetAppearance() == eaDead && ((tar->IsClient() && !tar->CastToClient()->GetFeigned()) || tar->IsBot())) {
		return false;
	}

	LogBotPreChecks("{} says, 'AISpellCast {} checks on {}.'", GetCleanName(), GetSpellTypeNameByID(GetControlledBotSpellType(iSpellTypes)), tar->GetCleanName()); //deleteme

	if (GetSpellHold(GetControlledBotSpellType(iSpellTypes))) {
		LogBotHoldChecks("{} says, 'Cancelling {} on {} due to GetSpellHold.'", GetCleanName(), GetSpellTypeNameByID(GetControlledBotSpellType(iSpellTypes)), tar->GetCleanName()); //deleteme
		return false;
	}

	if (!DelayChecks(iSpellTypes, 0, tar, true)) {
		LogBotDelayChecks("{} says, 'Cancelling {} on {} due to DelayChecks.'", GetCleanName(), GetSpellTypeNameByID(GetControlledBotSpellType(iSpellTypes)), tar->GetCleanName()); //deleteme
		return false;
	}

	if (!ThresholdChecks(iSpellTypes, 0, tar, true)) {
		LogBotThresholdChecks("{} says, 'Cancelling {} on {} due to ThresholdChecks.'", GetCleanName(), GetSpellTypeNameByID(GetControlledBotSpellType(iSpellTypes)), tar->GetCleanName()); //deleteme
		return false;
	}

	uint8 botClass = GetClass();
	uint8 botLevel = GetLevel();

	bool checked_los = false;	//we do not check LOS until we are absolutely sure we need to, and we only do it once.

	BotSpell botSpell;
	botSpell.SpellId = 0;
	botSpell.SpellIndex = 0;
	botSpell.ManaCost = 0;

	switch (iSpellTypes) {
		case SpellType_Mez:
			return BotCastMez(tar, botLevel, checked_los, botSpell, raid);
		case SpellType_Heal:
			return BotCastHeal(tar, botLevel, botClass, botSpell, raid);
		case SpellType_Root:
			return BotCastRoot(tar, botLevel, iSpellTypes, botSpell, checked_los);
		case SpellType_Buff:
			return BotCastBuff(tar, botLevel, botClass);
		case SpellType_Escape:
			return BotCastEscape(tar, botClass, botSpell, iSpellTypes);
		case SpellType_Nuke:
			return BotCastNuke(tar, botLevel, botClass, botSpell, checked_los);
		case SpellType_Dispel:
			return BotCastDispel(tar, botSpell, iSpellTypes, checked_los);
		case SpellType_Pet:
			return BotCastPet(tar, botClass, botSpell);
		case SpellType_InCombatBuff:
			return BotCastCombatBuff(tar, botLevel, botClass);
		case SpellType_Lifetap:
			return BotCastLifetap(tar, botLevel, botSpell, checked_los, iSpellTypes);
		case SpellType_Snare:
			return BotCastSnare(tar, botLevel, botSpell, checked_los, iSpellTypes);
		case SpellType_DOT:
			return BotCastDOT(tar, botLevel, botSpell, checked_los);
		case SpellType_Slow:
			return BotCastSlow(tar, botLevel, botClass, botSpell, checked_los, raid);
		case SpellType_Debuff:
			return BotCastDebuff(tar, botLevel, botSpell, checked_los);
		case SpellType_Cure:
			return BotCastCure(tar, botClass, botSpell, raid);
		case SpellType_HateRedux:
			return BotCastHateReduction(tar, botLevel, botSpell);
		case SpellType_InCombatBuffSong:
			return BotCastCombatSong(tar, botLevel);
		case SpellType_OutOfCombatBuffSong:
			return BotCastSong(tar, botLevel);
		case SpellType_Resurrect:
		case SpellType_PreCombatBuff:
		case SpellType_PreCombatBuffSong:
		default:
			return false;
	}

	return false;
}

bool Bot::BotCastSong(Mob* tar, uint8 botLevel) {
	bool casted_spell = false;

	if (GetClass() != Class::Bard || tar != this || IsEngaged()) { // Out-of-Combat songs can not be cast in combat
		return false;
	}

	std::list<BotSpell_wPriority> botSpellList = GetPrioritizedBotSpellsBySpellType(this, SpellType_OutOfCombatBuffSong);

	for (const auto& s : botSpellList) {
		if (!PrecastChecks(s.SpellId, tar, SpellType_OutOfCombatBuffSong)) {
			continue;
		}

		if (AIDoSpellCast(s.SpellIndex, tar, s.ManaCost)) {
			//BotGroupSay(this, "Singing [OOC] song [%s].", GetSpellName(s.SpellId)); //Bards don't need to spam song messages.
			
			return true;
		}
	}

	return casted_spell;
}

bool Bot::BotCastCombatSong(Mob* tar, uint8 botLevel) {
	bool casted_spell = false;

	if (tar != this) { // In-Combat songs can be cast Out-of-Combat in preparation for battle
		return false;
	}
	
	std::list<BotSpell_wPriority> botSpellList = GetPrioritizedBotSpellsBySpellType(this, SpellType_InCombatBuffSong);

	for (const auto& s : botSpellList) {
		if (!PrecastChecks(s.SpellId, tar, SpellType_InCombatBuffSong)) {
			continue;
		}

		if (AIDoSpellCast(s.SpellIndex, tar, s.ManaCost)) {
			//BotGroupSay(this, "Singing [IC] song [%s].", GetSpellName(s.SpellId)); //Bards don't need to spam song messages.
			
			return true;
		}
	}

	return casted_spell;
}

bool Bot::BotCastHateReduction(Mob* tar, uint8 botLevel, const BotSpell& botSpell) {
	bool casted_spell = false;

	std::list<BotSpell_wPriority> botSpellList = GetPrioritizedBotSpellsBySpellType(this, SpellType_HateRedux);

	if (GetClass() == Class::Bard) {
		for (const auto& s : botSpellList) {
			if (spells[s.SpellId].target_type != ST_Target) {
				continue;
			}

			if (!PrecastChecks(s.SpellId, tar, SpellType_HateRedux)) {
				continue;
			}

			if (AIDoSpellCast(s.SpellIndex, tar, s.ManaCost)) {
				//BotGroupSay(this, "Attempting to reduce my hate on %s with [%s].", tar->GetCleanName(), GetSpellName(s.SpellId));
				
				return true;
			}
		}
	}
	else {
		for (const auto& s : botSpellList) {
			if (!PrecastChecks(s.SpellId, tar, SpellType_HateRedux)) {
				continue;
			}

			if (AIDoSpellCast(s.SpellIndex, tar, s.ManaCost)) {
				BotGroupSay(this, "Attempting to reduce my hate on %s with [%s].", tar->GetCleanName(), GetSpellName(s.SpellId));
				
				return true;
			}
		}
	}

	return casted_spell;
}

bool Bot::BotCastCure(Mob* tar, uint8 botClass, BotSpell& botSpell, Raid* raid) { //TODO with hold/delay/threshold implementations
	bool casted_spell = false;

	if (
		GetNeedsCured(tar) &&
		(tar->DontCureMeBefore() < Timer::GetCurrentTime()) &&
		GetNumberNeedingHealedInGroup(25, false, raid) <= 0 &&
		GetNumberNeedingHealedInGroup(40, false, raid) <= 2
	) {
		botSpell = GetBestBotSpellForCure(this, tar);

		if (!IsValidSpell(botSpell.SpellId)) {
			return false;
		}

		uint32 TempDontCureMeBeforeTime = tar->DontCureMeBefore();

		if (AIDoSpellCast(botSpell.SpellIndex, tar, botSpell.ManaCost, &TempDontCureMeBeforeTime)) {
			std::string cureType = "cure";

			if (IsGroupSpell(botSpell.SpellId)) {
				cureType = "group cure";
			}

			BotGroupSay(this, "Attempting to %s %s with [%s].", cureType, tar->GetCleanName(), GetSpellName(botSpell.SpellId));
		}

		if (tar != this && tar->IsOfClientBot()) {
			if (tar->IsBot()) {
				tar->m_botSpellType_Cure.Start(tar->CastToBot()->GetSpellDelay(BotSpellTypes::Cure));
			} /* TODO with client commands
			else if (tar->IsClient()) {
				tar->m_botSpellType_Cure.Start(tar->CastToClient()->GetSpellDelay(BotSpellTypes::Cure));
			} */
		}
		else {
			m_botSpellType_Cure.Start(GetSpellDelay(BotSpellTypes::Cure));
		}
	}

	return casted_spell;
}

bool Bot::BotCastDebuff(Mob* tar, uint8 botLevel, BotSpell& botSpell, bool checked_los) {
	bool casted_spell = false;

	if ((tar->GetHPRatio() <= 99.0f) && (tar->GetHPRatio() > 20.0f)) {
		if (!checked_los && (!CheckLosFN(tar) || !CheckWaterLoS(tar))) {
			return false;
		}

		std::list<BotSpell_wPriority> buffSpellList = GetPrioritizedBotSpellsBySpellType(this, SpellType_Debuff);

		for (const auto& s : buffSpellList) {

			if (!PrecastChecks(s.SpellId, tar, SpellType_Debuff)) {
				return false;
			}

			if (AIDoSpellCast(s.SpellIndex, tar, s.ManaCost)) {
				BotGroupSay(this, "Debuffing %s with [%s].", tar->GetCleanName(), GetSpellName(s.SpellId));

				return true;
			}
		}
	}

	return casted_spell;
}

bool Bot::BotCastSlow(Mob* tar, uint8 botLevel, uint8 botClass, BotSpell& botSpell, const bool& checked_los, Raid* raid) {
	bool casted_spell = false;
	if (tar->GetHPRatio() <= 99.0f) {

		if (!checked_los && (!CheckLosFN(tar) || !CheckWaterLoS(tar))) {
			return false;
		}


		switch (botClass) {
			case Class::Bard: {
				std::list<BotSpell_wPriority> botSpellList = GetPrioritizedBotSpellsBySpellType(this, SpellType_Slow);

				for (const auto& s : botSpellList) {
					if (spells[s.SpellId].target_type != ST_Target) {
						continue;
					}

					if (!PrecastChecks(s.SpellId, tar, SpellType_Slow)) {
						continue;
					}

					if (AIDoSpellCast(s.SpellIndex, tar, s.ManaCost)) {
						//BotGroupSay(this, "Attempting to slow %s with [%s].", tar->GetCleanName(), GetSpellName(botSpell.SpellId)); //Bards don't need to spam song messages.
						return true;
					}
				}

				break;
			}
			case Class::Enchanter: {
				botSpell = GetBestBotSpellForMagicBasedSlow(this);

				break;
			}
			case Class::Shaman:
			case Class::Beastlord: {
				botSpell = GetBestBotSpellForDiseaseBasedSlow(this);

				if (botSpell.SpellId == 0 || ((tar->GetMR() - 50) < (tar->GetDR() + spells[botSpell.SpellId].resist_difficulty))) {
					botSpell = GetBestBotSpellForMagicBasedSlow(this);
				}

				break;
			}
		}

		if (!PrecastChecks(botSpell.SpellId, tar, SpellType_Slow)) {
			return false;
		}

		if (AIDoSpellCast(botSpell.SpellIndex, tar, botSpell.ManaCost)) {
			if (GetClass() != Class::Bard) {
				BotGroupSay(this, "Attempting to slow %s with [%s].", tar->GetCleanName(), GetSpellName(botSpell.SpellId));
			}

			return true;
		}
	}

	return casted_spell;
}

bool Bot::BotCastDOT(Mob* tar, uint8 botLevel, const BotSpell& botSpell, const bool& checked_los) {
	bool casted_spell = false;

	if ((tar->GetHPRatio() <= 98.0f) && (tar->DontDotMeBefore() < Timer::GetCurrentTime()) && (tar->GetHPRatio() > 15.0f)) {
		if (!checked_los && (!CheckLosFN(tar) || !CheckWaterLoS(tar))) {
			return false;
		}

		std::list<BotSpell_wPriority> botSpellList = GetPrioritizedBotSpellsBySpellType(this, SpellType_DOT);

		for (const auto& s : botSpellList) {
			if (!PrecastChecks(s.SpellId, tar, SpellType_DOT)) {
				continue;
			}

			uint32 TempDontDotMeBefore = tar->DontDotMeBefore();

			if (AIDoSpellCast(s.SpellIndex, tar, s.ManaCost, &TempDontDotMeBefore)) {
				casted_spell = true;
				BotGroupSay(this, "Casting a DoT on %s with [%s].", tar->GetCleanName(), GetSpellName(s.SpellId));
			}

			if (TempDontDotMeBefore != tar->DontDotMeBefore()) {
				tar->SetDontDotMeBefore(TempDontDotMeBefore);
			}

			if (casted_spell) {
				return casted_spell;
			}
		}
	}

	return casted_spell;
}

bool Bot::BotCastSnare(Mob* tar, uint8 botLevel, BotSpell& botSpell, const bool& checked_los, uint32 iSpellTypes) {
	bool casted_spell = false;

	if (tar->DontSnareMeBefore() < Timer::GetCurrentTime()) {
		if (!checked_los && (!CheckLosFN(tar) || !CheckWaterLoS(tar))) {
			return false;
		}

		std::list<BotSpell_wPriority> botSpellList = GetPrioritizedBotSpellsBySpellType(this, SpellType_Snare);

		for (const auto& s : botSpellList) {
			if (!PrecastChecks(s.SpellId, tar, SpellType_Snare)) {
				continue;
			}

			uint32 TempDontSnareMeBefore = tar->DontSnareMeBefore();

			if (AIDoSpellCast(s.SpellIndex, tar, s.ManaCost, &TempDontSnareMeBefore)) {
				casted_spell = true;
				BotGroupSay(this, "Snaring %s with [%s].", tar->GetCleanName(), GetSpellName(s.SpellId));
			}

			if (TempDontSnareMeBefore != tar->DontSnareMeBefore()) {
				tar->SetDontSnareMeBefore(TempDontSnareMeBefore);
			}

			if (casted_spell) {
				return casted_spell;
			}
		}
	}

	return casted_spell;
}

bool Bot::BotCastLifetap(Mob* tar, uint8 botLevel, BotSpell& botSpell, const bool& checked_los, uint32 iSpellTypes) {
	bool casted_spell = false;

	if (GetHPRatio() < 90.0f) {
		if (!checked_los && (!CheckLosFN(tar) || !CheckWaterLoS(tar))) {
			return false;
		}

		std::list<BotSpell_wPriority> botSpellList = GetPrioritizedBotSpellsBySpellType(this, SpellType_Lifetap);

		for (const auto& s : botSpellList) {
			if (!PrecastChecks(s.SpellId, tar, SpellType_Lifetap)) {
				continue;
			}

			if (AIDoSpellCast(s.SpellIndex, tar, s.ManaCost)) {
				BotGroupSay(this, "Lifetapping %s with [%s].", tar->GetCleanName(), GetSpellName(s.SpellId));

				return true;
			}
		}
	}

	return casted_spell;
}

bool Bot::BotCastCombatBuff(Mob* tar, uint8 botLevel, uint8 botClass) {
	bool casted_spell = false;

	if (tar->DontBuffMeBefore() < Timer::GetCurrentTime()) {
		std::list<BotSpell_wPriority> buffSpellList = GetPrioritizedBotSpellsBySpellType(this, SpellType_InCombatBuff);

		for (const auto& s : buffSpellList) {
			if (!PrecastChecks(s.SpellId, tar, SpellType_InCombatBuff)) {
				continue;
			}

			uint32 TempDontBuffMeBefore = tar->DontBuffMeBefore();

			if (AIDoSpellCast(s.SpellIndex, tar, s.ManaCost, &TempDontBuffMeBefore)) {
				casted_spell = true;
				BotGroupSay(this, "Using an In-Combat Buff on %s with [%s].", tar->GetCleanName(), GetSpellName(s.SpellId));
			}

			if (TempDontBuffMeBefore != tar->DontBuffMeBefore()) {
				tar->SetDontBuffMeBefore(TempDontBuffMeBefore);
			}

			if (casted_spell) {
				return casted_spell;
			}
		}
	}

	return casted_spell;
}

bool Bot::BotCastPet(Mob* tar, uint8 botClass, BotSpell& botSpell) {
	bool casted_spell = false;

	if (!IsPet() && !GetPetID() && !IsBotCharmer()) {
		if (botClass == Class::Wizard) {
			auto buffs_max = GetMaxBuffSlots();
			auto my_buffs = GetBuffs();
			int familiar_buff_slot = -1;
			if (buffs_max && my_buffs) {
				for (int index = 0; index < buffs_max; ++index) {
					if (IsEffectInSpell(my_buffs[index].spellid, SE_Familiar)) {
						MakePet(my_buffs[index].spellid, spells[my_buffs[index].spellid].teleport_zone);
						familiar_buff_slot = index;
						break;
					}
				}
			}
			if (GetPetID()) {
				return casted_spell;
			}
			if (familiar_buff_slot >= 0) {
				BuffFadeBySlot(familiar_buff_slot);
				return casted_spell;
			}

			botSpell = GetFirstBotSpellBySpellType(this, SpellType_Pet);
		}
		else if (botClass == Class::Magician) {
			botSpell = GetBestBotMagicianPetSpell(this);
		}
		else {
			botSpell = GetFirstBotSpellBySpellType(this, SpellType_Pet);
		}

		if (!IsValidSpell(botSpell.SpellId)) {
			return casted_spell;
		}

		casted_spell = AIDoSpellCast(botSpell.SpellIndex, tar, botSpell.ManaCost);

		if (casted_spell) {
			BotGroupSay(this, "Summoning a pet [%s].", GetSpellName(botSpell.SpellId));

			return true;
		}
	}
	return casted_spell;
}

bool Bot::BotCastDispel(Mob* tar, BotSpell& botSpell, uint32 iSpellTypes, const bool& checked_los) {
	bool casted_spell = false;

	if (tar->GetHPRatio() > 95.0f) {
		if (!checked_los && (!CheckLosFN(tar) || !CheckWaterLoS(tar))) {
			return false;
		}

		if (tar->CountDispellableBuffs() <= 0) {
			return false;
		}

		std::list<BotSpell_wPriority> buffSpellList = GetPrioritizedBotSpellsBySpellType(this, SpellType_Dispel);

		for (const auto& s : buffSpellList) {

			if (!PrecastChecks(s.SpellId, tar, SpellType_Dispel)) {
				return false;
			}

			if (AIDoSpellCast(s.SpellIndex, tar, s.ManaCost)) {
				BotGroupSay(this, "Attempting to dispel %s with [%s].", tar->GetCleanName(), GetSpellName(s.SpellId));

				return true;
			}
		}
	}
	return casted_spell;
}

bool Bot::BotCastNuke(Mob* tar, uint8 botLevel, uint8 botClass, BotSpell& botSpell, const bool& checked_los) {

	bool casted_spell = false;
	uint8 currentHPRatio = tar->GetHPRatio();
	uint8 hpCheck = 0;

	switch (GetClass()) {
	}
	if (hpCheck <= currentHPRatio) {
		if (!checked_los && (!CheckLosFN(tar) || !CheckWaterLoS(tar))) {
			return false;
		}

		uint8 stunChance = (tar->IsCasting() ? RuleI(Bots, StunCastChanceIfCasting) : RuleI(Bots, StunCastChanceNormal));

		if (botClass == Class::Paladin) {
			stunChance = RuleI(Bots, StunCastChancePaladins);
		}

		if (!tar->GetSpecialAbility(UNSTUNABLE) && !tar->IsStunned() && (zone->random.Int(1, 100) <= stunChance)) {
			botSpell = GetBestBotSpellForStunByTargetType(this, ST_Target);
		}

		if (!PrecastChecks(botSpell.SpellId, tar, SpellType_Nuke)) {
			botSpell = GetBestBotSpellForNukeByBodyType(this, tar->GetBodyType());
		}

		if (botClass == Class::Wizard && !PrecastChecks(botSpell.SpellId, tar, SpellType_Nuke)) {
			botSpell = GetBestBotWizardNukeSpellByTargetResists(this, tar);
		}

		if (!PrecastChecks(botSpell.SpellId, tar, SpellType_Nuke)) {
			std::list<BotSpell_wPriority> botSpellList = GetPrioritizedBotSpellsBySpellType(this, SpellType_Nuke);

			for (const auto& s : botSpellList) {
				if (!PrecastChecks(s.SpellId, tar, SpellType_Nuke)) {
					continue;
				}

				botSpell.SpellId = s.SpellId;
				botSpell.SpellIndex = s.SpellIndex;
				botSpell.ManaCost = s.ManaCost;
			}
		}

		if (!PrecastChecks(botSpell.SpellId, tar, SpellType_Nuke)) {
			return false;
		}

		if (AIDoSpellCast(botSpell.SpellIndex, tar, botSpell.ManaCost)) {
			std::string nukeType = ".";
			if (IsPBAENukeSpell(botSpell.SpellId)) {
				nukeType = " [PBAE].";
			}
			else if (IsAERainNukeSpell(botSpell.SpellId)) {
				nukeType = " [AERain].";
			}
			else if (IsAEDurationSpell(botSpell.SpellId)) {
				nukeType = " [AERain].";
			}
			else if (IsAENukeSpell(botSpell.SpellId)) {
				nukeType = " [AE].";
			}

			BotGroupSay(this, "Nuking %s with [%s]%s", tar->GetCleanName(), GetSpellName(botSpell.SpellId), nukeType);

			return true;
		}
	}

	return casted_spell;
}

bool Bot::BotCastEscape(Mob*& tar, uint8 botClass, BotSpell& botSpell, uint32 iSpellTypes) {

	bool casted_spell = false;
	auto hpr = (uint8) GetHPRatio();
	bool mayGetAggro = false;

	if (hpr > 15 && ((botClass == Class::Wizard) || (botClass == Class::Enchanter) || (botClass == Class::Ranger))) {
		mayGetAggro = HasOrMayGetAggro();
	}

	if (hpr <= 15 || mayGetAggro) {
		std::list<BotSpell_wPriority> botSpellList = GetPrioritizedBotSpellsBySpellType(this, SpellType_Escape);

		for (const auto& s : botSpellList) {

			if (IsInvulnerabilitySpell(s.SpellId)) {
				tar = this; //target self for invul type spells
			}

			if (!PrecastChecks(s.SpellId, tar, SpellType_Escape)) {
				continue;
			}

			if (AIDoSpellCast(s.SpellIndex, tar, s.ManaCost)) {
				BotGroupSay(this, "Attempting to escape from %s [%s].", tar->GetCleanName(), GetSpellName(s.SpellId));

				return true;
			}
		}
	}
	return casted_spell;
}

bool Bot::BotCastBuff(Mob* tar, uint8 botLevel, uint8 botClass) {
	bool casted_spell = false;
	if (tar->DontBuffMeBefore() < Timer::GetCurrentTime()) {
		std::list<BotSpell_wPriority> buffSpellList = GetPrioritizedBotSpellsBySpellType(this, SpellType_Buff);

		for (const auto& s : buffSpellList) {
			if (!PrecastChecks(s.SpellId, tar, SpellType_Buff)) {
				continue;
			}

			uint32 TempDontBuffMeBefore = tar->DontBuffMeBefore();

			if (AIDoSpellCast(s.SpellIndex, tar, s.ManaCost, &TempDontBuffMeBefore)) {
				casted_spell = true;
				BotGroupSay(this, "Buffing %s with [%s].", tar->GetCleanName(), GetSpellName(s.SpellId));
			}

			if (TempDontBuffMeBefore != tar->DontBuffMeBefore()) {
				tar->SetDontBuffMeBefore(TempDontBuffMeBefore);
			}

			if (casted_spell) {
				return casted_spell;
			}
		}
	}

	return casted_spell;
}

bool Bot::BotCastRoot(Mob* tar, uint8 botLevel, uint32 iSpellTypes, BotSpell& botSpell, const bool& checked_los) {
	bool casted_spell = false;

	if (!tar->IsRooted() && tar->DontRootMeBefore() < Timer::GetCurrentTime()) {
		if (!checked_los && (!CheckLosFN(tar) || !CheckWaterLoS(tar))) {
			return false;
		}

		std::list<BotSpell_wPriority> botSpellList = GetPrioritizedBotSpellsBySpellType(this, SpellType_Root);

		for (const auto& s : botSpellList) {
			if (!PrecastChecks(s.SpellId, tar, SpellType_Root)) {
				continue;
			}

			uint32 TempDontRootMeBefore = tar->DontRootMeBefore();

			if (AIDoSpellCast(s.SpellIndex, tar, s.ManaCost, &TempDontRootMeBefore)) {
				casted_spell = true;
				BotGroupSay(this, "Attempting to escape from %s [%s].", tar->GetCleanName(), GetSpellName(s.SpellId));
			}

			if (TempDontRootMeBefore != tar->DontRootMeBefore()) {
				tar->SetDontRootMeBefore(TempDontRootMeBefore);
			}

			if (casted_spell) {
				return true;
			}
		}
	}
	return casted_spell;
}

bool Bot::BotCastHeal(Mob* tar, uint8 botLevel, uint8 botClass, BotSpell& botSpell, Raid* raid) { // TODO once commands are implemented
	bool casted_spell = false;

	if (GetSpellHold(BotSpellTypes::Heal)) {
		return casted_spell;
	}

	if ((botClass == Class::Cleric) || (botClass == Class::Druid) || (botClass == Class::Shaman) || (botClass == Class::Paladin) || (botClass == Class::Beastlord) || (botClass == Class::Ranger)) {

		uint8 hpr = tar->GetHPRatio();
		std::string spellType = "None";
		auto targetName = tar->GetCleanName();
		std::string myName = GetCleanName();
		uint8 veryFastHealMinThreshold = GetDefaultSpellMinThreshold(BotSpellTypes::VeryFastHeals);
		uint8 veryFastHealMaxThreshold = GetDefaultSpellMaxThreshold(BotSpellTypes::VeryFastHeals);
		uint8 fastHealMinThreshold = GetDefaultSpellMinThreshold(BotSpellTypes::FastHeals);
		uint8 fastHealMaxThreshold = GetDefaultSpellMaxThreshold(BotSpellTypes::FastHeals);
		uint8 regularHealMinThreshold = GetDefaultSpellMinThreshold(BotSpellTypes::RegularHeals);
		uint8 regularHealMaxThreshold = GetDefaultSpellMaxThreshold(BotSpellTypes::RegularHeals);
		uint8 groupHealMinThreshold = GetDefaultSpellMinThreshold(BotSpellTypes::GroupHeals);
		uint8 groupHealMaxThreshold = GetDefaultSpellMaxThreshold(BotSpellTypes::GroupHeals);
		uint8 completeHealMinThreshold = GetDefaultSpellMinThreshold(BotSpellTypes::CompleteHeals);
		uint8 completeHealMaxThreshold = GetDefaultSpellMaxThreshold(BotSpellTypes::CompleteHeals);
		uint8 groupHoTHealMinThreshold = GetDefaultSpellMinThreshold(BotSpellTypes::GroupHoTHeals);
		uint8 groupHoTHealMaxThreshold = GetDefaultSpellMaxThreshold(BotSpellTypes::GroupHoTHeals);
		uint8 hotHealMinThreshold = GetDefaultSpellMinThreshold(BotSpellTypes::HoTHeals);
		uint8 hotHealMaxThreshold = GetDefaultSpellMaxThreshold(BotSpellTypes::HoTHeals);
		uint32 veryFastHealDelay = GetDefaultSpellDelay(BotSpellTypes::VeryFastHeals);
		uint32 fastHealDelay = GetDefaultSpellDelay(BotSpellTypes::FastHeals);
		uint32 regularHealDelay = GetDefaultSpellDelay(BotSpellTypes::RegularHeals);
		uint32 completeHealDelay = GetDefaultSpellDelay(BotSpellTypes::CompleteHeals);
		uint32 hotHealDelay = GetDefaultSpellDelay(BotSpellTypes::HoTHeals);
		uint32 groupHealDelay = GetDefaultSpellDelay(BotSpellTypes::GroupHeals);
		uint32 groupHoTHealDelay = GetDefaultSpellDelay(BotSpellTypes::GroupHoTHeals);
		uint32 currentTime = Timer::GetCurrentTime();
		uint32 veryFastHealTime = tar->DontVeryFastHealMeBefore();
		uint32 fastHealTime = tar->DontFastHealMeBefore();
		uint32 regularHealTime = tar->DontRegularHealMeBefore();
		uint32 completeHealTime = tar->DontCompleteHealMeBefore();
		uint32 hotHealTime = tar->DontHotHealMeBefore();
		uint32 groupHealTime = tar->DontGroupHealMeBefore();
		uint32 groupHoTHealTime = tar->DontGroupHoTHealMeBefore();

		if (tar->IsBot()) {
			veryFastHealMinThreshold = tar->CastToBot()->GetSpellMinThreshold(BotSpellTypes::VeryFastHeals);
			veryFastHealMaxThreshold = tar->CastToBot()->GetSpellMaxThreshold(BotSpellTypes::VeryFastHeals);
			fastHealMinThreshold = tar->CastToBot()->GetSpellMinThreshold(BotSpellTypes::FastHeals);
			fastHealMaxThreshold = tar->CastToBot()->GetSpellMaxThreshold(BotSpellTypes::FastHeals);
			regularHealMinThreshold = tar->CastToBot()->GetSpellMinThreshold(BotSpellTypes::RegularHeals);
			regularHealMaxThreshold = tar->CastToBot()->GetSpellMaxThreshold(BotSpellTypes::RegularHeals);
			groupHealMinThreshold = tar->CastToBot()->GetSpellMinThreshold(BotSpellTypes::GroupHeals);
			groupHealMaxThreshold = tar->CastToBot()->GetSpellMaxThreshold(BotSpellTypes::GroupHeals);
			groupHoTHealMinThreshold = tar->CastToBot()->GetSpellMinThreshold(BotSpellTypes::GroupHoTHeals);
			groupHoTHealMaxThreshold = tar->CastToBot()->GetSpellMaxThreshold(BotSpellTypes::GroupHoTHeals);
			completeHealMinThreshold = tar->CastToBot()->GetSpellMinThreshold(BotSpellTypes::CompleteHeals);
			completeHealMaxThreshold = tar->CastToBot()->GetSpellMaxThreshold(BotSpellTypes::CompleteHeals);
			hotHealMinThreshold = tar->CastToBot()->GetSpellMinThreshold(BotSpellTypes::HoTHeals);
			hotHealMaxThreshold = tar->CastToBot()->GetSpellMaxThreshold(BotSpellTypes::HoTHeals);
			veryFastHealDelay = tar->CastToBot()->GetSpellDelay(BotSpellTypes::VeryFastHeals);
			fastHealDelay = tar->CastToBot()->GetSpellDelay(BotSpellTypes::FastHeals);
			regularHealDelay = tar->CastToBot()->GetSpellDelay(BotSpellTypes::RegularHeals);
			groupHealDelay = tar->CastToBot()->GetSpellDelay(BotSpellTypes::GroupHeals);
			completeHealDelay = tar->CastToBot()->GetSpellDelay(BotSpellTypes::CompleteHeals);
			hotHealDelay = tar->CastToBot()->GetSpellDelay(BotSpellTypes::HoTHeals);
			groupHoTHealDelay = tar->CastToBot()->GetSpellDelay(BotSpellTypes::GroupHoTHeals);
		}
		/*else if (tar->IsClient()) { // TODO Add client command support
			veryFastHealMinThreshold = tar->CastToClient()->GetSpellMinThreshold(BotSpellTypes::VeryFastHeals);
			veryFastHealMaxThreshold = tar->CastToClient()->GetSpellMaxThreshold(BotSpellTypes::VeryFastHeals);
			fastHealMinThreshold = tar->CastToClient()->GetSpellMinThreshold(BotSpellTypes::FastHeals);
			fastHealMaxThreshold = tar->CastToClient()->GetSpellMaxThreshold(BotSpellTypes::FastHeals);
			regularHealMinThreshold = tar->CastToClient()->GetSpellMinThreshold(BotSpellTypes::RegularHeals);
			regularHealMaxThreshold = tar->CastToClient()->GetSpellMaxThreshold(BotSpellTypes::RegularHeals);
			groupHealMinThreshold = tar->CastToClient()->GetSpellMinThreshold(BotSpellTypes::GroupHeals);
			groupHealMaxThreshold = tar->CastToClient()->GetSpellMaxThreshold(BotSpellTypes::GroupHeals);
			groupHoTHealMinThreshold = tar->CastToClient()->GetSpellMinThreshold(BotSpellTypes::GroupHoTHeals);
			groupHoTHealMaxThreshold = tar->CastToClient()->GetSpellMaxThreshold(BotSpellTypes::GroupHoTHeals);
			completeHealMinThreshold = tar->CastToClient()->GetSpellMinThreshold(BotSpellTypes::CompleteHeals);
			completeHealMaxThreshold = tar->CastToClient()->GetSpellMaxThreshold(BotSpellTypes::CompleteHeals);
			hotHealMinThreshold = tar->CastToClient()->GetSpellMinThreshold(BotSpellTypes::HoTHeals);
			hotHealMaxThreshold = tar->CastToClient()->GetSpellMaxThreshold(BotSpellTypes::HoTHeals);
			veryFastHealDelay = tar->CastToClient()->GetSpellDelay(BotSpellTypes::VeryFastHeals);
			fastHealDelay = tar->CastToClient()->GetSpellDelay(BotSpellTypes::FastHeals);
			regularHealDelay = tar->CastToClient()->GetSpellDelay(BotSpellTypes::RegularHeals);
			groupHealDelay = tar->CastToClient()->GetSpellDelay(BotSpellTypes::GroupHeals);
			completeHealDelay = tar->CastToClient()->GetSpellDelay(BotSpellTypes::CompleteHeals);
			hotHealDelay = tar->CastToClient()->GetSpellDelay(BotSpellTypes::HoTHeals);
			groupHoTHealDelay = tar->CastToClient()->GetSpellDelay(BotSpellTypes::GroupHoTHeals);
		}*/
		else if (tar->IsPet()) {
			switch (GetBotStance()) {
			case EQ::constants::stanceEfficient:
				veryFastHealMinThreshold = 0;
				veryFastHealMaxThreshold = 20;
				fastHealMinThreshold = 0;
				fastHealMaxThreshold = 35;
				regularHealMinThreshold = 0;
				regularHealMaxThreshold = 55;
				completeHealMinThreshold = 0;
				completeHealMaxThreshold = 70;
				hotHealMinThreshold = 0;
				hotHealMaxThreshold = 0;
				groupHealMinThreshold = 0;
				groupHealMaxThreshold = 0;
				groupHoTHealMinThreshold = 0;
				groupHoTHealMaxThreshold = 0;
				break;
			case EQ::constants::stanceReactive:
				veryFastHealMinThreshold = 0;
				veryFastHealMaxThreshold = 20;
				fastHealMinThreshold = 0;
				fastHealMaxThreshold = 35;
				regularHealMinThreshold = 0;
				regularHealMaxThreshold = 55;
				completeHealMinThreshold = 0;
				completeHealMaxThreshold = 70;
				hotHealMinThreshold = 0;
				hotHealMaxThreshold = 85;
				groupHealMinThreshold = 0;
				groupHealMaxThreshold = 0;
				groupHoTHealMinThreshold = 0;
				groupHoTHealMaxThreshold = 0;
				break;
			case EQ::constants::stanceAggressive:
				veryFastHealMinThreshold = 0;
				veryFastHealMaxThreshold = 0;
				fastHealMinThreshold = 0;
				fastHealMaxThreshold = 0;
				regularHealMinThreshold = 0;
				regularHealMaxThreshold = 0;
				completeHealMinThreshold = 0;
				completeHealMaxThreshold = 0;
				hotHealMinThreshold = 0;
				hotHealMaxThreshold = 0;
				groupHealMinThreshold = 0;
				groupHealMaxThreshold = 0;
				groupHoTHealMinThreshold = 0;
				groupHoTHealMaxThreshold = 0;
				break;
			case EQ::constants::stanceBurn:
				veryFastHealMinThreshold = 0;
				veryFastHealMaxThreshold = 20;
				fastHealMinThreshold = 0;
				fastHealMaxThreshold = 35;
				regularHealMinThreshold = 0;
				regularHealMaxThreshold = 35;
				completeHealMinThreshold = 0;
				completeHealMaxThreshold = 0;
				hotHealMinThreshold = 0;
				hotHealMaxThreshold = 0;
				groupHealMinThreshold = 0;
				groupHealMaxThreshold = 0;
				groupHoTHealMinThreshold = 0;
				groupHoTHealMaxThreshold = 0;
				break;
			case EQ::constants::stanceBurnAE:
				veryFastHealMinThreshold = 0;
				veryFastHealMaxThreshold = 15;
				fastHealMinThreshold = 0;
				fastHealMaxThreshold = 25;
				regularHealMinThreshold = 0;
				regularHealMaxThreshold = 25;
				completeHealMinThreshold = 0;
				completeHealMaxThreshold = 0;
				hotHealMinThreshold = 0;
				hotHealMaxThreshold = 0;
				groupHealMinThreshold = 0;
				groupHealMaxThreshold = 0;
				groupHoTHealMinThreshold = 0;
				groupHoTHealMaxThreshold = 0;
				break;
			case EQ::constants::stanceBalanced:
			default:
				veryFastHealMinThreshold = 0;
				veryFastHealMaxThreshold = 20;
				fastHealMinThreshold = 0;
				fastHealMaxThreshold = 35;
				regularHealMinThreshold = 0;
				regularHealMaxThreshold = 55;
				completeHealMinThreshold = 0;
				completeHealMaxThreshold = 0;
				hotHealMinThreshold = 0;
				hotHealMaxThreshold = 0;
				groupHealMinThreshold = 0;
				groupHealMaxThreshold = 0;
				groupHoTHealMinThreshold = 0;
				groupHoTHealMaxThreshold = 0;
				break;
			}
		}

		if (hpr >= veryFastHealMinThreshold && hpr <= veryFastHealMaxThreshold && veryFastHealTime <= currentTime && !GetSpellHold(BotSpellTypes::VeryFastHeals)) {
			botSpell = GetBestBotSpellForVeryFastHeal(this);

			if (botSpell.SpellId != 0) {
				spellType = "Very Fast Heal";
			}

			if (!PrecastChecks(botSpell.SpellId, nullptr, SpellType_Heal)) {
				botSpell.SpellId = 0;
			}
		}

		if (botSpell.SpellId == 0 && hpr >= fastHealMinThreshold && hpr <= fastHealMaxThreshold && fastHealTime <= currentTime && !GetSpellHold(BotSpellTypes::FastHeals)) {
			botSpell = GetBestBotSpellForFastHeal(this);

			if (botSpell.SpellId != 0) {
				spellType = "Fast Heal";
			}

			if (!PrecastChecks(botSpell.SpellId, nullptr, SpellType_Heal)) {
				botSpell.SpellId = 0;
			}
		}

		if (GetNumberNeedingHealedInGroup(groupHealMaxThreshold, false, GetRaid()) >= RuleI(Bots, MinGroupHealTargets) && groupHealTime <= currentTime && !GetSpellHold(BotSpellTypes::GroupHeals)) {
			botSpell = GetBestBotSpellForGroupHeal(this);

			if (botSpell.SpellId != 0) {
				spellType = "Group Regular Heal";
			}

			if (!PrecastChecks(botSpell.SpellId, nullptr, SpellType_Heal)) {
				botSpell.SpellId = 0;
			}
		}

		if (botSpell.SpellId == 0 && hpr >= regularHealMinThreshold && hpr <= regularHealMaxThreshold && regularHealTime <= currentTime && !GetSpellHold(BotSpellTypes::RegularHeals)) {
			botSpell = GetBestBotSpellForRegularSingleTargetHeal(this);

			if (botSpell.SpellId != 0) {
				spellType = "Regular Heal";
			}

			if (!PrecastChecks(botSpell.SpellId, nullptr, SpellType_Heal)) {
				botSpell.SpellId = 0;
			}
		}

		if (botSpell.SpellId == 0 && hpr >= completeHealMinThreshold && hpr <= completeHealMaxThreshold && completeHealTime <= currentTime && !GetSpellHold(BotSpellTypes::CompleteHeals)) {
			botSpell = GetBestBotSpellForPercentageHeal(this);

			if (botSpell.SpellId != 0 && !PrecastChecks(botSpell.SpellId, nullptr, SpellType_Heal)) {
				botSpell.SpellId = 0;
			}

			if (botSpell.SpellId != 0 && (botClass == Class::Cleric)) {
				spellType = "Complete Heal";
			}

			if (botSpell.SpellId != 0 && (botClass != Class::Cleric)) {
				spellType = "Percentage Heal";
			}
		}

		if (GetNumberNeedingHealedInGroup(groupHoTHealMaxThreshold, false, GetRaid()) >= RuleI(Bots, MinGroupHealTargets) && groupHoTHealTime <= currentTime && !GetSpellHold(BotSpellTypes::GroupHoTHeals)) {
			botSpell = GetBestBotSpellForGroupHealOverTime(this);

			if (botSpell.SpellId != 0) {
				spellType = "Group HoT Heal";
			}

			if (!PrecastChecks(botSpell.SpellId, nullptr, SpellType_Heal)) {
				botSpell.SpellId = 0;
			}
		}

		if (botSpell.SpellId == 0 && hpr >= hotHealMinThreshold && hpr <= hotHealMaxThreshold && hotHealTime <= currentTime && !GetSpellHold(BotSpellTypes::HoTHeals)) {
			if (botSpell.SpellId == 0 && botClass == Class::Bard) {
				botSpell = GetFirstBotSpellBySpellType(this, SpellType_Heal);
			}

			botSpell = GetBestBotSpellForHealOverTime(this);

			if (botSpell.SpellId != 0) {
				spellType = "HoT Heal";
			}

			if (!PrecastChecks(botSpell.SpellId, nullptr, SpellType_Heal)) {
				botSpell.SpellId = 0;
			}
		}

		if (!PrecastChecks(botSpell.SpellId, nullptr, SpellType_Heal)) {
			return false;
		}

		if (AIDoSpellCast(botSpell.SpellIndex, tar, botSpell.ManaCost)) {
			casted_spell = true;
		}

		if (casted_spell) {
			if (botClass != Class::Bard) {
				if (IsGroupSpell(botSpell.SpellId)) {
					if (IsRaidGrouped()) {
						uint32 r_group = raid->GetGroup(GetName());
						BotGroupSay(this, "Healing the group with %s [%s].", spells[botSpell.SpellId].name, spellType);
						std::vector<RaidMember> raid_group_members = raid->GetRaidGroupMembers(r_group);
						for (int i = 0; i < raid_group_members.size(); ++i) {
							if (raid_group_members.at(i).member && !raid_group_members.at(i).member->qglobal)
								raid_group_members.at(i).member->SetDontHealMeBefore(currentTime + groupHealDelay);
						}
					}
					else if (IsGrouped()) {
						if (HasGroup()) {
							Group* g = GetGroup();
							if (g) {
								BotGroupSay(this, "Healing the group with %s [%s].", spells[botSpell.SpellId].name, spellType);
								for (int i = 0; i < MAX_GROUP_MEMBERS; i++) {
									if (g->members[i] && !g->members[i]->qglobal)
										g->members[i]->SetDontHealMeBefore(currentTime + groupHealDelay);
								}
							}
						}
					}
				}
				else {
					if (tar != this)
						BotGroupSay(this, "Healing %s with %s [%s]", targetName, spells[botSpell.SpellId].name, spellType);
					if (spellType == "Very Fast Heal")
						tar->SetDontVeryFastHealMeBefore(currentTime + veryFastHealDelay);
					if (spellType == "Fast Heal")
						tar->SetDontFastHealMeBefore(currentTime + fastHealDelay);
					if (spellType == "Regular Heal")
						tar->SetDontRegularHealMeBefore(currentTime + regularHealDelay);
					if (spellType == "Complete Heal")
						tar->SetDontCompleteHealMeBefore(currentTime + completeHealDelay);
					if (spellType == "HoT Heal")
						tar->SetDontHotHealMeBefore(currentTime + hotHealDelay);
				}
			}
		}
	}

	return casted_spell;
}

bool Bot::BotCastMez(Mob* tar, uint8 botLevel, bool checked_los, BotSpell& botSpell, Raid* raid) {
	bool casted_spell = false;
	Mob* addMob = nullptr;

	if (!checked_los && (!CheckLosFN(tar) || !CheckWaterLoS(tar))) {
		return false;
	}

	//TODO
	//Check if single target or AoE mez is best
	//if (TARGETS ON MT IS => 3 THEN botSpell = AoEMez)
	//if (TARGETS ON MT IS <= 2 THEN botSpell = BestMez)


	std::list<BotSpell_wPriority> botSpellList = GetPrioritizedBotSpellsBySpellType(this, SpellType_Mez);

	for (const auto& s : botSpellList) {

		if (!PrecastChecks(s.SpellId, nullptr, SpellType_Mez)) {
			continue;
		}

		Mob* addMob = GetFirstIncomingMobToMez(this, s.SpellId);

		if (!addMob) {
			continue;
		}

		if (!PrecastChecks(s.SpellId, addMob, SpellType_Mez)) {
			return false;
		}

		if (AIDoSpellCast(s.SpellIndex, addMob, s.ManaCost)) {
			BotGroupSay(this, "Attempting to mez %s with [%s].", addMob->GetCleanName(), GetSpellName(s.SpellId));

			return true;
		}
	}

	return casted_spell;
}

bool Bot::AIDoSpellCast(int32 i, Mob* tar, int32 mana_cost, uint32* oDontDoAgainBefore) {
	bool result = false;

	// manacost has special values, -1 is no mana cost, -2 is instant cast (no mana)
	int32 manaCost = mana_cost;

	if (manaCost == -1) {
		manaCost = spells[AIBot_spells[i].spellid].mana;
	} else if (manaCost == -2) {
		manaCost = 0;
	}

	int64 hasMana = GetMana();

	// Allow bots to cast buff spells even if they are out of mana
	if (
		RuleB(Bots, FinishBuffing) &&
		manaCost > hasMana && AIBot_spells[i].type & SpellType_Buff
	) {
		SetMana(manaCost);
	}

	float dist2 = 0;

	if (AIBot_spells[i].type & SpellType_Escape) {
		dist2 = 0;
	} else
		dist2 = DistanceSquared(m_Position, tar->GetPosition());

	if (
		(
			(
				(
					(spells[AIBot_spells[i].spellid].target_type==ST_GroupTeleport && AIBot_spells[i].type == SpellType_Heal) ||
					spells[AIBot_spells[i].spellid].target_type ==ST_AECaster ||
					spells[AIBot_spells[i].spellid].target_type ==ST_Group ||
					spells[AIBot_spells[i].spellid].target_type ==ST_AEBard ||
					(
						tar == this && spells[AIBot_spells[i].spellid].target_type != ST_TargetsTarget
					)
				) &&
				dist2 <= spells[AIBot_spells[i].spellid].aoe_range*spells[AIBot_spells[i].spellid].aoe_range
			) ||
			dist2 <= GetActSpellRange(AIBot_spells[i].spellid, spells[AIBot_spells[i].spellid].range)*GetActSpellRange(AIBot_spells[i].spellid, spells[AIBot_spells[i].spellid].range)
		) &&
		(
			mana_cost <= GetMana() ||
			IsBotNonSpellFighter()
		)
	) {
		casting_spell_AIindex = i;
		LogAI("spellid [{}] tar [{}] mana [{}] Name [{}]", AIBot_spells[i].spellid, tar->GetName(), mana_cost, spells[AIBot_spells[i].spellid].name);
		result = Mob::CastSpell(AIBot_spells[i].spellid, tar->GetID(), EQ::spells::CastingSlot::Gem2, spells[AIBot_spells[i].spellid].cast_time, AIBot_spells[i].manacost == -2 ? 0 : mana_cost, oDontDoAgainBefore, -1, -1, 0, &(AIBot_spells[i].resist_adjust));

		if (IsCasting() && IsSitting())
			Stand();
	}

	// if the spell wasn't casted, then take back any extra mana that was given to the bot to cast that spell
	if (!result) {
		SetMana(hasMana);
	}
	else {
		if (CalcSpellRecastTimer(AIBot_spells[i].spellid) > 0) {
			SetSpellRecastTimer(AIBot_spells[i].spellid);
		}

		SetBotSpellCastDelay(AIBot_spells[i].spellid, tar);
		LogBotDelayChecksDetail("{} says, 'Setting {} Delay for {} on {}.'", GetCleanName(), CastToBot()->GetSpellTypeNameByID(CastToBot()->GetControlledBotSpellType(CastToBot()->GetBotSpellType(spell_id))), spells[spell_id].name, spelltar->GetCleanName()); //deleteme
	}

	return result;
}

bool Bot::AI_PursueCastCheck() {
	bool result = false;

	if (AIautocastspell_timer->Check(false)) {

		AIautocastspell_timer->Disable();	//prevent the timer from going off AGAIN while we are casting.

		LogAIDetail("Bot Engaged (pursuing) autocast check triggered. Trying to cast offensive spells");

		if (!AICastSpell(GetTarget(), 100, SpellType_Snare)) {
			if (!AICastSpell(GetTarget(), 100, SpellType_Lifetap) && !AICastSpell(GetTarget(), 100, SpellType_Nuke)) {
			}
			result = true;
		}

		if (!AIautocastspell_timer->Enabled()) {
			AIautocastspell_timer->Start(RandomTimer(100, 250), false);
		}
	}

	return result;
}

bool Bot::AI_IdleCastCheck() {
	bool result = false;

	if (AIautocastspell_timer->Check(false)) {
		LogAIDetail("Bot Non-Engaged autocast check triggered: [{}]", GetCleanName());
		AIautocastspell_timer->Disable();	//prevent the timer from going off AGAIN while we are casting.

		bool pre_combat = false;
		Client* test_against = nullptr;

		if (HasGroup() && GetGroup()->GetLeader() && GetGroup()->GetLeader()->IsClient()) {
			test_against = GetGroup()->GetLeader()->CastToClient();
		} else if (GetOwner() && GetOwner()->IsClient()) {
			test_against = GetOwner()->CastToClient();
		}

		if (test_against) {
			pre_combat = test_against->GetBotPrecombat();
		}

		//Ok, IdleCastCheck depends of class.
		switch (GetClass()) {
		// Healers WITHOUT pets will check if a heal is needed before buffing.
		case Class::Cleric:
		case Class::Paladin:
		case Class::Ranger: {
			if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, 100, BotAISpellRange, SpellType_Cure)) {
				if (!AICastSpell(this, 100, SpellType_Heal)) {
					if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, 100, BotAISpellRange, SpellType_Heal)) {
						if (!AICastSpell(this, 100, SpellType_Buff)) {
							if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, 100, BotAISpellRange, SpellType_Buff)) {
							}
						}
					}
				}
			}

			result = true;
			break;
		}
		case Class::Monk:
		case Class::Rogue:
		case Class::Warrior:
		case Class::Berserker: {
			if (!AICastSpell(this, 100, SpellType_Cure)) {
				if (!AICastSpell(this, 100, SpellType_Heal)) {
					if (!AICastSpell(this, 100, SpellType_Buff)) {
						if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, 100, BotAISpellRange, SpellType_Buff)) {
						}
					}
				}
			}

			result = true;
			break;
		}
		// Pets class will first cast their pet, then buffs

		case Class::Magician:
		case Class::ShadowKnight:
		case Class::Necromancer:
		case Class::Enchanter: {
			if (!AICastSpell(this, 100, SpellType_Pet)) {
				if (!AICastSpell(this, 100, SpellType_Cure)) {
					if (!AICastSpell(GetPet(), 100, SpellType_Cure)) {
						if (!AICastSpell(this, 100, SpellType_Buff)) {
							if (!AICastSpell(GetPet(), 100, SpellType_Heal)) {
								if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, 100, BotAISpellRange, SpellType_Buff)) {
								}
							}
						}
					}
				}
			}

			result = true;
			break;
		}
		case Class::Druid:
		case Class::Shaman:
		case Class::Beastlord: {
			if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, 100, BotAISpellRange, SpellType_Cure)) {
				if (!AICastSpell(this, 100, SpellType_Pet)) {
					if (!AICastSpell(this, 100, SpellType_Heal)) {
						if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, 100, BotAISpellRange, SpellType_Heal)) {
							if (!AICastSpell(this, 100, SpellType_Buff)) {
								if (!AICastSpell(GetPet(), 100, SpellType_Heal)) {
									if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, 100, BotAISpellRange, SpellType_Buff)) {
									}
								}
							}
						}
					}
				}
			}

			result = true;
			break;
		}
		case Class::Wizard: { // This can eventually be move into the Class::Beastlord case handler once pre-combat is fully implemented
			if (pre_combat) {
				if (!AICastSpell(this, 100, SpellType_Pet)) {
					if (!AICastSpell(this, 100, SpellType_Cure)) {
						if (!AICastSpell(this, 100, SpellType_Heal)) {
							if (!AICastSpell(this, 100, SpellType_Buff)) {
								if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, 100, BotAISpellRange, SpellType_PreCombatBuff)) {
									if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, 100, BotAISpellRange, SpellType_Buff)) {
									}
								}
							}
						}
					}
				}
			}
			else {
				if (!AICastSpell(this, 100, SpellType_Cure)) {
					if (!AICastSpell(this, 100, SpellType_Pet)) {
						if (!AICastSpell(this, 100, SpellType_Heal)) {
							if (!AICastSpell(this, 100, SpellType_Buff)) {
								if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, 100, BotAISpellRange, SpellType_Buff)) {
								}
							}
						}
					}
				}
			}

			result = true;
			break;
		}
		case Class::Bard: {
			if (pre_combat) {
				if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, 100, BotAISpellRange, SpellType_Cure)) {
					if (!AICastSpell(this, 100, SpellType_Buff)) {
						if (!AICastSpell(this, 100, SpellType_PreCombatBuffSong)) {
							if (!AICastSpell(this, 100, SpellType_InCombatBuffSong)) {
							}
						}
					}
				}
			}
			else {
				if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, 100, BotAISpellRange, SpellType_Cure)) {
					if (!AICastSpell(this, 100, SpellType_Buff)) {
						if (!AICastSpell(this, 100, SpellType_OutOfCombatBuffSong)) {
							if (!AICastSpell(this, 100, SpellType_InCombatBuffSong)) {
							}
						}
					}
				}
			}

			result = true;
			break;
		}
		default:
			break;
		}

		if (!AIautocastspell_timer->Enabled())
			AIautocastspell_timer->Start(RandomTimer(500, 2000), false); // avg human response is much less than 5 seconds..even for non-combat situations...
	}

	return result;
}

bool Bot::AI_EngagedCastCheck() {
	bool result = false;
	bool failedToCast = false;

	if (GetTarget() && AIautocastspell_timer->Check(false)) {

		AIautocastspell_timer->Disable();	//prevent the timer from going off AGAIN while we are casting.

		uint8 botClass = GetClass();
		bool mayGetAggro = HasOrMayGetAggro();

		LogAIDetail("Engaged autocast check triggered (BOTS). Trying to cast healing spells then maybe offensive spells");

		if (botClass == Class::Cleric) {
			if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Escape), SpellType_Escape)) {
				if (!AICastSpell(this, GetChanceToCastBySpellType(SpellType_Heal), SpellType_Heal)) {
					if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, GetChanceToCastBySpellType(SpellType_Heal), BotAISpellRange, SpellType_Heal)) {
						if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Debuff), SpellType_Debuff)) {
							if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, GetChanceToCastBySpellType(SpellType_InCombatBuff), BotAISpellRange, SpellType_InCombatBuff)) {
								if (!AICastSpell(GetTarget(), mayGetAggro?0:GetChanceToCastBySpellType(SpellType_Nuke), SpellType_Nuke)) {
									//AIautocastspell_timer->Start(RandomTimer(100, 250), false);		// Do not give healer classes a lot of time off or your tank's die
									failedToCast = true;
								}
							}
						}
					}
				}
			}
		}
		else if (botClass == Class::Druid) {
			if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Escape), SpellType_Escape)) {
				if (!AICastSpell(this, GetChanceToCastBySpellType(SpellType_Heal), SpellType_Heal)) {
					if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, GetChanceToCastBySpellType(SpellType_Heal), BotAISpellRange, SpellType_Heal)) {
						if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Debuff), SpellType_Debuff)) {
							if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, GetChanceToCastBySpellType(SpellType_InCombatBuff), BotAISpellRange, SpellType_InCombatBuff)) {
								if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_DOT), SpellType_DOT)) {
									if (!AICastSpell(GetTarget(), mayGetAggro?0:GetChanceToCastBySpellType(SpellType_Nuke), SpellType_Nuke)) {
										//AIautocastspell_timer->Start(RandomTimer(100, 250), false);		// Do not give healer classes a lot of time off or your tank's die
										failedToCast = true;
									}
								}
							}
						}
					}
				}
			}
		}
		else if (botClass == Class::Shaman) {
			if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Escape), SpellType_Escape)) {
				if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Slow), SpellType_Slow)) {
					if (!AICastSpell(this, GetChanceToCastBySpellType(SpellType_Heal), SpellType_Heal)) {
						if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, GetChanceToCastBySpellType(SpellType_Heal), BotAISpellRange, SpellType_Heal)) {
							if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Debuff), SpellType_Debuff)) {
								if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, GetChanceToCastBySpellType(SpellType_InCombatBuff), BotAISpellRange, SpellType_InCombatBuff)) {
									if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_DOT), SpellType_DOT)) {
										if (!AICastSpell(GetTarget(), mayGetAggro?0:GetChanceToCastBySpellType(SpellType_Nuke), SpellType_Nuke)) {
											if (!AICastSpell(GetPet(), GetChanceToCastBySpellType(SpellType_Heal), SpellType_Heal)) {
												failedToCast = true;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
		else if (botClass == Class::Ranger) {
			if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Escape), SpellType_Escape)) {
				if (!AICastSpell(this, GetChanceToCastBySpellType(SpellType_Heal), SpellType_Heal)) {
					if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, GetChanceToCastBySpellType(SpellType_InCombatBuff), BotAISpellRange, SpellType_InCombatBuff)) {
						if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_DOT), SpellType_DOT)) {
							if (!AICastSpell(GetTarget(), mayGetAggro?0:GetChanceToCastBySpellType(SpellType_Nuke), SpellType_Nuke)) {
								if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, GetChanceToCastBySpellType(SpellType_Heal), BotAISpellRange, SpellType_Heal)) {
									failedToCast = true;
								}
							}
						}
					}
				}
			}
		}
		else if (botClass == Class::Beastlord) {
			if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Slow), SpellType_Slow)) {
				if (!AICastSpell(this, GetChanceToCastBySpellType(SpellType_Heal), SpellType_Heal)) {
					if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Debuff), SpellType_Debuff)) {
						if (!AICastSpell(this, GetChanceToCastBySpellType(SpellType_Pet), SpellType_Pet)) {
							if (!AICastSpell(GetPet(), GetChanceToCastBySpellType(SpellType_Heal), SpellType_Heal)) {
								if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, GetChanceToCastBySpellType(SpellType_InCombatBuff), BotAISpellRange, SpellType_InCombatBuff)) {
									if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_DOT), SpellType_DOT)) {
										if (!AICastSpell(GetTarget(), mayGetAggro?0:GetChanceToCastBySpellType(SpellType_Nuke), SpellType_Nuke)) {
											if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, GetChanceToCastBySpellType(SpellType_Heal), BotAISpellRange, SpellType_Heal)) {
												failedToCast = true;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
		else if (botClass == Class::Wizard) {
			if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Escape), SpellType_Escape)) {
				if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Debuff), SpellType_Debuff)) {
					if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, GetChanceToCastBySpellType(SpellType_InCombatBuff), BotAISpellRange, SpellType_InCombatBuff)) {
						if (!AICastSpell(GetTarget(), mayGetAggro?0:GetChanceToCastBySpellType(SpellType_Nuke), SpellType_Nuke)) {
							failedToCast = true;
						}
					}
				}
			}
		}
		else if (botClass == Class::Paladin) {
			if (!AICastSpell(this, GetChanceToCastBySpellType(SpellType_Heal), SpellType_Heal)) {
				if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, GetChanceToCastBySpellType(SpellType_InCombatBuff), BotAISpellRange, SpellType_InCombatBuff)) {
					if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Debuff), SpellType_Debuff)) {
						if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, GetChanceToCastBySpellType(SpellType_Heal), BotAISpellRange, SpellType_Heal)) {
							if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Nuke), SpellType_Nuke)) {
								failedToCast = true;
							}
						}
					}
				}
			}
		}
		else if (botClass == Class::ShadowKnight) {
			if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Lifetap), SpellType_Lifetap)) {
				if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, GetChanceToCastBySpellType(SpellType_InCombatBuff), BotAISpellRange, SpellType_InCombatBuff)) {
					if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Debuff), SpellType_Debuff)) {
						if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_DOT), SpellType_DOT)) {
							if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Nuke), SpellType_Nuke)) {
								failedToCast = true;
							}
						}
					}
				}
			}
		}
		else if (botClass == Class::Magician) {
			if (!AICastSpell(this, GetChanceToCastBySpellType(SpellType_Pet), SpellType_Pet)) {
				if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Debuff), SpellType_Debuff)) {
					if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, GetChanceToCastBySpellType(SpellType_InCombatBuff), BotAISpellRange, SpellType_InCombatBuff)) {
						if (!AICastSpell(GetPet(), GetChanceToCastBySpellType(SpellType_Heal), SpellType_Heal)) {
							if (!AICastSpell(GetTarget(), mayGetAggro?0:GetChanceToCastBySpellType(SpellType_Nuke), SpellType_Nuke)) {
								failedToCast = true;
							}
						}
					}
				}
			}
		}
		else if (botClass == Class::Necromancer) {
			if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Escape), SpellType_Escape)) {
				if (!AICastSpell(this, GetChanceToCastBySpellType(SpellType_Pet), SpellType_Pet)) {
					if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Debuff), SpellType_Debuff)) {
						if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Lifetap), SpellType_Lifetap)) {
							if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, GetChanceToCastBySpellType(SpellType_InCombatBuff), BotAISpellRange, SpellType_InCombatBuff)) {
								if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_DOT), SpellType_DOT)) {
									if (!AICastSpell(GetPet(), GetChanceToCastBySpellType(SpellType_Heal), SpellType_Heal)) {
										if (!AICastSpell(GetTarget(), mayGetAggro?0:GetChanceToCastBySpellType(SpellType_Nuke), SpellType_Nuke)) {
											failedToCast = true;
										}
									}
								}
							}
						}
					}
				}
			}
		}
		else if (botClass == Class::Enchanter) {
			if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Mez), SpellType_Mez)) {
				if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Escape), SpellType_Escape)) {
					if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Slow), SpellType_Slow)) {
						if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Debuff), SpellType_Debuff)) {
							if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, GetChanceToCastBySpellType(SpellType_InCombatBuff), BotAISpellRange, SpellType_InCombatBuff)) {
								if (!AICastSpell(GetTarget(), mayGetAggro?0:GetChanceToCastBySpellType(SpellType_DOT), SpellType_DOT)) {
									if (!AICastSpell(GetTarget(), mayGetAggro?0:GetChanceToCastBySpellType(SpellType_Nuke), SpellType_Nuke)) {
										failedToCast = true;
									}
								}
							}
						}
					}
				}
			}
		}
		else if (botClass == Class::Bard) {
			if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Escape), SpellType_Escape)) {// Bards will use their escape songs
				if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, GetChanceToCastBySpellType(SpellType_HateRedux), BotAISpellRange, SpellType_HateRedux)) {
					if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Slow), SpellType_Slow)) {
						if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Debuff), SpellType_Debuff)) {
							if (!AICastSpell(this, GetChanceToCastBySpellType(SpellType_InCombatBuffSong), SpellType_InCombatBuffSong)) {
								if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, GetChanceToCastBySpellType(SpellType_InCombatBuff), BotAISpellRange, SpellType_InCombatBuff)) {
									if (!AICastSpell(GetTarget(), mayGetAggro ? 0 : GetChanceToCastBySpellType(SpellType_DOT), SpellType_DOT)) {// Bards will use their dot songs
										if (!AICastSpell(GetTarget(), mayGetAggro ? 0 : GetChanceToCastBySpellType(SpellType_Nuke), SpellType_Nuke)) {// Bards will use their nuke songs
											failedToCast = true;
										}
									}
								}
							}
						}
					}
				}
			}
		}
		else if (botClass == Class::Berserker) {
			if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Escape), SpellType_Escape)) {
				if (!AICastSpell(this, GetChanceToCastBySpellType(SpellType_Heal), SpellType_Heal)) {
					if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Debuff), SpellType_Debuff)) {
						if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, GetChanceToCastBySpellType(SpellType_InCombatBuff), BotAISpellRange, SpellType_InCombatBuff)) {
							if (!AICastSpell(this, GetChanceToCastBySpellType(SpellType_InCombatBuffSong), SpellType_InCombatBuffSong)) {
								if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_DOT), SpellType_DOT)) {
									if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Nuke), SpellType_Nuke)) {
										failedToCast = true;
									}
								}
							}
						}
					}
				}
			}
		}
		else if (botClass == Class::Monk) {
			if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Escape), SpellType_Escape)) {
				if (!AICastSpell(this, GetChanceToCastBySpellType(SpellType_Heal), SpellType_Heal)) {
					if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Debuff), SpellType_Debuff)) {
						if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, GetChanceToCastBySpellType(SpellType_InCombatBuff), BotAISpellRange, SpellType_InCombatBuff)) {
							if (!AICastSpell(this, GetChanceToCastBySpellType(SpellType_InCombatBuffSong), SpellType_InCombatBuffSong)) {
								if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_DOT), SpellType_DOT)) {
									if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Nuke), SpellType_Nuke)) {
										failedToCast = true;
									}
								}
							}
						}
					}
				}
			}
		}
		else if (botClass == Class::Rogue) {
			if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Escape), SpellType_Escape)) {
				if (!AICastSpell(this, GetChanceToCastBySpellType(SpellType_Heal), SpellType_Heal)) {
					if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Debuff), SpellType_Debuff)) {
						if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, GetChanceToCastBySpellType(SpellType_InCombatBuff), BotAISpellRange, SpellType_InCombatBuff)) {
							if (!AICastSpell(this, GetChanceToCastBySpellType(SpellType_InCombatBuffSong), SpellType_InCombatBuffSong)) {
								if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_DOT), SpellType_DOT)) {
									if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Nuke), SpellType_Nuke)) {
										failedToCast = true;
									}
								}
							}
						}
					}
				}
			}
		}
		else if (botClass == Class::Warrior) {
			if (!AICastSpell(this, GetChanceToCastBySpellType(SpellType_Heal), SpellType_Heal)) {
				if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, GetChanceToCastBySpellType(SpellType_InCombatBuff), BotAISpellRange, SpellType_InCombatBuff)) {
					if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Debuff), SpellType_Debuff)) {
						if (!AICastSpell(this, GetChanceToCastBySpellType(SpellType_InCombatBuffSong), SpellType_InCombatBuffSong)) {
							if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_DOT), SpellType_DOT)) {
								if (!AICastSpell(GetTarget(), GetChanceToCastBySpellType(SpellType_Nuke), SpellType_Nuke)) {
									failedToCast = true;
								}
							}
						}
					}
				}
			}
		}

		if (!AIautocastspell_timer->Enabled()) {
			AIautocastspell_timer->Start(RandomTimer(150, 300), false);
		}

		if (!failedToCast) {
			result = true;
		}
	}

	return result;
}

bool Bot::AIHealRotation(Mob* tar, bool useFastHeals) {

	if (!tar) {
		return false;
	}

	if (!AI_HasSpells())
		return false;

	if (tar->GetAppearance() == eaDead) {
		if ((tar->IsClient() && tar->CastToClient()->GetFeigned()) || tar->IsBot()) {
			// do nothing
		}
		else {
			return false;
		}
	}

	uint8 botLevel = GetLevel();

	bool castedSpell = false;

	BotSpell botSpell;
	botSpell.SpellId = 0;
	botSpell.SpellIndex = 0;
	botSpell.ManaCost = 0;

	if (useFastHeals) {
		botSpell = GetBestBotSpellForRegularSingleTargetHeal(this);

		if (!IsValidSpell(botSpell.SpellId))
			botSpell = GetBestBotSpellForFastHeal(this);
	}
	else {
		botSpell = GetBestBotSpellForPercentageHeal(this);

		if (!IsValidSpell(botSpell.SpellId)) {
			botSpell = GetBestBotSpellForRegularSingleTargetHeal(this);
		}
		if (!IsValidSpell(botSpell.SpellId)) {
			botSpell = GetFirstBotSpellForSingleTargetHeal(this);
		}
		if (!IsValidSpell(botSpell.SpellId)) {
			botSpell = GetFirstBotSpellBySpellType(this, SpellType_Heal);
		}
	}

	LogAIDetail("heal spellid [{}] fastheals [{}] casterlevel [{}]",
		botSpell.SpellId, ((useFastHeals) ? ('T') : ('F')), GetLevel());

	LogAIDetail("target [{}] current_time [{}] donthealmebefore [{}]", tar->GetCleanName(), Timer::GetCurrentTime(), tar->DontHealMeBefore());

	// If there is still no spell id, then there isn't going to be one so we are done
	if (!IsValidSpell(botSpell.SpellId)) {
		return false;
	}

	// Can we cast this spell on this target?
	if (!
		(
			spells[botSpell.SpellId].target_type == ST_GroupTeleport ||
			spells[botSpell.SpellId].target_type == ST_Target ||
			tar == this
		) &&
		tar->CanBuffStack(botSpell.SpellId, botLevel, true) < 0
	) {
		return false;
	}

	uint32 TempDontHealMeBeforeTime = tar->DontHealMeBefore();
	if (IsValidSpellRange(botSpell.SpellId, tar)) {
		castedSpell = AIDoSpellCast(botSpell.SpellIndex, tar, botSpell.ManaCost, &TempDontHealMeBeforeTime);
	}

	if (castedSpell) {
		BotGroupSay(
			this,
			fmt::format(
				"Casting {} on {}, please stay in range!",
				spells[botSpell.SpellId].name,
				tar->GetCleanName()
			).c_str()
		);
	}

	return castedSpell;
}

std::list<BotSpell> Bot::GetBotSpellsForSpellEffect(Bot* botCaster, int spellEffect) {
	std::list<BotSpell> result;

	if (!botCaster) {
		return result;
	}

	if (auto bot_owner = botCaster->GetBotOwner(); !bot_owner) {
		return result;
	}

	if (botCaster->AI_HasSpells()) {
		std::vector<BotSpells_Struct> botSpellList = botCaster->AIBot_spells;

		for (int i = botSpellList.size() - 1; i >= 0; i--) {
			if (!IsValidSpell(botSpellList[i].spellid)) {
				// this is both to quit early to save cpu and to avoid casting bad spells
				// Bad info from database can trigger this incorrectly, but that should be fixed in DB, not here
				continue;
			}

			if (IsEffectInSpell(botSpellList[i].spellid, spellEffect) || GetSpellTriggerSpellID(botSpellList[i].spellid, spellEffect)) {
				BotSpell botSpell;
				botSpell.SpellId = botSpellList[i].spellid;
				botSpell.SpellIndex = i;
				botSpell.ManaCost = botSpellList[i].manacost;

				result.push_back(botSpell);
			}
		}
	}

	return result;
}

std::list<BotSpell> Bot::GetBotSpellsForSpellEffectAndTargetType(Bot* botCaster, int spellEffect, SpellTargetType targetType) {
	std::list<BotSpell> result;

	if (!botCaster) {
		return result;
	}

	if (auto bot_owner = botCaster->GetBotOwner(); !bot_owner) {
		return result;
	}

	if (botCaster->AI_HasSpells()) {
		std::vector<BotSpells_Struct> botSpellList = botCaster->AIBot_spells;

		for (int i = botSpellList.size() - 1; i >= 0; i--) {
			if (!IsValidSpell(botSpellList[i].spellid)) {
				// this is both to quit early to save cpu and to avoid casting bad spells
				// Bad info from database can trigger this incorrectly, but that should be fixed in DB, not here
				continue;
			}

			if (
				(
					IsEffectInSpell(botSpellList[i].spellid, spellEffect) ||
					GetSpellTriggerSpellID(botSpellList[i].spellid, spellEffect)
				) &&
				spells[botSpellList[i].spellid].target_type == targetType
			) {
				BotSpell botSpell;
				botSpell.SpellId = botSpellList[i].spellid;
				botSpell.SpellIndex = i;
				botSpell.ManaCost = botSpellList[i].manacost;
				result.push_back(botSpell);
			}
		}
	}

	return result;
}

std::list<BotSpell> Bot::GetBotSpellsBySpellType(Bot* botCaster, uint32 spellType) {
	std::list<BotSpell> result;

	if (!botCaster) {
		return result;
	}

	if (auto bot_owner = botCaster->GetBotOwner(); !bot_owner) {
		return result;
	}

	if (botCaster->AI_HasSpells()) {
		std::vector<BotSpells_Struct> botSpellList = botCaster->AIBot_spells;

		for (int i = botSpellList.size() - 1; i >= 0; i--) {
			if (!IsValidSpell(botSpellList[i].spellid)) {
				// this is both to quit early to save cpu and to avoid casting bad spells
				// Bad info from database can trigger this incorrectly, but that should be fixed in DB, not here
				continue;
			}

			if (botSpellList[i].type & spellType) {
				BotSpell botSpell;
				botSpell.SpellId = botSpellList[i].spellid;
				botSpell.SpellIndex = i;
				botSpell.ManaCost = botSpellList[i].manacost;

				result.push_back(botSpell);
			}
		}
	}

	return result;
}

std::list<BotSpell_wPriority> Bot::GetPrioritizedBotSpellsBySpellType(Bot* botCaster, uint32 spellType) {
	std::list<BotSpell_wPriority> result;

	if (botCaster && botCaster->AI_HasSpells()) {
		std::vector<BotSpells_Struct> botSpellList = botCaster->AIBot_spells;

		for (int i = botSpellList.size() - 1; i >= 0; i--) {
			if (!IsValidSpell(botSpellList[i].spellid)) {
				// this is both to quit early to save cpu and to avoid casting bad spells
				// Bad info from database can trigger this incorrectly, but that should be fixed in DB, not here
				continue;
			}

			if (botSpellList[i].type & spellType) {
				BotSpell_wPriority botSpell;
				botSpell.SpellId = botSpellList[i].spellid;
				botSpell.SpellIndex = i;
				botSpell.ManaCost = botSpellList[i].manacost;
				botSpell.Priority = botSpellList[i].priority;

				result.push_back(botSpell);
			}
		}

		if (result.size() > 1) {
			result.sort(
				[](BotSpell_wPriority const& l, BotSpell_wPriority const& r) {
					return l.Priority < r.Priority;
				}
			);
		}
	}

	return result;
}

BotSpell Bot::GetFirstBotSpellBySpellType(Bot* botCaster, uint32 spellType) {
	BotSpell result;

	result.SpellId = 0;
	result.SpellIndex = 0;
	result.ManaCost = 0;

	if (botCaster && botCaster->AI_HasSpells()) {
		std::vector<BotSpells_Struct> botSpellList = botCaster->AIBot_spells;

		for (int i = botSpellList.size() - 1; i >= 0; i--) {
			if (!IsValidSpell(botSpellList[i].spellid)) {
				// this is both to quit early to save cpu and to avoid casting bad spells
				// Bad info from database can trigger this incorrectly, but that should be fixed in DB, not here
				continue;
			}

			if ((botSpellList[i].type & spellType) && botCaster->CheckSpellRecastTimer(botSpellList[i].spellid)) {
				result.SpellId = botSpellList[i].spellid;
				result.SpellIndex = i;
				result.ManaCost = botSpellList[i].manacost;

				break;
			}
		}
	}

	return result;
}

BotSpell Bot::GetBestBotSpellForVeryFastHeal(Bot* botCaster) {
	BotSpell result;

	result.SpellId = 0;
	result.SpellIndex = 0;
	result.ManaCost = 0;

	if (botCaster) {
		std::list<BotSpell> botSpellList = GetBotSpellsForSpellEffect(botCaster, SE_CurrentHP);

		for (auto botSpellListItr : botSpellList) {
			// Assuming all the spells have been loaded into this list by level and in descending order
			if (IsVeryFastHealSpell(botSpellListItr.SpellId) && botCaster->CheckSpellRecastTimer(botSpellListItr.SpellId)) {
				result.SpellId = botSpellListItr.SpellId;
				result.SpellIndex = botSpellListItr.SpellIndex;
				result.ManaCost = botSpellListItr.ManaCost;

				break;
			}
		}
	}

	return result;
}

BotSpell Bot::GetBestBotSpellForFastHeal(Bot *botCaster) {
	BotSpell result;

	result.SpellId = 0;
	result.SpellIndex = 0;
	result.ManaCost = 0;

	if (botCaster) {
		std::list<BotSpell> botSpellList = GetBotSpellsForSpellEffect(botCaster, SE_CurrentHP);

		for (auto botSpellListItr : botSpellList) {
			// Assuming all the spells have been loaded into this list by level and in descending order
			if (IsFastHealSpell(botSpellListItr.SpellId) && botCaster->CheckSpellRecastTimer(botSpellListItr.SpellId)) {
				result.SpellId = botSpellListItr.SpellId;
				result.SpellIndex = botSpellListItr.SpellIndex;
				result.ManaCost = botSpellListItr.ManaCost;

				break;
			}
		}
	}

	return result;
}

BotSpell Bot::GetBestBotSpellForHealOverTime(Bot* botCaster) {
	BotSpell result;

	result.SpellId = 0;
	result.SpellIndex = 0;
	result.ManaCost = 0;

	if (botCaster) {
		std::list<BotSpell> botHoTSpellList = GetBotSpellsForSpellEffect(botCaster, SE_HealOverTime);
		std::vector<BotSpells_Struct> botSpellList = botCaster->AIBot_spells;

		for (auto botSpellListItr : botHoTSpellList) {
			// Assuming all the spells have been loaded into this list by level and in descending order
			if (IsHealOverTimeSpell(botSpellListItr.SpellId)) {
				for (int i = botSpellList.size() - 1; i >= 0; i--) {
					if (!IsValidSpell(botSpellList[i].spellid)) {
						// this is both to quit early to save cpu and to avoid casting bad spells
						// Bad info from database can trigger this incorrectly, but that should be fixed in DB, not here
						continue;
					}

					if (
						botSpellList[i].spellid == botSpellListItr.SpellId &&
						(botSpellList[i].type & SpellType_Heal) &&
						botCaster->CheckSpellRecastTimer(botSpellListItr.SpellId)
					) {
						result.SpellId = botSpellListItr.SpellId;
						result.SpellIndex = botSpellListItr.SpellIndex;
						result.ManaCost = botSpellListItr.ManaCost;
					}
				}

				break;
			}
		}
	}

	return result;
}

BotSpell Bot::GetBestBotSpellForPercentageHeal(Bot *botCaster) {
	BotSpell result;

	result.SpellId = 0;
	result.SpellIndex = 0;
	result.ManaCost = 0;

	if (botCaster && botCaster->AI_HasSpells()) {
		std::vector<BotSpells_Struct> botSpellList = botCaster->AIBot_spells;

		for (int i = botSpellList.size() - 1; i >= 0; i--) {
			if (!IsValidSpell(botSpellList[i].spellid)) {
				// this is both to quit early to save cpu and to avoid casting bad spells
				// Bad info from database can trigger this incorrectly, but that should be fixed in DB, not here
				continue;
			}

			if (IsCompleteHealSpell(botSpellList[i].spellid) && botCaster->CheckSpellRecastTimer(botSpellList[i].spellid)) {
				result.SpellId = botSpellList[i].spellid;
				result.SpellIndex = i;
				result.ManaCost = botSpellList[i].manacost;
				break;
			}
		}
	}

	return result;
}

BotSpell Bot::GetBestBotSpellForRegularSingleTargetHeal(Bot* botCaster) {
	BotSpell result;

	result.SpellId = 0;
	result.SpellIndex = 0;
	result.ManaCost = 0;

	if (botCaster) {
		std::list<BotSpell> botSpellList = GetBotSpellsForSpellEffect(botCaster, SE_CurrentHP);

		for (std::list<BotSpell>::iterator botSpellListItr = botSpellList.begin(); botSpellListItr != botSpellList.end(); ++botSpellListItr) {
			// Assuming all the spells have been loaded into this list by level and in descending order
			if (IsRegularSingleTargetHealSpell(botSpellListItr->SpellId) && botCaster->CheckSpellRecastTimer(botSpellListItr->SpellId)) {
				result.SpellId = botSpellListItr->SpellId;
				result.SpellIndex = botSpellListItr->SpellIndex;
				result.ManaCost = botSpellListItr->ManaCost;
				break;
			}
		}
	}

	return result;
}

BotSpell Bot::GetFirstBotSpellForSingleTargetHeal(Bot* botCaster) {
	BotSpell result;

	result.SpellId = 0;
	result.SpellIndex = 0;
	result.ManaCost = 0;

	if (botCaster) {
		std::list<BotSpell> botSpellList = GetBotSpellsForSpellEffect(botCaster, SE_CurrentHP);

		for (std::list<BotSpell>::iterator botSpellListItr = botSpellList.begin(); botSpellListItr != botSpellList.end(); ++botSpellListItr) {
			// Assuming all the spells have been loaded into this list by level and in descending order
			if (
				(
					IsRegularSingleTargetHealSpell(botSpellListItr->SpellId) ||
					IsFastHealSpell(botSpellListItr->SpellId)
				) &&
				botCaster->CheckSpellRecastTimer(botSpellListItr->SpellId)
			) {
				result.SpellId = botSpellListItr->SpellId;
				result.SpellIndex = botSpellListItr->SpellIndex;
				result.ManaCost = botSpellListItr->ManaCost;
				break;
			}
		}
	}

	return result;
}

BotSpell Bot::GetBestBotSpellForGroupHeal(Bot* botCaster) {
	BotSpell result;

	result.SpellId = 0;
	result.SpellIndex = 0;
	result.ManaCost = 0;

	const std::vector<Mob*> v = botCaster->GatherSpellTargets();
	int spellAERange = 0;
	int targetCount = 0;

	if (botCaster) {
		std::list<BotSpell> botSpellList = GetBotSpellsForSpellEffect(botCaster, SE_CurrentHP);

		for (std::list<BotSpell>::iterator botSpellListItr = botSpellList.begin(); botSpellListItr != botSpellList.end(); ++botSpellListItr) {
			// Assuming all the spells have been loaded into this list by level and in descending order
			if (IsRegularGroupHealSpell(botSpellListItr->SpellId) && botCaster->CheckSpellRecastTimer(botSpellListItr->SpellId)) {
				spellAERange = botCaster->GetActSpellRange(botSpellListItr->SpellId, spells[botSpellListItr->SpellId].aoe_range);
				targetCount = 0;

				for (Mob* m : v) {
					if (spellAERange >= Distance(botCaster->GetPosition(), m->GetPosition())) {
						++targetCount;
					}
				}

				if (targetCount < RuleI(Bots, MinGroupHealTargets)) {
					continue;
				}

				result.SpellId = botSpellListItr->SpellId;
				result.SpellIndex = botSpellListItr->SpellIndex;
				result.ManaCost = botSpellListItr->ManaCost;
				break;
			}
		}
	}

	return result;
}

BotSpell Bot::GetBestBotSpellForGroupHealOverTime(Bot* botCaster) {
	BotSpell result;

	result.SpellId = 0;
	result.SpellIndex = 0;
	result.ManaCost = 0;

	const std::vector<Mob*> v = botCaster->GatherSpellTargets();
	int spellAERange = 0;
	int targetCount = 0;

	if (botCaster) {
		std::list<BotSpell> botSpellList = GetBotSpellsForSpellEffect(botCaster, SE_HealOverTime);

		for (std::list<BotSpell>::iterator botSpellListItr = botSpellList.begin(); botSpellListItr != botSpellList.end(); ++botSpellListItr) {
			// Assuming all the spells have been loaded into this list by level and in descending order
		// Assuming all the spells have been loaded into this list by level and in descending order
			if (IsGroupHealOverTimeSpell(botSpellListItr->SpellId) && botCaster->CheckSpellRecastTimer(botSpellListItr->SpellId)) {
				spellAERange = botCaster->GetActSpellRange(botSpellListItr->SpellId, spells[botSpellListItr->SpellId].aoe_range);
				targetCount = 0;

				for (Mob* m : v) {
					if (spellAERange >= Distance(botCaster->GetPosition(), m->GetPosition())) {
						++targetCount;
					}
				}

				if (targetCount < RuleI(Bots, MinGroupHealTargets)) {
					continue;
				}

				result.SpellId = botSpellListItr->SpellId;
				result.SpellIndex = botSpellListItr->SpellIndex;
				result.ManaCost = botSpellListItr->ManaCost;

				break;
			}
		}
	}

	return result;
}

BotSpell Bot::GetBestBotSpellForGroupCompleteHeal(Bot* botCaster) {
	BotSpell result;

	result.SpellId = 0;
	result.SpellIndex = 0;
	result.ManaCost = 0;

	if (botCaster) {
		std::list<BotSpell> botSpellList = GetBotSpellsForSpellEffect(botCaster, SE_CompleteHeal);

		for(std::list<BotSpell>::iterator botSpellListItr = botSpellList.begin(); botSpellListItr != botSpellList.end(); ++botSpellListItr) {
			// Assuming all the spells have been loaded into this list by level and in descending order
			if (
				IsGroupCompleteHealSpell(botSpellListItr->SpellId) &&
				botCaster->CheckSpellRecastTimer(botSpellListItr->SpellId)
			) {
				result.SpellId = botSpellListItr->SpellId;
				result.SpellIndex = botSpellListItr->SpellIndex;
				result.ManaCost = botSpellListItr->ManaCost;
				break;
			}
		}
	}

	return result;
}

BotSpell Bot::GetBestBotSpellForMez(Bot* botCaster) {
	BotSpell result;

	result.SpellId = 0;
	result.SpellIndex = 0;
	result.ManaCost = 0;

	if (botCaster) {
		std::list<BotSpell> botSpellList = GetBotSpellsForSpellEffect(botCaster, SE_Mez);

		for (std::list<BotSpell>::iterator botSpellListItr = botSpellList.begin(); botSpellListItr != botSpellList.end(); ++botSpellListItr) {
			// Assuming all the spells have been loaded into this list by level and in descending order
			if (
				IsMesmerizeSpell(botSpellListItr->SpellId) &&
				botCaster->CheckSpellRecastTimer(botSpellListItr->SpellId)
			) {
				result.SpellId = botSpellListItr->SpellId;
				result.SpellIndex = botSpellListItr->SpellIndex;
				result.ManaCost = botSpellListItr->ManaCost;

				break;
			}
		}
	}

	return result;
}

BotSpell Bot::GetBestBotSpellForMagicBasedSlow(Bot* botCaster) {
	BotSpell result;

	result.SpellId = 0;
	result.SpellIndex = 0;
	result.ManaCost = 0;

	if (botCaster) {
		std::list<BotSpell> botSpellList = GetBotSpellsForSpellEffect(botCaster, SE_AttackSpeed);

		for (std::list<BotSpell>::iterator botSpellListItr = botSpellList.begin(); botSpellListItr != botSpellList.end(); ++botSpellListItr) {
			// Assuming all the spells have been loaded into this list by level and in descending order
			if (
				IsSlowSpell(botSpellListItr->SpellId) &&
				spells[botSpellListItr->SpellId].resist_type == RESIST_MAGIC &&
				botCaster->CheckSpellRecastTimer(botSpellListItr->SpellId)
			) {
				result.SpellId = botSpellListItr->SpellId;
				result.SpellIndex = botSpellListItr->SpellIndex;
				result.ManaCost = botSpellListItr->ManaCost;
				break;
			}
		}
	}

	return result;
}

BotSpell Bot::GetBestBotSpellForDiseaseBasedSlow(Bot* botCaster) {
	BotSpell result;

	result.SpellId = 0;
	result.SpellIndex = 0;
	result.ManaCost = 0;

	if (botCaster) {
		std::list<BotSpell> botSpellList = GetBotSpellsForSpellEffect(botCaster, SE_AttackSpeed);

		for (std::list<BotSpell>::iterator botSpellListItr = botSpellList.begin(); botSpellListItr != botSpellList.end(); ++botSpellListItr) {
			// Assuming all the spells have been loaded into this list by level and in descending order
			if (
				IsSlowSpell(botSpellListItr->SpellId) &&
				spells[botSpellListItr->SpellId].resist_type == RESIST_DISEASE &&
				botCaster->CheckSpellRecastTimer(botSpellListItr->SpellId)
			) {
				result.SpellId = botSpellListItr->SpellId;
				result.SpellIndex = botSpellListItr->SpellIndex;
				result.ManaCost = botSpellListItr->ManaCost;

				break;
			}
		}
	}

	return result;
}

Mob* Bot::GetFirstIncomingMobToMez(Bot* botCaster, int16 spellid) {
	Mob* result = nullptr;
	uint32 spell_range = 0;
	int buff_count = 0;
	bool testBool = false;

	if (botCaster && IsMesmerizeSpell(spellid)) {
		for (auto& close_mob : botCaster->close_mobs) {
			bool mobFound = false;
			NPC* npc = close_mob.second->CastToNPC();
			std::vector<Mob*> eligibleTargets;

			if (npc->IsMezzed()) {
				continue;
			}

			if (!botCaster->IsValidTargetType(spellid, GetSpellTargetType(spellid), npc->GetBodyType())) {
				continue;
			}

			const std::vector<Mob*> v = botCaster->GatherSpellTargets();
			for (Mob* m : v) {
				if (npc->IsOnHatelist(m)) {
					mobFound = true;
					break;
				}
			}

			if (!mobFound) {
				continue;
			}

			if (npc->GetHPRatio() < botCaster->GetSpellMinThreshold(BotSpellTypes::Mez) || npc->GetHPRatio() > botCaster->GetSpellMaxThreshold(BotSpellTypes::Mez)) {
				continue;
			}

			if (!botCaster->CheckLosFN(npc) || !botCaster->CheckWaterLoS(npc)) { //TODO || !botCaster->CheckLosCheat(botCaster, npc)) {
				continue;
			}

			spell_range = botCaster->GetActSpellRange(spellid, spells[spellid].range);

			if (DistanceSquaredNoZ(npc->GetPosition(), botCaster->GetPosition()) <= (spell_range * spell_range)) {
				if (botCaster->IsMobEngagedByAnyone(npc)) {
					continue;
				}

				buff_count = npc->GetMaxTotalSlots();
				auto npc_buffs = npc->GetBuffs();

				for (int i = 0; i < buff_count; i++) {
					if (IsDetrimentalSpell(npc_buffs[i].spellid) && IsEffectInSpell(npc_buffs[i].spellid, SE_CurrentHP)) {
						continue;
					}
				}

				if (zone->random.Int(1, 100) <= RuleI(Bots, MezChance)) {
					botCaster->m_botSpellType_Mez.Start(RuleI(Bots, MezSuccessDelay));
					result = npc;
					return result;
				}
			}

			if (testBool) {
				eligibleTargets.push_back(npc);
			}

			continue;
		}

		botCaster->m_botSpellType_Mez.Start(RuleI(Bots, MezFailDelay));
	}

	return result;
}

BotSpell Bot::GetBestBotMagicianPetSpell(Bot *botCaster) {
	BotSpell result;

	result.SpellId = 0;
	result.SpellIndex = 0;
	result.ManaCost = 0;

	if (botCaster) {
		std::list<BotSpell> botSpellList = GetBotSpellsForSpellEffect(botCaster, SE_SummonPet);

		std::string petType = GetBotMagicianPetType(botCaster);

		for(std::list<BotSpell>::iterator botSpellListItr = botSpellList.begin(); botSpellListItr != botSpellList.end(); ++botSpellListItr) {
			// Assuming all the spells have been loaded into this list by level and in descending order
			if (IsSummonPetSpell(botSpellListItr->SpellId) && botCaster->CheckSpellRecastTimer(botSpellListItr->SpellId)) {
				if (!strncmp(spells[botSpellListItr->SpellId].teleport_zone, petType.c_str(), petType.length())) {
					result.SpellId = botSpellListItr->SpellId;
					result.SpellIndex = botSpellListItr->SpellIndex;
					result.ManaCost = botSpellListItr->ManaCost;

					break;
				}
			}
		}
	}

	return result;
}

std::string Bot::GetBotMagicianPetType(Bot* botCaster) {
	std::string result;

	if (botCaster) {
		if (botCaster->IsPetChooser()) {
			switch(botCaster->GetPetChooserID()) {
				case 0:
					result = std::string("SumWater");
					break;
				case 1:
					result = std::string("SumFire");
					break;
				case 2:
					result = std::string("SumAir");
					break;
				case 3:
					result = std::string("SumEarth");
					break;
				default:
					result = std::string("MonsterSum");
					break;
			}
		}
		else {
			if (botCaster->GetLevel() == 2)
				result = std::string("SumWater");
			else if (botCaster->GetLevel() == 3)
				result = std::string("SumFire");
			else if (botCaster->GetLevel() == 4)
				result = std::string("SumAir");
			else if (botCaster->GetLevel() == 5)
				result = std::string("SumEarth");
			else if (botCaster->GetLevel() < 30) {
				// Under level 30
				int counter = zone->random.Int(0, 3);

				switch(counter) {
					case 0:
					result = std::string("SumWater");
					break;
				case 1:
					result = std::string("SumFire");
					break;
				case 2:
					result = std::string("SumAir");
					break;
				case 3:
					result = std::string("SumEarth");
					break;
				default:
					result = std::string("MonsterSum");
					break;
				}
			}
			else {
				// Over level 30
				int counter = zone->random.Int(0, 4);

				switch(counter) {
					case 0:
					result = std::string("SumWater");
					break;
				case 1:
					result = std::string("SumFire");
					break;
				case 2:
					result = std::string("SumAir");
					break;
				case 3:
					result = std::string("SumEarth");
					break;
				default:
					result = std::string("MonsterSum");
					break;
				}
			}
		}
	}

	return result;
}

BotSpell Bot::GetBestBotSpellForNukeByTargetType(Bot* botCaster, SpellTargetType targetType) {
	BotSpell result;

	result.SpellId = 0;
	result.SpellIndex = 0;
	result.ManaCost = 0;

	if (botCaster) {
		std::list<BotSpell> botSpellList = GetBotSpellsForSpellEffectAndTargetType(botCaster, SE_CurrentHP, targetType);

		for(std::list<BotSpell>::iterator botSpellListItr = botSpellList.begin(); botSpellListItr != botSpellList.end(); ++botSpellListItr) {
			// Assuming all the spells have been loaded into this list by level and in descending order
			if ((IsPureNukeSpell(botSpellListItr->SpellId) || IsDamageSpell(botSpellListItr->SpellId)) && botCaster->CheckSpellRecastTimer(botSpellListItr->SpellId)) {
				if (!botCaster->DoResistCheck(botCaster->GetTarget(), botSpellListItr->SpellId, RuleI(Bots, NukeResistLimit))) {
					continue;
				}

				result.SpellId = botSpellListItr->SpellId;
				result.SpellIndex = botSpellListItr->SpellIndex;
				result.ManaCost = botSpellListItr->ManaCost;

				break;
			}
		}
	}

	return result;
}

BotSpell Bot::GetBestBotSpellForStunByTargetType(Bot* botCaster, SpellTargetType targetType)
{
	BotSpell result;

	result.SpellId = 0;
	result.SpellIndex = 0;
	result.ManaCost = 0;

	if (botCaster)
	{
		std::list<BotSpell> botSpellList = GetBotSpellsForSpellEffectAndTargetType(botCaster, SE_Stun, targetType);

		for(std::list<BotSpell>::iterator botSpellListItr = botSpellList.begin(); botSpellListItr != botSpellList.end(); ++botSpellListItr)
		{
			// Assuming all the spells have been loaded into this list by level and in descending order
			if (IsStunSpell(botSpellListItr->SpellId) && botCaster->CheckSpellRecastTimer(botSpellListItr->SpellId)) {
				if (!botCaster->DoResistCheck(botCaster->GetTarget(), botSpellListItr->SpellId, RuleI(Bots, NukeResistLimit))) {
					continue;
				}

				result.SpellId = botSpellListItr->SpellId;
				result.SpellIndex = botSpellListItr->SpellIndex;
				result.ManaCost = botSpellListItr->ManaCost;
				break;
			}
		}
	}

	return result;
}

BotSpell Bot::GetBestBotWizardNukeSpellByTargetResists(Bot* botCaster, Mob* target) {
	BotSpell result;

	result.SpellId = 0;
	result.SpellIndex = 0;
	result.ManaCost = 0;

	if (botCaster && target) {

		const int lureResisValue = -100;

		int32 level_mod = (target->GetLevel() - botCaster->GetLevel()) * (target->GetLevel() - botCaster->GetLevel()) / 2;

		if (target->GetLevel() - botCaster->GetLevel() < 0) {
			level_mod = -level_mod;
		}
		const int maxTargetResistValue = RuleI(Bots, NukeResistLimit);
		bool selectLureNuke = false;

		if (((target->GetMR() + level_mod) > maxTargetResistValue) && ((target->GetCR() + level_mod) > maxTargetResistValue) && ((target->GetFR() + level_mod) > maxTargetResistValue))
			selectLureNuke = true;


		std::list<BotSpell> botSpellList = GetBotSpellsForSpellEffectAndTargetType(botCaster, SE_CurrentHP, ST_Target);

		BotSpell firstWizardMagicNukeSpellFound;
		firstWizardMagicNukeSpellFound.SpellId = 0;
		firstWizardMagicNukeSpellFound.SpellIndex = 0;
		firstWizardMagicNukeSpellFound.ManaCost = 0;
		bool spellSelected = false;

		for (std::list<BotSpell>::iterator botSpellListItr = botSpellList.begin(); botSpellListItr != botSpellList.end(); ++botSpellListItr) {
			// Assuming all the spells have been loaded into this list by level and in descending order
			if (!botCaster->IsValidSpellRange(botSpellListItr->SpellId, botCaster->GetTarget())) {
				continue;
			}

			if (botCaster->CheckSpellRecastTimer(botSpellListItr->SpellId)) {
				if (selectLureNuke && (spells[botSpellListItr->SpellId].resist_difficulty < lureResisValue)) {
					if (botCaster->DoResistCheck(target, botSpellListItr->SpellId, RuleI(Bots, NukeResistLimit))) {
						spellSelected = true;
					}
				}
				else if (!selectLureNuke && IsPureNukeSpell(botSpellListItr->SpellId)) {
					if (((target->GetMR() < target->GetCR()) || (target->GetMR() < target->GetFR())) && (GetSpellResistType(botSpellListItr->SpellId) == RESIST_MAGIC)
						&& (spells[botSpellListItr->SpellId].resist_difficulty > lureResisValue) && botCaster->DoResistCheck(target, botSpellListItr->SpellId, RuleI(Bots, NukeResistLimit)))
					{
						spellSelected = true;
					}
					else if (((target->GetCR() < target->GetMR()) || (target->GetCR() < target->GetFR())) && (GetSpellResistType(botSpellListItr->SpellId) == RESIST_COLD)
						&& (spells[botSpellListItr->SpellId].resist_difficulty > lureResisValue) && botCaster->DoResistCheck(target, botSpellListItr->SpellId, RuleI(Bots, NukeResistLimit)))
					{
						spellSelected = true;
					}
					else if (((target->GetFR() < target->GetCR()) || (target->GetFR() < target->GetMR())) && (GetSpellResistType(botSpellListItr->SpellId) == RESIST_FIRE)
						&& (spells[botSpellListItr->SpellId].resist_difficulty > lureResisValue) && botCaster->DoResistCheck(target, botSpellListItr->SpellId, RuleI(Bots, NukeResistLimit)))
					{
						spellSelected = true;
					}
					else if ((GetSpellResistType(botSpellListItr->SpellId) == RESIST_MAGIC) && (spells[botSpellListItr->SpellId].resist_difficulty > lureResisValue) && !IsStunSpell(botSpellListItr->SpellId)) {
						firstWizardMagicNukeSpellFound.SpellId = botSpellListItr->SpellId;
						firstWizardMagicNukeSpellFound.SpellIndex = botSpellListItr->SpellIndex;
						firstWizardMagicNukeSpellFound.ManaCost = botSpellListItr->ManaCost;
					}
				}
			}

			if (spellSelected) {
				result.SpellId = botSpellListItr->SpellId;
				result.SpellIndex = botSpellListItr->SpellIndex;
				result.ManaCost = botSpellListItr->ManaCost;

				break;
			}
		}

		if (!spellSelected) {
			for (std::list<BotSpell>::iterator botSpellListItr = botSpellList.begin(); botSpellListItr != botSpellList.end(); ++botSpellListItr) {
				// Assuming all the spells have been loaded into this list by level and in descending order

				if (botCaster->CheckSpellRecastTimer(botSpellListItr->SpellId)) {
					if (botCaster->DoResistCheck(target, botSpellListItr->SpellId, RuleI(Bots, NukeResistLimit))) {
						spellSelected = true;
					}
				}
				if (spellSelected) {
					result.SpellId = botSpellListItr->SpellId;
					result.SpellIndex = botSpellListItr->SpellIndex;
					result.ManaCost = botSpellListItr->ManaCost;

					break;
				}
			}
		}

		if (result.SpellId == 0) {
			result = firstWizardMagicNukeSpellFound;
		}
	}

	return result;
}

BotSpell Bot::GetDebuffBotSpell(Bot* botCaster, Mob *tar) {
	BotSpell result;

	result.SpellId = 0;
	result.SpellIndex = 0;
	result.ManaCost = 0;

	if (!tar || !botCaster)
		return result;

	if (botCaster->AI_HasSpells()) {
		std::vector<BotSpells_Struct> botSpellList = botCaster->AIBot_spells;

		for (int i = botSpellList.size() - 1; i >= 0; i--) {
			if (!IsValidSpell(botSpellList[i].spellid)) {
				// this is both to quit early to save cpu and to avoid casting bad spells
				// Bad info from database can trigger this incorrectly, but that should be fixed in DB, not here
				continue;
			}

			if (((botSpellList[i].type & SpellType_Debuff) || IsDebuffSpell(botSpellList[i].spellid))
				&& (!tar->IsImmuneToSpell(botSpellList[i].spellid, botCaster)
				&& tar->CanBuffStack(botSpellList[i].spellid, botCaster->GetLevel(), true) >= 0)
				&& botCaster->CheckSpellRecastTimer(botSpellList[i].spellid)) {
				result.SpellId = botSpellList[i].spellid;
				result.SpellIndex = i;
				result.ManaCost = botSpellList[i].manacost;

				break;
			}
		}
	}

	return result;
}

BotSpell Bot::GetBestBotSpellForResistDebuff(Bot* botCaster, Mob *tar) {
	BotSpell result;

	result.SpellId = 0;
	result.SpellIndex = 0;
	result.ManaCost = 0;

	if (!tar || !botCaster) {
		return result;
	}

	int level_mod = (tar->GetLevel() - botCaster->GetLevel())* (tar->GetLevel() - botCaster->GetLevel()) / 2;
	if (tar->GetLevel() - botCaster->GetLevel() < 0) {
		level_mod = -level_mod;
	}

	bool needsMagicResistDebuff = (tar->GetMR() + level_mod) > 100;
	bool needsColdResistDebuff = (tar->GetCR() + level_mod) > 100;
	bool needsFireResistDebuff = (tar->GetFR() + level_mod) > 100;
	bool needsPoisonResistDebuff = (tar->GetPR() + level_mod) > 100;
	bool needsDiseaseResistDebuff = (tar->GetDR() + level_mod) > 100;

	if (botCaster->AI_HasSpells()) {
		std::vector<BotSpells_Struct> botSpellList = botCaster->AIBot_spells;

		for (int i = botSpellList.size() - 1; i >= 0; i--) {
			if (!IsValidSpell(botSpellList[i].spellid)) {
				// this is both to quit early to save cpu and to avoid casting bad spells
				// Bad info from database can trigger this incorrectly, but that should be fixed in DB, not here
				continue;
			}

			if (((botSpellList[i].type & SpellType_Debuff) || IsResistDebuffSpell(botSpellList[i].spellid))
				&& ((needsMagicResistDebuff && (IsEffectInSpell(botSpellList[i].spellid, SE_ResistMagic)) || IsEffectInSpell(botSpellList[i].spellid, SE_ResistAll))
				|| (needsColdResistDebuff && (IsEffectInSpell(botSpellList[i].spellid, SE_ResistCold)) || IsEffectInSpell(botSpellList[i].spellid, SE_ResistAll))
				|| (needsFireResistDebuff && (IsEffectInSpell(botSpellList[i].spellid, SE_ResistFire)) || IsEffectInSpell(botSpellList[i].spellid, SE_ResistAll))
				|| (needsPoisonResistDebuff && (IsEffectInSpell(botSpellList[i].spellid, SE_ResistPoison)) || IsEffectInSpell(botSpellList[i].spellid, SE_ResistAll))
				|| (needsDiseaseResistDebuff && (IsEffectInSpell(botSpellList[i].spellid, SE_ResistDisease)) || IsEffectInSpell(botSpellList[i].spellid, SE_ResistAll)))
				&& (!tar->IsImmuneToSpell(botSpellList[i].spellid, botCaster)
				&& tar->CanBuffStack(botSpellList[i].spellid, botCaster->GetLevel(), true) >= 0)
				&& botCaster->CheckSpellRecastTimer(botSpellList[i].spellid)) {
				result.SpellId = botSpellList[i].spellid;
				result.SpellIndex = i;
				result.ManaCost = botSpellList[i].manacost;

				break;
			}
		}
	}

	return result;
}

BotSpell Bot::GetBestBotSpellForCure(Bot* botCaster, Mob *tar) {
	BotSpell_wPriority result;
	bool spellSelected = false;

	result.SpellId = 0;
	result.SpellIndex = 0;
	result.ManaCost = 0;

	if (!tar)
		return result;

	int countNeedsCured = 0;
	bool isPoisoned = tar->FindType(SE_PoisonCounter);
	bool isDiseased = tar->FindType(SE_DiseaseCounter);
	bool isCursed = tar->FindType(SE_CurseCounter);
	bool isCorrupted = tar->FindType(SE_CorruptionCounter);

	if (botCaster && botCaster->AI_HasSpells()) {
		std::list<BotSpell_wPriority> cureList = GetPrioritizedBotSpellsBySpellType(botCaster, SpellType_Cure);

		if (tar->HasGroup()) {
			Group *g = tar->GetGroup();

			if (g) {
				for( int i = 0; i<MAX_GROUP_MEMBERS; i++) {
					if (g->members[i] && !g->members[i]->qglobal) {
						if (botCaster->GetNeedsCured(g->members[i]))
							countNeedsCured++;
					}
				}
			}
		}

		//Check for group cure first
		if (countNeedsCured > 2) {
			for (std::list<BotSpell_wPriority>::iterator itr = cureList.begin(); itr != cureList.end(); ++itr) {
				BotSpell selectedBotSpell = *itr;

				if (IsGroupSpell(itr->SpellId) && botCaster->CheckSpellRecastTimer(selectedBotSpell.SpellId)) {
					if (selectedBotSpell.SpellId == 0)
						continue;

					if (isPoisoned && IsEffectInSpell(selectedBotSpell.SpellId, SE_PoisonCounter)) {
						spellSelected = true;
					}
					else if (isDiseased && IsEffectInSpell(selectedBotSpell.SpellId, SE_DiseaseCounter)) {
						spellSelected = true;
					}
					else if (isCursed && IsEffectInSpell(selectedBotSpell.SpellId, SE_CurseCounter)) {
						spellSelected = true;
					}
					else if (isCorrupted && IsEffectInSpell(selectedBotSpell.SpellId, SE_CorruptionCounter)) {
						spellSelected = true;
					}
					else if (IsEffectInSpell(selectedBotSpell.SpellId, SE_DispelDetrimental)) {
						spellSelected = true;
					}

					if (spellSelected)
					{
						result.SpellId = selectedBotSpell.SpellId;
						result.SpellIndex = selectedBotSpell.SpellIndex;
						result.ManaCost = selectedBotSpell.ManaCost;

						break;
					}
				}
			}
		}

		//no group cure for target- try to find single target spell
		if (!spellSelected) {
			for(std::list<BotSpell_wPriority>::iterator itr = cureList.begin(); itr != cureList.end(); ++itr) {
				BotSpell selectedBotSpell = *itr;

				if (botCaster->CheckSpellRecastTimer(selectedBotSpell.SpellId)) {
					if (selectedBotSpell.SpellId == 0)
						continue;

					if (isPoisoned && IsEffectInSpell(selectedBotSpell.SpellId, SE_PoisonCounter)) {
						spellSelected = true;
					}
					else if (isDiseased && IsEffectInSpell(selectedBotSpell.SpellId, SE_DiseaseCounter)) {
						spellSelected = true;
					}
					else if (isCursed && IsEffectInSpell(selectedBotSpell.SpellId, SE_CurseCounter)) {
						spellSelected = true;
					}
					else if (isCorrupted && IsEffectInSpell(selectedBotSpell.SpellId, SE_CorruptionCounter)) {
						spellSelected = true;
					}
					else if (IsEffectInSpell(selectedBotSpell.SpellId, SE_DispelDetrimental)) {
						spellSelected = true;
					}

					if (spellSelected)
					{
						result.SpellId = selectedBotSpell.SpellId;
						result.SpellIndex = selectedBotSpell.SpellIndex;
						result.ManaCost = selectedBotSpell.ManaCost;

						break;
					}
				}
			}
		}
	}

	return result;
}

uint8 Bot::GetChanceToCastBySpellType(uint32 spellType)
{
	uint8 spell_type_index = SPELL_TYPE_COUNT;
	switch (spellType) {
	case SpellType_Nuke:
		spell_type_index = spellTypeIndexNuke;
		break;
	case SpellType_Heal:
		spell_type_index = spellTypeIndexHeal;
		break;
	case SpellType_Root:
		spell_type_index = spellTypeIndexRoot;
		break;
	case SpellType_Buff:
		spell_type_index = spellTypeIndexBuff;
		break;
	case SpellType_Escape:
		spell_type_index = spellTypeIndexEscape;
		break;
	case SpellType_Pet:
		spell_type_index = spellTypeIndexPet;
		break;
	case SpellType_Lifetap:
		spell_type_index = spellTypeIndexLifetap;
		break;
	case SpellType_Snare:
		spell_type_index = spellTypeIndexSnare;
		break;
	case SpellType_DOT:
		spell_type_index = spellTypeIndexDot;
		break;
	case SpellType_Dispel:
		spell_type_index = spellTypeIndexDispel;
		break;
	case SpellType_InCombatBuff:
		spell_type_index = spellTypeIndexInCombatBuff;
		break;
	case SpellType_Mez:
		spell_type_index = spellTypeIndexMez;
		break;
	case SpellType_Charm:
		spell_type_index = spellTypeIndexCharm;
		break;
	case SpellType_Slow:
		spell_type_index = spellTypeIndexSlow;
		break;
	case SpellType_Debuff:
		spell_type_index = spellTypeIndexDebuff;
		break;
	case SpellType_Cure:
		spell_type_index = spellTypeIndexCure;
		break;
	case SpellType_Resurrect:
		spell_type_index = spellTypeIndexResurrect;
		break;
	case SpellType_HateRedux:
		spell_type_index = spellTypeIndexHateRedux;
		break;
	case SpellType_InCombatBuffSong:
		spell_type_index = spellTypeIndexInCombatBuffSong;
		break;
	case SpellType_OutOfCombatBuffSong:
		spell_type_index = spellTypeIndexOutOfCombatBuffSong;
		break;
	case SpellType_PreCombatBuff:
		spell_type_index = spellTypeIndexPreCombatBuff;
		break;
	case SpellType_PreCombatBuffSong:
		spell_type_index = spellTypeIndexPreCombatBuffSong;
		break;
	default:
		break;
	}

	if (spell_type_index >= SPELL_TYPE_COUNT)
		return 0;

	uint8 class_index = GetClass();
	if (class_index > Class::Berserker || class_index < Class::Warrior)
		return 0;
	--class_index;

	EQ::constants::StanceType stance_type = GetBotStance();
	if (stance_type < EQ::constants::stancePassive || stance_type > EQ::constants::stanceBurnAE)
		return 0;

	uint8 stance_index = EQ::constants::ConvertStanceTypeToIndex(stance_type);
	uint8 type_index = nHSND;

	if (HasGroup()) {
		if (IsGroupHealer()/* || IsRaidHealer()*/)
			type_index |= pH;
		if (IsGroupSlower()/* || IsRaidSlower()*/)
			type_index |= pS;
		if (IsGroupNuker()/* || IsRaidNuker()*/)
			type_index |= pN;
		if (IsGroupDoter()/* || IsRaidDoter()*/)
			type_index |= pD;
	}

	return database.botdb.GetSpellCastingChance(spell_type_index, class_index, stance_index, type_index);
}

bool Bot::AI_AddBotSpells(uint32 bot_spell_id) {
	// ok, this function should load the list, and the parent list then shove them into the struct and sort
	npc_spells_id = bot_spell_id;
	AIBot_spells.clear();
	AIBot_spells_enforced.clear();
	if (!bot_spell_id) {
		AIautocastspell_timer->Disable();
		return false;
	}

	auto* spell_list = content_db.GetBotSpells(bot_spell_id);
	if (!spell_list) {
		AIautocastspell_timer->Disable();
		return false;
	}

	auto* parentlist = content_db.GetBotSpells(spell_list->parent_list);

	auto debug_msg = fmt::format(
		"Loading Bot spells onto {}: dbspellsid={}, level={}",
		GetName(),
		bot_spell_id,
		GetLevel()
	);

	debug_msg.append(
		fmt::format(
			" (found, {})",
			spell_list->entries.size()
		)
	);

	LogAI("[{}]", debug_msg);
	for (const auto &iter: spell_list->entries) {
		LogAIDetail("([{}]) [{}]", iter.spellid, spells[iter.spellid].name);
	}

	LogAI("fin (spell list)");

	uint16 attack_proc_spell = -1;
	int8 proc_chance = 3;
	uint16 range_proc_spell = -1;
	int16 rproc_chance = 0;
	uint16 defensive_proc_spell = -1;
	int16 dproc_chance = 0;
	uint32 _fail_recast = 0;
	uint32 _engaged_no_sp_recast_min = 0;
	uint32 _engaged_no_sp_recast_max = 0;
	uint8 _engaged_beneficial_self_chance = 0;
	uint8 _engaged_beneficial_other_chance = 0;
	uint8 _engaged_detrimental_chance = 0;
	uint32 _pursue_no_sp_recast_min = 0;
	uint32 _pursue_no_sp_recast_max = 0;
	uint8 _pursue_detrimental_chance = 0;
	uint32 _idle_no_sp_recast_min = 0;
	uint32 _idle_no_sp_recast_max = 0;
	uint8 _idle_beneficial_chance = 0;

	if (parentlist) {
		attack_proc_spell = parentlist->attack_proc;
		proc_chance = parentlist->proc_chance;
		range_proc_spell = parentlist->range_proc;
		rproc_chance = parentlist->rproc_chance;
		defensive_proc_spell = parentlist->defensive_proc;
		dproc_chance = parentlist->dproc_chance;
		_fail_recast = parentlist->fail_recast;
		_engaged_no_sp_recast_min = parentlist->engaged_no_sp_recast_min;
		_engaged_no_sp_recast_max = parentlist->engaged_no_sp_recast_max;
		_engaged_beneficial_self_chance = parentlist->engaged_beneficial_self_chance;
		_engaged_beneficial_other_chance = parentlist->engaged_beneficial_other_chance;
		_engaged_detrimental_chance = parentlist->engaged_detrimental_chance;
		_pursue_no_sp_recast_min = parentlist->pursue_no_sp_recast_min;
		_pursue_no_sp_recast_max = parentlist->pursue_no_sp_recast_max;
		_pursue_detrimental_chance = parentlist->pursue_detrimental_chance;
		_idle_no_sp_recast_min = parentlist->idle_no_sp_recast_min;
		_idle_no_sp_recast_max = parentlist->idle_no_sp_recast_max;
		_idle_beneficial_chance = parentlist->idle_beneficial_chance;
		for (auto &e : parentlist->entries) {
			if (
				EQ::ValueWithin(GetLevel(), e.minlevel, e.maxlevel) &&
				e.spellid &&
				!IsSpellInBotList(spell_list, e.spellid)
			) {
				if (!e.bucket_name.empty() && !e.bucket_value.empty()) {
					if (!CheckDataBucket(e.bucket_name, e.bucket_value, e.bucket_comparison)) {
						continue;
					}
				}

				const auto& bs = GetBotSpellSetting(e.spellid);
				if (bs) {
					if (!bs->is_enabled) {
						continue;
					}

					AddSpellToBotList(
						bs->priority,
						e.spellid,
						e.type,
						e.manacost,
						e.recast_delay,
						e.resist_adjust,
						e.minlevel,
						e.maxlevel,
						bs->min_hp,
						bs->max_hp,
						e.bucket_name,
						e.bucket_value,
						e.bucket_comparison
					);
					continue;
				}

				if (!GetBotEnforceSpellSetting()) {
					AddSpellToBotList(
						e.priority,
						e.spellid,
						e.type,
						e.manacost,
						e.recast_delay,
						e.resist_adjust,
						e.minlevel,
						e.maxlevel,
						e.min_hp,
						e.max_hp,
						e.bucket_name,
						e.bucket_value,
						e.bucket_comparison
					);
				} else {
					AddSpellToBotEnforceList(
						e.priority,
						e.spellid,
						e.type,
						e.manacost,
						e.recast_delay,
						e.resist_adjust,
						e.minlevel,
						e.maxlevel,
						e.min_hp,
						e.max_hp,
						e.bucket_name,
						e.bucket_value,
						e.bucket_comparison
					);
				}
			}
		}
	}

	attack_proc_spell = spell_list->attack_proc;
	proc_chance = spell_list->proc_chance;

	range_proc_spell = spell_list->range_proc;
	rproc_chance = spell_list->rproc_chance;

	defensive_proc_spell = spell_list->defensive_proc;
	dproc_chance = spell_list->dproc_chance;

	//If any casting variables are defined in the current list, ignore those in the parent list.
	if (
		spell_list->fail_recast ||
		spell_list->engaged_no_sp_recast_min ||
		spell_list->engaged_no_sp_recast_max ||
		spell_list->engaged_beneficial_self_chance ||
		spell_list->engaged_beneficial_other_chance ||
		spell_list->engaged_detrimental_chance ||
		spell_list->pursue_no_sp_recast_min ||
		spell_list->pursue_no_sp_recast_max ||
		spell_list->pursue_detrimental_chance ||
		spell_list->idle_no_sp_recast_min ||
		spell_list->idle_no_sp_recast_max ||
		spell_list->idle_beneficial_chance
	) {
		_fail_recast = spell_list->fail_recast;
		_engaged_no_sp_recast_min = spell_list->engaged_no_sp_recast_min;
		_engaged_no_sp_recast_max = spell_list->engaged_no_sp_recast_max;
		_engaged_beneficial_self_chance = spell_list->engaged_beneficial_self_chance;
		_engaged_beneficial_other_chance = spell_list->engaged_beneficial_other_chance;
		_engaged_detrimental_chance = spell_list->engaged_detrimental_chance;
		_pursue_no_sp_recast_min = spell_list->pursue_no_sp_recast_min;
		_pursue_no_sp_recast_max = spell_list->pursue_no_sp_recast_max;
		_pursue_detrimental_chance = spell_list->pursue_detrimental_chance;
		_idle_no_sp_recast_min = spell_list->idle_no_sp_recast_min;
		_idle_no_sp_recast_max = spell_list->idle_no_sp_recast_max;
		_idle_beneficial_chance = spell_list->idle_beneficial_chance;
	}

	for (auto &e : spell_list->entries) {
		if (EQ::ValueWithin(GetLevel(), e.minlevel, e.maxlevel) && e.spellid) {
			if (!e.bucket_name.empty() && !e.bucket_value.empty()) {
				if (!CheckDataBucket(e.bucket_name, e.bucket_value, e.bucket_comparison)) {
					continue;
				}
			}

			const auto& bs = GetBotSpellSetting(e.spellid);
			if (bs) {
				if (!bs->is_enabled) {
					continue;
				}

				AddSpellToBotList(
					bs->priority,
					e.spellid,
					e.type,
					e.manacost,
					e.recast_delay,
					e.resist_adjust,
					e.minlevel,
					e.maxlevel,
					bs->min_hp,
					bs->max_hp,
					e.bucket_name,
					e.bucket_value,
					e.bucket_comparison
				);
				continue;
			}

			if (!GetBotEnforceSpellSetting()) {
				AddSpellToBotList(
					e.priority,
					e.spellid,
					e.type,
					e.manacost,
					e.recast_delay,
					e.resist_adjust,
					e.minlevel,
					e.maxlevel,
					e.min_hp,
					e.max_hp,
					e.bucket_name,
					e.bucket_value,
					e.bucket_comparison
				);
			} else {
				AddSpellToBotEnforceList(
					e.priority,
					e.spellid,
					e.type,
					e.manacost,
					e.recast_delay,
					e.resist_adjust,
					e.minlevel,
					e.maxlevel,
					e.min_hp,
					e.max_hp,
					e.bucket_name,
					e.bucket_value,
					e.bucket_comparison
				);
			}
		}
	}

	std::sort(AIBot_spells.begin(), AIBot_spells.end(), [](const BotSpells_Struct& a, const BotSpells_Struct& b) {
		return a.priority > b.priority;
	});

	if (IsValidSpell(attack_proc_spell)) {
		AddProcToWeapon(attack_proc_spell, true, proc_chance);

		if (RuleB(Spells, NPCInnateProcOverride)) {
			innate_proc_spell_id = attack_proc_spell;
		}
	}

	if (IsValidSpell(range_proc_spell)) {
		AddRangedProc(range_proc_spell, (rproc_chance + 100));
	}

	if (IsValidSpell(defensive_proc_spell)) {
		AddDefensiveProc(defensive_proc_spell, (dproc_chance + 100));
	}

	//Set AI casting variables

	AISpellVar.fail_recast = (_fail_recast) ? _fail_recast : RuleI(Spells, AI_SpellCastFinishedFailRecast);
	AISpellVar.engaged_no_sp_recast_min = (_engaged_no_sp_recast_min) ? _engaged_no_sp_recast_min : RuleI(Spells, AI_EngagedNoSpellMinRecast);
	AISpellVar.engaged_no_sp_recast_max = (_engaged_no_sp_recast_max) ? _engaged_no_sp_recast_max : RuleI(Spells, AI_EngagedNoSpellMaxRecast);
	AISpellVar.engaged_beneficial_self_chance = (_engaged_beneficial_self_chance) ? _engaged_beneficial_self_chance : RuleI(Spells, AI_EngagedBeneficialSelfChance);
	AISpellVar.engaged_beneficial_other_chance = (_engaged_beneficial_other_chance) ? _engaged_beneficial_other_chance : RuleI(Spells, AI_EngagedBeneficialOtherChance);
	AISpellVar.engaged_detrimental_chance = (_engaged_detrimental_chance) ? _engaged_detrimental_chance : RuleI(Spells, AI_EngagedDetrimentalChance);
	AISpellVar.pursue_no_sp_recast_min = (_pursue_no_sp_recast_min) ? _pursue_no_sp_recast_min : RuleI(Spells, AI_PursueNoSpellMinRecast);
	AISpellVar.pursue_no_sp_recast_max = (_pursue_no_sp_recast_max) ? _pursue_no_sp_recast_max : RuleI(Spells, AI_PursueNoSpellMaxRecast);
	AISpellVar.pursue_detrimental_chance = (_pursue_detrimental_chance) ? _pursue_detrimental_chance : RuleI(Spells, AI_PursueDetrimentalChance);
	AISpellVar.idle_no_sp_recast_min = (_idle_no_sp_recast_min) ? _idle_no_sp_recast_min : RuleI(Spells, AI_IdleNoSpellMinRecast);
	AISpellVar.idle_no_sp_recast_max = (_idle_no_sp_recast_max) ? _idle_no_sp_recast_max : RuleI(Spells, AI_IdleNoSpellMaxRecast);
	AISpellVar.idle_beneficial_chance = (_idle_beneficial_chance) ? _idle_beneficial_chance : RuleI(Spells, AI_IdleBeneficialChance);

	if (AIBot_spells.empty()) {
		AIautocastspell_timer->Disable();
	} else {
		AIautocastspell_timer->Trigger();
	}
	return true;
}

bool IsSpellInBotList(DBbotspells_Struct* spell_list, uint16 iSpellID) {
	auto it = std::find_if (
		spell_list->entries.begin(),
		spell_list->entries.end(),
		[iSpellID](const DBbotspells_entries_Struct &a) {
			return a.spellid == iSpellID;
		}
	);

	return it != spell_list->entries.end();
}

DBbotspells_Struct* ZoneDatabase::GetBotSpells(uint32 bot_spell_id)
{
	if (!bot_spell_id) {
		return nullptr;
	}

	auto c = bot_spells_cache.find(bot_spell_id);
	if (c != bot_spells_cache.end()) { // it's in the cache, easy =)
		return &c->second;
	}

	if (!bot_spells_loadtried.count(bot_spell_id)) { // no reason to ask the DB again if we have failed once already
		bot_spells_loadtried.insert(bot_spell_id);

		auto n = NpcSpellsRepository::FindOne(content_db, bot_spell_id);
		if (!n.id) {
			return nullptr;
		}

		DBbotspells_Struct spell_set;

		spell_set.parent_list = n.parent_list;
		spell_set.attack_proc = n.attack_proc;
		spell_set.proc_chance = n.proc_chance;
		spell_set.range_proc = n.range_proc;
		spell_set.rproc_chance = n.rproc_chance;
		spell_set.defensive_proc = n.defensive_proc;
		spell_set.dproc_chance = n.dproc_chance;
		spell_set.fail_recast = n.fail_recast;
		spell_set.engaged_no_sp_recast_min = n.engaged_no_sp_recast_min;
		spell_set.engaged_no_sp_recast_max = n.engaged_no_sp_recast_max;
		spell_set.engaged_beneficial_self_chance = n.engaged_b_self_chance;
		spell_set.engaged_beneficial_other_chance = n.engaged_b_other_chance;
		spell_set.engaged_detrimental_chance = n.engaged_d_chance;
		spell_set.pursue_no_sp_recast_min = n.pursue_no_sp_recast_min;
		spell_set.pursue_no_sp_recast_max = n.pursue_no_sp_recast_max;
		spell_set.pursue_detrimental_chance = n.pursue_d_chance;
		spell_set.idle_no_sp_recast_min = n.idle_no_sp_recast_min;
		spell_set.idle_no_sp_recast_max = n.idle_no_sp_recast_max;
		spell_set.idle_beneficial_chance = n.idle_b_chance;

		auto bse = BotSpellsEntriesRepository::GetWhere(
			content_db,
			fmt::format(
				"npc_spells_id = {}",
				bot_spell_id
			)
		);

		if (!bse.empty()) {
			for (const auto& e : bse) {
				DBbotspells_entries_Struct entry;
				entry.spellid = e.spellid;
				entry.type = e.type;
				entry.minlevel = e.minlevel;
				entry.maxlevel = e.maxlevel;
				entry.manacost = e.manacost;
				entry.recast_delay = e.recast_delay;
				entry.priority = e.priority;
				entry.min_hp = e.min_hp;
				entry.max_hp = e.max_hp;
				entry.resist_adjust = e.resist_adjust;
				entry.bucket_name = e.bucket_name;
				entry.bucket_value = e.bucket_value;
				entry.bucket_comparison = e.bucket_comparison;

				// some spell types don't make much since to be priority 0, so fix that
				if (!(entry.type & SPELL_TYPES_INNATE) && entry.priority == 0) {
					entry.priority = 1;
				}

				if (e.resist_adjust) {
					entry.resist_adjust = e.resist_adjust;
				} else if (IsValidSpell(e.spellid)) {
					entry.resist_adjust = spells[e.spellid].resist_difficulty;
				}

				spell_set.entries.push_back(entry);
			}
		}

		bot_spells_cache.emplace(std::make_pair(bot_spell_id, spell_set));

		return &bot_spells_cache[bot_spell_id];
	}

	return nullptr;
}

// adds a spell to the list, taking into account priority and resorting list as needed.
void Bot::AddSpellToBotList(
	int16 in_priority,
	uint16 in_spell_id,
	uint32 in_type,
	int16 in_mana_cost,
	int32 in_recast_delay,
	int16 in_resist_adjust,
	uint8 in_min_level,
	uint8 in_max_level,
	int8 in_min_hp,
	int8 in_max_hp,
	std::string in_bucket_name,
	std::string in_bucket_value,
	uint8 in_bucket_comparison
) {
	if (!IsValidSpell(in_spell_id)) {
		return;
	}

	HasAISpell = true;
	BotSpells_Struct t;

	t.priority          = in_priority;
	t.spellid           = in_spell_id;
	t.type              = in_type;
	t.manacost          = in_mana_cost;
	t.recast_delay      = in_recast_delay;
	t.time_cancast      = 0;
	t.resist_adjust     = in_resist_adjust;
	t.minlevel          = in_min_level;
	t.maxlevel          = in_max_level;
	t.min_hp            = in_min_hp;
	t.max_hp            = in_max_hp;
	t.bucket_name       = in_bucket_name;
	t.bucket_value      = in_bucket_value;
	t.bucket_comparison = in_bucket_comparison;

	AIBot_spells.push_back(t);

	// If we're going from an empty list, we need to start the timer
	if (AIBot_spells.empty()) {
		AIautocastspell_timer->Start(RandomTimer(0, 300), false);
	}
}

// adds spells to the list ^spells that are returned if ^enforce is enabled
void Bot::AddSpellToBotEnforceList(
	int16 iPriority,
	uint16 iSpellID,
	uint32 iType,
	int16 iManaCost,
	int32 iRecastDelay,
	int16 iResistAdjust,
	uint8 min_level,
	uint8 max_level,
	int8 min_hp,
	int8 max_hp,
	std::string bucket_name,
	std::string bucket_value,
	uint8 bucket_comparison
) {
	if (!IsValidSpell(iSpellID)) {
		return;
	}

	HasAISpell = true;
	BotSpells_Struct t;

	t.priority = iPriority;
	t.spellid = iSpellID;
	t.type = iType;
	t.manacost = iManaCost;
	t.recast_delay = iRecastDelay;
	t.time_cancast = 0;
	t.resist_adjust = iResistAdjust;
	t.minlevel = min_level;
	t.maxlevel = maxlevel;
	t.min_hp = min_hp;
	t.max_hp = max_hp;
	t.bucket_name = bucket_name;
	t.bucket_value = bucket_value;
	t.bucket_comparison = bucket_comparison;

	AIBot_spells_enforced.push_back(t);
}

//this gets called from InterruptSpell() for failure or SpellFinished() for success
void Bot::AI_Bot_Event_SpellCastFinished(bool iCastSucceeded, uint16 slot) {
	if (slot == 1) {
		uint32 recovery_time = 0;
		if (iCastSucceeded) {
			if (casting_spell_AIindex < AIBot_spells.size()) {
				recovery_time += spells[AIBot_spells[casting_spell_AIindex].spellid].recovery_time;
				if (AIBot_spells[casting_spell_AIindex].recast_delay >= 0) {
					if (AIBot_spells[casting_spell_AIindex].recast_delay < 10000) {
						AIBot_spells[casting_spell_AIindex].time_cancast = Timer::GetCurrentTime() + (AIBot_spells[casting_spell_AIindex].recast_delay*1000);
					}
				}
				else {
					AIBot_spells[casting_spell_AIindex].time_cancast = Timer::GetCurrentTime() + spells[AIBot_spells[casting_spell_AIindex].spellid].recast_time;
				}
			}
			if (recovery_time < AIautocastspell_timer->GetSetAtTrigger()) {
				recovery_time = AIautocastspell_timer->GetSetAtTrigger();
			}
			AIautocastspell_timer->Start(recovery_time, false);
		}
		else {
			AIautocastspell_timer->Start(AISpellVar.fail_recast, false);
		}
		casting_spell_AIindex = AIBot_spells.size();
	}
}

bool Bot::HasBotSpellEntry(uint16 spellid) {
	auto* spell_list = content_db.GetBotSpells(GetBotSpellID());

	if (!spell_list) {
		return false;
	}

	// Check if Spell ID is found in Bot Spell Entries
	for (auto& e : spell_list->entries) {
		if (spellid == e.spellid) {
			return true;
		}
	}

	return false;
}

bool Bot::IsValidSpellRange(uint16 spell_id, Mob const* tar) {
	if (!IsValidSpell(spell_id)) {
		return false;
	}

	if (tar) {
		int spellrange = (GetActSpellRange(spell_id, spells[spell_id].range) * GetActSpellRange(spell_id, spells[spell_id].range));
		if (spellrange >= DistanceSquared(m_Position, tar->GetPosition())) {
			return true;
		}

		spellrange = (GetActSpellRange(spell_id, spells[spell_id].aoe_range) * GetActSpellRange(spell_id, spells[spell_id].aoe_range));
		if (spellrange >= DistanceSquared(m_Position, tar->GetPosition())) {
			return true;
		}
	}

	return false;
}

BotSpell Bot::GetBestBotSpellForNukeByBodyType(Bot* botCaster, bodyType bodyType) {
	BotSpell result;

	result.SpellId = 0;
	result.SpellIndex = 0;
	result.ManaCost = 0;

	if (!botCaster || !bodyType) {
		return result;
	}

	switch (bodyType) {
		case BT_Undead:
		case BT_SummonedUndead:
		case BT_Vampire: {
			result = GetBestBotSpellForNukeByTargetType(botCaster, ST_Undead);
		}
		case BT_Summoned:
		case BT_Summoned2:
		case BT_Summoned3: {
			if (result.SpellId == 0) {
				result = GetBestBotSpellForNukeByTargetType(botCaster, ST_Summoned);
			}
		}
		case BT_Animal: {
			if (result.SpellId == 0) {
				result = GetBestBotSpellForNukeByTargetType(botCaster, ST_Animal);
			}
		}
		case BT_Plant: {
			if (result.SpellId == 0) {
				result = GetBestBotSpellForNukeByTargetType(botCaster, ST_Plant);
			}
		}
		case BT_Giant: {
			if (result.SpellId == 0) {
				result = GetBestBotSpellForNukeByTargetType(botCaster, ST_Giant);
			}
		}
		case BT_Dragon: {
			if (result.SpellId == 0) {
				result = GetBestBotSpellForNukeByTargetType(botCaster, ST_Dragon);
			}
		}
		default: {
			return result;
		}
	}
}
