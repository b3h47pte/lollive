#pragma once
#ifndef _LOLIMAGEANALYZER_H
#define _LOLIMAGEANALYZER_H

#include "ImageAnalyzer.h"

/*
 * Base class for analyzing League of Legends screenshots. Eventually need to split it to analyze
 * spectator mode/LCS, as well as a regular player playing.
 * Not exactly sure how to go about doing this yet since I'm not familiar with how OpenCV works. HA.
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

private:
};

#endif