#include "Dispatch.h"
#include "VideoAnalyzer.h"
#include "VideoFetcher.h"
#include <functional>

std::shared_ptr<Dispatch> Dispatch::Singleton = std::shared_ptr<Dispatch>(new Dispatch);

Dispatch::Dispatch() {

}

Dispatch::~Dispatch() {

}

/*
 * Retrieves current data that is fetched from the stream. 
 * If the stream wasn't already being analyzed, then return a "waiting" 
 * status meaning that the user should try again soon. 
 * 
 * Otherwise, return JSON describing the game's state.
 */
std::string Dispatch::GetJSONResponse(std::string& game, std::string& mode, std::string& url) {
  bool bJustMade = false;
  std::shared_ptr<DispatchObject> dispatchObj;
  mMappingMutex.lock();
  try {
    dispatchObj = mMapping.at(url);
  } catch (...) {
    // not found so we make a new object
    bJustMade = true;
    std::shared_ptr<DispatchObject> newObj(new DispatchObject);
    newObj->mAnalyze = CreateAnalyzer(game, mode);
    newObj->mFetch = CreateVideoFetcher(url, std::bind(&VideoAnalyzer::NotifyNewFrame, newObj->mAnalyze, 
      std::placeholders::_1, std::placeholders::_2)); // This starts the process of analyzing the video so it must be made last.

    mMapping[url] = newObj;
  }
  mMappingMutex.unlock();

  if (bJustMade) {
    return "{\"status\":\"waiting\"}";
  }

  // The dispatcher doesn't know about the data the analyzer has so we need the analyzer
  // to return the data in JSON format for us.
  return dispatchObj->mAnalyze->GetCurrentDataJSON();
}

std::shared_ptr<class VideoAnalyzer> Dispatch::CreateAnalyzer(std::string& game, std::string& mode) {
  return NULL;
}

std::shared_ptr<class VideoFetcher> Dispatch::CreateVideoFetcher(std::string& url, std::function<void(IMAGE_PATH_TYPE, IMAGE_TIMESTAMP_TYPE)> cb) {
  // Create a unique ID for this game.
  std::string id = ""; 
  std::shared_ptr<VideoFetcher> newFetch(new VideoFetcher(id, url, cb));
  newFetch->BeginFetch();
  return newFetch;
}