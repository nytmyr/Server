#ifndef ANTICHEAT_H
#define ANTICHEAT_H
class CheatManager;
class Client;

#include "../common/timer.h"
#include "../common/rulesys.h"
#include <glm/ext/vector_float3.hpp>
#include "../common/eq_packet_structs.h"
#include "../common/eq_packet.h"

typedef enum {
	Collision = 1,
	TeleportB,
	TeleportA,
	ZoneLine,
	Unknown0x5,
	Unknown0x6,
	SpellA, // Titanium - UF
	Unknown0x8,
	SpellB // Used in RoF+
} UpdateMovementType;

typedef enum {
	ShadowStep,
	KnockBack,
	Port,
	Assist,
	Sense,
	MAX_EXEMPTIONS
} ExemptionType;

typedef enum {
	MQWarp,
	MQWarpShadowStep,
	MQWarpKnockBack,
	MQWarpLight,
	MQZone,
	MQZoneUnknownDest,
	MQGate,
	MQGhost,
	MQFastMem,
	MQWarpAbsolute
} CheatTypes;

namespace WarpTypes {
	constexpr int Bypass = 0;
	constexpr int LargeWarp = 1;
    constexpr int Absolute = 2;
    constexpr int ShadowStepSuspicion = 3;
    constexpr int KnockbackSuspicion = 4;
    constexpr int LightWarpSuspicion = 5;
    constexpr int Zone = 6;
    constexpr int ZoneUnknown = 7;
    constexpr int Gate = 8;
    constexpr int Ghost = 9;
    constexpr int FastMem = 10;
    constexpr int Unhandled = 11;
    constexpr int OP_BankerChangeNonBanker = 12;
    constexpr int OP_CastSpellWithItemDiffSpell = 13;
    constexpr int OP_CastSpellWithItemLvlReq = 14;
    constexpr int OP_ControlBoat = 15;
    constexpr int OP_Disarm = 16;
    constexpr int OP_Illusion = 17;
    constexpr int CastMoveItem = 18;
    constexpr int OP_PickPocketTooQuick = 19;
    constexpr int OP_PickPocketOutRange = 20;
    constexpr int VendorCheatBuy = 21;
    constexpr int OP_SpawnAppearanceAT_Invis = 22;
    constexpr int OP_SpawnAppearanceAT_Sneak = 23;
    constexpr int OP_SpawnAppearanceAT_Size = 24;
    constexpr int TargetUntargetable = 25;
    constexpr int TargetBeyondClipMouseBindSight = 26;
    constexpr int TargetBeyondClipMouse = 27;
    constexpr int MQInstantCamp = 28;
    constexpr int OP_MemorizeSpell = 29;
    constexpr int BankerDistanceCoinFrom = 30;
    constexpr int BankerDistanceSharedCoinFrom = 31;
    constexpr int BankerDistanceCoinTo = 32;
    constexpr int BankerDistanceSharedCoinTo = 33;
    constexpr int BankerDistanceItems = 34;
    constexpr int BankerDistanceMoney = 35;
    constexpr int ItemEquipOnlyClick = 36;
    constexpr int ItemRaceClassClick = 37;
    constexpr int ItemClickUnequipped = 38;
    constexpr int BazaarBuyMoney = 39;
    constexpr int BazaarOutsideBuyMoney = 40;
    constexpr int BecomeNPCCommand = 41;
    constexpr int DeleteCorpseCommand = 42;
    constexpr int EmoteCommand = 43;
    constexpr int FindCommand = 44;
    constexpr int GoToCommand = 45;
    constexpr int HideMeCommand = 46;
    constexpr int KickCommand = 47;
    constexpr int KillCommand = 48;
    constexpr int LastNameCommand = 49;
    constexpr int NameCommand = 50;
    constexpr int ToggleCommand = 51;
    constexpr int ZoneCommand = 52;
    constexpr int ZoneCommand2 = 53;
}

class CheatManager {
public:
	CheatManager()
	{
		SetExemptStatus(ShadowStep, false);
		SetExemptStatus(KnockBack, false);
		SetExemptStatus(Port, false);
		SetExemptStatus(Assist, false);
		SetExemptStatus(Sense, false);
		m_distance_since_last_position_check = 0.0f;
		m_cheat_detect_moved                 = false;
		m_target                             = nullptr;
		m_time_since_last_memorization       = 0;
		m_time_since_last_position_check     = 0;
		m_time_since_last_warp_detection.Start();
		m_time_since_last_movement_history.Start(70000);
		m_warp_counter = 0;
	}
	void SetClient(Client *cli);
	void SetExemptStatus(ExemptionType type, bool v);
	bool GetExemptStatus(ExemptionType type);
	void CheatDetected(CheatTypes type, glm::vec3 position1, glm::vec3 position2 = glm::vec3(0, 0, 0));
	void MovementCheck(glm::vec3 updated_position);
	void MovementCheck(uint32 time_between_checks = 1000);
	void CheckMemTimer();
	void ProcessMovementHistory(const EQApplicationPacket *app);
	void ProcessSpawnApperance(uint16 spawn_id, uint16 type, uint32 parameter);
	void ProcessItemVerifyRequest(int32 slot_id, uint32 target_id);
	void ClientProcess();
private:
	bool  m_exemption[ExemptionType::MAX_EXEMPTIONS]{};
	float m_distance_since_last_position_check;
	bool  m_cheat_detect_moved;

	Client *m_target;
	uint32 m_time_since_last_position_check;
	uint32 m_time_since_last_memorization;
	uint32 m_time_since_last_action{};
	Timer  m_time_since_last_warp_detection;
	Timer  m_time_since_last_movement_history;
	uint32 m_warp_counter;
};

#endif //ANTICHEAT_H
