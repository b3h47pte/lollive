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
private:
};

#endif