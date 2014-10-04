#pragma once
#ifndef _LOLSPECTATORIMAGEANALYZER_H
#define _LOLSPECTATORIMAGEANALYZER_H

#include "LeagueImageAnalyzer.h"

/*
 * Provides the bulk of the image analysis capabilities for 
 * SPECTATOR MODE in LEAGUE OF LEGENDS. 
 */
class LeagueSpectatorImageAnalyzer : public LeagueImageAnalyzer {
public:
  LeagueSpectatorImageAnalyzer(IMAGE_PATH_TYPE ImagePath);
  virtual ~LeagueSpectatorImageAnalyzer();

  virtual bool IsValidFrame();

protected:
  // Gets the match timer from the header bar up top.
  virtual int AnalyzeMatchTime();

  // Camera Analysis.
  virtual void AnalyzeMapPosition(double& xPos, double& yPos);
  virtual cv::Rect GetMapSection();

  // Team Kills. Also part of the header bar at the top of the screen.
  virtual int AnalyzeTeamKills(ELeagueTeams team);
  virtual cv::Rect GetTeamKillsSection(ELeagueTeams team);

  // Team Gold. Part of the header bar at the top of the screen.
  virtual int AnalyzeTeamGold(ELeagueTeams team);
  virtual cv::Rect GetTeamGoldSection(ELeagueTeams team);

  // Team Tower Kills. Final part of the header bar at the top of the screen.
  virtual int AnalyzeTeamTowerKills(ELeagueTeams team);
  virtual cv::Rect GetTeamTowerKillSection(ELeagueTeams team);

  // PLAYER DATA.
  // As specified by LeagueImageAnalyzer, the player 'index' is something that is left for us
  // to decide. Thus, the logical choice is to go, from top to bottom, the list of champions on
  // either side of the spectator screen (depending on the team). 
  // 
  // However, this order isn't necessarily the same as the order found in the spectator box 
  // (the one that lists the champion kills, assists, deaths, and CS), though it generally is. Therefore, we say the 
  // list of champions on either side of the screen is authoritative in determining the
  // player's index. 

  // Player Champion. 
  virtual std::string AnalyzePlayerChampion(uint idx, ELeagueTeams team, bool* isDead, bool* isLowOnHealth, int* championLevel);
  virtual cv::Rect GetPlayerChampionSection(uint idx, ELeagueTeams team);

  // Player Name
  virtual std::string AnalyzePlayerName(uint idx, ELeagueTeams team);
  virtual cv::Rect GetPlayerNameSection(uint idx, ELeagueTeams team);

  // Player Items
  virtual cv::Rect GetPlayerItemSection(uint playerIdx, ELeagueTeams team, uint itemIdx);

  // Player Score. 
  // TODO: Return the champion that the scoreboard say it is associated with. This way we can see if the scoreboard matches up with the sidebar.
  // For now, just assume that it does
  virtual std::string AnalyzePlayerScore(uint idx, ELeagueTeams team, int* kills, int* deaths, int* assists, int* cs);
  virtual cv::Rect GetPlayerKDASection(uint idx, ELeagueTeams team);
  virtual cv::Rect GetPlayerCSSection(uint idx, ELeagueTeams team);
  virtual std::string FixScore(std::string inScore);
  virtual bool IsValidScore(std::string &inScore);

  // EVENT DATA.
  // Events are the same everywhere, so the only thing we have to change is where to grab the events from.
  virtual cv::Rect GetMinibarSection(uint idx);
  virtual cv::Rect GetAnnouncementSection();
  virtual cv::Rect GetMinibarOriginalResolution();
  virtual cv::Rect GetMinibarObjectiveIconOriginalResolution();

  virtual int GetMinibarBackgroundAllyChannel() { return GetMinibarBGAllyChannel();  }
  virtual int GetMinibarBackgroundEnemeyChannel() { return GetMinibarBGEnemyChannel();  }
private:
  // Config File
  DECLARE_CONFIG_VARIABLE(ValidFrameThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)

  DECLARE_CONFIG_VARIABLE(SmallRefImageX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(SmallRefImageY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(LargeRefImageX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(LargeRefImageY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)

  DECLARE_CONFIG_VARIABLE(MatchTimeX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(MatchTimeY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(MatchTimeWidth, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(MatchTimeHeight, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(MatchTimeThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(MatchTimeResizeX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(MatchTimeResizeY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)

  DECLARE_CONFIG_VARIABLE(TeamKillsBlueX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(TeamKillsPurpleX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(TeamKillsY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(TeamKillsWidth, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(TeamKillsHeight, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(TeamKillsThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(TeamKillsResizeX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(TeamKillsResizeY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)

  DECLARE_CONFIG_VARIABLE(TeamGoldBlueX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(TeamGoldPurpleX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(TeamGoldY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(TeamGoldWidth, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(TeamGoldHeight, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(TeamGoldThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(TeamGoldResizeX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(TeamGoldResizeY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)

  DECLARE_CONFIG_VARIABLE(TowerKillsBlueX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(TowerKillsPurpleX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(TowerKillsY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(TowerKillsWidth, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(TowerKillsHeight, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(TowerKillsThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(TowerKillsResizeX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(TowerKillsResizeY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)

  DECLARE_CONFIG_VARIABLE(PlayerChampLevelX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerChampLevelY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerChampLevelWidth, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerChampLevelHeight, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerChampImageWidth, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerChampImageHeight, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerChampLevelThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerChampLevelResizeX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerChampLevelResizeY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)

  DECLARE_CONFIG_VARIABLE(PlayerChampY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerChampYIncr, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerChampBlueX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerChampPurpleX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)

  DECLARE_CONFIG_VARIABLE(PlayerNameThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerNameResizeX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerNameResizeY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerNameY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerNameYIncr, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerNameBlueX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerNamePurpleX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerNameWidth, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerNameHeight, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)

  DECLARE_CONFIG_VARIABLE(PlayerKDAThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerKDAResizeX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerKDAResizeY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerKDAY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerKDAYIncr, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerKDABlueX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerKDAPurpleX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerKDAWidth, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerKDAHeight, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)

  DECLARE_CONFIG_VARIABLE(PlayerCSThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerCSResizeX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerCSResizeY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerCSY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerCSYIncr, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerCSBlueX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerCSPurpleX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerCSWidth, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerCSHeight, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)

  DECLARE_CONFIG_VARIABLE(PlayerItemBlueX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerItemPurpleX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerItemXIncr, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerItemY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerItemYIncr, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerItemWidth, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerItemHeight, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)

  DECLARE_CONFIG_VARIABLE(MapThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(MapResizeX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(MapResizeY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(MapX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(MapY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(MapWidth, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(MapHeight, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)

  DECLARE_CONFIG_VARIABLE(AnnouncementX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(AnnouncementY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(AnnouncementWidth, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(AnnouncementHeight, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)

  DECLARE_CONFIG_VARIABLE(MinibarEventX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(MinibarEventY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(MinibarEventYIncr, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(MinibarEventWidth, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(MinibarEventHeight, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(MinibarObjectiveIconTargetX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(MinibarObjectiveIconTargetY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(MinibarBGAllyChannel, int, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0)
  DECLARE_CONFIG_VARIABLE(MinibarBGEnemyChannel, int, ConfigManager::CONFIG_LEAGUE_FILENAME, "SpectatorImageAnalyzer", 0)
};

#endif