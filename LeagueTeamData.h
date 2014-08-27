#pragma once
#ifndef _LEAGUETEAMDATA_H
#define _LEAGUETEAMDATA_H

enum ELeagueTeams {
  ELT_BLUE,
  ELT_PURPLE
};

struct LeagueTeamData {
  ELeagueTeams team;
  int kills;
};

typedef std::shared_ptr<LeagueTeamData> PtrLeagueTeamData;

#endif