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

private:
};

#endif