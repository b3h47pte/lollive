#pragma once
#ifndef _DISPATCH_H
#define _DISPATCH_H

#include "common.h"
#include <mutex>

/*
 * Dispatch Object.
 */
struct DispatchObject {

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
  std::string GetJSONResponse(std::string& game, std::string& mode, std::string& url);

  friend class WebFrontend;
protected:
  Dispatch();
  static std::shared_ptr<class Dispatch> Singleton;

private:
  std::mutex mMappingMutex;
  // Mapping from the URL to the dispatch object;
  // Game and mode are ignored because a URL can only point to one game/mode anyways.
  std::map < std::string, std::shared_ptr<DispatchObject>> mMapping;

};

#endif 