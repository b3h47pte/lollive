#pragma once
#ifndef _LOLIMAGEANALYZER_H
#define _LOLIMAGEANALYZER_H

#include "ImageAnalyzer.h"
#include "LeagueTeamData.h"

/*
 * Base class for analyzing League of Legends screenshots. Eventually need to split it to analyze
 * spectator mode/LCS, as well as a regular player playing.
 * Not exactly sure how to go about doing this yet since I'm not familiar with how OpenCV works. HA.
 */
class LeagueImageAnalyzer: public ImageAnalyzer {
public:
  LeagueImageAnalyzer(IMAGE_PATH_TYPE ImagePath);
  virtual ~LeagueImageAnalyzer();

  // There are some things that are common to all games, whether or not its in spectator mode or not.
  // This includes the current time, the map, kills for both teams. However, getting this information
  // must be handled by the subclasses.
  virtual bool Analyze();

protected:
  // Get match time specified in seconds since match start.
  virtual int AnalyzeMatchTime() = 0;

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
private:
};

#endif