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
const std::string LEAGUE_ITEM_FORMAT = "%1_ImagePack_items_Embeds__e_%2_%3.png";
inline std::string CreateLeagueItemFileName(int count, std::string& id, std::string& name) {
  std::string filename = LEAGUE_ITEM_FORMAT;
  size_t countIdx = filename.find("%1");
  filename.replace(countIdx, 2, std::to_string(count).c_str());
  size_t idIdx = filename.find("%2");
  filename.replace(idIdx, 2, id);
  size_t nameIdx = filename.find("%3");
  filename.replace(nameIdx, 2, name);
  return filename;
}

#endif