#ifndef SSF_LOGIC_H
#define SSF_LOGIC_H

#include "data_verification.h"
#include "eq_constants.h"

class SSFLogic {
public:
    static bool IsAnyChallengeRuleset(uint8 flag) { return flag != SSFFlagTypes::Normal && flag != SSFFlagTypes::Undeclared; }
    static bool IsAnySelfFound(uint8 flag) { return EQ::ValueWithin(flag, SSFFlagTypes::SelfFound, SSFFlagTypes::SFSoloHardcore); }
    static bool IsAnySoloRuleset(uint8 flag) { return flag == SSFFlagTypes::Solo || flag == SSFFlagTypes::SoloHardcore || flag == SSFFlagTypes::SFSolo || flag == SSFFlagTypes::SFSoloHardcore; }
    static bool IsHardcore(uint8 flag) { return flag == SSFFlagTypes::Hardcore || flag == SSFFlagTypes::SoloHardcore || flag == SSFFlagTypes::SFHardcore || flag == SSFFlagTypes::SFSoloHardcore; }
    static bool SelfFoundMismatch(uint8 my_flag, uint8 other_flag) { return IsAnySelfFound(my_flag) && !IsAnySelfFound(other_flag) || !IsAnySelfFound(my_flag) && IsAnySelfFound(other_flag); }

    static std::string AddSelfFoundToName(uint8 flag, std::string name);
    static std::string StripSelfFoundFromName(std::string name);

    static std::string GetSSFKeyPrefix(uint8 flag);
    static std::string GetPlayerType(uint8 flag);
};

#endif