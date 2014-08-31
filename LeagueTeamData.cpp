#include "LeagueTeamData.h"

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