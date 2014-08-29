#pragma once
#ifndef _LEAGUETEAMDATA_H
#define _LEAGUETEAMDATA_H

#include "common.h"
#include "LeaguePlayerData.h"

enum ELeagueTeams {
  ELT_BLUE,
  ELT_PURPLE
};

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
    std::cout << "Team Tower Kills: " << kills << std::endl;
    for (int i = 0; i < 5; ++i) players[i]->Print();
  }
};
typedef std::shared_ptr<LeagueTeamData> PtrLeagueTeamData;

#endif