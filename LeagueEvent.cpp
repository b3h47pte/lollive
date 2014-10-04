#include "LeagueEvent.h"
#include "cjson/cJSON.h"
/*
 * Update the league event. Probably do some supporting champions voting here? TODO.
 */
void LeagueEvent::Update(PtrLeagueEvent newEvent, int timestamp) {

}

cJSON* LeagueEvent::CreateJSON() {
  cJSON* newJson = cJSON_CreateObject();
  cJSON_AddNumberToObject(newJson, GetTeamJsonId().c_str(), RelevantTeam);
  cJSON_AddNumberToObject(newJson, GetEventJsonId().c_str(), EventId);
  cJSON_AddItemToObject(newJson, GetAdditionalInfoJsonId().c_str(), cJSON_CreateString(AdditionalInfo.c_str()));
  cJSON_AddItemToObject(newJson, GetPlayerInstigatorJsonId().c_str(), cJSON_CreateString(PlayerInstigator.c_str()));
  cJSON* supporting = cJSON_CreateArray();
  for (std::string name : SupportingPlayers) {
    cJSON_AddItemToArray(supporting, cJSON_CreateString(name.c_str()));
  }
  cJSON_AddItemToObject(newJson, GetSupportingPlayerJsonId().c_str(), supporting);
  cJSON_AddNumberToObject(newJson, GetTimestampJsonId().c_str(), Timestamp);
  cJSON_AddNumberToObject(newJson, GetKillTypeJsonId().c_str(), KillType);

  return newJson;
}