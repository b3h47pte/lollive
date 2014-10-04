#pragma once
#ifndef _LOLIMAGEANALYZER_H
#define _LOLIMAGEANALYZER_H

#include "ImageAnalyzer.h"
#include "LeaguePlayerData.h"
#include "LeagueTeamData.h"
#include "LeagueEvent.h"
#include "LeagueChampionDatabase.h"
#include "LeagueItemDatabase.h"
#include "LeagueEventDatabase.h"

/*
 * Base class for analyzing League of Legends screenshots. Eventually need to split it to analyze
 * spectator mode/LCS, as well as a regular player playing.
 * 
 * TODO: Need to be able to adapt to the HUD size the player has chosen. Currently assume we use 100%.
 */
class LeagueImageAnalyzer: public ImageAnalyzer {
public:
  LeagueImageAnalyzer(IMAGE_PATH_TYPE ImagePath);
  virtual ~LeagueImageAnalyzer();

  // There are some things that are common to all games, whether or not its in spectator mode or not.
  // This includes the current time, the map, kills for both teams. However, getting this information
  // must be handled by the subclasses.
  virtual bool Analyze();

  bool GetIsDraftBan() const { return bIsDraftBan; }

  void GetMapLocation(double& x, double& y) {
    x = mapLocX;
    y = mapLocY;
  }

protected:
  // See if the image is 1080p prior to scaling
  bool bIs1080p;

  // Whether or not we are in the draft ban stage
  // Detection of this is specified by subclasses
  virtual bool AnalyzeIsDraftBan() { return false; }
  bool bIsDraftBan;

  // Get match time specified in seconds since match start.
  virtual int AnalyzeMatchTime() = 0;

  // Camera Location Analysis. We represent the map as being a unit square.
  // So any position on the map is represented as a percentage.
  virtual void AnalyzeMapPosition(double& xPos, double& yPos) = 0;
  virtual cv::Rect GetMapSection() = 0;
  double mapLocX, mapLocY;

  // Event Analysis. Events are usually listed in league as a little colored bar that has some images on it.
  // However, inhibitor kills are only displayed as an announcement. 
  virtual std::shared_ptr<VectorPtrLeagueEvent> GetMinibarEvents();
  virtual cv::Rect GetMinibarSection(uint idx) = 0;
  virtual cv::Rect GetMinibarOriginalResolution() = 0;
  virtual cv::Rect GetMinibarObjectiveIconOriginalResolution() = 0;

  virtual PtrLeagueEvent GetAnnouncementEvent();
  virtual cv::Rect GetAnnouncementSection()= 0 ;

  // Team Analysis. There are two teams in all League of Legends games [if Riot ever adds a one team mode or 3+ team mode, fml],
  // and the amount of information we have about each team is limited. 
  // The process is as follows:
  //   Analyze will call AnalyzeTeamData for both teams.
  //   AnalyzeTeamData will then proceed to call specific functions (i.e. AnalyzeTeamGold) for the input team.
  //    Sometimes, this information is not available in certain modes and thus, this gives the subclass a way to ignore that property
  //    while keeping all relevant team data together.
  //   Therefore, AnalyzeTeamData will call ALL possible team data functions regardless of whether or not that information is 
  //   available in the current mode.
  virtual PtrLeagueTeamData AnalyzeTeamData(ELeagueTeams team);

  // Team Kills
  virtual int AnalyzeTeamKills(ELeagueTeams team) = 0;
  virtual cv::Rect GetTeamKillsSection(ELeagueTeams team) = 0;

  // Team Gold
  virtual int AnalyzeTeamGold(ELeagueTeams team) = 0;
  virtual cv::Rect GetTeamGoldSection(ELeagueTeams team) = 0;

  // Team Tower Kills
  virtual int AnalyzeTeamTowerKills(ELeagueTeams team) = 0;
  virtual cv::Rect GetTeamTowerKillSection(ELeagueTeams team) = 0;

  // Player Analysis. Pass in an index to the player. What this index could mean is different dependent on the mode.
  // By default we will want information for players 0-4 (inclusive). Subclasses are free to ignore as many as they wish.
  virtual PtrLeaguePlayerData AnalyzePlayerData(uint idx, ELeagueTeams team);

  // Player Champion. Whether the champion is dead. Low on health? And its level.
  virtual std::string AnalyzePlayerChampion(uint idx, ELeagueTeams team, bool* isDead, bool* isLowOnHealth, int* championLevel) = 0;
  virtual cv::Rect GetPlayerChampionSection(uint idx, ELeagueTeams team) = 0;
  std::shared_ptr<const LeagueChampionDatabase> ChampionDatabase;

  // Player Name. 
  virtual std::string AnalyzePlayerName(uint idx, ELeagueTeams team) = 0;
  virtual cv::Rect GetPlayerNameSection(uint idx, ELeagueTeams team) = 0;

