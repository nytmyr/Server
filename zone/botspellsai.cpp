/*	EQEMu: Everquest Server Emulator
	Copyright (C) 2001-2016 EQEMu Development Team (http://eqemulator.org)

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; version 2 ogroupf the License.

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
#include "../common/strings.h"
#include "../common/repositories/bot_spells_entries_repository.h"
#include "../common/repositories/npc_spells_repository.h"

#if EQDEBUG >= 12
	#define BotAI_DEBUG_Spells	25
#elif EQDEBUG >= 9
	#define BotAI_DEBUG_Spells	10
#else
	#define BotAI_DEBUG_Spells	-1
#endif

bool Bot::AICastSpell(Mob* tar, uint8 iChance, uint32 iSpellTypes) {

	// Bot AI
	//Raid* raid = entity_list.GetRaidByBotName(this->GetName()); //possible fix
	// Transition directly to raid AI.  Implemented to allow for different response within raids.
	Raid* raid = entity_list.GetRaidByBot(this);
	if (raid) {
		return AICastSpell_Raid(tar, iChance, iSpellTypes);
	}

	if (!tar) {
		return false;
	}

	if (tar->IsFamiliar()) {
		return false;
	}

	if (!AI_HasSpells()) {
		return false;
	}

	if (iChance < 100) {
		if (zone->random.Int(0, 100) > iChance) {
			return false;
		}
	}

	if (tar->GetAppearance() == eaDead) {
		if ((tar->IsClient() && tar->CastToClient()->GetFeigned()) || tar->IsBot()) {
			// do nothing
		}
		else {
			return false;
		}
	}

	uint8 botClass = GetClass();
	uint8 botLevel = GetLevel();

	bool checked_los = false;	//we do not check LOS until we are absolutely sure we need to, and we only do it once.

	bool castedSpell = false;

	BotSpell botSpell;
	botSpell.SpellId = 0;
	botSpell.SpellIndex = 0;
	botSpell.ManaCost = 0;

	switch (iSpellTypes) {
		case SpellType_Mez: {
			if (CanCastBySpellType(this, tar, SpellType_Mez)) {
				if (tar->GetBodyType() != BT_Giant) {
					if (!checked_los) {
						if (!CheckLosFN(tar) || !CheckWaterLoS(this, tar))
							break;	//cannot see target... we assume that no spell is going to work since we will only be casting detrimental spells in this call

						checked_los = true;
					}
					if ((botClass == NECROMANCER) && tar->GetBodyType() != BT_Undead && tar->GetBodyType() != BT_SummonedUndead && tar->GetBodyType() != BT_Vampire)
						break;
					//TODO
					//Check if single target or AoE mez is best
					//if (TARGETS ON MT IS => 3 THEN botSpell = AoEMez)
					//if (TARGETS ON MT IS <= 2 THEN botSpell = BestMez)

					botSpell = GetBestBotSpellForMez(this);

					if (botSpell.SpellId == 0)
						break;
					if (!DoResistCheck(this, tar, botSpell.SpellId, RuleI(Bots, MezResistLimit)))
						break;

					Mob* addMob = GetFirstIncomingMobToMez(this, botSpell);

					if (!addMob) {
						//Say("!addMob.");
						break;
					}

					if (!(!addMob->IsImmuneToSpell(botSpell.SpellId, this) && addMob->CanBuffStack(botSpell.SpellId, botLevel, true) >= 0))
						break;

					

					if (IsValidSpellRange(botSpell.SpellId, addMob)) {
						castedSpell = AIDoSpellCast(botSpell.SpellIndex, addMob, botSpell.ManaCost);
					}
					else {
						break;
					}

					if (castedSpell) {
						BotGroupSay(this, "Attempting to mez %s with [%s].", addMob->GetCleanName(), GetSpellName(botSpell.SpellId));
						//m_mez_delay_timer.Start(GetMezDelay());
					}
				}
				break;
			}
			break;
		}
		case SpellType_Heal: {
			if (CanCastBySpellType(this, tar, SpellType_Heal)) {
				if ((botClass == CLERIC) || (botClass == DRUID) || (botClass == SHAMAN) || (botClass == PALADIN) || (botClass == BEASTLORD) || (botClass == RANGER)) {

					uint8 hpr = tar->GetHPRatio();
					std::string spellType = "None";
					std::string targetName = tar->GetCleanName();
					std::string myName = GetCleanName();
					uint8 fasthealThreshold = 35;
					uint8 regularhealThreshold = 55;
					uint8 completehealThreshold = 70;
					uint8 hothealThreshold = 85;
					uint32 fasthealDelay = 2500;
					uint32 regularhealDelay = 4500;
					uint32 completehealDelay = 8000;
					uint32 hothealDelay = 22000;
					uint32 currentTime = Timer::GetCurrentTime();
					uint32 fasthealTime = tar->DontFastHealMeBefore();
					uint32 regularhealTime = tar->DontRegularHealMeBefore();
					uint32 completehealTime = tar->DontCompleteHealMeBefore();
					uint32 hothealTime = tar->DontHotHealMeBefore();
					uint32 grouphealDelay = RuleI(Bots, GroupHealTimer);
					uint32 grouphealTime = tar->DontHealMeBefore();

					if (tar->IsBot()) {
						fasthealThreshold = tar->CastToBot()->GetFastHealThreshold();
						regularhealThreshold = tar->CastToBot()->GetHealThreshold();
						completehealThreshold = tar->CastToBot()->GetCompleteHealThreshold();
						hothealThreshold = tar->CastToBot()->GetHotHealThreshold();
						fasthealDelay = tar->CastToBot()->GetFastHealDelay();
						regularhealDelay = tar->CastToBot()->GetHealDelay();
						completehealDelay = tar->CastToBot()->GetCompleteHealDelay();
						hothealDelay = tar->CastToBot()->GetHotHealDelay();
					}
					else if (tar->IsClient()) {
						fasthealThreshold = tar->CastToClient()->GetClientFastHealThreshold();
						regularhealThreshold = tar->CastToClient()->GetClientHealThreshold();
						completehealThreshold = tar->CastToClient()->GetClientCompleteHealThreshold();
						hothealThreshold = tar->CastToClient()->GetClientHotHealThreshold();
						fasthealDelay = tar->CastToClient()->GetClientFastHealDelay();
						regularhealDelay = tar->CastToClient()->GetClientHealDelay();
						completehealDelay = tar->CastToClient()->GetClientCompleteHealDelay();
						hothealDelay = tar->CastToClient()->GetClientHotHealDelay();
					}
					else if (tar->IsPet()) {
						fasthealThreshold = 35;
						regularhealThreshold = 55;
						completehealThreshold = 0;
						hothealThreshold = 70;
					}
					
					if (hpr <= fasthealThreshold && fasthealTime <= currentTime) {
						botSpell = GetBestBotSpellForFastHeal(this);
						if (botSpell.SpellId != 0)
							spellType = "Fast Heal";
					}
					if (botSpell.SpellId == 0 && hpr <= regularhealThreshold && regularhealTime <= currentTime) {
						botSpell = GetBestBotSpellForRegularSingleTargetHeal(this);
						if (botSpell.SpellId != 0)
							spellType = "Regular Heal";
					}
					if (botSpell.SpellId == 0 && hpr <= completehealThreshold && completehealTime <= currentTime) {
						if (completehealThreshold <= 90 && GetNumberNeedingHealedInGroup(completehealThreshold, false) >= 3 && grouphealTime <= currentTime)
							botSpell = GetBestBotSpellForGroupHeal(this);
						if (botSpell.SpellId != 0)
							spellType = "Group Regular Heal";
						if (botSpell.SpellId == 0) {
							botSpell = GetBestBotSpellForPercentageHeal(this);
							if (botSpell.SpellId != 0 && (botClass == CLERIC))
								spellType = "Complete Heal";
							if (botSpell.SpellId != 0 && (botClass != CLERIC))
								spellType = "Percentage Heal";
						}
						//if (botSpell.SpellId == 0) {
						//	botSpell = GetBestBotSpellForRegularSingleTargetHeal(this);
						//	if (botSpell.SpellId != 0)
						//		spellType = "Regular Heal";
						//}
					}
					if (botSpell.SpellId == 0 && hpr <= hothealThreshold && hothealTime <= currentTime) {
						if (botSpell.SpellId == 0 && botClass == BARD)
							botSpell = GetFirstBotSpellBySpellType(this, SpellType_Heal);
						if (botSpell.SpellId == 0 && hothealThreshold <= 90 && GetNumberNeedingHealedInGroup(hothealThreshold, false) >= 3 && grouphealTime <= currentTime) {
							botSpell = GetBestBotSpellForGroupHealOverTime(this);
							if (botSpell.SpellId != 0)
								spellType = "Group HoT Heal";
						}
						if (botSpell.SpellId == 0) {
							botSpell = GetBestBotSpellForHealOverTime(this);
							if (botSpell.SpellId != 0)
								spellType = "HoT Heal";
						}
					}
					if (botSpell.SpellId == 0)
						break;

					if (!(spells[botSpell.SpellId].target_type == ST_GroupTeleport || spells[botSpell.SpellId].target_type == ST_Target || tar == this)
						&& !(tar->CanBuffStack(botSpell.SpellId, botLevel, true) >= 0))
						break;

					//if (!tar->CheckSpellLevelRestriction(botSpell.SpellId))
					//	break;

					if (IsValidSpellRange(botSpell.SpellId, tar) || botClass == BARD) {
						if (IsTargetAlreadyReceivingSpell(tar, botSpell.SpellId, spellType)) {
							break;
						}
						castedSpell = AIDoSpellCast(botSpell.SpellIndex, tar, botSpell.ManaCost);
					}
					else {
						break;
					}

					if (castedSpell) {
						if (botClass != BARD) {
							if (IsGroupSpell(botSpell.SpellId)) {
								if (IsRaidGrouped()) {
									uint32 r_group = raid->GetGroup(GetName());
									BotGroupSay(this, "Healing the group with %s [%s].", spells[botSpell.SpellId].name, spellType);
									std::vector<RaidMember> raid_group_members = raid->GetRaidGroupMembers(r_group);
									for (int i = 0; i < raid_group_members.size(); ++i) {
										if (raid_group_members.at(i).member && !raid_group_members.at(i).member->qglobal)
											raid_group_members.at(i).member->SetDontHealMeBefore(currentTime + grouphealDelay);
									}
								}
								else if (IsGrouped()) {
									if (HasGroup()) {
										Group* g = GetGroup();
										if (g) {
											BotGroupSay(this, "Healing the group with %s [%s].", spells[botSpell.SpellId].name, spellType);
											for (int i = 0; i < MAX_GROUP_MEMBERS; i++) {
												if (g->members[i] && !g->members[i]->qglobal)
													g->members[i]->SetDontHealMeBefore(currentTime + grouphealDelay);
											}
										}
									}
								}
							}
							else {
								if (tar != this)
									BotGroupSay(this, "Healing %s with %s [%s]", targetName, spells[botSpell.SpellId].name, spellType);
								if (spellType == "Fast Heal")
									tar->SetDontFastHealMeBefore(currentTime + fasthealDelay);
								if (spellType == "Regular Heal")
									tar->SetDontRegularHealMeBefore(currentTime + regularhealDelay);
								if (spellType == "Complete Heal")
									tar->SetDontCompleteHealMeBefore(currentTime + completehealDelay);
								if (spellType == "HoT Heal")
									tar->SetDontHotHealMeBefore(currentTime + hothealDelay);
							}
						}
					}
				}
			}
			break;
		}
		case SpellType_Root: {
			if (CanCastBySpellType(this, tar, SpellType_Root)) {
				if (!tar->IsRooted()) { // && tar->DontRootMeBefore() < Timer::GetCurrentTime()) {
					if (!checked_los) {
						if (!CheckLosFN(tar) || !CheckWaterLoS(this, tar))
							break;	//cannot see target... we assume that no spell is going to work since we will only be casting detrimental spells in this call

						checked_los = true;
					}

				// TODO: If there is a ranger in the group then don't allow root spells

					/* Original AI
					botSpell = GetFirstBotSpellBySpellType(this, SpellType_Root);
					
					if (!DoResistCheck(this, tar, botSpell.SpellId, RuleI(Bots, RootResistLimit)))
						break;
					
					if (botSpell.SpellId == 0)
						break;
					
					if (tar->CanBuffStack(botSpell.SpellId, botLevel, true) == 0)
						break;
					
					uint32 TempDontRootMeBefore = tar->DontRootMeBefore();
					
					castedSpell = AIDoSpellCast(botSpell.SpellIndex, tar, botSpell.ManaCost, &TempDontRootMeBefore);
					if (castedSpell)
						BotGroupSay(this, "Rooting %s with [%s].", tar->GetCleanName(), GetSpellName(botSpell.SpellId));
					
					m_root_delay_timer.Start(GetRootDelay());
					//if (TempDontRootMeBefore != tar->DontRootMeBefore())
						//tar->SetDontRootMeBefore(TempDontRootMeBefore);
					*/
					std::list<BotSpell_wPriority> spellList = GetPrioritizedBotSpellsBySpellType(this, SpellType_Root);
					for (std::list<BotSpell_wPriority>::iterator itr = spellList.begin(); itr != spellList.end(); ++itr) {
						BotSpell_wPriority botSpell = *itr;
						if (botSpell.SpellId == 0)
							continue;
						if (!CheckSpellRecastTimers(this, botSpell.SpellIndex))
							continue;

						if (!(!tar->IsImmuneToSpell(botSpell.SpellId, this) && tar->CanBuffStack(botSpell.SpellId, botLevel, false) >= 0))
							continue;
						if (!DoResistCheck(this, tar, botSpell.SpellId, RuleI(Bots, RootResistLimit)))
							continue;

						if (IsValidSpellRange(botSpell.SpellId, tar)) {
							castedSpell = AIDoSpellCast(botSpell.SpellIndex, tar, botSpell.ManaCost);
						}
						else {
							continue;
						}

						if (castedSpell) {
							BotGroupSay(this, "Rooting %s with [%s].", tar->GetCleanName(), GetSpellName(botSpell.SpellId));
							//m_root_delay_timer.Start(GetRootDelay());
							break;
						}
					}
					break;
				}
				break;
			}
			break;
		}
		case SpellType_Buff: {
			if (CanCastBySpellType(this, tar, SpellType_Buff)) {
					/* Original AI
					if (tar->DontBuffMeBefore() < Timer::GetCurrentTime()) {
						std::list<BotSpell> buffSpellList = GetBotSpellsBySpellType(this, SpellType_Buff);
						
						for (std::list<BotSpell>::iterator itr = buffSpellList.begin(); itr != buffSpellList.end(); ++itr) {
							BotSpell selectedBotSpell = *itr; 
						*/
						std::list<BotSpell_wPriority> spellList = GetPrioritizedBotSpellsBySpellType(this, SpellType_Buff);
						for (std::list<BotSpell_wPriority>::iterator itr = spellList.begin(); itr != spellList.end(); ++itr) {
							BotSpell_wPriority selectedBotSpell = *itr;

							if (selectedBotSpell.SpellId == 0)
								continue;

							if (tar->IsBlockedBuff(selectedBotSpell.SpellId))
								continue;

							// no pet buffs with illusions.. use #bot command to cast illusions
							// if (IsEffectInSpell(selectedBotSpell.SpellId, SE_Illusion) && tar != this)
							if (IsEffectInSpell(selectedBotSpell.SpellId, SE_Illusion) && tar->IsPet())
								continue;

							//no teleport spells use #bot command to cast teleports
							if (IsEffectInSpell(selectedBotSpell.SpellId, SE_Teleport) || IsEffectInSpell(selectedBotSpell.SpellId, SE_Succor))
								continue;

							// can not cast buffs for your own pet only on another pet that isn't yours
							if ((spells[selectedBotSpell.SpellId].target_type == ST_Pet) && (tar != GetPet()))
								continue;

							// Validate target

							if (
								!(
									(
										spells[selectedBotSpell.SpellId].target_type == ST_Target ||
										spells[selectedBotSpell.SpellId].target_type == ST_Pet ||
										(tar == this && spells[selectedBotSpell.SpellId].target_type != ST_TargetsTarget) ||
										spells[selectedBotSpell.SpellId].target_type == ST_Group ||
										spells[selectedBotSpell.SpellId].target_type == ST_GroupTeleport ||
										(botClass == BARD && spells[selectedBotSpell.SpellId].target_type == ST_AEBard)
										) &&
									!tar->IsImmuneToSpell(selectedBotSpell.SpellId, this) &&
									tar->CanBuffStack(selectedBotSpell.SpellId, botLevel, true) >= 0
									)
								) {
								continue;
							}

							// Put the zone levitate and movement check here since bots are able to bypass the client casting check
							if ((IsEffectInSpell(selectedBotSpell.SpellId, SE_Levitate) && !zone->CanLevitate())
								|| (IsEffectInSpell(selectedBotSpell.SpellId, SE_MovementSpeed) && !zone->CanCastOutdoor())) {
								if (botClass != BARD || !IsSpellUsableThisZoneType(selectedBotSpell.SpellId, zone->GetZoneType())) {
									continue;
								}
							}

							switch (tar->GetArchetype())
							{
							case ARCHETYPE_CASTER:
								//TODO: probably more caster specific spell effects in here
								if (tar && tar->IsBot() && IsEffectInSpell(selectedBotSpell.SpellId, SE_AttackSpeed) && tar->GetLevel() > tar->CastToBot()->GetStopMeleeLevel())
								{
									continue;
								}
								else if (IsEffectInSpell(selectedBotSpell.SpellId, SE_ReverseDS) || IsEffectInSpell(selectedBotSpell.SpellId, SE_ATK) || IsEffectInSpell(selectedBotSpell.SpellId, SE_STR)) {
									continue;
								}
								break;
							case ARCHETYPE_MELEE:
								if (IsEffectInSpell(selectedBotSpell.SpellId, SE_IncreaseSpellHaste) || IsEffectInSpell(selectedBotSpell.SpellId, SE_ManaPool) ||
									IsEffectInSpell(selectedBotSpell.SpellId, SE_CastingLevel) || IsEffectInSpell(selectedBotSpell.SpellId, SE_ManaRegen_v2) ||
									IsEffectInSpell(selectedBotSpell.SpellId, SE_CurrentMana))
								{
									continue;
								}
								break;
							case ARCHETYPE_HYBRID:
								//Hybrids get all buffs
							default:
								break;
							}
							/* Original AI
							if (botClass == ENCHANTER && IsEffectInSpell(selectedBotSpell.SpellId, SE_Rune))
							{
								float manaRatioToCast = 75.0f;
							
								switch (GetBotStance()) {
								case EQ::constants::stanceEfficient:
									manaRatioToCast = 90.0f;
									break;
								case EQ::constants::stanceBalanced:
								case EQ::constants::stanceAggressive:
									manaRatioToCast = 75.0f;
									break;
								case EQ::constants::stanceReactive:
								case EQ::constants::stanceBurn:
								case EQ::constants::stanceBurnAE:
									manaRatioToCast = 50.0f;
									break;
								default:
									manaRatioToCast = 75.0f;
									break;
								}
							
								//If we're at specified mana % or below, don't rune as enchanter
								if (GetManaRatio() <= manaRatioToCast)
									break;
							}
							*/

							if ((IsEffectInSpell(selectedBotSpell.SpellId, SE_ResistMagic) ||
								IsEffectInSpell(selectedBotSpell.SpellId, SE_ResistFire) ||
								IsEffectInSpell(selectedBotSpell.SpellId, SE_ResistCold) ||
								IsEffectInSpell(selectedBotSpell.SpellId, SE_ResistPoison) ||
								IsEffectInSpell(selectedBotSpell.SpellId, SE_ResistDisease) ||
								IsEffectInSpell(selectedBotSpell.SpellId, SE_ResistCorruption) ||
								IsEffectInSpell(selectedBotSpell.SpellId, SE_ResistAll))
								&& !GetAutoResist()) {
								continue;
							}

							if (IsEffectInSpell(selectedBotSpell.SpellId, SE_DamageShield)
								&& !GetAutoDS()) {
								continue;
							}

							if (!CheckSpellRecastTimers(this, selectedBotSpell.SpellIndex))
								continue;

							//uint32 TempDontBuffMeBefore = tar->DontBuffMeBefore();

							//if (!tar->CheckSpellLevelRestriction(selectedBotSpell.SpellId))
							//	break;

							if (IsValidSpellRange(selectedBotSpell.SpellId, tar)) {
								if (IsTargetAlreadyReceivingSpell(tar, selectedBotSpell.SpellId)) {
									continue;
								}
								castedSpell = AIDoSpellCast(selectedBotSpell.SpellIndex, tar, selectedBotSpell.ManaCost);
							}
							else {
								continue;
							}

							//if (TempDontBuffMeBefore != tar->DontBuffMeBefore())
								//tar->SetDontBuffMeBefore(TempDontBuffMeBefore);

							if (castedSpell) {
								BotGroupSay(this, "Buffing %s with [%s].", tar->GetCleanName(), GetSpellName(selectedBotSpell.SpellId));
								//m_buff_delay_timer.Start(GetBuffDelay());
								break;
							}
						}
					//}
				break;
			}
			break;
		}
		case SpellType_Escape: {
			if (CanCastBySpellType(this, tar, SpellType_Escape)) {
				uint8 hpr = (uint8)GetHPRatio();
				bool mayGetAggro = false;
				/* Original AI
#ifdef IPC
				if (hpr <= 5 || (IsNPC() && CastToNPC()->IsInteractive() && tar != this))
#else
				if (hpr > 15 && ((botClass == WIZARD) || (botClass == ENCHANTER) || (botClass == RANGER)))
					mayGetAggro = HasOrMayGetAggro(); //classes have hate reducing spells

				if (hpr < 50 && (botClass == NECROMANCER))
					mayGetAggro = HasOrMayGetAggro(); //classes have hate reducing spells
				
				if (hpr <= 15 || mayGetAggro)
#endif			
				{
					//botSpell = GetFirstBotSpellBySpellType(this, SpellType_Escape);
				*/

					std::list<BotSpell_wPriority> spellList = GetPrioritizedBotSpellsBySpellType(this, SpellType_Escape);
					for (std::list<BotSpell_wPriority>::iterator itr = spellList.begin(); itr != spellList.end(); ++itr) {
						BotSpell_wPriority botSpell = *itr;

						if (botSpell.SpellId == 0)
							continue;
						if (!CheckSpellRecastTimers(this, botSpell.SpellIndex))
							continue;

						if (IsInvulnerabilitySpell(botSpell.SpellId))
							tar = this; //target self for invul type spells

						if (IsValidSpellRange(botSpell.SpellId, tar) || botClass == BARD || botClass == SHADOWKNIGHT || botClass == NECROMANCER) {
							castedSpell = AIDoSpellCast(botSpell.SpellIndex, tar, botSpell.ManaCost);
						}
						else {
							continue;
						}

						if (castedSpell) {
							BotGroupSay(this, "Attempting to escape from %s [%s].", tar->GetCleanName(), GetSpellName(botSpell.SpellId));
							//m_escape_delay_timer.Start(GetEscapeDelay());
							break;
						}
					}
					break;
				//}
				//break;
			}
			break;
		}
		case SpellType_Nuke: {
			if (CanCastBySpellType(this, tar, SpellType_Nuke)) {
				if (!checked_los) {
					if (!CheckLosFN(tar) || !CheckWaterLoS(this, tar))
						break;	//cannot see target... we assume that no spell is going to work since we will only be casting detrimental spells in this call

					checked_los = true;
				}
				/* Original AI
				if (botClass == CLERIC || botClass == ENCHANTER)
				{
					float manaRatioToCast = 75.0f;
				
					switch (GetBotStance()) {
					case EQ::constants::stanceEfficient:
						manaRatioToCast = 90.0f;
						break;
					case EQ::constants::stanceBalanced:
						manaRatioToCast = 75.0f;
						break;
					case EQ::constants::stanceReactive:
					case EQ::constants::stanceAggressive:
						manaRatioToCast = 50.0f;
						break;
					case EQ::constants::stanceBurn:
					case EQ::constants::stanceBurnAE:
						manaRatioToCast = 25.0f;
						break;
					default:
						manaRatioToCast = 50.0f;
						break;
					}
				
					//If we're at specified mana % or below, don't nuke as cleric or enchanter
					if (GetManaRatio() <= manaRatioToCast)
						break;
				}
				*/

				if (botClass == MAGICIAN || botClass == SHADOWKNIGHT || botClass == NECROMANCER || botClass == PALADIN || botClass == RANGER || botClass == DRUID || botClass == CLERIC || botClass == WIZARD) {
					if ((botClass == CLERIC || botClass == PALADIN || botClass == SHADOWKNIGHT || botClass == NECROMANCER) && (tar->GetBodyType() == BT_Undead || tar->GetBodyType() == BT_SummonedUndead || tar->GetBodyType() == BT_Vampire))
						botSpell = GetBestBotSpellForNukeByTargetType(this, ST_Undead);
					else if ((botClass == CLERIC || botClass == RANGER || botClass == DRUID || botClass == MAGICIAN) && (tar->GetBodyType() == BT_Summoned || tar->GetBodyType() == BT_Summoned2 || tar->GetBodyType() == BT_Summoned3))
						botSpell = GetBestBotSpellForNukeByTargetType(this, ST_Summoned);
					else if ((botClass == RANGER || botClass == DRUID) && tar->GetBodyType() == BT_Animal)
						botSpell = GetBestBotSpellForNukeByTargetType(this, ST_Animal);
					else if (botClass == NECROMANCER && tar->GetBodyType() == BT_Plant)
						botSpell = GetBestBotSpellForNukeByTargetType(this, ST_Plant);
					else if (botClass == WIZARD && tar->GetBodyType() == BT_Giant)
						botSpell = GetBestBotSpellForNukeByTargetType(this, ST_Giant);
					else if (botClass == WIZARD && tar->GetBodyType() == BT_Dragon)
						botSpell = GetBestBotSpellForNukeByTargetType(this, ST_Dragon);
				}

				if (botClass == PALADIN || botClass == DRUID || botClass == CLERIC || botClass == 1 || botClass == WIZARD || botClass == ENCHANTER || botClass == NECROMANCER) {
					if (botSpell.SpellId == 0) {
						uint8 stunChance = (tar->IsCasting() ? RuleI(Bots, StunCastChanceIfCasting) : RuleI(Bots, StunCastChanceNormal));

						if (botClass == PALADIN)
							stunChance = RuleI(Bots, StunCastChanceIfCastingPaladins);

						if (!tar->GetSpecialAbility(UNSTUNABLE) && !tar->IsStunned() && (zone->random.Int(1, 100) <= stunChance)) {
							botSpell = GetBestBotSpellForStunByTargetType(this, ST_Target);
						}
					}
				}

				if (botClass == WIZARD && botSpell.SpellId == 0) {
					botSpell = GetBestBotWizardNukeSpellByTargetResists(this, tar);
				}

				if (botSpell.SpellId != 0) {
					if (!(!tar->IsImmuneToSpell(botSpell.SpellId, this) && tar->CanBuffStack(botSpell.SpellId, botLevel, false) >= 0))
						botSpell.SpellId = 0;

					if (IsFearSpell(botSpell.SpellId)) {
						// don't let fear cast if the npc isn't snared or rooted
						if (tar->GetSnaredAmount() == -1) {
							if (!tar->IsRooted())
								botSpell.SpellId = 0;
						}
					}
					if (!DoResistCheck(this, tar, botSpell.SpellId, RuleI(Bots, NukeResistLimit)))
						botSpell.SpellId = 0;

					if (IsValidSpellRange(botSpell.SpellId, tar)) {
						castedSpell = AIDoSpellCast(botSpell.SpellIndex, tar, botSpell.ManaCost);
					}
					else {
						botSpell.SpellId = 0;
					}
					if (castedSpell) {
						BotGroupSay(this, "Nuking %s with [%s].", tar->GetCleanName(), GetSpellName(botSpell.SpellId));
						//m_nuke_delay_timer.Start(GetNukeDelay());
						break;
					}
				}
				else {
					std::list<BotSpell_wPriority> spellList = GetPrioritizedBotSpellsBySpellType(this, SpellType_Nuke);
					for (std::list<BotSpell_wPriority>::iterator itr = spellList.begin(); itr != spellList.end(); ++itr) {
						BotSpell_wPriority botSpell = *itr;
						if (botSpell.SpellId == 0)
							continue;
						if (!CheckSpellRecastTimers(this, botSpell.SpellIndex))
							continue;
						if (GetSpellTargetType(botSpell.SpellId) == ST_Plant && tar->GetBodyType() != BT_Plant)
							continue;
						if (GetSpellTargetType(botSpell.SpellId) == ST_Undead && (tar->GetBodyType() != BT_Undead || tar->GetBodyType() != BT_SummonedUndead || tar->GetBodyType() != BT_Vampire))
							continue;
						if (GetSpellTargetType(botSpell.SpellId) == ST_Summoned && (tar->GetBodyType() != BT_Summoned && tar->GetBodyType() != BT_Summoned2 && tar->GetBodyType() != BT_Summoned3))
							continue;
						if (GetSpellTargetType(botSpell.SpellId) == ST_Giant && tar->GetBodyType() != BT_Giant)
							continue;
						if (GetSpellTargetType(botSpell.SpellId) == ST_Dragon && tar->GetBodyType() != BT_Dragon)
							continue;

						if (!(!tar->IsImmuneToSpell(botSpell.SpellId, this) && tar->CanBuffStack(botSpell.SpellId, botLevel, false) >= 0))
							continue;
						if (IsFearSpell(botSpell.SpellId)) {
							// don't let fear cast if the npc isn't snared or rooted
							if (tar->GetSnaredAmount() == -1) {
								if (!tar->IsRooted())
									break;
							}
						}
						if (!DoResistCheck(this, tar, botSpell.SpellId, RuleI(Bots, NukeResistLimit)))
							continue;

						if (IsValidSpellRange(botSpell.SpellId, tar)) {
							castedSpell = AIDoSpellCast(botSpell.SpellIndex, tar, botSpell.ManaCost);
						}
						else {
							continue;
						}
						if (castedSpell) {
							BotGroupSay(this, "Nuking %s with [%s].", tar->GetCleanName(), GetSpellName(botSpell.SpellId));
							//m_nuke_delay_timer.Start(GetNukeDelay());
							break;
						}
					}
				}		
			}
			break;
		}
		case SpellType_Dispel: {
			if (CanCastBySpellType(this, tar, SpellType_Dispel)) {
				if (!checked_los) {
					if (!CheckLosFN(tar) || !CheckWaterLoS(this, tar))
						break;	//cannot see target... we assume that no spell is going to work since we will only be casting detrimental spells in this call
					checked_los = true;
				}

				if (tar->CountDispellableBuffs() > 0) {
					botSpell = GetFirstBotSpellBySpellType(this, SpellType_Dispel);
					if (!(!tar->IsImmuneToSpell(botSpell.SpellId, this) && tar->CanBuffStack(botSpell.SpellId, botLevel, false) >= 0))
						break;

					if (!DoResistCheck(this, tar, botSpell.SpellId, RuleI(Bots, DispelResistLimit)))
						break;

					if (botSpell.SpellId == 0)
						break;

					if (IsValidSpellRange(botSpell.SpellId, tar)) {
						castedSpell = AIDoSpellCast(botSpell.SpellIndex, tar, botSpell.ManaCost);
					}
					else {
						break;
					}

					if (castedSpell) {
						BotGroupSay(this, "Attempting to dispel %s with [%s].", tar->GetCleanName(), GetSpellName(botSpell.SpellId));
						//m_dispel_delay_timer.Start(GetDispelDelay());
					}
				}
			}
			break;
		}
		case SpellType_Pet: {
			if (CanCastBySpellType(this, tar, SpellType_Pet)) {
				//keep mobs from recasting pets when they have them.
				if (!IsPet() && !GetPetID() && !IsBotCharmer()) {
					if (botClass == WIZARD) {
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
						if (GetPetID())
							break;

						if (familiar_buff_slot >= 0) {
							BuffFadeBySlot(familiar_buff_slot);
							break;
						}

						botSpell = GetFirstBotSpellBySpellType(this, SpellType_Pet);
					}
					else if (botClass == MAGICIAN) {
						botSpell = GetBestBotMagicianPetSpell(this);
					}
					else {
						botSpell = GetFirstBotSpellBySpellType(this, SpellType_Pet);
					}

					if (botSpell.SpellId == 0)
						break;

					castedSpell = AIDoSpellCast(botSpell.SpellIndex, tar, botSpell.ManaCost);

					if (castedSpell) {
						BotGroupSay(this, "Summoning a pet [%s].", GetSpellName(botSpell.SpellId));
					}
				}
				break;
			}
			break;
		}
		case SpellType_InCombatBuff: {
			if (CanCastBySpellType(this, tar, SpellType_InCombatBuff)) {
				if (!checked_los && (botClass != BARD || botClass != CLERIC || botClass != PALADIN || botClass != SHAMAN)) {
					if (!CheckLosFN(tar) || !CheckWaterLoS(this, tar))
						break;	//cannot see target... we assume that no spell is going to work since we will only be casting detrimental spells in this call

					checked_los = true;
				}

				if ((botClass != BARD)) {
					if (((botClass == CLERIC) || (botClass == PALADIN) || (botClass == SHADOWKNIGHT)) && GetLevel() >= GetStopMeleeLevel())
						break;

					std::list<BotSpell_wPriority> spellList = GetPrioritizedBotSpellsBySpellType(this, SpellType_InCombatBuff);
					for (std::list<BotSpell_wPriority>::iterator itr = spellList.begin(); itr != spellList.end(); ++itr) {
						BotSpell_wPriority botSpell = *itr;
						if (botSpell.SpellId == 0)
							continue;
						if (!CheckSpellRecastTimers(this, botSpell.SpellIndex))
							continue;

						if (!(!tar->IsImmuneToSpell(botSpell.SpellId, this) && tar->CanBuffStack(botSpell.SpellId, botLevel, false) >= 0))
							continue;
						if ((botClass == SHADOWKNIGHT) && !DoResistCheck(this, tar, botSpell.SpellId, RuleI(Bots, InCombatBuffResistLimit)))
							continue;

						if (IsValidSpellRange(botSpell.SpellId, tar) || botClass != SHADOWKNIGHT) {
							castedSpell = AIDoSpellCast(botSpell.SpellIndex, tar, botSpell.ManaCost);
						}
						else {
							continue;
						}
						
						if (castedSpell) {
							//m_incombatbuff_delay_timer.Start(GetInCombatBuffDelay());
							if (botClass == SHADOWKNIGHT) {
								BotGroupSay(this, "Using an In-Combat Buff on %s with [%s].", tar->GetCleanName(), GetSpellName(botSpell.SpellId));
							}
							break;
						}
					}
				}
				else if (botClass == BARD) {
					//if (tar->DontBuffMeBefore() < Timer::GetCurrentTime()) {
						std::list<BotSpell> inCombatBuffList = GetBotSpellsBySpellType(this, SpellType_InCombatBuff);

						for (std::list<BotSpell>::iterator itr = inCombatBuffList.begin(); itr != inCombatBuffList.end(); ++itr) {
							BotSpell selectedBotSpell = *itr;

							if (selectedBotSpell.SpellId == 0)
								continue;

							if (CheckSpellRecastTimers(this, itr->SpellIndex)) {
								uint32 TempDontBuffMeBefore = tar->DontBuffMeBefore();

								// no buffs with illusions.. use #bot command to cast illusions
								if (IsEffectInSpell(selectedBotSpell.SpellId, SE_Illusion) && tar != this)
									continue;

								//no teleport spells use #bot command to cast teleports
								if (IsEffectInSpell(selectedBotSpell.SpellId, SE_Teleport) || IsEffectInSpell(selectedBotSpell.SpellId, SE_Succor))
									continue;

								// can not cast buffs for your own pet only on another pet that isn't yours
								if ((spells[selectedBotSpell.SpellId].target_type == ST_Pet) && (tar != GetPet()))
									continue;

								// Validate target

								if (!((spells[selectedBotSpell.SpellId].target_type == ST_Target || spells[selectedBotSpell.SpellId].target_type == ST_Pet || tar == this ||
									spells[selectedBotSpell.SpellId].target_type == ST_Group || spells[selectedBotSpell.SpellId].target_type == ST_GroupTeleport ||
									(botClass == BARD && spells[selectedBotSpell.SpellId].target_type == ST_AEBard))
									&& !tar->IsImmuneToSpell(selectedBotSpell.SpellId, this)
									&& (tar->CanBuffStack(selectedBotSpell.SpellId, botLevel, true) >= 0))) {
									continue;
								}

								// Put the zone levitate and movement check here since bots are able to bypass the client casting check
								if ((IsEffectInSpell(selectedBotSpell.SpellId, SE_Levitate) && !zone->CanLevitate())
									|| (IsEffectInSpell(selectedBotSpell.SpellId, SE_MovementSpeed) && !zone->CanCastOutdoor())) {
									if (!IsSpellUsableThisZoneType(selectedBotSpell.SpellId, zone->GetZoneType())) {
										continue;
									}
								}

								if (!IsGroupSpell(selectedBotSpell.SpellId)) {
									//Only check archetype if song is not a group spell
									switch (tar->GetArchetype()) {
									case ARCHETYPE_CASTER:
										//TODO: probably more caster specific spell effects in here
										if (tar && tar->IsBot() && IsEffectInSpell(selectedBotSpell.SpellId, SE_AttackSpeed) && tar->GetLevel() > tar->CastToBot()->GetStopMeleeLevel())
										{
											continue;
										}
										else if (IsEffectInSpell(selectedBotSpell.SpellId, SE_ReverseDS) || IsEffectInSpell(selectedBotSpell.SpellId, SE_ATK) || IsEffectInSpell(selectedBotSpell.SpellId, SE_STR)) {
											continue;
										}
										break;
									case ARCHETYPE_MELEE:
										if (IsEffectInSpell(selectedBotSpell.SpellId, SE_IncreaseSpellHaste) || IsEffectInSpell(selectedBotSpell.SpellId, SE_ManaPool) ||
											IsEffectInSpell(selectedBotSpell.SpellId, SE_CastingLevel) || IsEffectInSpell(selectedBotSpell.SpellId, SE_ManaRegen_v2) ||
											IsEffectInSpell(selectedBotSpell.SpellId, SE_CurrentMana))
										{
											continue;
										}
										break;
									case ARCHETYPE_HYBRID:
										//Hybrids get all buffs
									default:
										break;
									}
								}

								castedSpell = AIDoSpellCast(selectedBotSpell.SpellIndex, tar, selectedBotSpell.ManaCost, &TempDontBuffMeBefore);

								//if (TempDontBuffMeBefore != tar->DontBuffMeBefore())
									//tar->SetDontBuffMeBefore(TempDontBuffMeBefore);
							}

							if (castedSpell)
								break;
						}
					//}
				}
				break;
			}
			break;
		}
		case SpellType_Lifetap: {
			if (CanCastBySpellType(this, tar, SpellType_Lifetap)) {
				if (!checked_los) {
					if (!CheckLosFN(tar) || !CheckWaterLoS(this, tar))
						break;	//cannot see target... we assume that no spell is going to work since we will only be casting detrimental spells in this call

					checked_los = true;
				}
				/* Original AI
				botSpell = GetFirstBotSpellBySpellType(this, SpellType_Lifetap);

				if (!DoResistCheck(this, tar, botSpell.SpellId, RuleI(Bots, LifetapResistLimit)))
					break;

				if (botSpell.SpellId == 0)
					break;

				if (!(!tar->IsImmuneToSpell(botSpell.SpellId, this) && (tar->CanBuffStack(botSpell.SpellId, botLevel, true) >= 0)))
					break;

				castedSpell = AIDoSpellCast(botSpell.SpellIndex, tar, botSpell.ManaCost);
				if (castedSpell)
					BotGroupSay(this, "Lifetapping %s with [%s].", tar->GetCleanName(), GetSpellName(botSpell.SpellId));

				m_lifetap_delay_timer.Start(GetLifetapDelay());
				*/
				std::list<BotSpell_wPriority> spellList = GetPrioritizedBotSpellsBySpellType(this, SpellType_Lifetap);
				for (std::list<BotSpell_wPriority>::iterator itr = spellList.begin(); itr != spellList.end(); ++itr) {
					BotSpell_wPriority botSpell = *itr;
					if (botSpell.SpellId == 0)
						continue;
					if (!CheckSpellRecastTimers(this, botSpell.SpellIndex))
						continue;

					if (!(!tar->IsImmuneToSpell(botSpell.SpellId, this) && tar->CanBuffStack(botSpell.SpellId, botLevel, false) >= 0))
						continue;
					if (!DoResistCheck(this, tar, botSpell.SpellId, RuleI(Bots, LifetapResistLimit)))
						continue;

					if (IsValidSpellRange(botSpell.SpellId, tar)) {
						castedSpell = AIDoSpellCast(botSpell.SpellIndex, tar, botSpell.ManaCost);
					}
					else {
						continue;
					}

					if (castedSpell) {
						BotGroupSay(this, "Lifetapping %s with [%s].", tar->GetCleanName(), GetSpellName(botSpell.SpellId));
						//m_lifetap_delay_timer.Start(GetLifetapDelay());
						break;
					}
				}
				break;
			}
			break;
		}
		case SpellType_Snare: {
			if (CanCastBySpellType(this, tar, SpellType_Snare)) {
				//if (tar->DontSnareMeBefore() < Timer::GetCurrentTime()) {
					if (!checked_los) {
						if (!CheckLosFN(tar) || !CheckWaterLoS(this, tar))
							break;	//cannot see target... we assume that no spell is going to work since we will only be casting detrimental spells in this call

						checked_los = true;
					}
					/* Original AI
					botSpell = GetFirstBotSpellBySpellType(this, SpellType_Snare);

					if (!DoResistCheck(this, tar, botSpell.SpellId, RuleI(Bots, SnareResistLimit)))
						break;

					if (botSpell.SpellId == 0)
						break;

					if (!(!tar->IsImmuneToSpell(botSpell.SpellId, this) && tar->CanBuffStack(botSpell.SpellId, botLevel, true) >= 0))
						break;

					uint32 TempDontSnareMeBefore = tar->DontSnareMeBefore();

				if (IsValidSpellRange(botSpell.SpellId, tar)) {
					castedSpell = AIDoSpellCast(botSpell.SpellIndex, tar, botSpell.ManaCost, &TempDontSnareMeBefore);
					if (castedSpell)
						BotGroupSay(this, "Snaring %s with [%s].", tar->GetCleanName(), GetSpellName(botSpell.SpellId));

					m_snare_delay_timer.Start(GetSnareDelay());
					//if (TempDontSnareMeBefore != tar->DontSnareMeBefore())
						//tar->SetDontSnareMeBefore(TempDontSnareMeBefore);
					*/
					std::list<BotSpell_wPriority> spellList = GetPrioritizedBotSpellsBySpellType(this, SpellType_Snare);
					for (std::list<BotSpell_wPriority>::iterator itr = spellList.begin(); itr != spellList.end(); ++itr) {
						BotSpell_wPriority botSpell = *itr;
						if (botSpell.SpellId == 0)
							continue;
						if (!CheckSpellRecastTimers(this, botSpell.SpellIndex))
							continue;

						if (!(!tar->IsImmuneToSpell(botSpell.SpellId, this) && tar->CanBuffStack(botSpell.SpellId, botLevel, false) >= 0))
							continue;
						if (!DoResistCheck(this, tar, botSpell.SpellId, RuleI(Bots, SnareResistLimit)))
							continue;

						if (IsValidSpellRange(botSpell.SpellId, tar)) {
							castedSpell = AIDoSpellCast(botSpell.SpellIndex, tar, botSpell.ManaCost);
						}
						else {
							continue;
						}

						if (castedSpell) {
							BotGroupSay(this, "Snaring %s with [%s].", tar->GetCleanName(), GetSpellName(botSpell.SpellId));
							//m_snare_delay_timer.Start(GetSnareDelay());
							break;
						}
					}
					break;
				//}
				break;
			}
			break;
		}
		case SpellType_DOT: {
			if (CanCastBySpellType(this, tar, SpellType_DOT)) {
				//if (tar->DontDotMeBefore() < Timer::GetCurrentTime() && tar->GetHPRatio() > 15.0f) {
					if (!checked_los) {
						if (!CheckLosFN(tar) || !CheckWaterLoS(this, tar))
							break;	//cannot see target... we assume that no spell is going to work since we will only be casting detrimental spells in this call

						checked_los = true;
					}

					if (botClass == BARD) {
						std::list<BotSpell_wPriority> dotList = GetPrioritizedBotSpellsBySpellType(this, SpellType_DOT);

						const int maxDotSelect = 5;
						int dotSelectCounter = 0;

						for (std::list<BotSpell_wPriority>::iterator itr = dotList.begin(); itr != dotList.end(); ++itr) {
							BotSpell_wPriority selectedBotSpell = *itr;

							if (selectedBotSpell.SpellId == 0)
								continue;
							if (!CheckSpellRecastTimers(this, selectedBotSpell.SpellIndex))
								continue;

							if (!(!tar->IsImmuneToSpell(selectedBotSpell.SpellId, this) && tar->CanBuffStack(selectedBotSpell.SpellId, botLevel, true) >= 0))
								continue;

							//uint32 TempDontDotMeBefore = tar->DontDotMeBefore();

							if (!DoResistCheck(this, tar, selectedBotSpell.SpellId, RuleI(Bots, DoTResistLimit)))
								continue;

							if (IsValidSpellRange(selectedBotSpell.SpellId, tar)) {
								castedSpell = AIDoSpellCast(selectedBotSpell.SpellIndex, tar, selectedBotSpell.ManaCost);
							}
							else {
								continue;
							}

							if (castedSpell) {
								BotGroupSay(this, "Casting a DoT on %s with [%s].", tar->GetCleanName(), GetSpellName(selectedBotSpell.SpellId));
								//m_dot_delay_timer.Start(GetDotDelay());
							}
							//if (TempDontDotMeBefore != tar->DontDotMeBefore())
								//tar->SetDontDotMeBefore(TempDontDotMeBefore);

							dotSelectCounter++;

							if ((dotSelectCounter == maxDotSelect) || castedSpell)
								break;
						}
					}
					else {
						/* Original AI
						std::list<BotSpell> dotList = GetBotSpellsBySpellType(this, SpellType_DOT);

						const int maxDotSelect = 5;
						int dotSelectCounter = 0;

						for (std::list<BotSpell>::iterator itr = dotList.begin(); itr != dotList.end(); ++itr) {
							BotSpell selectedBotSpell = *itr;

							if (selectedBotSpell.SpellId == 0)
								continue;

							if (CheckSpellRecastTimers(this, itr->SpellIndex))
							{

								if (!(!tar->IsImmuneToSpell(selectedBotSpell.SpellId, this) && tar->CanBuffStack(selectedBotSpell.SpellId, botLevel, true) >= 0))
									continue;

								uint32 TempDontDotMeBefore = tar->DontDotMeBefore();

								if (!DoResistCheck(this, tar, selectedBotSpell.SpellId, RuleI(Bots, DoTResistLimit)))
									return selectedBotSpell.SpellId = 0;

								castedSpell = AIDoSpellCast(selectedBotSpell.SpellIndex, tar, selectedBotSpell.ManaCost, &TempDontDotMeBefore);
								if (castedSpell)
									BotGroupSay(this, "Casting a DoT on %s with [%s].", tar->GetCleanName(), GetSpellName(selectedBotSpell.SpellId));

								m_dot_delay_timer.Start(GetDotDelay());
								//if (TempDontDotMeBefore != tar->DontDotMeBefore())
									//tar->SetDontDotMeBefore(TempDontDotMeBefore);
							}

							dotSelectCounter++;

							if ((dotSelectCounter == maxDotSelect) || castedSpell)
								break;
							*/
							const int maxDotSelect = 5;
							int dotSelectCounter = 0;

							std::list<BotSpell_wPriority> spellList = GetPrioritizedBotSpellsBySpellType(this, SpellType_DOT);
							for (std::list<BotSpell_wPriority>::iterator itr = spellList.begin(); itr != spellList.end(); ++itr) {
								BotSpell_wPriority botSpell = *itr;
								if (botSpell.SpellId == 0)
									continue;
								if (!CheckSpellRecastTimers(this, botSpell.SpellIndex))
									continue;

								if (!(!tar->IsImmuneToSpell(botSpell.SpellId, this) && tar->CanBuffStack(botSpell.SpellId, botLevel, false) >= 0))
									continue;
								if (!DoResistCheck(this, tar, botSpell.SpellId, RuleI(Bots, DoTResistLimit)))
									continue;

								if (IsValidSpellRange(botSpell.SpellId, tar)) {
									castedSpell = AIDoSpellCast(botSpell.SpellIndex, tar, botSpell.ManaCost);
								}
								else {
									continue;
								}

								if (castedSpell) {
									BotGroupSay(this, "Casting a DoT on %s with [%s].", tar->GetCleanName(), GetSpellName(botSpell.SpellId));
									//m_dot_delay_timer.Start(GetDotDelay());
								}

								dotSelectCounter++;

								if ((dotSelectCounter == maxDotSelect) || castedSpell)
									break;
							}
							break;
						//}
					}
				//}
				break;
			}
			break;
		}
		case SpellType_Slow: {
			if (CanCastBySpellType(this, tar, SpellType_Slow)) {
				if (!checked_los) {
					if (!CheckLosFN(tar) || !CheckWaterLoS(this, tar))
						break;	//cannot see target... we assume that no spell is going to work since we will only be casting detrimental spells in this call

					checked_los = true;
				}

				switch (botClass) {
					case BARD: {
						// probably needs attackable check
						std::list<BotSpell_wPriority> botSongList = GetPrioritizedBotSpellsBySpellType(this, SpellType_Slow);
						for (auto iter : botSongList) {
							if (!iter.SpellId)
								continue;
							if (!CheckSpellRecastTimers(this, iter.SpellIndex))
								continue;

							if (spells[iter.SpellId].zone_type != -1 && zone->GetZoneType() != -1 && spells[iter.SpellId].zone_type != zone->GetZoneType() && zone->CanCastOutdoor() != 1) // is this bit or index?
								continue;
							if (spells[iter.SpellId].target_type != ST_Target)
								continue;
							if (tar->CanBuffStack(iter.SpellId, botLevel, true) < 0)
								continue;

							if (!DoResistCheck(this, tar, iter.SpellId, RuleI(Bots, SlowResistLimit)))
								continue;

							if (IsValidSpellRange(iter.SpellId, tar)) {
								castedSpell = AIDoSpellCast(iter.SpellIndex, tar, iter.ManaCost);
							}

							if (!castedSpell)
								continue;

							//if (castedSpell)
								//BotGroupSay(this, "Slowing %s with [%s].", tar->GetCleanName(), GetSpellName(iter.SpellId));

							if (castedSpell) {
								//m_slow_delay_timer.Start(GetSlowDelay());
								break;
							}
						}

						break;
					}
					case ENCHANTER: {
						botSpell = GetBestBotSpellForMagicBasedSlow(this);
						break;
					}
					case SHAMAN:
					case BEASTLORD: {
						botSpell = GetBestBotSpellForMagicBasedSlow(this);
						if (!DoResistCheck(this, tar, botSpell.SpellId, RuleI(Bots, SlowResistLimit))) {
							botSpell = GetBestBotSpellForDiseaseBasedSlow(this);
						}
						break;
					}
				}

				if (!DoResistCheck(this, tar, botSpell.SpellId, RuleI(Bots, SlowResistLimit)))
					break;

				if (botSpell.SpellId == 0)
					break;

				if (!CheckSpellRecastTimers(this, botSpell.SpellIndex))
					break;

				if (!(!tar->IsImmuneToSpell(botSpell.SpellId, this) && tar->CanBuffStack(botSpell.SpellId, botLevel, false) >= 0))
					break;

				if (IsValidSpellRange(botSpell.SpellId, tar)) {
					castedSpell = AIDoSpellCast(botSpell.SpellIndex, tar, botSpell.ManaCost);
				}
				else {
					break;
				}

				if (castedSpell) {
					if (botClass != BARD) {
						BotGroupSay(this, "Attempting to slow %s with [%s].", tar->GetCleanName(), GetSpellName(botSpell.SpellId));
					}
					//m_slow_delay_timer.Start(GetSlowDelay());
				}
			}
			break;
		}
		case SpellType_Debuff: {
			if (CanCastBySpellType(this, tar, SpellType_Debuff)) {
				//if (tar->GetHPRatio() > 25.0f)
				//{
					if (!checked_los) {
						if (!CheckLosFN(tar) || !CheckWaterLoS(this, tar))
							break;	//cannot see target... we assume that no spell is going to work since we will only be casting detrimental spells in this call

						checked_los = true;
					}
					/* Original AI
					botSpell = GetBestBotSpellForResistDebuff(this, tar);
					
					if (botSpell.SpellId == 0)
						botSpell = GetDebuffBotSpell(this, tar);
					
					if (!DoResistCheck(this, tar, botSpell.SpellId, RuleI(Bots, DebuffResistLimit)))
						break;
					
					if (botSpell.SpellId == 0)
						break;
					
					if (!(!tar->IsImmuneToSpell(botSpell.SpellId, this) && (tar->CanBuffStack(botSpell.SpellId, botLevel, true) >= 0)))
						break;
					
					castedSpell = AIDoSpellCast(botSpell.SpellIndex, tar, botSpell.ManaCost);
					if (castedSpell)
						BotGroupSay(this, "Debuffing %s with [%s].", tar->GetCleanName(), GetSpellName(botSpell.SpellId));
					
					m_debuff_delay_timer.Start(GetDebuffDelay());
					*/
					std::list<BotSpell_wPriority> spellList = GetPrioritizedBotSpellsBySpellType(this, SpellType_Debuff);
					for (std::list<BotSpell_wPriority>::iterator itr = spellList.begin(); itr != spellList.end(); ++itr) {
						BotSpell_wPriority botSpell = *itr;
						if (botSpell.SpellId == 0)
							continue;
						if (!CheckSpellRecastTimers(this, botSpell.SpellIndex))
							continue;
							
						if (!(!tar->IsImmuneToSpell(botSpell.SpellId, this) && tar->CanBuffStack(botSpell.SpellId, botLevel, false) >= 0))
							continue;
						if (!DoResistCheck(this, tar, botSpell.SpellId, RuleI(Bots, DebuffResistLimit)))
							continue;

						if (IsValidSpellRange(botSpell.SpellId, tar)) {
							castedSpell = AIDoSpellCast(botSpell.SpellIndex, tar, botSpell.ManaCost);
						}
						else {
							continue;
						}

						if (castedSpell && botClass != BARD) {
							BotGroupSay(this, "Debuffing %s with [%s].", tar->GetCleanName(), GetSpellName(botSpell.SpellId));
							//m_debuff_delay_timer.Start(GetDebuffDelay());
							break;
						}
					}
					break;

				//}
			}
			break;
		}
		case SpellType_Cure: {
			if (CanCastBySpellType(this, tar, SpellType_Cure)) {
				//if (GetNeedsCured(tar) && (tar->DontCureMeBefore() < Timer::GetCurrentTime()) && !(GetNumberNeedingHealedInGroup(25, false) > 0) && !(GetNumberNeedingHealedInGroup(40, false) > 2))
				if (GetNeedsCured(tar) && !(GetNumberNeedingHealedInGroup(25, false) > 0) && !(GetNumberNeedingHealedInGroup(40, false) > 2))
				{
					botSpell = GetBestBotSpellForCure(this, tar, false);

					if (botSpell.SpellId == 0)
						break;

					if (!IsValidSpellRange(botSpell.SpellId, tar) && IsGroupSpell(botSpell.SpellId)) {
						botSpell = GetBestBotSpellForCure(this, tar, true);
					}

					if (botSpell.SpellId == 0)
						break;

					if (!CheckSpellRecastTimers(this, botSpell.SpellIndex))
						break;

					//uint32 TempDontCureMeBeforeTime = tar->DontCureMeBefore();

					if (IsValidSpellRange(botSpell.SpellId, tar)) {
						if (IsTargetAlreadyReceivingSpell(tar, botSpell.SpellId)) {
							break;
						}
						castedSpell = AIDoSpellCast(botSpell.SpellIndex, tar, botSpell.ManaCost);
					}
					else {
						break;
					}

					if (castedSpell) {
						BotGroupSay(this, "Attempting to cure %s with [%s].", tar->GetCleanName(), GetSpellName(botSpell.SpellId));
						//m_cure_delay_timer.Start(GetCureDelay());
					}
				}
				break;
			}
			break;
		}
		case SpellType_Resurrect:
			break;
		case SpellType_HateRedux: {
			if (CanCastBySpellType(this, tar, SpellType_HateRedux)) {
				// assumed group member at this point
				if (botClass == BARD) {
					std::list<BotSpell_wPriority> botSongList = GetPrioritizedBotSpellsBySpellType(this, SpellType_HateRedux);
					for (auto iter : botSongList) {
						if (!iter.SpellId)
							continue;
						if (!CheckSpellRecastTimers(this, iter.SpellIndex))
							continue;

						if (spells[iter.SpellId].zone_type != -1 && zone->GetZoneType() != -1 && spells[iter.SpellId].zone_type != zone->GetZoneType() && zone->CanCastOutdoor() != 1) // is this bit or index?
							continue;
						if (spells[iter.SpellId].target_type != ST_Target)
							continue;
						if (tar->CanBuffStack(iter.SpellId, botLevel, true) < 0)
							continue;

						if (!DoResistCheck(this, tar, botSpell.SpellId, RuleI(Bots, HateReduxResistLimit)))
							continue;

						if (IsValidSpellRange(iter.SpellId, tar)) {
							castedSpell = AIDoSpellCast(iter.SpellIndex, tar, iter.ManaCost);
						}
						else {
							continue;
						}

						if (castedSpell) {
							BotGroupSay(this, "Attempting to reduce my hate on %s with [%s].", tar->GetCleanName(), GetSpellName(iter.SpellId));
							//m_hateredux_delay_timer.Start(GetHateReduxDelay());
							break;
						}
					}
				}
				else {
					std::list<BotSpell_wPriority> spellList = GetPrioritizedBotSpellsBySpellType(this, SpellType_HateRedux);
					for (auto iter : spellList) {
						if (!iter.SpellId)
							continue;
						if (!CheckSpellRecastTimers(this, iter.SpellIndex))
							continue;

						if (IsValidSpellRange(iter.SpellId, tar)) {
							castedSpell = AIDoSpellCast(iter.SpellIndex, tar, iter.ManaCost);
						}
						else {
							continue;
						}

						if (castedSpell) {
							BotGroupSay(this, "Attempting to reduce my hate on %s with [%s].", tar->GetCleanName(), GetSpellName(iter.SpellId));
							//m_hateredux_delay_timer.Start(GetHateReduxDelay());
							break;
						}
					}
				}

				break;
			}
			break;
		}
		case SpellType_InCombatBuffSong: {
			if (CanCastBySpellType(this, tar, SpellType_InCombatBuffSong)) {
				if (botClass != BARD || tar != this) // In-Combat songs can be cast Out-of-Combat in preparation for battle
					break;

				std::list<BotSpell_wPriority> botSongList = GetPrioritizedBotSpellsBySpellType(this, SpellType_InCombatBuffSong);
				for (auto iter : botSongList) {
					if (!iter.SpellId)
						continue;
					if (!CheckSpellRecastTimers(this, iter.SpellIndex))
						continue;

					if (spells[iter.SpellId].zone_type != -1 && zone->GetZoneType() != -1 && spells[iter.SpellId].zone_type != zone->GetZoneType() && zone->CanCastOutdoor() != 1) // is this bit or index?
						continue;
					switch (spells[iter.SpellId].target_type) {
					case ST_AEBard:
					case ST_AECaster:
					case ST_GroupTeleport:
					case ST_Group:
					case ST_Self:
						break;
					default:
						continue;
					}
					if (tar->CanBuffStack(iter.SpellId, botLevel, true) < 0)
						continue;

					castedSpell = AIDoSpellCast(iter.SpellIndex, tar, iter.ManaCost);

					if (castedSpell)
						break;
				}

				break;
			}
			break;
		}
		case SpellType_OutOfCombatBuffSong: {
			if (CanCastBySpellType(this, tar, SpellType_OutOfCombatBuffSong)) {
				if (botClass != BARD || tar != this || IsEngaged()) // Out-of-Combat songs can not be cast in combat
					break;

				std::list<BotSpell_wPriority> botSongList = GetPrioritizedBotSpellsBySpellType(this, SpellType_OutOfCombatBuffSong);
				for (auto iter : botSongList) {
					if (!iter.SpellId)
						continue;
					if (!CheckSpellRecastTimers(this, iter.SpellIndex))
						continue;

					if (spells[iter.SpellId].zone_type != -1 && zone->GetZoneType() != -1 && spells[iter.SpellId].zone_type != zone->GetZoneType() && zone->CanCastOutdoor() != 1) // is this bit or index?
						continue;
					switch (spells[iter.SpellId].target_type) {
					case ST_AEBard:
					case ST_AECaster:
					case ST_GroupTeleport:
					case ST_Group:
					case ST_Self:
						break;
					default:
						continue;
					}
					if (tar->CanBuffStack(iter.SpellId, botLevel, true) < 0)
						continue;

					castedSpell = AIDoSpellCast(iter.SpellIndex, tar, iter.ManaCost);

					if (castedSpell)
						break;
				}

				break;
			}
			break;
		}
		case SpellType_PreCombatBuff: {
			if (CanCastBySpellType(this, tar, SpellType_PreCombatBuff)) {
				break;
			}
			break;
		}
		case SpellType_PreCombatBuffSong: {
			if (CanCastBySpellType(this, tar, SpellType_PreCombatBuffSong)) {
				break;
			}
			break;
		}
		default:
			break;
	}

	return castedSpell;
}

