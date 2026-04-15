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

std::string SSFLogic::GetChallengeRulesetKeyPrefix(uint8 flag) {
    if (!flag) { return ""; }

    switch (flag) {
        case SSFFlagTypes::Hardcore:
            return "Hardcore";
        case SSFFlagTypes::Solo:
            return "Solo";
        case SSFFlagTypes::SoloHardcore:
            return "HardcoreSolo";
        case SSFFlagTypes::SelfFound:
            return "SelfFound";
        case SSFFlagTypes::SFSolo:
            return "SoloSelfFound";
        case SSFFlagTypes::SFHardcore:
            return "HardcoreSelfFound";
        case SSFFlagTypes::SFSoloHardcore:
            return "HardcoreSoloSelfFound";
        default:
            return "";
    }
}

std::string SSFLogic::GetChallengeRulesetPlayerType(uint8 flag) {
    if (!flag) { return ""; }

    switch (flag) {
        case SSFFlagTypes::Hardcore:
            return "Hardcore";
        case SSFFlagTypes::Solo:
            return "Solo";
        case SSFFlagTypes::SoloHardcore:
            return "Hardcore Solo";
        case SSFFlagTypes::SelfFound:
            return "Self Found";
        case SSFFlagTypes::SFSolo:
            return "Solo Self Found";
        case SSFFlagTypes::SFHardcore:
            return "Hardcore Self Found";
        case SSFFlagTypes::SFSoloHardcore:
            return "Hardcore Solo Self Found";
        default:
            return "";
    }
}
