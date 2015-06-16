#include "CompileTimeSettings.h"
#include "Dispatch.h"
#include "VideoAnalyzer.h"
#include "LeagueVideoAnalyzer.h"
#include "VideoFetcher.h"
#include "TestVideoFetch.h"
#include "Authentication.h"

#include "Poco/Foundation.h"
#include "Poco/URI.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"

#include "cjson/cJSON.h"

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
 * Create a new "DispatchObject" state object and starts off a new thread to begin processing the input stream.
 */
void Dispatch::BeginNewDispatch(const std::string& eventId, const std::string& game, const std::string& configPath, const std::string& streamUrl, const std::string& apiUrl, uint16_t apiPort, bool bIsDebug) {
  std::shared_ptr<DispatchObject> dispatchObject;
  mMappingMutex.lock();
  try {
    dispatchObject = mMapping.at(streamUrl);
    mMappingMutex.unlock();
    return;
  } catch (...) {
    dispatchObject = std::shared_ptr<DispatchObject>(new DispatchObject);
    
    Poco::URI uri(apiUrl);
    dispatchObject->apiHost = uri.getHost();
    dispatchObject->apiPath = uri.getPathAndQuery();
    if (dispatchObject->apiPath.empty()) {
      dispatchObject->apiPath = "/";
    }
    dispatchObject->apiPort = apiPort;
    dispatchObject->eventId = eventId;
    dispatchObject->gameShorthand = game;

    mMapping[streamUrl] = dispatchObject;
    std::thread newThread = std::thread(std::bind(&Dispatch::Thread_StartNewDispatch, this, dispatchObject, eventId, game, configPath, streamUrl, bIsDebug));
    newThread.detach();
  }
  mMappingMutex.unlock();
}

void Dispatch::Thread_StartNewDispatch(std::shared_ptr<DispatchObject> newObj, const std::string& eventId, const std::string& game, const std::string& configPath, const std::string& url, bool bIsDebug) {
  newObj->mAnalyze = CreateAnalyzer(newObj, eventId, game, configPath, bIsDebug);
  newObj->mFetch = CreateVideoFetcher(eventId, url, game, configPath, std::bind(&VideoAnalyzer::NotifyNewFrame, newObj->mAnalyze,
    std::placeholders::_1, std::placeholders::_2), bIsDebug); // This starts the process of analyzing the video so it must be made last.

  mMappingMutex.lock();
  mMapping.erase(url);
  mMappingMutex.unlock();
}

std::shared_ptr<class VideoAnalyzer> Dispatch::CreateAnalyzer(std::shared_ptr<DispatchObject> newObj, const std::string& eventId, const std::string& game, const std::string& configPath, bool bIsDebug) {
  std::shared_ptr<class VideoAnalyzer> analyzer = NULL;
  if (game == "league") {
    analyzer = std::shared_ptr<VideoAnalyzer>(new LeagueVideoAnalyzer(configPath, eventId, game, USE_REMOTE_CONFIG_PATH));
  }
  analyzer->AddJsonCallback(std::bind(&Dispatch::SendJSONDataToAPI, this, newObj, std::placeholders::_1));
  return analyzer;
}

std::shared_ptr<class VideoFetcher> Dispatch::CreateVideoFetcher(const std::string& eventId, const std::string& url, const std::string& game, const std::string& configPath, std::function<void(IMAGE_PATH_TYPE, IMAGE_FRAME_COUNT_TYPE)> cb, bool bIsDebug) {
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

    // THIS IS PURELY FOR DEBUGGING -- Change number as necessary
    std::dynamic_pointer_cast<TestVideoFetch>(newFetch)->SetStartFrame(948);
  }
  newFetch->BeginFetch();
  return newFetch;
}

void Dispatch::SendJSONDataToAPI(std::shared_ptr<DispatchObject> newObj, const std::string& json) {
  // Attach the HMAC to the message to verify that the data is coming from the server.
  cJSON* sendJson = NULL;
  std::string toParseJson = json;
  if (json == "") {
    sendJson = cJSON_CreateObject();
    toParseJson = "{}";
  } else {
    sendJson = cJSON_Parse(json.c_str());
  }

  std::string signature = Authentication::Get()->CalculateHMAC(toParseJson, ConfigManager::Get()->GetStringFromINI(ConfigManager::CONFIG_SERVER_FILENAME, "Keys", "PrivateKey", ""));
  cJSON_AddStringToObject(sendJson, "signature", signature.c_str());

  char* retChar = cJSON_PrintUnformatted(sendJson);
  std::string authedJson(retChar);
  delete[] retChar;

  Poco::Net::HTTPClientSession session(newObj->apiHost, (Poco::UInt16)newObj->apiPort);
  Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, newObj->apiPath);
  request.setContentType("application/json");
  request.setContentLength(authedJson.size());
  try {
    std::ostream& output = session.sendRequest(request);
    output << authedJson;
  } catch (...) {
    std::cout << "WARNING: Failed to send JSON data to the API server -- " << newObj->apiHost << "/" << newObj->apiPath << ":" << newObj->apiPort << std::endl;
  }
  cJSON_Delete(sendJson);
}
