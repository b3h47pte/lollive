#include "Dispatch.h"

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
  mMappingMutex.lock();
  try {
    std::shared_ptr<DispatchObject> dispatchObj = mMapping.at(url);
  } catch (...) {
    // not found so we make a new object
    bJustMade = true;
    std::shared_ptr<DispatchObject> newObj(new DispatchObject);
    mMapping[url] = newObj;
  }
  mMappingMutex.unlock();

  if (bJustMade) {
    return "{\"status\":\"waiting\"}";
  }


  return "";
}