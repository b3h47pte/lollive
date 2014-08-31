#pragma once
#ifndef _LEAGUELCSVIDEOANALYZER_H
#define _LEAGUELCSVIDEOANALYZER_H

#include "LeagueSpectatorVideoAnalyzer.h"

/*
* Video Analyzer specifically for the spectator mode.
*/
class LeagueLCSVideoAnalyzer : public LeagueSpectatorVideoAnalyzer {
protected:
  virtual std::shared_ptr<class ImageAnalyzer> CreateImageAnalyzer(std::string& path);
};

#endif