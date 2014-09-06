#pragma once
#ifndef _LEAGUEPLAYERDATA_H
#define _LEAGUEPLAYERDATA_H
#include "common.h"
#include "Data.h"

enum ELeagueTeams {
  ELT_BLUE,
  ELT_PURPLE
};


typedef std::shared_ptr<struct LeaguePlayerData> PtrLeaguePlayerData;
struct LeaguePlayerData {
  LeaguePlayerData() : name(""), champion(""),
    kills(0), deaths(0), assists(0), cs(0), isDead(false), level(0), isLowHealth(false), lastLowHealthUpdate(0),
    team(ELT_BLUE), playerIdx(0) {

  }


  std::string name;
  std::string champion;
  int kills;
  int deaths;
  int assists;
  int cs;
  bool isDead;
  int level;

  bool isLowHealth;
  int lastLowHealthUpdate;

  ELeagueTeams team;
  int playerIdx;

  std::string items[7];

  void Print() {
    std::cout << "- Player: " << name << " as " << champion << std::endl;
    std::cout << "-- Player Score: " << kills << "/" << deaths << "/" << assists << std::endl;
    std::cout << "-- Player CS: " << cs << std::endl;
    std::cout << "-- Is Dead?: " << isDead << std::endl;
    std::cout << "-- Is Low Health?: " << isLowHealth << std::endl;
    std::cout << "-- Player Level: " << level << std::endl;
    std::cout << "-- Player Items: ";
    for (auto& i : items) {
      std::cout << i << " ";
    }
    std::cout << std::endl;
  }

  void Update(PtrLeaguePlayerData inPlayer, int timeStamp, std::vector<std::shared_ptr<GenericDataStore>>& dataHistory);

  // JSONify
  struct cJSON* CreateJSON();
};

#endif