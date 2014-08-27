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

protected:
  // Gets the match timer from the header bar up top.
  virtual int AnalyzeMatchTime();

  // Team Kills. Also part of the header bar at the top of the screen.
  virtual int AnalyzeTeamKills(ELeagueTeams team);
  virtual cv::Rect GetTeamKillsSection(ELeagueTeams team);

  // Team Gold. Part of the header bar at the top of the screen.
  virtual int AnalyzeTeamGold(ELeagueTeams team);
  virtual cv::Rect GetTeamGoldSection(ELeagueTeams team);

  // Team Tower Kills. Final part of the header bar at the top of the screen.
  virtual int AnalyzeTeamTowerKills(ELeagueTeams team);
  virtual cv::Rect GetTeamTowerKillSection(ELeagueTeams team);
private:
};

#endif