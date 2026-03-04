#include "ssf_logic.h"

#include "eq_constants.h"
#include "rulesys.h"

std::string SSFLogic::AddSelfFoundToName(uint8 ssf_flag, std::string name) {
    if (!RuleB(SSF, ShowSSFTagsOnWho)) {
        return name;
    }

    if (SSFLogic::IsAnyChallengeRuleset(ssf_flag)) {
        std::string str = "[";

        switch (ssf_flag) {
            case SSFFlagTypes::Hardcore:
                str += "HC";
                break;
            case SSFFlagTypes::Solo:
                str += "S";
                break;
            case SSFFlagTypes::SoloHardcore:
                str += "HCS";
                break;
            case SSFFlagTypes::SelfFound:
                str += "SF";
                break;
            case SSFFlagTypes::SFSolo:
                str += "SSF";
                break;
            case SSFFlagTypes::SFHardcore:
                str += "HCSF";
                break;
            case SSFFlagTypes::SFSoloHardcore:
                str += "HCSSF";
                break;
            default:
                break;
        }

        str += "] ";

        auto remainingSpace = 63 - name.length();

        // Append the flag to the name if there's enough space
        if (remainingSpace > str.length()) {
            name = str + name;
        }
    }

    return name;
}

std::string SSFLogic::StripSelfFoundFromName(std::string name) {
    if (name[0] != '[') {
        return name;
    }

    size_t closing_bracket_pos = name.find("] ");

    if (closing_bracket_pos != std::string::npos) {
        return name.substr(closing_bracket_pos + 2);
    }

    return name;
}

std::string SSFLogic::GetSSFKeyPrefix(uint8 flag) {
    if (!flag) { return ""; }

    if (SSFLogic::IsAnyChallengeRuleset(flag)) {
        if (SSFLogic::IsHardcore(flag) && SSFLogic::IsAnySoloRuleset(flag) && !SSFLogic::IsAnySelfFound(flag)) {
            return "HardcoreSolo";
        }

        if (SSFLogic::IsHardcore(flag) && SSFLogic::IsAnySoloRuleset(flag) && SSFLogic::IsAnySelfFound(flag)) {
            return "HardcoreSoloSelfFound";
        }

        if (SSFLogic::IsHardcore(flag) && SSFLogic::IsAnySelfFound(flag)) {
            return "HardcoreSelfFound";
        }

        if (SSFLogic::IsHardcore(flag) && !SSFLogic::IsAnySelfFound(flag)) {
            return "Hardcore";
        }

        if (SSFLogic::IsAnySoloRuleset(flag) && SSFLogic::IsAnySelfFound(flag)) {
            return "SoloSelfFound";
        }

        if (SSFLogic::IsAnySoloRuleset(flag) && !SSFLogic::IsAnySelfFound(flag)) {
            return "Solo";
        }

        if (SSFLogic::IsAnySelfFound(flag)) {
            return "SelfFound";
        }
    }

    return "";
}

std::string SSFLogic::GetPlayerType(uint8 flag) {
    if (!flag) { return ""; }

    if (SSFLogic::IsAnyChallengeRuleset(flag)) {
        if (SSFLogic::IsHardcore(flag) && SSFLogic::IsAnySoloRuleset(flag) && !SSFLogic::IsAnySelfFound(flag)) {
            return "Hardcore Solo";
        }

        if (SSFLogic::IsHardcore(flag) && SSFLogic::IsAnySoloRuleset(flag) && SSFLogic::IsAnySelfFound(flag)) {
            return "Hardcore Solo Self Found";
        }

        if (SSFLogic::IsHardcore(flag) && SSFLogic::IsAnySelfFound(flag)) {
            return "Hardcore Self Found";
        }

        if (SSFLogic::IsHardcore(flag) && !SSFLogic::IsAnySelfFound(flag)) {
            return "Hardcore";
        }

        if (SSFLogic::IsAnySoloRuleset(flag) && SSFLogic::IsAnySelfFound(flag)) {
            return "Solo Self Found";
        }

        if (SSFLogic::IsAnySoloRuleset(flag) && !SSFLogic::IsAnySelfFound(flag)) {
            return "Solo";
        }

        if (SSFLogic::IsAnySelfFound(flag)) {
            return "Self Found";
        }
    }

    return "";
}
