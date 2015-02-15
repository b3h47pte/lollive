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

// Property Constants
const std::string LEAGUE_DRAFT_BANS_TEXT_BLUE = "LEAGUE_DRAFT_BANS_TEXT_BLUE";
const std::string LEAGUE_DRAFT_BANS_TEXT_RED = "LEAGUE_DRAFT_BANS_TEXT_RED";
const std::string LEAGUE_DRAFT_VERSUS_TEXT = "LEAGUE_DRAFT_VERSUS_TEXT";
const std::string LEAGUE_DRAFT_BANS_CHAMPIONS_BLUE = "LEAGUE_DRAFT_BANS_CHAMPIONS_BLUE";
const std::string LEAGUE_DRAFT_BANS_CHAMPIONS_RED = "LEAGUE_DRAFT_BANS_CHAMPIONS_RED";
const std::string LEAGUE_DRAFT_BANS_PERCENT_BLUE = "LEAGUE_DRAFT_BANS_PERCENT_BLUE";
const std::string LEAGUE_DRAFT_BANS_PERCENT_RED = "LEAGUE_DRAFT_BANS_PERCENT_RED";
const std::string LEAGUE_DRAFT_PICK_CHAMPIONS_BLUE = "LEAGUE_DRAFT_PICK_CHAMPIONS_BLUE";
const std::string LEAGUE_DRAFT_PICK_CHAMPIONS_RED = "LEAGUE_DRAFT_PICK_CHAMPIONS_RED";
const std::string LEAGUE_GAMESTATE_MATCH_TIME = "LEAGUE_GAMESTATE_MATCH_TIME";
const std::string LEAGUE_GAMESTATE_MAP = "LEAGUE_GAMESTATE_MAP";
const std::string LEAGUE_TEAM_KILLS_RED = "LEAGUE_TEAM_KILLS_RED";
const std::string LEAGUE_TEAM_KILLS_BLUE = "LEAGUE_TEAM_KILLS_BLUE";
const std::string LEAGUE_TEAM_GOLD_RED = "LEAGUE_TEAM_GOLD_RED";
const std::string LEAGUE_TEAM_GOLD_BLUE = "LEAGUE_TEAM_GOLD_BLUE";
const std::string LEAGUE_TEAM_TOWERS_RED = "LEAGUE_TEAM_TOWERS_RED";
const std::string LEAGUE_TEAM_TOWERS_BLUE = "LEAGUE_TEAM_TOWERS_BLUE";
const std::string LEAGUE_TEAM_GAMES_RED = "LEAGUE_TEAM_GAMES_RED";
const std::string LEAGUE_TEAM_GAMES_BLUE = "LEAGUE_TEAM_GAMES_BLUE";
const std::string LEAGUE_TEAM_NAME_RED = "LEAGUE_TEAM_NAME_RED";
const std::string LEAGUE_TEAM_NAME_BLUE = "LEAGUE_TEAM_NAME_BLUE";
const std::string LEAGUE_PLAYER_CHAMP_LEVEL_RED = "LEAGUE_PLAYER_CHAMP_LEVEL_RED";
const std::string LEAGUE_PLAYER_CHAMP_LEVEL_BLUE = "LEAGUE_PLAYER_CHAMP_LEVEL_BLUE";
const std::string LEAGUE_PLAYER_CHAMP_IMAGE_RED = "LEAGUE_PLAYER_CHAMP_IMAGE_RED";
const std::string LEAGUE_PLAYER_CHAMP_IMAGE_BLUE = "LEAGUE_PLAYER_CHAMP_IMAGE_BLUE";
const std::string LEAGUE_PLAYER_NAME_RED = "LEAGUE_PLAYER_NAME_RED";
const std::string LEAGUE_PLAYER_NAME_BLUE = "LEAGUE_PLAYER_NAME_BLUE";
const std::string LEAGUE_PLAYER_CS_RED = "LEAGUE_PLAYER_CS_RED";
const std::string LEAGUE_PLAYER_CS_BLUE = "LEAGUE_PLAYER_CS_BLUE";
const std::string LEAGUE_PLAYER_KDA_RED = "LEAGUE_PLAYER_KDA_RED";
const std::string LEAGUE_PLAYER_KDA_BLUE = "LEAGUE_PLAYER_KDA_BLUE";
const std::string LEAGUE_PLAYER_ITEMS_RED = "LEAGUE_PLAYER_ITEMS_RED";
const std::string LEAGUE_PLAYER_ITEMS_BLUE = "LEAGUE_PLAYER_ITEMS_BLUE";
const std::string LEAGUE_EVENT_MINIBAR = "LEAGUE_EVENT_MINIBAR";
const std::string LEAGUE_EVENT_ANNOUNCE = "LEAGUE_EVENT_ANNOUNCE";
const std::string LEAGUE_EVENT_MINIBAR_MAIN_ICON = "LEAGUE_EVENT_MINIBAR_MAIN_ICON";
const std::string LEAGUE_EVENT_MINIBAR_SUPPORTING_ICON = "LEAGUE_EVENT_MINIBAR_SUPPORTING_ICON";
const std::string LEAGUE_GAMESTATE_REPLAY = "LEAGUE_GAMESTATE_REPLAY";
#endif
