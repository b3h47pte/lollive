#pragma once
#ifndef _LEAGUEPLAYERDATA_H
#define _LEAGUEPLAYERDATA_H
#include "common.h"

struct LeaguePlayerData {
  std::string name;
  std::string champion;
  int kills;
  int deaths;
  int assists;
  int cs;
  bool isDead;
  bool isLowHealth;
  int level;

  void Print() {
    std::cout << "- Player: " << name << " as " << champion << std::endl;
    std::cout << "-- Player Score: " << kills << "/" << deaths << "/" << assists << std::endl;
    std::cout << "-- Player CS: " << cs << std::endl;
    std::cout << "-- Is Dead?: " << isDead << std::endl;
    std::cout << "-- Is Low Health?: " << isLowHealth << std::endl;
    std::cout << "-- Player Level: " << level << std::endl;
  }
};
typedef std::shared_ptr<LeaguePlayerData> PtrLeaguePlayerData;

#endif