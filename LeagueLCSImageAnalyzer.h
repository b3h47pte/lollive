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

  DECLARE_CONFIG_VARIABLE(CSYStart, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(CSYIncrement, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(CSXBlueStart, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(CSXPurpleStart, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(CSXSize, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)
  DECLARE_CONFIG_VARIABLE(CSYSize, double, ConfigManager::CONFIG_LEAGUE_FILENAME, "LCSImageAnalyzer", 0.0)

};

#endif

