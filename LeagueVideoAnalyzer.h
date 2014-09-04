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
  virtual std::string ParseJSON();

protected:
  // Need to be able to determine when the match is over. I do this by checking for a sequence of invalid frames.
  int continuousInvalidFrameCount;
  bool isMatchOver;

  // Need to be able to determine when we are in the pick/ban phase of a draft match. 
  // TODO: Make this work for regular games. For now focus on LCS.
  bool isDraftPhase;

  // This indicates that we are in the phase between the draft stage and the game actually starting
  bool isWaitingToStart;

private:
};

#endif