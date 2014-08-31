#include "LeaguePlayerData.h"
#include "cjson/cJSON.h"
/*
 * Update only if the new data makes sense.
 * For example, kills can't go down.
 */
void LeaguePlayerData::Update(PtrLeaguePlayerData inPlayer, int timeStamp) {
  // FOR NOW: Assume name and champion never changes
  // TODO: This will probably have to change at some point. Probably from user input.
  if (inPlayer->kills > kills) {
    kills = inPlayer->kills;
  }

  if (inPlayer->deaths > deaths) {
    deaths = inPlayer->deaths;
  }

  if (inPlayer->assists > assists) {
    assists = inPlayer->assists;
  }

  if (inPlayer->cs > cs) {
    cs = inPlayer->cs;
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
  return newJson;
}