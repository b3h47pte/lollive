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
  LeagueImageAnalyzer(IMAGE_PATH_TYPE ImagePath, const std::string& configPath, std::shared_ptr<std::unordered_map<std::string, T_EMPTY>> relevantProperties);
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

  virtual EGameId GetGameId() const { return EGI_League; }
protected:
  // See if the image is 1080p prior to scaling
  bool bIs1080p;

  // Whether or not we are in the draft ban stage
  // Detection of this is specified by subclasses
  virtual bool AnalyzeIsDraftBan();
  virtual cv::Rect GetBansTextSection(ELeagueTeams team);
  virtual cv::Rect GetVersusTextSection();
  bool bIsDraftBan;

  // Game State. 
  // Get match time specified in seconds since match start.
  virtual int AnalyzeMatchTime();
  virtual cv::Rect GetMatchTimeSection();
  // Checks whether or not we are currently either looking at the draft screen or the in-game screen.
  virtual bool IsValidFrame();

  // Camera Location Analysis. We represent the map as being a unit square.
  // So any position on the map is represented as a percentage.
  virtual void AnalyzeMapPosition(double& xPos, double& yPos);
  virtual cv::Rect GetMapSection();
  double mapLocX, mapLocY;

  // Event Analysis. Events are usually listed in league as a little colored bar that has some images on it.
  // However, inhibitor kills are only displayed as an announcement. 
  virtual std::shared_ptr<MapPtrLeagueEvent> GetMinibarEvents();
  virtual cv::Rect GetMinibarSection(uint idx);
  virtual cv::Rect GetMinibarOriginalResolution();
  virtual cv::Rect GetMinibarObjectiveIconOriginalResolution();
  virtual cv::Rect GetMinibarSupportingIconOriginalResolution();

  virtual PtrLeagueEvent GetAnnouncementEvent();
  virtual cv::Rect GetAnnouncementSection();

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
  virtual int AnalyzeTeamKills(ELeagueTeams team);
  virtual cv::Rect GetTeamKillsSection(ELeagueTeams team);

  // Team Gold
  virtual int AnalyzeTeamGold(ELeagueTeams team);
  virtual cv::Rect GetTeamGoldSection(ELeagueTeams team);

  // Team Tower Kills
  virtual int AnalyzeTeamTowerKills(ELeagueTeams team);
  virtual cv::Rect GetTeamTowerKillSection(ELeagueTeams team);

  // Player Analysis. Pass in an index to the player. What this index could mean is different dependent on the mode.
  // By default we will want information for players 0-4 (inclusive). Subclasses are free to ignore as many as they wish.
  virtual PtrLeaguePlayerData AnalyzePlayerData(uint idx, ELeagueTeams team);

  // Player Champion. Whether the champion is dead. Low on health? And its level.
  virtual std::string AnalyzePlayerChampion(uint idx, ELeagueTeams team, bool* isDead, bool* isLowOnHealth, int* championLevel);
  virtual cv::Rect GetPlayerChampionSection(uint idx, ELeagueTeams team);
  std::shared_ptr<const LeagueChampionDatabase> ChampionDatabase;

  // Champion Level
  virtual cv::Rect GetPlayerChampLevelSection(ELeagueTeams team, uint idx);

  // Player Name. 
  virtual std::string AnalyzePlayerName(uint idx, ELeagueTeams team);
  virtual cv::Rect GetPlayerNameSection(uint idx, ELeagueTeams team);

  // Player Score
  virtual std::string AnalyzePlayerScore(uint idx, ELeagueTeams team, int* kills, int* deaths, int* assists, int* cs);
  virtual cv::Rect GetPlayerKDASection(uint idx, ELeagueTeams team);
  virtual cv::Rect GetPlayerCSSection(uint idx, ELeagueTeams team);
  virtual std::string FixScore(std::string inScore);
  virtual bool IsValidScore(std::string &inScore);

  // Player Items -- Item index can be any number from 0-6 (6 items + 1 trinket).
  virtual std::string AnalyzePlayerItem(uint playerIdx, ELeagueTeams team, uint itemIdx);
  virtual cv::Rect GetPlayerItemSection(uint playerIdx, ELeagueTeams team, uint itemIdx);
  std::shared_ptr<const LeagueItemDatabase> ItemDatabase;

  // Utility Function to help us determine who the input champion is
  std::string FindMatchingChampion(cv::Mat filterImage, std::vector<std::string>& championHints, bool& isLowHealth, bool& isDead);
  bool ReverseMatchChampion(cv::Mat containerImage, std::string champion, int targetSizeX, int targetSizeY, cv::Point& output);

  // Bans
  virtual void GetBans(std::string* outArray, ELeagueTeams team);
  virtual cv::Rect GetBansSection(ELeagueTeams team, uint idx);
  virtual cv::Rect GetBansPercentageSection(ELeagueTeams team, uint idx);

  // Team Name
  virtual std::string GetTeamName(ELeagueTeams team);
  virtual cv::Rect GetTeamNameSection(ELeagueTeams team);

  // Team Series Score
  virtual int GetTeamGamesWon(ELeagueTeams team);
  virtual cv::Rect GetTeamGamesWonSection(ELeagueTeams team);

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
  DECLARE_CONFIG_VARIABLE(EventChampDetectThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "ImageAnalyzer", 0.0)

  DECLARE_CONFIG_VARIABLE(VersusText, std::string, ConfigManager::CONFIG_LEAGUE_FILENAME, "DraftBan", "")
  DECLARE_CONFIG_VARIABLE(VersusThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "DraftBan", 0.0)
  DECLARE_CONFIG_VARIABLE(VersusResizeX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "DraftBan", 0.0)
  DECLARE_CONFIG_VARIABLE(VersusResizeY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "DraftBan", 0.0)

  DECLARE_CONFIG_VARIABLE(BansText, std::string, ConfigManager::CONFIG_LEAGUE_FILENAME, "DraftBan", "")
  DECLARE_CONFIG_VARIABLE(BansThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "DraftBan", 0.0)
  DECLARE_CONFIG_VARIABLE(BansResizeX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "DraftBan", 0.0)
  DECLARE_CONFIG_VARIABLE(BansResizeY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "DraftBan", 0.0)
  DECLARE_CONFIG_VARIABLE(BanChampThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "DraftBan", 0.0)
  DECLARE_CONFIG_VARIABLE(BanChampResizeX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "DraftBan", 0.0)
  DECLARE_CONFIG_VARIABLE(BanChampResizeY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "DraftBan", 0.0)

  DECLARE_CONFIG_VARIABLE(ValidFrameThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "GameState", 0.0)
  DECLARE_CONFIG_VARIABLE(MatchTimeThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "GameState", 0.0)
  DECLARE_CONFIG_VARIABLE(MatchTimeResizeX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "GameState", 0.0)
  DECLARE_CONFIG_VARIABLE(MatchTimeResizeY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "GameState", 0.0)

  DECLARE_CONFIG_VARIABLE(MapThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "GameState", 0.0)
  DECLARE_CONFIG_VARIABLE(MapResizeX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "GameState", 0.0)
  DECLARE_CONFIG_VARIABLE(MapResizeY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "GameState", 0.0)

  DECLARE_CONFIG_VARIABLE(TeamGoldThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "TeamStats", 0.0)
  DECLARE_CONFIG_VARIABLE(TeamGoldResizeX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "TeamStats", 0.0)
  DECLARE_CONFIG_VARIABLE(TeamGoldResizeY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "TeamStats", 0.0)

  DECLARE_CONFIG_VARIABLE(TeamKillsThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "TeamStats", 0.0)
  DECLARE_CONFIG_VARIABLE(TeamKillsResizeX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "TeamStats", 0.0)
  DECLARE_CONFIG_VARIABLE(TeamKillsResizeY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "TeamStats", 0.0)

  DECLARE_CONFIG_VARIABLE(TowerKillsThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "TeamStats", 0.0)
  DECLARE_CONFIG_VARIABLE(TowerKillsResizeX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "TeamStats", 0.0)
  DECLARE_CONFIG_VARIABLE(TowerKillsResizeY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "TeamStats", 0.0)

  DECLARE_CONFIG_VARIABLE(TeamGamesThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "TeamStats", 0.0)
  DECLARE_CONFIG_VARIABLE(TeamGamesResizeX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "TeamStats", 0.0)
  DECLARE_CONFIG_VARIABLE(TeamGamesResizeY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "TeamStats", 0.0)

  DECLARE_CONFIG_VARIABLE(TeamNameThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "TeamStats", 0.0)
  DECLARE_CONFIG_VARIABLE(TeamNameResizeX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "TeamStats", 0.0)
  DECLARE_CONFIG_VARIABLE(TeamNameResizeY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "TeamStats", 0.0)

  DECLARE_CONFIG_VARIABLE(PlayerChampLevelThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "PlayerStats", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerChampLevelResizeX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "PlayerStats", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerChampLevelResizeY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "PlayerStats", 0.0)

  DECLARE_CONFIG_VARIABLE(PlayerNameThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "PlayerStats", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerNameResizeX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "PlayerStats", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerNameResizeY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "PlayerStats", 0.0)

  DECLARE_CONFIG_VARIABLE(PlayerKDAThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "PlayerStats", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerKDAResizeX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "PlayerStats", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerKDAResizeY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "PlayerStats", 0.0)

  DECLARE_CONFIG_VARIABLE(PlayerCSThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "PlayerStats", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerCSResizeX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "PlayerStats", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerCSResizeY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "PlayerStats", 0.0)

  DECLARE_CONFIG_VARIABLE(MinibarEventWidth, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "Event", 0.0)
  DECLARE_CONFIG_VARIABLE(MinibarEventHeight, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "Event", 0.0)
  DECLARE_CONFIG_VARIABLE(MinibarObjectiveIconTargetX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "Event", 0.0)
  DECLARE_CONFIG_VARIABLE(MinibarObjectiveIconTargetY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "Event", 0.0)
  DECLARE_CONFIG_VARIABLE(MinibarSupportingIconTargetX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "Event", 0.0)
  DECLARE_CONFIG_VARIABLE(MinibarSupportingIconTargetY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "Event", 0.0)

  DECLARE_CONFIG_VARIABLE(MinibarBackgroundAllyChannel, int, ConfigManager::CONFIG_LEAGUE_FILENAME, "Event", 0)
  DECLARE_CONFIG_VARIABLE(MinibarBackgroundEnemyChannel, int, ConfigManager::CONFIG_LEAGUE_FILENAME, "Event", 0)

  GENERATE_OBJECTIVE_CONFIG_STRINGS()
};

#endif
