#include "WebFrontend.h"
#include "WebLeagueHandler.h"
#include "Dispatch.h"

WebFrontend::WebFrontend() {
  // Create and start the server
  mServer = std::shared_ptr<CivetServer>(new CivetServer(NULL, NULL));

  // Game Handlers
  WebLeagueHandler* mLeagueHandler = new WebLeagueHandler();

  // Register Handlers
  mServer->addHandler("/league", mLeagueHandler);
}

WebFrontend::~WebFrontend() {

}