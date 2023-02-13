#include "bot.h"
#include "client.h"

bool Client::GetBotOption(BotOwnerOption boo) const {
	if (boo < _booCount) {
		return bot_owner_options[boo];
	}

	return false;
}

void Client::SetBotOption(BotOwnerOption boo, bool flag) {
	if (boo < _booCount) {
		bot_owner_options[boo] = flag;
	}
}

uint32 Client::GetBotCreationLimit(uint8 class_id)
{
	uint32 bot_creation_limit = RuleI(Bots, CreationLimit);

	const auto bucket_name = fmt::format(
		"bot_creation_limit{}",
		(
			class_id && IsPlayerClass(class_id) ?
			fmt::format(
				"_{}",
				Strings::ToLower(GetClassIDName(class_id))
			) :
			""
		)
	);

	auto bucket_value = GetBucket(bucket_name);
	if (!bucket_value.empty() && Strings::IsNumber(bucket_value)) {
		bot_creation_limit = std::stoul(bucket_value);
	}

	return bot_creation_limit;
}

int Client::GetBotRequiredLevel(uint8 class_id)
{
	int bot_character_level = RuleI(Bots, BotCharacterLevel);

	const auto bucket_name = fmt::format(
		"bot_required_level{}",
		(
			class_id && IsPlayerClass(class_id) ?
			fmt::format(
				"_{}",
				Strings::ToLower(GetClassIDName(class_id))
			) :
			""
		)
	);

	auto bucket_value = GetBucket(bucket_name);
	if (!bucket_value.empty() && Strings::IsNumber(bucket_value)) {
		bot_character_level = std::stoi(bucket_value);
	}

	return bot_character_level;
}

