#pragma once
#ifndef _LOLIMAGEANALYZER_H
#define _LOLIMAGEANALYZER_H

#include "ImageAnalyzer.h"
#include "LeaguePlayerData.h"
#include "LeagueTeamData.h"
#include "LeagueChampionDatabase.h"

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
  std::shared_ptr<const LeagueChampionDatabase> ChampionDatabase;
private:
};

#endif