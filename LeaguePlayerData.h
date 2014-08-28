#pragma once
#ifndef _LEAGUEPLAYERDATA_H
#define _LEAGUEPLAYERDATA_H

struct LeaguePlayerData {
  std::string name;
  std::string champion;
  int kills;
  int deaths;
  int assists;
  int cs;
};
typedef std::shared_ptr<LeaguePlayerData> PtrLeaguePlayerData;

#endif