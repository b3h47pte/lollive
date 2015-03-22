#pragma once
#ifndef _LEAGUEITEMDATA_H
#define _LEAGUEITEMDATA_H

#include "common.h"
#include "LeagueConstants.h"
#include "opencv2/core.hpp"

struct LeagueItemData {
  std::string itemID;
  cv::Mat itemImage;
  bool isEnchantment;
  std::string baseItemID;

  bool IsInvalid() {
    return (itemID == std::to_string(LEAGUE_INVALID_ITEM));
  }
};
typedef std::shared_ptr<LeagueItemData> PtrLeagueItemData;

#endif