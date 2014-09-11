#include "LeagueTeamData.h"
#include "cjson/cJSON.h"


PtrLeagueTeamData GetTeamData(std::shared_ptr<GenericDataStore> data, ELeagueTeams team) {
  PtrLeagueTeamData td = (team == ELT_BLUE) ? RetrieveData<PtrLeagueTeamData>(data, std::string("BlueTeam")) : RetrieveData<PtrLeagueTeamData>(data, std::string("PurpleTeam"));
  return td;
}



/*
 * Update only if the new data makes sense.
 * For example, kills can't go down.
 */
void LeagueTeamData::Update(PtrLeagueTeamData newData, int timeStamp, std::vector<std::shared_ptr<GenericDataStore>>& dataHistory) {
  if (newData->kills > kills) {
    kills = newData->kills;
  } else {
    kills = SmoothValueVariance<int>(newData->kills, kills, [&](std::shared_ptr<GenericDataStore> data) {
      return GetTeamData(data, team)->kills;
    }, dataHistory);
  }

  if (newData->gold > gold) {
    gold = newData->gold;
  } else {
    gold = SmoothValueVariance<int>(newData->gold, gold, [&](std::shared_ptr<GenericDataStore> data) {
      return GetTeamData(data, team)->gold;
    }, dataHistory);
  }

  if (newData->towerKills > towerKills) {
    towerKills = newData->towerKills;
  } else {
    towerKills = SmoothValueVariance<int>(newData->towerKills, towerKills, [&](std::shared_ptr<GenericDataStore> data) {
      return GetTeamData(data, team)->towerKills;
    }, dataHistory);
  }

  // Update bans
  for (int i = 0; i < 3; ++i) {
    bans[i] = newData->bans[i];
  }

  // Now make sure the players get updated too
  for (int i = 0; i < 5; ++i) {
    players[i]->Update(newData->players[i], timeStamp, dataHistory);
  }
}

// JSONify
cJSON* LeagueTeamData::CreateJSON() {
  cJSON* newJson = cJSON_CreateObject();
  cJSON_AddNumberToObject(newJson, ConfigManager::Get()->GetStringFromINI(ConfigManager::CONFIG_LEAGUE_FILENAME, TeamDataSection, JsonTeamKillsName, std::string("")).c_str(), kills);
  cJSON_AddNumberToObject(newJson, ConfigManager::Get()->GetStringFromINI(ConfigManager::CONFIG_LEAGUE_FILENAME, TeamDataSection, JsonTeamGoldName, std::string("")).c_str(), gold);
  cJSON_AddNumberToObject(newJson, ConfigManager::Get()->GetStringFromINI(ConfigManager::CONFIG_LEAGUE_FILENAME, TeamDataSection, JsonTeamTowersName, std::string("")).c_str(), towerKills);
  cJSON_AddStringToObject(newJson, ConfigManager::Get()->GetStringFromINI(ConfigManager::CONFIG_LEAGUE_FILENAME, TeamDataSection, JsonTeamNameName, std::string("")).c_str(), teamName.c_str());
  cJSON_AddNumberToObject(newJson, ConfigManager::Get()->GetStringFromINI(ConfigManager::CONFIG_LEAGUE_FILENAME, TeamDataSection, JsonTeamGamesWonName, std::string("")).c_str(), teamScore);

  cJSON* bansJson = cJSON_CreateArray();
  for (int i = 0; i < 3; ++i) {
    if (bans[i] != "") {
      cJSON_AddItemToArray(bansJson, cJSON_CreateString(bans[i].c_str()));
    }
  }
  cJSON_AddItemToObject(newJson, ConfigManager::Get()->GetStringFromINI(ConfigManager::CONFIG_LEAGUE_FILENAME, TeamDataSection, JsonTeamBansName, std::string("")).c_str(), bansJson);

  cJSON* playersJson = cJSON_CreateArray();
  for (int i = 0; i < 5; ++i) {
    cJSON_AddItemToArray(playersJson, players[i]->CreateJSON());
  }

  cJSON_AddItemToObject(newJson, ConfigManager::Get()->GetStringFromINI(ConfigManager::CONFIG_LEAGUE_FILENAME, TeamDataSection, JsonTeamPlayersName, std::string("")).c_str(), playersJson);

  return newJson;
}