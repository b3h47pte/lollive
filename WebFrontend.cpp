#include "WebFrontend.h"
#include "WebLeagueHandler.h"
#include "Dispatch.h"

WebFrontend::WebFrontend() {
  // Create and start the server
  const char *options[] = {
    "listening_ports", "127.0.0.1:8080",
    NULL
  };
  mServer = std::shared_ptr<CivetServer>(new CivetServer(options, NULL));

  // Game Handlers
  WebLeagueHandler* mLeagueHandler = new WebLeagueHandler();

  // Register Handlers
  mServer->addHandler("/league", mLeagueHandler);
}

WebFrontend::~WebFrontend() {

}