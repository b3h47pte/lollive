#pragma once
#ifndef _LEAGUETEAMDATA_H
#define _LEAGUETEAMDATA_H

#include "common.h"
#include "LeaguePlayerData.h"
#include "Data.h"

typedef std::shared_ptr<struct LeagueTeamData> PtrLeagueTeamData;
struct LeagueTeamData {
  LeagueTeamData() : team(ELT_BLUE), teamName(""), teamScore(-1), kills(-1), gold(-1), towerKills(-1) {
  }

  ELeagueTeams team;
  std::string teamName;
  int teamScore; // team score as in games won (when playing in a series).

  int kills;
  int gold;
  int towerKills;
  PtrLeaguePlayerData players[5];

  void Print() {
    std::cout << "======== TEAM " << team << " " << teamName  << "(" << teamScore << ")" <<" ========" << std::endl;
    std::cout << "Team Kills: " << kills << std::endl;
    std::cout << "Team Gold: " << gold << std::endl;
    std::cout << "Team Tower Kills: " << towerKills << std::endl;
    for (int i = 0; i < 5; ++i) players[i]->Print();
  }

  // This struct is smart enough to take in new team data and update itself in a logical manner.
  void Update(PtrLeagueTeamData newData, int timeStamp, std::vector<std::shared_ptr<GenericDataStore>>& dataHistory);

  // JSONify
  struct cJSON* CreateJSON();
};

#endif