bool Bot::AIDoSpellCast(uint8 i, Mob* tar, int32 mana_cost, uint32* oDontDoAgainBefore) {
	bool result = false;

	// manacost has special values, -1 is no mana cost, -2 is instant cast (no mana)
	int32 manaCost = mana_cost;

	if (manaCost == -1)
		manaCost = spells[AIBot_spells[i].spellid].mana;
	else if (manaCost == -2)
		manaCost = 0;

	int32 extraMana = 0;
	int32 hasMana = GetMana();

	// Allow bots to cast buff spells even if they are out of mana
	if (RuleB(Bots, FinishBuffing)) {
		if (manaCost > hasMana) {
			// Let's have the bots complete the buff time process
			if (AIBot_spells[i].type & SpellType_Buff) {
				extraMana = manaCost - hasMana;
				SetMana(manaCost);
			}
		}
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
			GetMana() == GetMaxMana()
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
		extraMana = false;
	}
	else { //handle spell recast and recast timers
		//if (GetClass() == BARD && IsGroupSpell(AIBot_spells[i].spellid)) {
		//	// Bard buff songs have been moved to their own npc spell type..
		//	// Buff stacking is now checked as opposed to manipulating the timer to avoid rapid casting

		//	//AIBot_spells[i].time_cancast = (spells[AIBot_spells[i].spellid].recast_time > (spells[AIBot_spells[i].spellid].buffduration * 6000)) ? Timer::GetCurrentTime() + spells[AIBot_spells[i].spellid].recast_time : Timer::GetCurrentTime() + spells[AIBot_spells[i].spellid].buffduration * 6000;
		//	//spellend_timer.Start(spells[AIBot_spells[i].spellid].cast_time);
		//}
		//else
		//	AIBot_spells[i].time_cancast = Timer::GetCurrentTime() + spells[AIBot_spells[i].spellid].recast_time;

		AIBot_spells[i].time_cancast = Timer::GetCurrentTime() + spells[AIBot_spells[i].spellid].recast_time;

		if (spells[AIBot_spells[i].spellid].timer_id > 0) {
			SetSpellRecastTimer(spells[AIBot_spells[i].spellid].timer_id, spells[AIBot_spells[i].spellid].recast_time);
		}
	}

	return result;
}

