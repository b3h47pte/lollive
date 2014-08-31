#pragma once
#ifndef _LEAGUESPECTATORVIDEOANALYZER_H
#define _LEAGUESPECTATORVIDEOANALYZER_H

#include "LeagueVideoAnalyzer.h"

/*
 * Video Analyzer specifically for the spectator mode.
 */
class LeagueSpectatorVideoAnalyzer : public LeagueVideoAnalyzer {
protected:
  virtual std::shared_ptr<class ImageAnalyzer> CreateImageAnalyzer(std::string& path);
};

#endif