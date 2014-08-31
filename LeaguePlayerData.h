#pragma once
#ifndef _LEAGUEPLAYERDATA_H
#define _LEAGUEPLAYERDATA_H
#include "common.h"

typedef std::shared_ptr<struct LeaguePlayerData> PtrLeaguePlayerData;
struct LeaguePlayerData {
  LeaguePlayerData() : name(""), champion(""),
    kills(0), deaths(0), assists(0), cs(0), isDead(false), level(0), isLowHealth(false), lastLowHealthUpdate(0) {

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

  void Print() {
    std::cout << "- Player: " << name << " as " << champion << std::endl;
    std::cout << "-- Player Score: " << kills << "/" << deaths << "/" << assists << std::endl;
    std::cout << "-- Player CS: " << cs << std::endl;
    std::cout << "-- Is Dead?: " << isDead << std::endl;
    std::cout << "-- Is Low Health?: " << isLowHealth << std::endl;
    std::cout << "-- Player Level: " << level << std::endl;
  }

  void Update(PtrLeaguePlayerData inPlayer, int timeStamp);
};

#endif