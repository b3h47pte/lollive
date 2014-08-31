#include "LeagueTeamData.h"
#include "cjson/cJSON.h"
/*
 * Update only if the new data makes sense.
 * For example, kills can't go down.
 */
void LeagueTeamData::Update(PtrLeagueTeamData newData, int timeStamp) {
  if (newData->kills > kills) {
    kills = newData->kills;
  }

  if (newData->gold > gold) {
    gold = newData->gold;
  }

  if (newData->towerKills > towerKills) {
    towerKills = newData->towerKills;
  }

  // Now make sure the players get updated too
  for (int i = 0; i < 5; ++i) {
    players[i]->Update(newData->players[i], timeStamp);
  }
}

// JSONify
cJSON* LeagueTeamData::CreateJSON() {
  cJSON* newJson = cJSON_CreateObject();
  cJSON_AddNumberToObject(newJson, "teamkills", kills);
  cJSON_AddNumberToObject(newJson, "teamgold", gold);
  cJSON_AddNumberToObject(newJson, "teamtowers", towerKills);

  cJSON* playersJson = cJSON_CreateArray();
  for (int i = 0; i < 5; ++i) {
    cJSON_AddItemToArray(playersJson, players[i]->CreateJSON());
  }

  cJSON_AddItemToObject(newJson, "players", playersJson);

  return newJson;
}