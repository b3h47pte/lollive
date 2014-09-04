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
};

#endif

