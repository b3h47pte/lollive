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
  if (champion == "") {
    champion = inPlayer->champion;
  }

  // TODO: This will probably have to change at some point. Probably from user input.
  if (inPlayer->kills > kills && inPlayer->kills - kills <= 3) {
    kills = inPlayer->kills;
  } else {
    kills = SmoothValueVariance<int>(inPlayer->kills, kills, [&](std::shared_ptr<GenericDataStore> data) {
      return GetPlayerData(data, team, playerIdx)->kills;
    }, dataHistory);
  }

  if (inPlayer->deaths > deaths && inPlayer->deaths - deaths <= 2) {
    deaths = inPlayer->deaths;
  } else {
    deaths = SmoothValueVariance<int>(inPlayer->deaths, deaths, [&](std::shared_ptr<GenericDataStore> data) {
      return GetPlayerData(data, team, playerIdx)->deaths;
    }, dataHistory);
  }

  if (inPlayer->assists > assists && inPlayer->assists - assists <= 3) {
    assists = inPlayer->assists;
  } else {
    assists = SmoothValueVariance<int>(inPlayer->assists, assists, [&](std::shared_ptr<GenericDataStore> data) {
      return GetPlayerData(data, team, playerIdx)->assists;
    }, dataHistory);
  }

  // Need some sanity check here. Hopefully 100cs is reasonable..?
  if (inPlayer->cs > cs && inPlayer->cs - cs < 100) {
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
  // TODO: Make threshold configurable
  if (inPlayer->isLowHealth != isLowHealth && (timeStamp - lastLowHealthUpdate) > 5) {
    isLowHealth = inPlayer->isLowHealth;
    lastLowHealthUpdate = timeStamp;
  }
}

// JSONify
cJSON* LeaguePlayerData::CreateJSON() {
  cJSON* newJson = cJSON_CreateObject();
  cJSON_AddNumberToObject(newJson, "kills", kills);
  cJSON_AddNumberToObject(newJson, "deaths", deaths);
  cJSON_AddNumberToObject(newJson, "assists", assists);
  cJSON_AddNumberToObject(newJson, "cs", cs);
  cJSON_AddNumberToObject(newJson, "level", level);
  cJSON_AddBoolToObject(newJson, "isDead", isDead);
  cJSON_AddBoolToObject(newJson, "isLowHealth", isLowHealth);
  cJSON_AddStringToObject(newJson, "name", name.c_str());
  cJSON_AddStringToObject(newJson, "champion", champion.c_str());

  cJSON* itemsArr = cJSON_CreateArray();
  for (int i = 0; i < 7; ++i) {
    cJSON_AddItemToArray(itemsArr, cJSON_CreateString(items[i].c_str()));
  }
  cJSON_AddItemToObject(newJson, "items", itemsArr);

  return newJson;
}