#pragma once
#ifndef _LEAGUEPLAYERDATA_H
#define _LEAGUEPLAYERDATA_H
#include "common.h"
#include "Data.h"

enum ELeagueTeams {
  ELT_BLUE,
  ELT_PURPLE,
  ELT_UNKNOWN
};


typedef std::shared_ptr<struct LeaguePlayerData> PtrLeaguePlayerData;
struct LeaguePlayerData {
  LeaguePlayerData() : name(""), champion(""),
    kills(0), deaths(0), assists(0), cs(0), isDead(false), level(0), 
    towers(0), inhibs(0), barons(0), dragons(0),
    isLowHealth(false), lastLowHealthUpdate(0), team(ELT_BLUE), playerIdx(0) {

  }


  std::string name;
  std::string champion;
  int kills;
  int deaths;
  int assists;
  int cs;
  bool isDead;
  int level;

  int towers;
  int inhibs;
  int barons;
  int dragons;

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
    std::cout << "-- Towers: " << towers << std::endl;
    std::cout << "-- Inhibs: " << inhibs << std::endl;
    std::cout << "-- Dragons: " << dragons << std::endl;
    std::cout << "-- Barons: " << barons << std::endl;
    std::cout << "-- Player Items: ";
    for (auto& i : items) {
      std::cout << i << " ";
    }
    std::cout << std::endl;
  }

  void Update(PtrLeaguePlayerData inPlayer, int timeStamp, std::vector<std::shared_ptr<GenericDataStore>>& dataHistory);

  // JSONify
  struct cJSON* CreateJSON();

  // Config File Constants
  const std::string PlayerDataSection = "PlayerData";
  const std::string KillUpdateThresholdName = "KillUpdateThreshold";
  const std::string DeathUpdateThresholdName = "DeathUpdateThreshold";
  const std::string AssistUpdateThresholdName = "AssistUpdateThreshold";
  const std::string CreepUpdateThresholdName = "CreepUpdateThreshold";
  const std::string LowHealthThresholdName = "LowHealthUpdateThreshold";

  const std::string JsonKillsName = "JsonKillsName";
  const std::string JsonDeathsName = "JsonDeathsName";
  const std::string JsonAssistsName = "JsonAssistsName";
  const std::string JsonCSName = "JsonCSName";
  const std::string JsonLevelName = "JsonLevelName";
  const std::string JsonTowersName = "JsonTowersName";
  const std::string JsonInhibsName = "JsonInhibsName";
  const std::string JsonBaronsName = "JsonBaronsName";
  const std::string JsonDragonsName = "JsonDragonsName";

  const std::string JsonIsDeadName = "JsonIsDeadName";
  const std::string JsonLowHealthName = "JsonLowHealthName";
  const std::string JsonNameName = "JsonNameName";
  const std::string JsonChampionName = "JsonChampionName";
  const std::string JsonItemsName = "JsonItemsName";
};

#endif