#include "Dispatch.h"
#include "VideoAnalyzer.h"
#include "LeagueSpectatorVideoAnalyzer.h"
#include "LeagueLCSVideoAnalyzer.h"
#include "VideoFetcher.h"
#include <functional>
#include <ctime>
#include <iomanip>
#include <thread>

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
    mMapping[url] = newObj;
    std::thread newThread = std::thread(std::bind(&Dispatch::Thread_StartNewDispatch, this, newObj, game, mode, url));
    newThread.detach();
  }
  mMappingMutex.unlock();

  if (bJustMade) {
    return "{\"status\":\"waiting\"}";
  }

  // The dispatcher doesn't know about the data the analyzer has so we need the analyzer
  // to return the data in JSON format for us.
  return dispatchObj->mAnalyze->GetCurrentDataJSON();
}

void Dispatch::Thread_StartNewDispatch(std::shared_ptr<DispatchObject> newObj, std::string& game, std::string& mode, std::string& url) {
  newObj->mAnalyze = CreateAnalyzer(game, mode);
  newObj->mFetch = CreateVideoFetcher(url, game, mode, std::bind(&VideoAnalyzer::NotifyNewFrame, newObj->mAnalyze,
    std::placeholders::_1, std::placeholders::_2)); // This starts the process of analyzing the video so it must be made last.

  mMappingMutex.lock();
  mMapping.erase(url);
  mMappingMutex.unlock();
}

std::shared_ptr<class VideoAnalyzer> Dispatch::CreateAnalyzer(std::string& game, std::string& mode) {
  if (game == "league") {
    if (mode == "lcs") {
      return std::shared_ptr<VideoAnalyzer>(new LeagueLCSVideoAnalyzer);
    } else if (mode == "spec") {
      return std::shared_ptr<VideoAnalyzer>(new LeagueSpectatorVideoAnalyzer);
    } else if (mode == "game") {
      return NULL; // not yet implemented
    }
  }
  return NULL;
}

std::shared_ptr<class VideoFetcher> Dispatch::CreateVideoFetcher(std::string& url, std::string& game, std::string& mode, std::function<void(IMAGE_PATH_TYPE, IMAGE_FRAME_COUNT_TYPE)> cb) {
  // Get the current date since it'll be part of the ID.
  std::time_t curTime = std::time(NULL);
  struct std::tm* utcTime = std::gmtime(&curTime);
  char buffer[80];
  std::strftime(buffer, 80, "%Y-%m-%d", utcTime);
  std::string timeStr(buffer);

  // Create a unique ID for this game.
  std::string id = timeStr + "-" + game + "-" + mode;
  std::shared_ptr<VideoFetcher> newFetch(new VideoFetcher(id, url, cb));
  newFetch->BeginFetch();
  return newFetch;
}