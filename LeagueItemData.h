#pragma once
#ifndef _LEAGUEITEMDATA_H
#define _LEAGUEITEMDATA_H

#include "common.h"
#include "LeagueConstants.h"

struct LeagueItemData {
  int itemCount;
  std::string itemID;
  std::string itemName;

  bool IsInvalid() {
    return (itemCount == LEAGUE_INVALID_ITEM);
  }
};
typedef std::shared_ptr<LeagueItemData> PtrLeagueItemData;

#endif