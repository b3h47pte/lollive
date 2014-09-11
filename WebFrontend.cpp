#include "WebFrontend.h"
#include "WebLeagueHandler.h"
#include "Dispatch.h"

WebFrontend::WebFrontend() {
  // Create and start the server
  const char *options[] = {
    "listening_ports", ConfigManager::Get()->GetStringFromINI(ConfigManager::CONFIG_GENERAL_FILENAME, FrontendSection, std::string("ListeningPorts"), std::string("")).c_str(),
    NULL
  };
  mServer = std::shared_ptr<CivetServer>(new CivetServer(options, NULL));

  // Game Handlers
  WebLeagueHandler* mLeagueHandler = new WebLeagueHandler();

  // Register Handlers
  mServer->addHandler(ConfigManager::Get()->GetStringFromINI(ConfigManager::CONFIG_GENERAL_FILENAME, FrontendSection, std::string("LeagueWebPath"), std::string("")), mLeagueHandler);
}

WebFrontend::~WebFrontend() {

}