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

  // Player Score. 
  // TODO: Return the champion that the scoreboard say it is associated with. This way we can see if the scoreboard matches up with the sidebar.
  // For now, just assume that it does
  virtual std::string AnalyzePlayerScore(uint idx, ELeagueTeams team, int* kills, int* deaths, int* assists, int* cs);
  virtual cv::Rect GetPlayerKDASection(uint idx, ELeagueTeams team);
  virtual cv::Rect GetPlayerCSSection(uint idx, ELeagueTeams team);
private:
};

#endif