  // Player Score
  virtual std::string AnalyzePlayerScore(uint idx, ELeagueTeams team, int* kills, int* deaths, int* assists, int* cs) = 0;
  virtual cv::Rect GetPlayerKDASection(uint idx, ELeagueTeams team) = 0;
  virtual cv::Rect GetPlayerCSSection(uint idx, ELeagueTeams team) = 0;

  // Player Items -- Item index can be any number from 0-6 (6 items + 1 trinket).
  virtual std::string AnalyzePlayerItem(uint playerIdx, ELeagueTeams team, uint itemIdx);
  virtual cv::Rect GetPlayerItemSection(uint playerIdx, ELeagueTeams team, uint itemIdx) = 0;
  std::shared_ptr<const LeagueItemDatabase> ItemDatabase;

  // Utility Function to help us determine who the input champion is
  std::string FindMatchingChampion(cv::Mat filterImage, std::vector<std::string>& championHints, bool& isLowHealth, bool& isDead);

private:
  DECLARE_CONFIG_VARIABLE(ChampImgSplitDimX, int, ConfigManager::CONFIG_LEAGUE_FILENAME, "ImageAnalyzer", 0)
  DECLARE_CONFIG_VARIABLE(ChampImgSplitDimY, int, ConfigManager::CONFIG_LEAGUE_FILENAME, "ImageAnalyzer", 0)
  DECLARE_CONFIG_VARIABLE(ChampImgHBuckets, int, ConfigManager::CONFIG_LEAGUE_FILENAME, "ImageAnalyzer", 0)
  DECLARE_CONFIG_VARIABLE(ChampImgSBuckets, int, ConfigManager::CONFIG_LEAGUE_FILENAME, "ImageAnalyzer", 0)
  DECLARE_CONFIG_VARIABLE(ChampImgVBuckets, int, ConfigManager::CONFIG_LEAGUE_FILENAME, "ImageAnalyzer", 0)
  DECLARE_CONFIG_VARIABLE(ChampDeadThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "ImageAnalyzer", 0.0)

  DECLARE_CONFIG_VARIABLE(AnnouncementLowH, int, ConfigManager::CONFIG_LEAGUE_FILENAME, "ImageAnalyzer", 0)
  DECLARE_CONFIG_VARIABLE(AnnouncementLowS, int, ConfigManager::CONFIG_LEAGUE_FILENAME, "ImageAnalyzer", 0)
  DECLARE_CONFIG_VARIABLE(AnnouncementLowV, int, ConfigManager::CONFIG_LEAGUE_FILENAME, "ImageAnalyzer", 0)
  DECLARE_CONFIG_VARIABLE(AnnouncementHighH, int, ConfigManager::CONFIG_LEAGUE_FILENAME, "ImageAnalyzer", 0)
  DECLARE_CONFIG_VARIABLE(AnnouncementHighS, int, ConfigManager::CONFIG_LEAGUE_FILENAME, "ImageAnalyzer", 0)
  DECLARE_CONFIG_VARIABLE(AnnouncementHighV, int, ConfigManager::CONFIG_LEAGUE_FILENAME, "ImageAnalyzer", 0)

  DECLARE_CONFIG_VARIABLE(MinibarEventMax, int, ConfigManager::CONFIG_LEAGUE_FILENAME, "ImageAnalyzer", 0)
  DECLARE_CONFIG_VARIABLE(KillObjective, std::string, ConfigManager::CONFIG_LEAGUE_FILENAME, "Objectives", "")
  DECLARE_CONFIG_VARIABLE(KillStreakObjective, std::string, ConfigManager::CONFIG_LEAGUE_FILENAME, "Objectives", "")
  DECLARE_CONFIG_VARIABLE(KillShutdownObjective, std::string, ConfigManager::CONFIG_LEAGUE_FILENAME, "Objectives", "")

  DECLARE_CONFIG_VARIABLE(BackgroundColorXPercentage, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "ImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(BackgroundColorY, int, ConfigManager::CONFIG_LEAGUE_FILENAME, "ImageAnalyzer", 0)
  DECLARE_CONFIG_VARIABLE(KillIconSobelThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "ImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(ObjectiveIconSobelThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "ImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(EventValueThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "ImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(EventIconMinimumWidth, int, ConfigManager::CONFIG_LEAGUE_FILENAME, "ImageAnalyzer", 0)
  DECLARE_CONFIG_VARIABLE(EventIconMinimumHeight, int, ConfigManager::CONFIG_LEAGUE_FILENAME, "ImageAnalyzer", 0)
  DECLARE_CONFIG_VARIABLE(EventIconSquareThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "ImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(EventIconContourThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "ImageAnalyzer", 0.0)

  GENERATE_OBJECTIVE_CONFIG_STRINGS()
};

#endif