int Client::GetBotSpawnLimit(uint8 class_id, uint32 spawned_bot_count)
{
	int bot_spawn_limit = RuleI(Bots, SpawnLimit);

	if (RuleB(Bots, TieredSpawnLimitUnlocks) && !GetGM()) {
		auto current_level = GetLevel();

		if (current_level < RuleI(Bots, TierOneBotSpawnLimitUnlockLevelRequirement)) {
			bot_spawn_limit = 0;
			Message(Chat::White, "You must reach level %i to unlock the ability to spawn %i bot(s).", RuleI(Bots, TierOneBotSpawnLimitUnlockLevelRequirement), RuleI(Bots, TierOneBotSpawnLimitUnlock));
			return 999;
		}
		else if (current_level >= RuleI(Bots, TierOneBotSpawnLimitUnlockLevelRequirement) && current_level < RuleI(Bots, TierTwoBotSpawnLimitUnlockLevelRequirement)) {
			bot_spawn_limit = RuleI(Bots, TierOneBotSpawnLimitUnlock);
			if (spawned_bot_count == bot_spawn_limit && current_level < RuleI(Bots, TierTwoBotSpawnLimitUnlockLevelRequirement)) {
				Message(Chat::White, "You can not spawn more than %i spawned bot(s) until you reach level %i.", bot_spawn_limit, RuleI(Bots, TierTwoBotSpawnLimitUnlockLevelRequirement));
				return 999;
			}
		}
		else if (current_level >= RuleI(Bots, TierTwoBotSpawnLimitUnlockLevelRequirement) && current_level < RuleI(Bots, TierThreeBotSpawnLimitUnlockLevelRequirement)) {
			bot_spawn_limit = RuleI(Bots, TierTwoBotSpawnLimitUnlock);
			if (spawned_bot_count == bot_spawn_limit && current_level < RuleI(Bots, TierThreeBotSpawnLimitUnlockLevelRequirement)) {
				Message(Chat::White, "You can not spawn more than %i spawned bot(s) until you reach level %i.", bot_spawn_limit, RuleI(Bots, TierThreeBotSpawnLimitUnlockLevelRequirement));
				return 999;
			}
		}
		else if (current_level >= RuleI(Bots, TierThreeBotSpawnLimitUnlockLevelRequirement) && current_level < RuleI(Bots, TierFourBotSpawnLimitUnlockLevelRequirement)) {
			bot_spawn_limit = RuleI(Bots, TierThreeBotSpawnLimitUnlock);
			if (spawned_bot_count == bot_spawn_limit && current_level < RuleI(Bots, TierFourBotSpawnLimitUnlockLevelRequirement)) {
				Message(Chat::White, "You can not spawn more than %i spawned bot(s) until you reach level %i.", bot_spawn_limit, RuleI(Bots, TierFourBotSpawnLimitUnlockLevelRequirement));
				return 999;
			}
		}
		else if (current_level >= RuleI(Bots, TierFourBotSpawnLimitUnlockLevelRequirement) && current_level < RuleI(Bots, TierFiveBotSpawnLimitUnlockLevelRequirement)) {
			bot_spawn_limit = RuleI(Bots, TierFourBotSpawnLimitUnlock);
			if (spawned_bot_count == bot_spawn_limit && current_level < RuleI(Bots, TierFiveBotSpawnLimitUnlockLevelRequirement)) {
				Message(Chat::White, "You can not spawn more than %i spawned bot(s) until you reach level %i.", bot_spawn_limit, RuleI(Bots, TierFiveBotSpawnLimitUnlockLevelRequirement));
				return 999;
			}
		}
		else if (current_level >= RuleI(Bots, TierFiveBotSpawnLimitUnlockLevelRequirement)) {
			bot_spawn_limit = RuleI(Bots, TierFiveBotSpawnLimitUnlock);
		}
	}

	const auto bucket_name = fmt::format(
		"bot_spawn_limit{}",
		(
			class_id && IsPlayerClass(class_id) ?
			fmt::format(
				"_{}",
				Strings::ToLower(GetClassIDName(class_id))
			) :
			""
		)
	);

	auto bucket_value = GetBucket(bucket_name);
	if (!bucket_value.empty() && Strings::IsNumber(bucket_value)) {
		bot_spawn_limit = std::stoi(bucket_value);
		return bot_spawn_limit;
	}

	if (RuleB(Bots, QuestableSpawnLimit)) {
		const auto query = fmt::format(
			"SELECT `value` FROM `quest_globals` WHERE `name` = '{}' AND `charid` = {} LIMIT 1",
			bucket_name,
			CharacterID()
		);

		auto results = database.QueryDatabase(query); // use 'database' for non-bot table calls
		if (!results.Success() || !results.RowCount()) {
			return bot_spawn_limit;
		}

		auto row = results.begin();
		bot_spawn_limit = std::stoi(row[0]);
	}

	return bot_spawn_limit;
}

void Client::SetBotCreationLimit(uint32 new_creation_limit, uint8 class_id)
{
	const auto bucket_name = fmt::format(
		"bot_creation_limit{}",
		(
			class_id && IsPlayerClass(class_id) ?
			fmt::format(
				"_{}",
				Strings::ToLower(GetClassIDName(class_id))
			) :
			""
		)
	);

	SetBucket(bucket_name, std::to_string(new_creation_limit));
}

void Client::SetBotRequiredLevel(int new_required_level, uint8 class_id)
{
	const auto bucket_name = fmt::format(
		"bot_required_level{}",
		(
			class_id && IsPlayerClass(class_id) ?
			fmt::format(
				"_{}",
				Strings::ToLower(GetClassIDName(class_id))
			) :
			""
		)
	);

	SetBucket(bucket_name, std::to_string(new_required_level));
}

void Client::SetBotSpawnLimit(int new_spawn_limit, uint8 class_id)
{
	const auto bucket_name = fmt::format(
		"bot_spawn_limit{}",
		(
			class_id && IsPlayerClass(class_id) ?
			fmt::format(
				"_{}",
				Strings::ToLower(GetClassIDName(class_id))
			) :
			""
		)
	);

	SetBucket(bucket_name, std::to_string(new_spawn_limit));
}

void Client::CampAllBots(uint8 class_id)
{
	Bot::BotOrderCampAll(this, class_id);
}
