#include "Dispatch.h"
#include "VideoAnalyzer.h"
#include "LeagueVideoAnalyzer.h"
#include "VideoFetcher.h"
#include "TestVideoFetch.h"
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
std::string Dispatch::GetJSONResponse(const std::string& game, const std::string& configPath, const std::string& url, bool bIsDebug) {
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
    std::thread newThread = std::thread(std::bind(&Dispatch::Thread_StartNewDispatch, this, newObj, game, configPath, url, bIsDebug));
    newThread.detach();
  }
  mMappingMutex.unlock();

  if (bJustMade) {
    return ConfigManager::Get()->GetStringFromINI(ConfigManager::CONFIG_GENERAL_FILENAME, DispatchSection, std::string("JustMadeResponse"), std::string(""));
  }

  // The dispatcher doesn't know about the data the analyzer has so we need the analyzer
  // to return the data in JSON format for us.
  return dispatchObj->mAnalyze->GetCurrentDataJSON();
}

void Dispatch::Thread_StartNewDispatch(std::shared_ptr<DispatchObject> newObj, std::string& game, std::string& configPath, std::string& url, bool bIsDebug) {
  newObj->mAnalyze = CreateAnalyzer(game, configPath, bIsDebug);
  newObj->mFetch = CreateVideoFetcher(url, game, configPath, std::bind(&VideoAnalyzer::NotifyNewFrame, newObj->mAnalyze,
    std::placeholders::_1, std::placeholders::_2), bIsDebug); // This starts the process of analyzing the video so it must be made last.

  mMappingMutex.lock();
  mMapping.erase(url);
  mMappingMutex.unlock();
}

std::shared_ptr<class VideoAnalyzer> Dispatch::CreateAnalyzer(std::string& game, std::string& configPath, bool bIsDebug) {
  if (game == "league") {
      return std::shared_ptr<VideoAnalyzer>(new LeagueVideoAnalyzer(configPath, !bIsDebug));
  }
  return NULL;
}

std::shared_ptr<class VideoFetcher> Dispatch::CreateVideoFetcher(std::string& url, std::string& game, std::string& configPath, std::function<void(IMAGE_PATH_TYPE, IMAGE_FRAME_COUNT_TYPE)> cb, bool bIsDebug) {
  // Get the current date since it'll be part of the ID.
  std::time_t curTime = std::time(NULL);
  struct std::tm* utcTime = std::gmtime(&curTime);
  char buffer[80];
  std::strftime(buffer, 80, "%Y-%m-%d", utcTime);
  std::string timeStr(buffer);

  // Create a unique ID for this game.
  std::string id = timeStr + "-" + game;
  std::shared_ptr<VideoFetcher> newFetch;
  if (!bIsDebug) {
    newFetch = std::shared_ptr<VideoFetcher>(new VideoFetcher(id, url, cb));
  } else{
    newFetch = std::shared_ptr<VideoFetcher>(new TestVideoFetch(id, url, cb));
  }
  newFetch->BeginFetch();
  return newFetch;
}
