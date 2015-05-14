#pragma once
#ifndef _LEAGUEVIDEOANALYZER_H
#define _LEAGUEVIDEOANALYZER_H

#include "VideoAnalyzer.h"
#include "LeaguePlayerData.h"

/*
 * Handles analyzing video footage from a League of Legends stream.
 */
class LeagueVideoAnalyzer: public VideoAnalyzer {
public:
  LeagueVideoAnalyzer(const std::string& configPath, bool isRemoteConfigPath);
  virtual ~LeagueVideoAnalyzer();

  static std::string LEAGUE_PROPERTY_FILENAME;
protected:
  // The generalized league video analyzer should be able to do some of the update itself
  // since some properties are common to all league games.
  virtual bool StoreData(std::shared_ptr<class ImageAnalyzer> img) override;

  // Get JSON
  virtual std::string ParseJSON();

protected:
  virtual std::shared_ptr<class ImageAnalyzer> CreateImageAnalyzer(std::string& path, const std::string& configPath) override;
  virtual void PostCreateImageAnalyzer(std::shared_ptr<class ImageAnalyzer> img) override;

  virtual void LoadImagePropertyFile();

  // Need to be able to determine when the match is over. I do this by checking for a sequence of invalid frames.
  // When the match is over, also determine who the winning team is.
  int continuousInvalidFrameCount;
  bool isMatchOver;
  ELeagueTeams winningTeam;

  // Need to keep track of the camera position as it changes over time.
  double lastMapPositionX;
  double lastMapPositionY;

  // Need to be able to determine when we are in the pick/ban phase of a draft match. 
  // TODO: Make this work for regular games. For now focus on LCS.
  bool isDraftPhase;

  // This indicates that we are in the phase between the draft stage and the game actually starting
  bool isWaitingToStart;

private:
  DECLARE_CONFIG_VARIABLE(MatchOverThreshold, int, ConfigManager::CONFIG_LEAGUE_FILENAME, "VideoAnalyzer", 0)
};

#endif
