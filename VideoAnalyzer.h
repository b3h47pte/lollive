#pragma once
#ifndef _VIDEOANALYZER_H
#define _VIDEOANALYZER_H

#include "common.h"
#include "Data.h"
#include <mutex>
#include <condition_variable>
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
  VideoAnalyzer(const std::string& configPath, const std::string& eventId, const std::string& gameShorthand, bool isRemoteConfigPath);
  virtual ~VideoAnalyzer();

  // Callback function to notify us about a new frame
  void NotifyNewFrame(IMAGE_PATH_TYPE path, IMAGE_FRAME_COUNT_TYPE time);

  // Returns all the data in JSON format.
  virtual std::string GetCurrentDataJSON() { return mDataJSON; }
  virtual std::string ParseJSON() = 0;

  void AddJsonCallback(std::function<void(const std::string&)> f);

protected:
  // Need to add some items to the JSON that only the VideoAnalyzer class has...
  // TODO: Make the JSON creation a function that takes in a JSON object pointer...
  void BaseModifyJSON(struct cJSON* json);

  virtual std::shared_ptr<class ImageAnalyzer> CreateImageAnalyzer(std::string& path, const std::string& configPath) = 0;
  virtual void LoadImagePropertyFile() {}

  // Must have mDataCV locked before calling this.
  virtual void PostCreateImageAnalyzer(std::shared_ptr<class ImageAnalyzer> img) = 0;

  // Overridden by subclasses so that they can do what is necessary to 
  // modify the data. Returns whether or not this frame was used. 
  virtual bool StoreData(std::shared_ptr<class ImageAnalyzer> img) = 0;

  // Data that we need to send back to the live stats API
  std::string eventId;
  std::string gameShorthand;

  // Frame count. We aren't allowed to call StoreData until this frame count matches the input image's frame count.
  size_t mFrameCount;

  std::mutex mDataMutex;
  std::unique_lock<std::mutex> mDataLock;
  std::condition_variable mDataCV;
  // Holds the current information about this video
  std::shared_ptr<GenericDataStore> mData;

  // Holds history of the raw information that we get from the image analyzer.
  // This allows us to correct for any weird one frame changes.
  std::vector<std::shared_ptr<GenericDataStore>> mDataHistory;

  // Cached data in JSON format. Updated whenever mData is updated.
  std::string mDataJSON;

  // Image Properties (These are the various things that we look for on the image).
  std::shared_ptr<std::unordered_map<std::string, T_EMPTY>> relevantProperties;

  void NotifyAllJsonCallbacks(const std::string& json);
private:
  std::string configPath;

  std::vector<std::function<void(const std::string&)> > jsonCallbacks;
};

#endif
