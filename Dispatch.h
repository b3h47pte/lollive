#pragma once
#ifndef _DISPATCH_H
#define _DISPATCH_H

#include "common.h"
#include <mutex>

/*
 * Dispatch Object.
 */
struct DispatchObject {
  // Holds everything we need to analyze the data of a stream
  // This includes the video fetcher and the video analyzer.
  std::shared_ptr<class VideoFetcher> mFetch;
  std::shared_ptr<class VideoAnalyzer> mAnalyze;
};

/*
 * The dispatcher's job is to act as the mediator between the RESTful API frontend
 * and the video analyzer/fetcher backend.
 * 
 * This means that the dispatcher must retain of a map of which stream is being analyzed 
 * where so we don't have multiple threads running off doing the same thing.
 * 
 * This is a singleton object. It is also forced to be thread-safe.
 */
class Dispatch {
public:
  static std::shared_ptr<class Dispatch> Get() { return Singleton; }
  ~Dispatch();

  // Get's the JSON response for the specified request.
  std::string GetJSONResponse(const std::string& game, const std::string& configPath, const std::string& url, bool bIsDebug);

  friend class WebFrontend;
protected:
  Dispatch();
  static std::shared_ptr<class Dispatch> Singleton;

private:
  std::mutex mMappingMutex;
  // Mapping from the URL to the dispatch object;
  // Game and mode are ignored because a URL can only point to one game/mode anyways.
  std::map < std::string, std::shared_ptr<DispatchObject>> mMapping;

  // Helper function to create the right analyzer
  std::shared_ptr<class VideoAnalyzer> CreateAnalyzer(std::string& game, std::string& configPath, bool bIsDebug);

  // Helper function to create the video fetcher. This also starts the process of pulling the video.
  std::shared_ptr<class VideoFetcher> CreateVideoFetcher(std::string& url, std::string& game, std::string& configPath, std::function<void(IMAGE_PATH_TYPE, IMAGE_FRAME_COUNT_TYPE)> cb, bool bIsDebug);

  // Helper function to spin up a thread to begin the video analysis
  void Thread_StartNewDispatch(std::shared_ptr<DispatchObject> newObj, std::string& game, std::string& configPath, std::string& url, bool bIsDebug);

  // Config File Constants (Found in general.ini)
  const std::string DispatchSection = "Dispatch";
};

#endif 
