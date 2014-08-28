#pragma once
#ifndef _LEAGUECHAMPIONDATA_H
#define _LEAGUECHAMPIONDATA_H

#include "common.h"
#include "opencv2/core.hpp"

struct LeagueChampionData {
  std::string longName;
  std::string shortName;
  cv::Mat image; 
};

typedef std::shared_ptr<LeagueChampionData> PtrLeagueChampionData;

#endif