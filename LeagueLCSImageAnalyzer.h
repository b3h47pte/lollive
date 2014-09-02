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

  // Need to get the team name
  virtual PtrLeagueTeamData AnalyzeTeamData(ELeagueTeams team);

  virtual std::string GetTeamName(ELeagueTeams team);
  virtual cv::Rect GetTeamNameSection(ELeagueTeams team);

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

