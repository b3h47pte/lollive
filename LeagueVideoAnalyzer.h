#pragma once
#ifndef _LEAGUEVIDEOANALYZER_H
#define _LEAGUEVIDEOANALYZER_H

#include "VideoAnalyzer.h"

/*
 * Handles analyzing video footage from a League of Legends stream.
 */
class LeagueVideoAnalyzer: public VideoAnalyzer {
public:
  LeagueVideoAnalyzer();
  virtual ~LeagueVideoAnalyzer();
protected:
  // The generalized league video analyzer should be able to do some of the update itself
  // since some properties are common to all league games.
  virtual bool StoreData(std::shared_ptr<class ImageAnalyzer> img);

  // Get JSON
  virtual std::string GetCurrentDataJSON();
private:
};

#endif