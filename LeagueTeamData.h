#pragma once
#ifndef _LEAGUETEAMDATA_H
#define _LEAGUETEAMDATA_H

#include "common.h"
#include "LeaguePlayerData.h"

enum ELeagueTeams {
  ELT_BLUE,
  ELT_PURPLE
};

typedef std::shared_ptr<struct LeagueTeamData> PtrLeagueTeamData;
struct LeagueTeamData {
  ELeagueTeams team;
  int kills;
  int gold;
  int towerKills;
  PtrLeaguePlayerData players[5];

  void Print() {
    std::cout << "======== TEAM " << team << " ========" << std::endl;
    std::cout << "Team Kills: " << kills << std::endl;
    std::cout << "Team Gold: " << gold << std::endl;
    std::cout << "Team Tower Kills: " << towerKills << std::endl;
    for (int i = 0; i < 5; ++i) players[i]->Print();
  }

  // This struct is smart enough to take in new team data and update itself in a logical manner.
  void Update(PtrLeagueTeamData newData, int timeStamp);

  // JSONify
  struct cJSON* CreateJSON();
};

#endif