bool Bot::AI_PursueCastCheck() {
	bool result = false;

	if (AIautocastspell_timer->Check(false)) {

		AIautocastspell_timer->Disable();	//prevent the timer from going off AGAIN while we are casting.

		LogAIDetail("Bot Engaged (pursuing) autocast check triggered. Trying to cast offensive spells");

		if (!AICastSpell(GetTarget(), 100, SpellType_Snare)) {
			if (!AICastSpell(GetTarget(), 100, SpellType_Lifetap)) {
				if (!AICastSpell(GetTarget(), 100, SpellType_Nuke)) {
					/*AIautocastspell_timer->Start(RandomTimer(500, 2000), false);
					result = true;*/
					result = true;
				}

				result = true;
			}

			result = true;
		}

		if (!AIautocastspell_timer->Enabled())
			AIautocastspell_timer->Start(RandomTimer(100, 250), false);
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

		//if (HasGroup() && GetGroup()->GetLeader() && GetGroup()->GetLeader()->IsClient()) {
		//	test_against = GetGroup()->GetLeader()->CastToClient();
		//} else if (GetOwner() && GetOwner()->IsClient()) {
		//	test_against = GetOwner()->CastToClient();
		//}

		if (GetOwner() && GetOwner()->IsClient()) {
			test_against = GetOwner()->CastToClient();
		}

		if (test_against) {
			pre_combat = test_against->GetBotPrecombat();
		}

		//Ok, IdleCastCheck depends of class.
		switch (GetClass()) {
		// Healers WITHOUT pets will check if a heal is needed before buffing.
		case CLERIC:
		case PALADIN:
		case RANGER:
		case MONK:
		case ROGUE:
		case WARRIOR:
		case BERSERKER: {
			if (!AICastSpell(this, 100, SpellType_Heal)) {
				if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, 100, BotAISpellRange, SpellType_Heal)) {
					if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, 100, BotAISpellRange, SpellType_Cure)) {
						if (!AICastSpell(this, 100, SpellType_Buff)) {
							if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, 100, BotAISpellRange, SpellType_Buff)) {
								//
							}
						}
					}
				}
			}

			result = true;
			break;
		}
		// Pets class will first cast their pet, then buffs
		case DRUID:
		case MAGICIAN:
		case SHADOWKNIGHT:
		case SHAMAN:
		case NECROMANCER:
		case ENCHANTER:
		case BEASTLORD: {
			if (!AICastSpell(this, 100, SpellType_Heal)) {
				if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, 100, BotAISpellRange, SpellType_Heal)) {
					if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, 100, BotAISpellRange, SpellType_Cure)) {
						if (!AICastSpell(this, 100, SpellType_Pet)) {
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
		case WIZARD: { // This can eventually be move into the BEASTLORD case handler once pre-combat is fully implemented
			if (pre_combat) {
				if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, 100, BotAISpellRange, SpellType_Cure)) {
					if (!AICastSpell(this, 100, SpellType_Pet)) {
						if (!AICastSpell(this, 100, SpellType_Heal)) {
							if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, 100, BotAISpellRange, SpellType_Heal)) {
								if (!AICastSpell(this, 100, SpellType_Buff)) {
									if (!AICastSpell(GetPet(), 100, SpellType_Heal)) {
										if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, 100, BotAISpellRange, SpellType_PreCombatBuff)) {
											if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, 100, BotAISpellRange, SpellType_Buff)) {
											}
										}
									}
								}
							}
						}
					}
				}
			}
			else {
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
			}

			result = true;
			break;
		}
		case BARD: {
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
							//if (!AICastSpell(this, 100, SpellType_InCombatBuffSong)) {
							//}
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
		EQ::constants::StanceType botStance = GetBotStance();
		bool mayGetAggro = HasOrMayGetAggro();

		LogAIDetail("Engaged autocast check triggered (BOTS). Trying to cast healing spells then maybe offensive spells");

		if (botClass == CLERIC) {
			if (!AICastSpell(GetTarget(), 100, SpellType_Escape)) {
				if (!AICastSpell(this, 100, SpellType_Heal)) {
					if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, 100, BotAISpellRange, SpellType_Heal)) {
						if (!AICastSpell(GetTarget(), 100, SpellType_Dispel)) {
							if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, 100, BotAISpellRange, SpellType_Cure)) {
								if (!AICastSpell(GetTarget(), 100, SpellType_Debuff)) {
									if (!AICastSpell(this, 100, SpellType_InCombatBuff)) {
										if (!AICastSpell(GetTarget(), mayGetAggro ? 0 : 100, SpellType_Nuke)) {
											if (!AICastSpell(GetTarget(), mayGetAggro ? 0 : 100, SpellType_Root)) {
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
		else if (botClass == DRUID) {
			if (!AICastSpell(GetTarget(), 100, SpellType_Escape)) {
				if (!AICastSpell(this, 100, SpellType_Heal)) {
					if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, 100, BotAISpellRange, SpellType_Heal)) {
						if (!AICastSpell(GetTarget(), 100, SpellType_Dispel)) {
							if (!AICastSpell(GetTarget(), 100, SpellType_Debuff)) {
								if (!AICastSpell(GetTarget(), mayGetAggro ? 0 : 100, SpellType_DOT)) {
									if (!AICastSpell(GetTarget(), mayGetAggro ? 0 : 100, SpellType_Snare)) {
										if (!AICastSpell(GetTarget(), mayGetAggro ? 0 : 100, SpellType_Nuke)) {
											if (!AICastSpell(GetTarget(), mayGetAggro ? 0 : 100, SpellType_Root)) {
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
		else if (botClass == SHAMAN) {
			if (!AICastSpell(GetTarget(), 100, SpellType_Escape)) {
				if (!AICastSpell(this, 100, SpellType_Heal)) {
					if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, 100, BotAISpellRange, SpellType_Heal)) {
						if (!AICastSpell(GetTarget(), 100, SpellType_Dispel)) {
							if (!AICastSpell(GetTarget(), 100, SpellType_Debuff)) {
								if (!AICastSpell(GetTarget(), 100, SpellType_Slow)) {
									if (!AICastSpell(GetPet(), 100, SpellType_Heal)) {
										if (!AICastSpell(this, 100, SpellType_InCombatBuff)) {
											if (!AICastSpell(GetTarget(), mayGetAggro ? 0 : 100, SpellType_DOT)) {
												if (!AICastSpell(GetTarget(), mayGetAggro ? 0 : 100, SpellType_Nuke)) {
													if (!AICastSpell(GetTarget(), mayGetAggro ? 0 : 100, SpellType_Root)) {
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
			}
		}
		else if (botClass == RANGER) {
			if (!AICastSpell(GetTarget(), 100, SpellType_Escape)) {
				if (!AICastSpell(this, 100, SpellType_Heal)) {
					if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, 100, BotAISpellRange, SpellType_Heal)) {
						if (!AICastSpell(GetTarget(), 100, SpellType_HateRedux)) {
							if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, 100, BotAISpellRange, SpellType_Cure)) {
								if (!AICastSpell(GetTarget(), 100, SpellType_Dispel)) {
									if (!AICastSpell(GetTarget(), mayGetAggro ? 0 : 100, SpellType_DOT)) {
										if (!AICastSpell(GetTarget(), mayGetAggro ? 0 : 100, SpellType_Snare)) {
											if (!AICastSpell(GetTarget(), mayGetAggro ? 0 : 100, SpellType_Nuke)) {
												if (!AICastSpell(GetTarget(), mayGetAggro ? 0 : 100, SpellType_Root)) {
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
		}
		else if (botClass == BEASTLORD) {
			if (!AICastSpell(this, 100, SpellType_Heal)) {
				if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, 100, BotAISpellRange, SpellType_Heal)) {
					if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, 100, BotAISpellRange, SpellType_Cure)) {
						if (!AICastSpell(GetTarget(), 100, SpellType_Dispel)) {
							if (!AICastSpell(GetTarget(), 100, SpellType_Debuff)) {
								if (!AICastSpell(GetTarget(), 100, SpellType_Slow)) {
									if (!AICastSpell(GetPet(), 100, SpellType_Heal)) {
										if (!AICastSpell(this, 25, SpellType_Pet)) {
											if (!AICastSpell(GetTarget(), mayGetAggro ? 0 : 100, SpellType_DOT)) {
												if (!AICastSpell(GetTarget(), mayGetAggro ? 0 : 100, SpellType_Nuke)) {
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
		}
		else if (botClass == WIZARD) {
			if (!AICastSpell(GetTarget(), 100, SpellType_Escape)) {
				if (!AICastSpell(GetTarget(), 100, SpellType_HateRedux)) {
					if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, 100, BotAISpellRange, SpellType_Cure)) {
						if (!AICastSpell(GetTarget(), 100, SpellType_Dispel)) {
							if (!AICastSpell(GetTarget(), mayGetAggro ? 0 : 100, SpellType_Snare)) {
								if (!AICastSpell(GetTarget(), mayGetAggro ? 0 : 100, SpellType_Nuke)) {
									if (!AICastSpell(GetTarget(), mayGetAggro ? 0 : 100, SpellType_Root)) {
										failedToCast = true;
									}
								}
							}
						}
					}
				}
			}
		}
		else if (botClass == PALADIN) {
			if (!AICastSpell(GetTarget(), 100, SpellType_Escape)) {
				if (!AICastSpell(this, 100, SpellType_Heal)) {
					if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, 100, BotAISpellRange, SpellType_Heal)) {
						if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, 100, BotAISpellRange, SpellType_Cure)) {
							if (!AICastSpell(GetTarget(), 100, SpellType_Dispel)) {
								if (!AICastSpell(GetTarget(), 100, SpellType_Nuke)) {
									if (!AICastSpell(this, 100, SpellType_InCombatBuff)) {
										if (!AICastSpell(GetTarget(), 100, SpellType_Root)) {
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
		else if (botClass == SHADOWKNIGHT) {
			if (!AICastSpell(GetTarget(), 100, SpellType_Escape)) {
				if (!AICastSpell(GetTarget(), 100, SpellType_HateRedux)) {
					if (!AICastSpell(GetTarget(), 100, SpellType_Lifetap)) {
						if (!AICastSpell(GetTarget(), 100, SpellType_Dispel)) {
							if (!AICastSpell(GetTarget(), 100, SpellType_Debuff)) {
								if (!AICastSpell(this, 25, SpellType_Pet)) {
									if (!AICastSpell(GetPet(), 100, SpellType_Heal)) {
										if (!AICastSpell(GetTarget(), 100, SpellType_InCombatBuff)) {
											if (!AICastSpell(GetTarget(), 100, SpellType_Snare)) {
												if (!AICastSpell(GetTarget(), 100, SpellType_DOT)) {
													if (!AICastSpell(GetTarget(), 100, SpellType_Nuke)) {
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
			}
		}
		else if (botClass == MAGICIAN) {
			if (!AICastSpell(this, 25, SpellType_Pet)) {
				if (!AICastSpell(GetPet(), 100, SpellType_Heal)) {
					if (!AICastSpell(GetTarget(), 100, SpellType_Dispel)) {
						if (!AICastSpell(GetTarget(), 100, SpellType_Debuff)) {
							if (!AICastSpell(GetTarget(), mayGetAggro ? 0 : 100, SpellType_Nuke)) {
								failedToCast = true;
							}
						}
					}
				}
			}
		}
		else if (botClass == NECROMANCER) {
			if (!AICastSpell(GetTarget(), 100, SpellType_Mez)) {
				if (!AICastSpell(GetTarget(), 100, SpellType_Escape)) {
					if (!AICastSpell(GetTarget(), 100, SpellType_Dispel)) {
						if (!AICastSpell(GetTarget(), 100, SpellType_Debuff)) {
							if (!AICastSpell(GetTarget(), 100, SpellType_Lifetap)) {
								if (!AICastSpell(GetTarget(), mayGetAggro ? 0 : 100, SpellType_Snare)) {
									if (!AICastSpell(GetTarget(), mayGetAggro ? 0 : 100, SpellType_DOT)) {
										if (!AICastSpell(this, 25, SpellType_Pet)) {
											if (!AICastSpell(GetPet(), 100, SpellType_Heal)) {
												if (!AICastSpell(GetTarget(), mayGetAggro ? 0 : 100, SpellType_Nuke)) {
													if (!AICastSpell(GetTarget(), mayGetAggro ? 0 : 100, SpellType_Root)) {
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
			}
		}
		else if (botClass == ENCHANTER) {
			if (!AICastSpell(GetTarget(), 100, SpellType_Mez)) {
				if (!AICastSpell(GetTarget(), 100, SpellType_Escape)) {
					if (!AICastSpell(GetTarget(), 100, SpellType_HateRedux)) {
						if (!AICastSpell(GetTarget(), 100, SpellType_Dispel)) {
							if (!AICastSpell(GetTarget(), 100, SpellType_Debuff)) {
								if (!AICastSpell(GetTarget(), 100, SpellType_Slow)) {
									if (!AICastSpell(GetTarget(), mayGetAggro ? 0 : 100, SpellType_DOT)) {
										if (!AICastSpell(GetTarget(), mayGetAggro ? 0 : 100, SpellType_Nuke)) {
											if (!AICastSpell(GetTarget(), mayGetAggro ? 0 : 100, SpellType_Root)) {
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
		else if (botClass == BARD) {
			if (!AICastSpell(GetTarget(), 100, SpellType_Mez)) {
				if (!AICastSpell(GetTarget(), 100, SpellType_Escape)) {
					if (!AICastSpell(GetTarget(), 100, SpellType_HateRedux)) {
						if (!entity_list.Bot_AICheckCloseBeneficialSpells(this, 100, BotAISpellRange, SpellType_Cure)) {
							if (!AICastSpell(this, 100, SpellType_InCombatBuffSong)) {
								if (!AICastSpell(GetTarget(), 100, SpellType_Dispel)) {
									if (!AICastSpell(GetTarget(), 100, SpellType_Debuff)) {
										if (!AICastSpell(GetTarget(), 100, SpellType_Slow)) {
											if (!AICastSpell(GetTarget(), mayGetAggro ? 0 : 100, SpellType_DOT)) {
												if (!AICastSpell(GetTarget(), mayGetAggro ? 0 : 100, SpellType_Nuke)) {
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

		if (botSpell.SpellId == 0)
			botSpell = GetBestBotSpellForFastHeal(this);
	}
	else {
		botSpell = GetBestBotSpellForPercentageHeal(this);

		if (botSpell.SpellId == 0) {
			botSpell = GetBestBotSpellForRegularSingleTargetHeal(this);
		}
		if (botSpell.SpellId == 0) {
			botSpell = GetFirstBotSpellForSingleTargetHeal(this);
		}
		if (botSpell.SpellId == 0) {
			botSpell = GetFirstBotSpellBySpellType(this, SpellType_Heal);
		}
	}

	LogAIDetail("heal spellid [{}] fastheals [{}] casterlevel [{}]",
		botSpell.SpellId, ((useFastHeals) ? ('T') : ('F')), GetLevel());

	LogAIDetail("target [{}] current_time [{}] donthealmebefore [{}]", tar->GetCleanName(), Timer::GetCurrentTime(), tar->DontHealMeBefore());

	// If there is still no spell id, then there isn't going to be one so we are done
	if (botSpell.SpellId == 0)
		return false;

	// Can we cast this spell on this target?
	if (!(spells[botSpell.SpellId].target_type == ST_GroupTeleport || spells[botSpell.SpellId].target_type == ST_Target || tar == this)
		&& !(tar->CanBuffStack(botSpell.SpellId, botLevel, true) >= 0))
		return false;

	uint32 TempDontHealMeBeforeTime = tar->DontHealMeBefore();

	if (IsValidSpellRange(botSpell.SpellId, tar)) {
		castedSpell = AIDoSpellCast(botSpell.SpellIndex, tar, botSpell.ManaCost, &TempDontHealMeBeforeTime);
	}

	if(castedSpell)
		//Say("Casting %s on %s, please stay in range!", spells[botSpell.SpellId].name, tar->GetCleanName());
		BotGroupSay(this, "Casting [%s] on [%s], please stay in range!", spells[botSpell.SpellId].name, tar->GetCleanName());

	return castedSpell;
}

std::list<BotSpell> Bot::GetBotSpellsForSpellEffect(Bot* botCaster, int spellEffect) {
	std::list<BotSpell> result;

	auto bot_owner = botCaster->GetBotOwner();
	if (!bot_owner) {
		return result;
	}

	if (botCaster && botCaster->AI_HasSpells()) {
		std::vector<BotSpells_Struct> botSpellList = botCaster->AIBot_spells;

		for (int i = botSpellList.size() - 1; i >= 0; i--) {
			if (!IsValidSpell(botSpellList[i].spellid)) {
				// this is both to quit early to save cpu and to avoid casting bad spells
				// Bad info from database can trigger this incorrectly, but that should be fixed in DB, not here
				continue;
			}

			if (IsEffectInSpell(botSpellList[i].spellid, spellEffect) || GetTriggerSpellID(botSpellList[i].spellid, spellEffect)) {
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

	auto bot_owner = botCaster->GetBotOwner();
	if (!bot_owner) {
		return result;
	}

	if (botCaster && botCaster->AI_HasSpells()) {
		std::vector<BotSpells_Struct> botSpellList = botCaster->AIBot_spells;

		for (int i = botSpellList.size() - 1; i >= 0; i--) {
			if (!IsValidSpell(botSpellList[i].spellid)) {
				// this is both to quit early to save cpu and to avoid casting bad spells
				// Bad info from database can trigger this incorrectly, but that should be fixed in DB, not here
				continue;
			}

			if (IsEffectInSpell(botSpellList[i].spellid, spellEffect) || GetTriggerSpellID(botSpellList[i].spellid, spellEffect)) {
				if (spells[botSpellList[i].spellid].target_type == targetType) {
					BotSpell botSpell;
					botSpell.SpellId = botSpellList[i].spellid;
					botSpell.SpellIndex = i;
					botSpell.ManaCost = botSpellList[i].manacost;

					result.push_back(botSpell);
				}
			}
		}
	}

	return result;
}

std::list<BotSpell> Bot::GetBotSpellsBySpellType(Bot* botCaster, uint32 spellType) {
	std::list<BotSpell> result;

	auto bot_owner = botCaster->GetBotOwner();
	if (!bot_owner) {
		return result;
	}

	if (botCaster && botCaster->AI_HasSpells()) {
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
				[](BotSpell_wPriority& l, BotSpell_wPriority& r) {
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

			if ((botSpellList[i].type & spellType) && CheckSpellRecastTimers(botCaster, i)) {
				result.SpellId = botSpellList[i].spellid;
				result.SpellIndex = i;
				result.ManaCost = botSpellList[i].manacost;

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

		for (std::list<BotSpell>::iterator botSpellListItr = botSpellList.begin(); botSpellListItr != botSpellList.end(); ++botSpellListItr) {
			// Assuming all the spells have been loaded into this list by level and in descending order
			if (IsFastHealSpell(botSpellListItr->SpellId) && CheckSpellRecastTimers(botCaster, botSpellListItr->SpellIndex)) {
				result.SpellId = botSpellListItr->SpellId;
				result.SpellIndex = botSpellListItr->SpellIndex;
				result.ManaCost = botSpellListItr->ManaCost;

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

		for (std::list<BotSpell>::iterator botSpellListItr = botHoTSpellList.begin(); botSpellListItr != botHoTSpellList.end(); ++botSpellListItr) {
			// Assuming all the spells have been loaded into this list by level and in descending order
			if (IsHealOverTimeSpell(botSpellListItr->SpellId)) {
				for (int i = botSpellList.size() - 1; i >= 0; i--) {
					if (!IsValidSpell(botSpellList[i].spellid)) {
						// this is both to quit early to save cpu and to avoid casting bad spells
						// Bad info from database can trigger this incorrectly, but that should be fixed in DB, not here
						continue;
					}

					if (
						botSpellList[i].spellid == botSpellListItr->SpellId &&
						(botSpellList[i].type & SpellType_Heal) &&
						CheckSpellRecastTimers(botCaster, botSpellListItr->SpellIndex)
					) {
						result.SpellId = botSpellListItr->SpellId;
						result.SpellIndex = botSpellListItr->SpellIndex;
						result.ManaCost = botSpellListItr->ManaCost;
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

			if (IsCompleteHealSpell(botSpellList[i].spellid) && CheckSpellRecastTimers(botCaster, i)) {
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
			if (IsRegularSingleTargetHealSpell(botSpellListItr->SpellId) && CheckSpellRecastTimers(botCaster, botSpellListItr->SpellIndex)) {
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
				CheckSpellRecastTimers(botCaster, botSpellListItr->SpellIndex)
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

	if (botCaster) {
		std::list<BotSpell> botSpellList = GetBotSpellsForSpellEffect(botCaster, SE_CurrentHP);

		for (std::list<BotSpell>::iterator botSpellListItr = botSpellList.begin(); botSpellListItr != botSpellList.end(); ++botSpellListItr) {
			// Assuming all the spells have been loaded into this list by level and in descending order
			if (
				IsRegularGroupHealSpell(botSpellListItr->SpellId) &&
				CheckSpellRecastTimers(botCaster, botSpellListItr->SpellIndex)
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

BotSpell Bot::GetBestBotSpellForGroupHealOverTime(Bot* botCaster) {
	BotSpell result;

	result.SpellId = 0;
	result.SpellIndex = 0;
	result.ManaCost = 0;

	if (botCaster) {
		std::list<BotSpell> botHoTSpellList = GetBotSpellsForSpellEffect(botCaster, SE_HealOverTime);
		std::vector<BotSpells_Struct> botSpellList = botCaster->AIBot_spells;

		for (std::list<BotSpell>::iterator botSpellListItr = botHoTSpellList.begin(); botSpellListItr != botHoTSpellList.end(); ++botSpellListItr) {
			// Assuming all the spells have been loaded into this list by level and in descending order
			if (IsGroupHealOverTimeSpell(botSpellListItr->SpellId)) {

				for (int i = botSpellList.size() - 1; i >= 0; i--) {
					if (!IsValidSpell(botSpellList[i].spellid)) {
						// this is both to quit early to save cpu and to avoid casting bad spells
						// Bad info from database can trigger this incorrectly, but that should be fixed in DB, not here
						continue;
					}

					if (
						botSpellList[i].spellid == botSpellListItr->SpellId &&
						(botSpellList[i].type & SpellType_Heal) &&
						CheckSpellRecastTimers(botCaster, botSpellListItr->SpellIndex)
					) {
						result.SpellId = botSpellListItr->SpellId;
						result.SpellIndex = botSpellListItr->SpellIndex;
						result.ManaCost = botSpellListItr->ManaCost;
					}
				}

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
				CheckSpellRecastTimers(botCaster, botSpellListItr->SpellIndex)
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
				IsMezSpell(botSpellListItr->SpellId) &&
				CheckSpellRecastTimers(botCaster, botSpellListItr->SpellIndex)
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
				CheckSpellRecastTimers(botCaster, botSpellListItr->SpellIndex)
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
				CheckSpellRecastTimers(botCaster, botSpellListItr->SpellIndex)
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

Mob* Bot::GetFirstIncomingMobToMez(Bot* botCaster, BotSpell botSpell) {
	Mob* result = 0;
	bool go_next = false;

	if (botCaster && IsMezSpell(botSpell.SpellId)) {

		std::list<NPC*> npc_list;
		entity_list.GetNPCList(npc_list);

		for (std::list<NPC*>::iterator itr = npc_list.begin(); itr != npc_list.end(); ++itr) {
			NPC* npc = *itr;

			if (DistanceSquaredNoZ(npc->GetPosition(), botCaster->GetPosition()) <= botCaster->GetActSpellRange(botSpell.SpellId, spells[botSpell.SpellId].range)) {
				if (!npc->IsMezzed()) {
					if (botCaster->IsRaidGrouped()) {
						Raid* raid = botCaster->GetRaid();
						if (raid) {
							std::vector<RaidMember> raid_group_members = raid->GetMembers();
							for (std::vector<RaidMember>::iterator iter = raid_group_members.begin(); iter != raid_group_members.end(); ++iter) {
								if (!go_next) {
									if (iter->member && entity_list.IsMobInZone(iter->member)) {
										if (npc->IsOnHatelist(iter->member)) {
											if (
												iter->member->GetTarget() != npc || (
													!iter->member->IsCasting() && (
														(iter->member->IsClient() && !iter->member->CastToClient()->AutoAttackEnabled() && !iter->member->CastToClient()->AutoFireEnabled())
														|| (iter->member->IsBot() && !iter->member->CastToBot()->GetAttackFlag() && (!iter->member->CastToBot()->GetPullFlag() || !iter->member->CastToBot()->GetPullingFlag()))
														)
													)
												) {
												int buff_count = npc->GetMaxTotalSlots();
												auto npc_buffs = npc->GetBuffs();
												for (int i = 0; i < buff_count; i++) {
													if (IsDetrimentalSpell(npc_buffs[i].spellid) && IsEffectInSpell(npc_buffs[i].spellid, SE_CurrentHP)) {
														go_next = true;
														break;
													}
												}
												if (!go_next) {
													result = npc;
												}
											}
											else {
												go_next = true;
											}
										}
									}
								}
							}
							if (result) {
								if (zone->random.Int(1, 100) <= RuleI(Bots, MezChance)) {
									botCaster->m_mez_delay_timer.Start(RuleI(Bots, MezSuccessDelay));
									result = npc;
									return result;
								}
								result = 0;
							}
							go_next = false;
						}
					}
					else if (botCaster->IsGrouped()) {
						Group* g = botCaster->GetGroup();
						if (g) {
							for (int counter = 0; counter < g->GroupCount(); counter++) {
								if (!go_next) {
									if (g->members[counter] && entity_list.IsMobInZone(g->members[counter])) {
										if (npc->IsOnHatelist(g->members[counter])) {
											if (
												g->members[counter]->GetTarget() != npc || (
													!g->members[counter]->IsCasting() && (
														(g->members[counter]->IsClient() && !g->members[counter]->CastToClient()->AutoAttackEnabled() && !g->members[counter]->CastToClient()->AutoFireEnabled())
														|| (g->members[counter]->IsBot() && !g->members[counter]->CastToBot()->GetAttackFlag() && (!g->members[counter]->CastToBot()->GetPullFlag() || !g->members[counter]->CastToBot()->GetPullingFlag()))
														)
													)
												) {
												int buff_count = npc->GetMaxTotalSlots();
												auto npc_buffs = npc->GetBuffs();
												bool dot_check = false;
												for (int i = 0; i < buff_count; i++) {
													if (IsDetrimentalSpell(npc_buffs[i].spellid) && IsEffectInSpell(npc_buffs[i].spellid, SE_CurrentHP)) {
														go_next = true;
														break;
													}
												}
												if (!go_next) {
													result = npc;
												}
											}
											else {
												go_next = true;
											}
										}
									}
								}
							}
							if (result) {
								if (zone->random.Int(1, 100) <= RuleI(Bots, MezChance)) {
									botCaster->m_mez_delay_timer.Start(RuleI(Bots, MezSuccessDelay));
									result = npc;
									return result;
								}
								result = 0;
							}
							go_next = false;
						}
					}
				}
			}
		}
		botCaster->m_mez_delay_timer.Start(RuleI(Bots, MezFailDelay));
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
			if (IsSummonPetSpell(botSpellListItr->SpellId) && CheckSpellRecastTimers(botCaster, botSpellListItr->SpellIndex)) {
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
		if (botCaster->GetBotPetSetTypeSetting() != 255) {
			switch(botCaster->GetBotPetSetTypeSetting()) {
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
				case 5:
					if (RuleI(Bots, MageEpicPet) == 1 && botCaster->GetLevel() >= RuleI(Bots, MageEpicPetMinLvl)) {
						auto inst = botCaster->GetBotItem(13);
						if (inst && inst->GetItem()) {
							auto botitem = inst->GetItem()->ID;
							if (botitem == 550005)
								result = std::string("SumMageMultiElement");
						}
						else {
							result = std::string("MonsterSum");
						}
					}
					else if (RuleI(Bots, MageEpicPet) == 2 && botCaster->GetLevel() >= RuleI(Bots, MageEpicPetMinLvl)) {
						result = std::string("SumMageMultiElement");
					}
					else {
						result = std::string("MonsterSum");
					}
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
					case 5:
						if (RuleI(Bots, MageEpicPet) == 1 && botCaster->GetLevel() >= RuleI(Bots, MageEpicPetMinLvl)) {
							auto inst = botCaster->GetBotItem(13);
							if (inst && inst->GetItem()) {
								auto botitem = inst->GetItem()->ID;
								if (botitem == 550005)
									result = std::string("SumMageMultiElement");
							}
							else {
								result = std::string("MonsterSum");
							}
						}
						else if (RuleI(Bots, MageEpicPet) == 2 && botCaster->GetLevel() >= RuleI(Bots, MageEpicPetMinLvl)) {
							result = std::string("SumMageMultiElement");
						}
						else {
							result = std::string("MonsterSum");
						}
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
					case 5:
						if (RuleI(Bots, MageEpicPet) == 1 && botCaster->GetLevel() >= RuleI(Bots, MageEpicPetMinLvl)) {
							auto inst = botCaster->GetBotItem(13);
							if (inst && inst->GetItem()) {
								auto botitem = inst->GetItem()->ID;
								if (botitem == 550005)
									result = std::string("SumMageMultiElement");
							}
							else {
								result = std::string("MonsterSum");
							}
						}
						else if (RuleI(Bots, MageEpicPet) == 2 && botCaster->GetLevel() >= RuleI(Bots, MageEpicPetMinLvl)) {
							result = std::string("SumMageMultiElement");
						}
						else {
							result = std::string("MonsterSum");
						}
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
			if((IsPureNukeSpell(botSpellListItr->SpellId) || IsDamageSpell(botSpellListItr->SpellId)) && CheckSpellRecastTimers(botCaster, botSpellListItr->SpellIndex)) {
				if (!DoResistCheck(botCaster, botCaster->GetTarget(), botSpellListItr->SpellId, RuleI(Bots, NukeResistLimit)))
					continue;
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
			if (IsStunSpell(botSpellListItr->SpellId) && CheckSpellRecastTimers(botCaster, botSpellListItr->SpellIndex))
			{
				if (!DoResistCheck(botCaster, botCaster->GetTarget(), botSpellListItr->SpellId, RuleI(Bots, NukeResistLimit)))
					continue;
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

		if(((target->GetMR() + level_mod) > maxTargetResistValue) && ((target->GetCR() + level_mod) > maxTargetResistValue) && ((target->GetFR() + level_mod) > maxTargetResistValue))
			selectLureNuke = true;


		std::list<BotSpell> botSpellList = GetBotSpellsForSpellEffectAndTargetType(botCaster, SE_CurrentHP, ST_Target);

		BotSpell firstWizardMagicNukeSpellFound;
		firstWizardMagicNukeSpellFound.SpellId = 0;
		firstWizardMagicNukeSpellFound.SpellIndex = 0;
		firstWizardMagicNukeSpellFound.ManaCost = 0;

		for(std::list<BotSpell>::iterator botSpellListItr = botSpellList.begin(); botSpellListItr != botSpellList.end(); ++botSpellListItr) {
			// Assuming all the spells have been loaded into this list by level and in descending order
			bool spellSelected = false;

			if(CheckSpellRecastTimers(botCaster, botSpellListItr->SpellIndex)) {
				if(selectLureNuke && (spells[botSpellListItr->SpellId].resist_difficulty < lureResisValue)) {
					if (DoResistCheck(botCaster, target, botSpellListItr->SpellId, RuleI(Bots, NukeResistLimit)) && GetSpellResistType(botSpellListItr->SpellId) == RESIST_MAGIC)
						spellSelected = true;
					if (DoResistCheck(botCaster, target, botSpellListItr->SpellId, RuleI(Bots, NukeResistLimit)) && GetSpellResistType(botSpellListItr->SpellId) == RESIST_FIRE)
						spellSelected = true;
					if (DoResistCheck(botCaster, target, botSpellListItr->SpellId, RuleI(Bots, NukeResistLimit)) && GetSpellResistType(botSpellListItr->SpellId) == RESIST_COLD)
						spellSelected = true;
				}
				else if(!selectLureNuke && IsPureNukeSpell(botSpellListItr->SpellId)) {
					if(((target->GetMR() < target->GetCR()) || (target->GetMR() < target->GetFR())) && (GetSpellResistType(botSpellListItr->SpellId) == RESIST_MAGIC)
						&& (spells[botSpellListItr->SpellId].resist_difficulty > lureResisValue))
					{
						spellSelected = true;
					}
					else if (((target->GetCR() < target->GetMR()) || (target->GetCR() < target->GetFR())) && (GetSpellResistType(botSpellListItr->SpellId) == RESIST_COLD)
						&& (spells[botSpellListItr->SpellId].resist_difficulty > lureResisValue))
					{
						spellSelected = true;
					}
					else if (((target->GetFR() < target->GetCR()) || (target->GetFR() < target->GetMR())) && (GetSpellResistType(botSpellListItr->SpellId) == RESIST_FIRE)
						&& (spells[botSpellListItr->SpellId].resist_difficulty > lureResisValue))
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

	if (botCaster && botCaster->AI_HasSpells()) {
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
				&& CheckSpellRecastTimers(botCaster, i)) {
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

	if (!tar)
		return result;

	int level_mod = (tar->GetLevel() - botCaster->GetLevel())* (tar->GetLevel() - botCaster->GetLevel()) / 2;
	if (tar->GetLevel() - botCaster->GetLevel() < 0)
	{
		level_mod = -level_mod;
	}
	bool needsMagicResistDebuff = (tar->GetMR() + level_mod) > 100 ? true: false;
	bool needsColdResistDebuff = (tar->GetCR() + level_mod) > 100 ? true: false;
	bool needsFireResistDebuff = (tar->GetFR() + level_mod) > 100 ? true: false;
	bool needsPoisonResistDebuff = (tar->GetPR() + level_mod) > 100 ? true: false;
	bool needsDiseaseResistDebuff = (tar->GetDR() + level_mod) > 100 ? true: false;

	if (botCaster && botCaster->AI_HasSpells()) {
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
				&& CheckSpellRecastTimers(botCaster, i)) {
				result.SpellId = botSpellList[i].spellid;
				result.SpellIndex = i;
				result.ManaCost = botSpellList[i].manacost;

				break;
			}
		}
	}

	return result;
}

bool Bot::DoResistCheck(Bot* botCaster, Mob* tar, uint16 botspellid, int32 resist_limit) {

	if (!botCaster || !tar || botspellid == 0)
		return false;

	int32 resist_difficulty = -spells[botspellid].resist_difficulty;
	int32 level_mod = (tar->GetLevel() - botCaster->GetLevel()) * (tar->GetLevel() - botCaster->GetLevel()) / 2;

	if (tar->GetLevel() - botCaster->GetLevel() < 0) {
		level_mod = -level_mod;
	}

	if (GetSpellResistType(botspellid) == RESIST_NONE)
		return true;
	if ((GetSpellResistType(botspellid) == RESIST_MAGIC) && ((tar->GetMR() + level_mod - resist_difficulty) > resist_limit))
		return false;
	if ((GetSpellResistType(botspellid) == RESIST_COLD) && ((tar->GetCR() + level_mod - resist_difficulty) > resist_limit))
		return false;
	if ((GetSpellResistType(botspellid) == RESIST_FIRE) && ((tar->GetFR() + level_mod - resist_difficulty) > resist_limit))
		return false;
	if ((GetSpellResistType(botspellid) == RESIST_POISON) && ((tar->GetPR() + level_mod - resist_difficulty) > resist_limit))
		return false;
	if ((GetSpellResistType(botspellid) == RESIST_DISEASE) && ((tar->GetDR() + level_mod - resist_difficulty) > resist_limit))
		return false;
	if ((GetSpellResistType(botspellid) == RESIST_CORRUPTION) && ((tar->GetCorrup() + level_mod - resist_difficulty) > resist_limit))
		return false;

	return true;
}

BotSpell Bot::GetBestBotSpellForCure(Bot* botCaster, Mob *tar, bool skipGroup) {
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
		if (tar->IsRaidGrouped()) {
			Raid* raid = tar->GetRaid();
			if (raid) {
				std::vector<RaidMember> raid_group_members = raid->GetMembers();
				for (std::vector<RaidMember>::iterator iter = raid_group_members.begin(); iter != raid_group_members.end(); ++iter) {
					if (iter->member && entity_list.IsMobInZone(iter->member)) {
						if (botCaster->GetNeedsCured(iter->member)) {
							countNeedsCured++;
						}
					}
				}
			}
		}
		else if (tar->HasGroup()) {
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
		if (countNeedsCured > 2 && !skipGroup) {
			for (std::list<BotSpell_wPriority>::iterator itr = cureList.begin(); itr != cureList.end(); ++itr) {
				BotSpell_wPriority selectedBotSpell = *itr;

				if (IsGroupSpell(itr->SpellId) && CheckSpellRecastTimers(botCaster, itr->SpellIndex)) {
					if (selectedBotSpell.SpellId == 0)
						continue;

					if (isPoisoned && IsEffectInSpell(itr->SpellId, SE_PoisonCounter)) {
						spellSelected = true;
					}
					else if (isDiseased && IsEffectInSpell(itr->SpellId, SE_DiseaseCounter)) {
						spellSelected = true;
					}
					else if (isCursed && IsEffectInSpell(itr->SpellId, SE_CurseCounter)) {
						spellSelected = true;
					}
					else if (isCorrupted && IsEffectInSpell(itr->SpellId, SE_CorruptionCounter)) {
						spellSelected = true;
					}
					else if (IsEffectInSpell(itr->SpellId, SE_DispelDetrimental)) {
						spellSelected = true;
					}

					if (spellSelected)
					{
						result.SpellId = itr->SpellId;
						result.SpellIndex = itr->SpellIndex;
						result.ManaCost = itr->ManaCost;

						break;
					}
				}
			}
		}

		//no group cure for target- try to find single target spell
		if (!spellSelected) {
			for(std::list<BotSpell_wPriority>::iterator itr = cureList.begin(); itr != cureList.end(); ++itr) {
				BotSpell_wPriority selectedBotSpell = *itr;

				if (CheckSpellRecastTimers(botCaster, itr->SpellIndex)) {
					if (selectedBotSpell.SpellId == 0)
						continue;

					if (isPoisoned && IsEffectInSpell(itr->SpellId, SE_PoisonCounter)) {
						spellSelected = true;
					}
					else if (isDiseased && IsEffectInSpell(itr->SpellId, SE_DiseaseCounter)) {
						spellSelected = true;
					}
					else if (isCursed && IsEffectInSpell(itr->SpellId, SE_CurseCounter)) {
						spellSelected = true;
					}
					else if (isCorrupted && IsEffectInSpell(itr->SpellId, SE_CorruptionCounter)) {
						spellSelected = true;
					}
					else if (IsEffectInSpell(itr->SpellId, SE_DispelDetrimental)) {
						spellSelected = true;
					}

					if (spellSelected)
					{
						result.SpellId = itr->SpellId;
						result.SpellIndex = itr->SpellIndex;
						result.ManaCost = itr->ManaCost;

						break;
					}
				}
			}
		}
	}

	return result;
}

void Bot::SetSpellRecastTimer(int timer_index, int32 recast_delay) {
	if (timer_index > 0 && timer_index <= MaxSpellTimer) {
		timers[timer_index - 1] = Timer::GetCurrentTime() + recast_delay;
	}
}

int32 Bot::GetSpellRecastTimer(Bot *caster, int timer_index) {
	int32 result = 0;
	if (caster) {
		if (timer_index > 0 && timer_index <= MaxSpellTimer) {
			result = caster->timers[timer_index - 1];
		}
	}
	return result;
}

bool Bot::CheckSpellRecastTimers(Bot *caster, int SpellIndex) {
	if (caster) {
		if (caster->AIBot_spells[SpellIndex].time_cancast < Timer::GetCurrentTime()) { //checks spell recast
			if (GetSpellRecastTimer(caster, spells[caster->AIBot_spells[SpellIndex].spellid].timer_id) < Timer::GetCurrentTime()) { //checks for spells on the same timer
				return true; //can cast spell
			}
		}
	}
	return false;
}

void Bot::SetDisciplineRecastTimer(int timer_index, int32 recast_delay) {
	if (timer_index > 0 && timer_index <= MaxDisciplineTimer) {
		timers[DisciplineReuseStart + timer_index - 1] = Timer::GetCurrentTime() + recast_delay;
	}
}

int32 Bot::GetDisciplineRecastTimer(Bot *caster, int timer_index) {
	int32 result = 0;
	if (caster) {
		if (timer_index > 0 && timer_index <= MaxDisciplineTimer) {
			result = caster->timers[DisciplineReuseStart + timer_index - 1];
		}
	}
	return result;
}

uint32 Bot::GetDisciplineRemainingTime(Bot *caster, int timer_index) {
	int32 result = 0;
	if (caster) {
		if (timer_index > 0 && timer_index <= MaxDisciplineTimer) {
			if (GetDisciplineRecastTimer(caster, timer_index) > Timer::GetCurrentTime())
				result = GetDisciplineRecastTimer(caster, timer_index) - Timer::GetCurrentTime();
		}
	}
	return result;
}

bool Bot::CheckDisciplineRecastTimers(Bot *caster, int timer_index) {
	if (caster) {
		if (GetDisciplineRecastTimer(caster, timer_index) < Timer::GetCurrentTime()) { //checks for spells on the same timer
			return true; //can cast spell
		}
	}
	return false;
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
		spell_type_index = SPELL_TYPE_COUNT;
		break;
	}
	if (spell_type_index >= SPELL_TYPE_COUNT)
		return 0;

	uint8 class_index = GetClass();
	if (class_index > BERSERKER || class_index < WARRIOR)
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

	if (spell_list) {
		debug_msg.append(
			fmt::format(
				" (found, {})",
				spell_list->entries.size()
			)
		);

		LogAI("[{}]", debug_msg);
		for (const auto &iter : spell_list->entries) {
			LogAIDetail("([{}]) [{}]", iter.spellid, spells[iter.spellid].name);
		}
	}
	else
	{
		debug_msg.append(" (not found)");
		LogAI("[{}]", debug_msg);
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

	if (spell_list->attack_proc >= 0) {
		attack_proc_spell = spell_list->attack_proc;
		proc_chance = spell_list->proc_chance;
	}

	if (spell_list->range_proc >= 0) {
		range_proc_spell = spell_list->range_proc;
		rproc_chance = spell_list->rproc_chance;
	}

	if (spell_list->defensive_proc >= 0) {
		defensive_proc_spell = spell_list->defensive_proc;
		dproc_chance = spell_list->dproc_chance;
	}

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

		bot_spells_cache.insert(std::make_pair(bot_spell_id, spell_set));

		return &bot_spells_cache[bot_spell_id];
	}

	return nullptr;
}

// adds a spell to the list, taking into account priority and resorting list as needed.
void Bot::AddSpellToBotList(
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
						AIBot_spells[casting_spell_AIindex].time_cancast = Timer::GetCurrentTime() + (AIBot_spells[casting_spell_AIindex].recast_delay * 1000);
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
	}
	return false;
}

bool Bot::IsTargetAlreadyReceivingSpell(Mob* tar, uint16 spellid, std::string healType) {

	if (!tar || !spellid) {
		return true;
	}

	Mob* ultimate_tar = tar;

	if (IsNPC() && CastToNPC()->GetSwarmOwner()) {
		TestDebug("[{}] says, 'Swarm pet detected.'", GetCleanName()); //deleteme
		return true;
	}
	if (tar->IsPet()) {
		ultimate_tar = tar->GetOwner();
	}
	if (ultimate_tar->IsClient() || ultimate_tar->IsBot()) {
		if (ultimate_tar->IsRaidGrouped()) {
			Raid* raid = ultimate_tar->GetRaid();
			if (raid) {
				std::vector<RaidMember> raid_group_members = raid->GetMembers();
				for (std::vector<RaidMember>::iterator iter = raid_group_members.begin(); iter != raid_group_members.end(); ++iter) {
					if (iter->member && entity_list.IsMobInZone(iter->member) && iter->member->IsBot()) {
						if (
							iter->member->IsCasting()
							&& iter->member->CastToBot()->casting_spell_targetid
							&& entity_list.GetMobID(iter->member->CastToBot()->casting_spell_targetid) == entity_list.GetMobID(tar->GetID())
							&& iter->member->CastingSpellID() == spellid
							) {
							if (GetBotSpellType(this, spellid) == SpellType_Heal) {
								uint32 currentTime = Timer::GetCurrentTime();
								uint32 fasthealTime = tar->DontFastHealMeBefore();
								uint32 regularhealTime = tar->DontRegularHealMeBefore();
								uint32 completehealTime = tar->DontCompleteHealMeBefore();
								uint32 hothealTime = tar->DontHotHealMeBefore();
								if (healType == "Fast Heal" && fasthealTime <= currentTime) {
									return false;
								}
								else if (healType == "Regular Heal" && regularhealTime <= currentTime) {
									return false;
								}
								else if (healType == "Complete Heal" && completehealTime <= currentTime) {
									return false;
								}
								else if (healType == "HoT Heal" && hothealTime <= currentTime) {
									return false;
								}
								return true;
							}
							else if (CanCastBySpellType(this, tar, GetBotSpellType(this, spellid))) {
								return false;
							}
							else {
								return true;
							}
						}
					}
				}
			}
		}
		else if (ultimate_tar->IsGrouped()) {
			Group* group = ultimate_tar->GetGroup();
			if (group) {
				for (int counter = 0; counter < group->GroupCount(); counter++) {
					Mob* group_member = group->members[counter];
					if (group_member && entity_list.IsMobInZone(group_member) && group_member->IsBot()) {
						if (
							group_member->IsCasting()
							&& group_member->CastToBot()->casting_spell_targetid
							&& entity_list.GetMobID(group_member->CastToBot()->casting_spell_targetid) == entity_list.GetMobID(tar->GetID())
							&& group_member->CastingSpellID() == spellid
							) {
							if (GetBotSpellType(this, spellid) == SpellType_Heal) {
								uint32 currentTime = Timer::GetCurrentTime();
								uint32 fasthealTime = tar->DontFastHealMeBefore();
								uint32 regularhealTime = tar->DontRegularHealMeBefore();
								uint32 completehealTime = tar->DontCompleteHealMeBefore();
								uint32 hothealTime = tar->DontHotHealMeBefore();
								if (healType == "Fast Heal" && fasthealTime <= currentTime) {
									return false;
								}
								else if (healType == "Regular Heal" && regularhealTime <= currentTime) {
									return false;
								}
								else if (healType == "Complete Heal" && completehealTime <= currentTime) {
									return false;
								}
								else if (healType == "HoT Heal" && hothealTime <= currentTime) {
									return false;
								}
								return true;
							}
							else {
								return true;
							}
						}
					}
				}
			}
		}
	}
	else {
		TestDebug("[{}] says, 'Found a different type of target?.'", ultimate_tar->GetCleanName()); //deleteme
		return true;
	}
	return false;
}

uint32 Bot::GetBotSpellType(Bot* botCaster, uint16 spellid) {

	if (!botCaster || !spellid) {
		return 0;
	}

	uint32 spellType = 0;

	std::vector<BotSpells_Struct> botSpellList = botCaster->AIBot_spells;
	
	for (int i = botSpellList.size() - 1; i >= 0; i--) {
		if (botSpellList[i].spellid == spellid) {
			spellType = botSpellList[i].type;
			break;
		}
	}
	return spellType;
}

void Bot::SetBotSpellDelay(Bot* botCaster, uint32 spellid, Mob* tar) {

	if (!botCaster || !spellid) {
		return;
	}

	uint32 spellType = GetBotSpellType(botCaster, spellid);

	if (!spellType) {
		return;
	}

	switch (spellType) {
		case SpellType_Nuke:
			botCaster->m_nuke_delay_timer.Start(botCaster->GetNukeDelay());
			break;
		case SpellType_Heal:
			if (!tar) {
				break;
			}
			/* this is set prior to spell finishing and not used here. Would need more type checks
			if (botCaster->GetClass() != BARD) {
				uint8 fasthealThreshold = 35;
				uint8 regularhealThreshold = 55;
				uint8 completehealThreshold = 70;
				uint8 hothealThreshold = 85;
				uint32 fasthealDelay = 2500;
				uint32 regularhealDelay = 4500;
				uint32 completehealDelay = 8000;
				uint32 hothealDelay = 22000;
				if (tar->IsBot()) {
					fasthealThreshold = tar->CastToBot()->GetFastHealThreshold();
					regularhealThreshold = tar->CastToBot()->GetHealThreshold();
					completehealThreshold = tar->CastToBot()->GetCompleteHealThreshold();
					hothealThreshold = tar->CastToBot()->GetHotHealThreshold();
					fasthealDelay = tar->CastToBot()->GetFastHealDelay();
					regularhealDelay = tar->CastToBot()->GetHealDelay();
					completehealDelay = tar->CastToBot()->GetCompleteHealDelay();
					hothealDelay = tar->CastToBot()->GetHotHealDelay();
				}
				else if (tar->IsClient()) {
					fasthealThreshold = tar->CastToClient()->GetClientFastHealThreshold();
					regularhealThreshold = tar->CastToClient()->GetClientHealThreshold();
					completehealThreshold = tar->CastToClient()->GetClientCompleteHealThreshold();
					hothealThreshold = tar->CastToClient()->GetClientHotHealThreshold();
					fasthealDelay = tar->CastToClient()->GetClientFastHealDelay();
					regularhealDelay = tar->CastToClient()->GetClientHealDelay();
					completehealDelay = tar->CastToClient()->GetClientCompleteHealDelay();
					hothealDelay = tar->CastToClient()->GetClientHotHealDelay();
				}
				else if (tar->IsPet()) {
					fasthealThreshold = 35;
					regularhealThreshold = 55;
					completehealThreshold = 0;
					hothealThreshold = 70;
				}
				tar->SetDontFastHealMeBefore(Timer::GetCurrentTime() + tar->GetFastHealDelay());
				tar->SetDontRegularHealMeBefore(Timer::GetCurrentTime() + tar->GetHealDelay());
				tar->SetDontCompleteHealMeBefore(Timer::GetCurrentTime() + tar->GetCompleteHealDelay());
				tar->SetDontHotHealMeBefore(Timer::GetCurrentTime() + tar->GetHotHealDelay());
			} */
			break;
		case SpellType_Root:
			botCaster->m_root_delay_timer.Start(botCaster->GetRootDelay());
			break;
		case SpellType_Buff:
			botCaster->m_buff_delay_timer.Start(botCaster->GetBuffDelay());
			break;
		case SpellType_Escape:
			botCaster->m_escape_delay_timer.Start(botCaster->GetEscapeDelay());
			break;
		case SpellType_Pet:
			// no delay currently
			break;
		case SpellType_Lifetap:
			botCaster->m_lifetap_delay_timer.Start(botCaster->GetLifetapDelay());
			break;
		case SpellType_Snare:
			botCaster->m_snare_delay_timer.Start(botCaster->GetSnareDelay());
			break;
		case SpellType_DOT:
			botCaster->m_dot_delay_timer.Start(botCaster->GetDotDelay());
			break;
		case SpellType_Dispel:
			botCaster->m_dispel_delay_timer.Start(botCaster->GetDispelDelay());
			break;
		case SpellType_InCombatBuff:
			if (botCaster->GetClass() != BARD) {
				botCaster->m_incombatbuff_delay_timer.Start(botCaster->GetInCombatBuffDelay());
			}
			break;
		case SpellType_Mez:
			botCaster->m_mez_delay_timer.Start(botCaster->GetMezDelay());
			break;
		case SpellType_Charm:
			// no delay currently
			break;
		case SpellType_Slow:
			botCaster->m_slow_delay_timer.Start(botCaster->GetSlowDelay());
			break;
		case SpellType_Debuff:
			botCaster->m_debuff_delay_timer.Start(botCaster->GetDebuffDelay());
			break;
		case SpellType_Cure:
			botCaster->m_cure_delay_timer.Start(botCaster->GetCureDelay());
			break;
		case SpellType_Resurrect:
			// no delay currently
			break;
		case SpellType_HateRedux:
			botCaster->m_hateredux_delay_timer.Start(botCaster->GetHateReduxDelay());
			break;
		case SpellType_InCombatBuffSong:
			// no delay currently
			break;
		case SpellType_OutOfCombatBuffSong:
			// no delay currently
			break;
		case SpellType_PreCombatBuff:
			// no delay currently
			break;
		case SpellType_PreCombatBuffSong:
			// no delay currently
			break;
		default:
			// no delay currently
			break;
	}
}
