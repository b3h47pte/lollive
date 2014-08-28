#pragma once
#ifndef _LEAGUETEAMDATA_H
#define _LEAGUETEAMDATA_H

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
};
typedef std::shared_ptr<LeagueTeamData> PtrLeagueTeamData;

#endif