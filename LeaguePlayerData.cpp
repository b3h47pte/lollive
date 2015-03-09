#include "LeaguePlayerData.h"
#include "LeagueTeamData.h"
#include "cjson/cJSON.h"

PtrLeaguePlayerData GetPlayerData(std::shared_ptr<GenericDataStore> data, ELeagueTeams team, int idx) {
  PtrLeagueTeamData td = (team == ELT_BLUE) ? RetrieveData<PtrLeagueTeamData>(data, std::string("BlueTeam")) : RetrieveData<PtrLeagueTeamData>(data, std::string("PurpleTeam"));
  return td->players[idx];
}

/*
 * Update only if the new data makes sense.
 * For example, kills can't go down.
 */
void LeaguePlayerData::Update(PtrLeaguePlayerData inPlayer, int timeStamp, std::vector<std::shared_ptr<GenericDataStore>>& dataHistory) {
  // If we don't yet have a champion, update the champion
  if (champion == "" || champion == "None") {
    champion = inPlayer->champion;
  }

  // TODO: This will probably have to change at some point. Probably from user input.
  int killThreshold = ConfigManager::Get()->GetIntFromINI(ConfigManager::CONFIG_LEAGUE_FILENAME, PlayerDataSection, KillUpdateThresholdName, 0);
  if (inPlayer->kills > kills && inPlayer->kills - kills <= killThreshold) {
    kills = inPlayer->kills;
  } else {
    kills = SmoothValueVariance<int>(inPlayer->kills, kills, [&](std::shared_ptr<GenericDataStore> data) {
      return GetPlayerData(data, team, playerIdx)->kills;
    }, dataHistory);
  }

  int deathThreshold = ConfigManager::Get()->GetIntFromINI(ConfigManager::CONFIG_LEAGUE_FILENAME, PlayerDataSection, DeathUpdateThresholdName, 0);
  if (inPlayer->deaths > deaths && inPlayer->deaths - deaths <= deathThreshold) {
    deaths = inPlayer->deaths;
  } else {
    deaths = SmoothValueVariance<int>(inPlayer->deaths, deaths, [&](std::shared_ptr<GenericDataStore> data) {
      return GetPlayerData(data, team, playerIdx)->deaths;
    }, dataHistory);
  }


  int assistThreshold = ConfigManager::Get()->GetIntFromINI(ConfigManager::CONFIG_LEAGUE_FILENAME, PlayerDataSection, AssistUpdateThresholdName, 0);
  if (inPlayer->assists > assists && inPlayer->assists - assists <= assistThreshold) {
    assists = inPlayer->assists;
  } else {
    assists = SmoothValueVariance<int>(inPlayer->assists, assists, [&](std::shared_ptr<GenericDataStore> data) {
      return GetPlayerData(data, team, playerIdx)->assists;
    }, dataHistory);
  }

  // Need some sanity check here. Hopefully 100cs is reasonable..?
  int csThreshold = ConfigManager::Get()->GetIntFromINI(ConfigManager::CONFIG_LEAGUE_FILENAME, PlayerDataSection, CreepUpdateThresholdName, 0);
  if (inPlayer->cs > cs && inPlayer->cs - cs < csThreshold) {
    cs = inPlayer->cs;
  } else {
    // Smooth just in case. Usually this won't result in anything special since CS usually changes constantly.
    cs = SmoothValueVariance<int>(inPlayer->cs, cs, [&](std::shared_ptr<GenericDataStore> data) {
      return GetPlayerData(data, team, playerIdx)->cs;
    }, dataHistory);
  }

  // Need to do a straight copy of items. No real way to sanity check this. 
  for (int i = 0; i < 7; ++i) {
    items[i] = inPlayer->items[i];
  }

  if (inPlayer->level > level) {
    level = inPlayer->level;
  }

  isDead = inPlayer->isDead;
  
  // isLowHealth is a tricky because it flashes on and off [we don't read the health bar].
  // This requires us to know when we last updated this value and introduce some arbitrary threshold.
  int lowHealthThreshold = ConfigManager::Get()->GetIntFromINI(ConfigManager::CONFIG_LEAGUE_FILENAME, PlayerDataSection, LowHealthThresholdName, 0);
  if (inPlayer->isLowHealth != isLowHealth && (timeStamp - lastLowHealthUpdate) > lowHealthThreshold) {
    isLowHealth = inPlayer->isLowHealth;
    lastLowHealthUpdate = timeStamp;
  }
}

// JSONify
cJSON* LeaguePlayerData::CreateJSON() {
  cJSON* newJson = cJSON_CreateObject();
  cJSON_AddNumberToObject(newJson, ConfigManager::Get()->GetStringFromINI(ConfigManager::CONFIG_LEAGUE_FILENAME, PlayerDataSection, JsonKillsName, std::string("")).c_str(), kills);
  cJSON_AddNumberToObject(newJson, ConfigManager::Get()->GetStringFromINI(ConfigManager::CONFIG_LEAGUE_FILENAME, PlayerDataSection, JsonDeathsName, std::string("")).c_str(), deaths);
  cJSON_AddNumberToObject(newJson, ConfigManager::Get()->GetStringFromINI(ConfigManager::CONFIG_LEAGUE_FILENAME, PlayerDataSection, JsonAssistsName, std::string("")).c_str(), assists);
  cJSON_AddNumberToObject(newJson, ConfigManager::Get()->GetStringFromINI(ConfigManager::CONFIG_LEAGUE_FILENAME, PlayerDataSection, JsonCSName, std::string("")).c_str(), cs);
  cJSON_AddNumberToObject(newJson, ConfigManager::Get()->GetStringFromINI(ConfigManager::CONFIG_LEAGUE_FILENAME, PlayerDataSection, JsonLevelName, std::string("")).c_str(), level);
  cJSON_AddBoolToObject(newJson, ConfigManager::Get()->GetStringFromINI(ConfigManager::CONFIG_LEAGUE_FILENAME, PlayerDataSection, JsonIsDeadName, std::string("")).c_str(), isDead);
  cJSON_AddBoolToObject(newJson, ConfigManager::Get()->GetStringFromINI(ConfigManager::CONFIG_LEAGUE_FILENAME, PlayerDataSection, JsonLowHealthName, std::string("")).c_str(), isLowHealth);
  cJSON_AddStringToObject(newJson, ConfigManager::Get()->GetStringFromINI(ConfigManager::CONFIG_LEAGUE_FILENAME, PlayerDataSection, JsonNameName, std::string("")).c_str(), name.c_str());
  cJSON_AddStringToObject(newJson, ConfigManager::Get()->GetStringFromINI(ConfigManager::CONFIG_LEAGUE_FILENAME, PlayerDataSection, JsonChampionName, std::string("")).c_str(), champion.c_str());

  cJSON* itemsArr = cJSON_CreateArray();
  for (int i = 0; i < 7; ++i) {
    cJSON_AddItemToArray(itemsArr, cJSON_CreateString(items[i].c_str()));
  }
  cJSON_AddItemToObject(newJson, ConfigManager::Get()->GetStringFromINI(ConfigManager::CONFIG_LEAGUE_FILENAME, PlayerDataSection, JsonItemsName, std::string("")).c_str(), itemsArr);

  return newJson;
}