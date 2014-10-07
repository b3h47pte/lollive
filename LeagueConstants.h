#pragma once
#ifndef _LEAGUECONSTANTS_H
#define _LEAGUECONSTANTS_H
#include "common.h"

// %i for player index. %t for team.
const std::string LEAGUE_CHAMPION_HINT = "player-%i-champion-%t";
inline std::string CreateLeagueChampionHint(unsigned int index, unsigned int team) {
  std::string hintKey = LEAGUE_CHAMPION_HINT;
  size_t pIdx = hintKey.find("%i");
  hintKey.replace(pIdx, 2, std::to_string(index).c_str());
  size_t teamIdx = hintKey.find("%t");
  hintKey.replace(teamIdx, 2, std::to_string(team).c_str());
  return hintKey;
}

// No champion (i.e. pick/ban screen)
const std::string LEAGUE_NO_CHAMPION = "None";

// Invalid Item Count/ID
const int LEAGUE_INVALID_ITEM = 9999;
const int LEAGUE_ITEM_SQUARE_SIZE = 64;

#endif