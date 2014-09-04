#pragma once
#ifndef _LEAGUECONSTANTS_H
#define _LEAGUECONSTANTS_H
#include "common.h"

// %i for player index. %t for team.
const std::string LEAGUE_CHAMPION_HINT = "player-%i-champion-%t";
inline std::string CreateLeagueChampionHint(uint index, uint team) {
  std::string hintKey = LEAGUE_CHAMPION_HINT;
  size_t pIdx = hintKey.find("%i");
  size_t teamIdx = hintKey.find("%t");
  hintKey.replace(pIdx, 2, std::to_string(index).c_str());
  hintKey.replace(teamIdx, 2, std::to_string(team).c_str());
  return hintKey;
}

// No champion (i.e. pick/ban screen)
const std::string LEAGUE_NO_CHAMPION = "None";

#endif