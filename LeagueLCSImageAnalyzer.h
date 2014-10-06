#pragma once
#ifndef _LEAGUELCSIMAGEANALYZER_H
#define _LEAGUELCSIMAGEANALYZER_H
#include "LeagueSpectatorImageAnalyzer.h"

/*
 * Class specifically for LCS.
 */
class LeagueLCSImageAnalyzer : public LeagueSpectatorImageAnalyzer {
public:
  LeagueLCSImageAnalyzer(IMAGE_PATH_TYPE ImagePath);
  virtual ~LeagueLCSImageAnalyzer();
protected:
  // Items
  virtual cv::Rect GetPlayerItemSection(uint playerIdx, ELeagueTeams team, uint itemIdx);

  // Need to determine when we are on the draft ban screen.
  // We do this by finding text on the screen. The most prominent text 
  // is the "BANS" text and the "VS" text.
  virtual bool AnalyzeIsDraftBan();
  virtual cv::Rect GetBansTextSection(ELeagueTeams team);
  virtual cv::Rect GetVersusTextSection();

  // Get bans during the draft stage
  virtual void GetBans(std::string* outArray, ELeagueTeams team);
  virtual cv::Rect GetBansSection(ELeagueTeams team, uint idx);
  virtual cv::Rect GetBansPercentageSection(cv::Rect banRect);

  // Need to get the team name
  virtual PtrLeagueTeamData AnalyzeTeamData(ELeagueTeams team);

  // Team Name
  virtual std::string GetTeamName(ELeagueTeams team);
  virtual cv::Rect GetTeamNameSection(ELeagueTeams team);

  // Team Series Score
  virtual int GetTeamGamesWon(ELeagueTeams team);
  virtual cv::Rect GetTeamGamesWonSection(ELeagueTeams team);

  // The location of the side bars for champions is slightly different than regular spectator mode.
  // Wtf Riot.
  virtual cv::Rect GetPlayerChampionSection(uint idx, ELeagueTeams team);
  virtual cv::Rect GetPlayerNameSection(uint idx, ELeagueTeams team);

  // THE SPECTATOR BOX IS DIFFERENT TOO. WHY.
  virtual cv::Rect GetPlayerKDASection(uint idx, ELeagueTeams team);
  virtual cv::Rect GetPlayerCSSection(uint idx, ELeagueTeams team);

  // Events
  virtual int GetMinibarBackgroundAllyChannel() { return GetMinibarBGAllyChannel(); }
  virtual int GetMinibarBackgroundEnemeyChannel() { return GetMinibarBGEnemyChannel(); }
  virtual cv::Rect GetMinibarObjectiveIconOriginalResolution();
  virtual cv::Rect GetMinibarSupportingIconOriginalResolution();
private:
  // Config File Constants
  DECLARE_CONFIG_VARIABLE(RefImageXSize, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(RefImageYSize, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)

  DECLARE_CONFIG_VARIABLE(VersusThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(VersusResizeX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(VersusResizeY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(VersusText, std::string, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", "")
  DECLARE_CONFIG_VARIABLE(VersusXStart, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(VersusYStart, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(VersusWidth, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(VersusHeight, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)

  DECLARE_CONFIG_VARIABLE(BansThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(BansResizeX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(BansResizeY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(BansText, std::string, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", "")
  DECLARE_CONFIG_VARIABLE(BansBlueXStart, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(BansPurpleXStart, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(BansYStart, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(BansWidth, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(BansHeight, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)

  DECLARE_CONFIG_VARIABLE(TeamGamesThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(TeamGamesResizeX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(TeamGamesResizeY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(TeamGamesBlueXStart, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(TeamGamesPurpleXStart, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(TeamGamesYStart, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(TeamGamesWidth, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(TeamGamesHeight, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)

  DECLARE_CONFIG_VARIABLE(TeamNameThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(TeamNameResizeX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(TeamNameResizeY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(TeamNameBlueXStart, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(TeamNamePurpleXStart, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(TeamNameYStart, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(TeamNameWidth, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(TeamNameHeight, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)

  DECLARE_CONFIG_VARIABLE(PlayerChampDraftYStart, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerChampDraftYIncr, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerChampDraftBlueXStart, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerChampDraftPurpleXStart, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerChampDraftWidth, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerChampDraftHeight, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)

  DECLARE_CONFIG_VARIABLE(PlayerChampYStart, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerChampYIncr, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerChampBlueXStart, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerChampPurpleXStart, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerChampWidth, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerChampHeight, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)

  DECLARE_CONFIG_VARIABLE(PlayerNameYStart, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerNameYIncr, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerNameBlueXStart, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerNamePurpleXStart, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerNameWidth, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerNameHeight, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)

  DECLARE_CONFIG_VARIABLE(PlayerKDAYStart, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerKDAYIncr, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerKDABlueXStart, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerKDAPurpleXStart, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerKDAWidth, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerKDAHeight, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)

  DECLARE_CONFIG_VARIABLE(PlayerItemBlueX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerItemPurpleX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerItemXIncr, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerItemY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerItemYIncr, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerItemWidth, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(PlayerItemHeight, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)

  DECLARE_CONFIG_VARIABLE(CSYStart, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(CSYIncrement, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(CSXBlueStart, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(CSXPurpleStart, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(CSXSize, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(CSYSize, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)

  DECLARE_CONFIG_VARIABLE(BanChampThreshold, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(BanChampResizeX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(BanChampResizeY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(BanChampYStart, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(BanChampBlueXStart, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(BanChampPurpleXStart, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(BanChampXIncrement, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(BanChampWidth, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(BanChampHeight, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)

  DECLARE_CONFIG_VARIABLE(BanPercentYAdjust, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(BanPercentHeightAdjust, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(MinibarBGAllyChannel, int, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0)
  DECLARE_CONFIG_VARIABLE(MinibarBGEnemyChannel, int, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0)
  // Size of the image in 1080p
  DECLARE_CONFIG_VARIABLE(MinibarObjectiveIconTargetX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(MinibarObjectiveIconTargetY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(MinibarSupportingIconTargetX, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(MinibarSupportingIconTargetY, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
};

#endif

