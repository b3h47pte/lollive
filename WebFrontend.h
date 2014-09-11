#pragma once
#ifndef _WEBFRONTEND_H
#define _WEBFRONTEND_H

#include "common.h"
#include "civetweb/CivetServer.h"

/*
 * Handles the RESTful API used to communicate with this program.
 * Routes: /{game}?mode={mode}
 * Game Options:
 *  - league
 */
class WebFrontend {
public:
  WebFrontend();
  ~WebFrontend();
private:
  std::shared_ptr<CivetServer> mServer;

  // Config File Constants (Found in general.ini)
  const std::string FrontendSection = "WebFrontend";
};

#endif