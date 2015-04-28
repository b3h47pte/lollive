#pragma once
#ifndef _LEAGUETEAMDATA_H
#define _LEAGUETEAMDATA_H

#include "common.h"
#include "LeaguePlayerData.h"
#include "Data.h"

typedef std::shared_ptr<struct LeagueTeamData> PtrLeagueTeamData;
struct LeagueTeamData {
  LeagueTeamData() : team(ELT_BLUE), teamName(""), teamScore(-1), kills(-1), gold(-1), towerKills(-1), currentDragonCount(0), totalDragonCount(0), baronCount(0), inhibCount(0) {
  }

  ELeagueTeams team;
  std::string teamName;
  int teamScore; // team score as in games won (when playing in a series).

  int kills;
  int gold;
  int towerKills;
  PtrLeaguePlayerData players[5];
  int currentDragonCount;
  int totalDragonCount;
  int baronCount;
  int inhibCount;

  // Champion bans -- useful when we actually get to see the draft stage
  std::string bans[3];

  void Print() {
    std::cout << "======== TEAM " << team << " " << teamName  << "(" << teamScore << ")" <<" ========" << std::endl;
    std::cout << "Team Kills: " << kills << std::endl;
    std::cout << "Team Gold: " << gold << std::endl;
    std::cout << "Team Tower Kills: " << towerKills << std::endl;
    std::cout << "Team Bans: " << bans[0] << " " << bans[1] << " " << bans[2] << std::endl;
    std::cout << "Dragon Count: " << currentDragonCount << " " << totalDragonCount << std::endl;
    std::cout << "Baron Count: " << baronCount << std::endl;
    std::cout << "Inhibitors: " << inhibCount << std::endl;
    for (int i = 0; i < 5; ++i) players[i]->Print();
  }

  // This struct is smart enough to take in new team data and update itself in a logical manner.
  void Update(PtrLeagueTeamData newData, int timeStamp, std::vector<std::shared_ptr<GenericDataStore>>& dataHistory);

  // JSONify
  struct cJSON* CreateJSON();
  
  // Config Constants
  const std::string TeamDataSection = "TeamData";
  const std::string JsonTeamNameName = "JsonTeamNameName";
  const std::string JsonTeamPlayersName = "JsonTeamPlayersName";
  const std::string JsonTeamKillsName = "JsonTeamKillsName";
  const std::string JsonTeamGoldName = "JsonTeamGoldName";
  const std::string JsonTeamTowersName = "JsonTeamTowersName";
  const std::string JsonTeamTotalDragonsName = "JsonTeamTotalDragonsName";
  const std::string JsonTeamCurrentDragonsName = "JsonTeamCurrentDragonsName";
  const std::string JsonTeamBaronsName = "JsonTeamBaronsName"; 
  const std::string JsonTeamInhibitorsName = "JsonTeamInhibitorsName";
  const std::string JsonTeamGamesWonName = "JsonTeamGamesWonName";
  
};

// Given a location, tell us which side of the map we are on (blue/purple)
// Assume that x and y are 0 <= x/y <= 1.0
inline ELeagueTeams GetTeamFromMapLocation(double x, double y) {
  if (x + y <= 0.5) {
    return ELT_BLUE;
  }
  return ELT_PURPLE;
}

inline ELeagueTeams GetOtherTeam(ELeagueTeams team) {
  return (team == ELT_BLUE) ? ELT_PURPLE : ELT_BLUE;
}

#endif
