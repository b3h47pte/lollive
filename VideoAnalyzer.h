#pragma once
#ifndef _VIDEOANALYZER_H
#define _VIDEOANALYZER_H

#include "common.h"
#include "Data.h"
#include <mutex>
#include <vector>
/*
 * Analyzes the video. Receives in frames from the video fetcher,
 * passes those frames to the image analyzer. 
 * 
 * In addition to just being a pass through to the image analyzer, the
 * video analyzer will have game/mode specific functions that help to
 * combine the data from the image analyzer to paint a coherent picture
 * of the game as time goes on. For example, let's say in frame 1, the image analyzer
 * says the blue team has 14 kills and then in frame 2, the image analyzer says the blue team
 * has 13 kills. The video analyzer will detect this anomaly and ignore the second frame's
 * '13 kills' analysis.
 *
 * This class must be thread safe.
 */
class VideoAnalyzer {
public:
  VideoAnalyzer();
  virtual ~VideoAnalyzer();

  // Callback function to notify us about a new frame
  void NotifyNewFrame(IMAGE_PATH_TYPE path, IMAGE_TIMESTAMP_TYPE time);

  // Returns all the data in JSON format.
  virtual std::string GetCurrentDataJSON() = 0;

protected:
  virtual std::shared_ptr<class ImageAnalyzer> CreateImageAnalyzer() = 0;

  // Overridden by subclasses so that they can do what is necessary to 
  // modify the data. Returns whether or not this frame was used. 
  virtual bool StoreData(std::shared_ptr<class ImageAnalyzer> img) = 0;

  std::mutex mDataMutex;
  // Holds the current information about this video
  std::shared_ptr<GenericDataStore> mData;
};

#endif