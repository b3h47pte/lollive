#include "WebLeagueHandler.h"
#include "civetweb/civetweb.h"
#include "Dispatch.h"
#include "FileUtility.h"

WebLeagueHandler::WebLeagueHandler() {

}

WebLeagueHandler::~WebLeagueHandler() {

}

bool WebLeagueHandler::handleGet(CivetServer *server, struct mg_connection *conn) {
  std::string inMode;
  std::string inStream;
  std::string inHost;
  std::string inPort;
  if (!CivetServer::getParam(conn, "mode", inMode)) {
    return false;
  }

  if (!CivetServer::getParam(conn, "stream", inStream)) {
    return false;
  }

  if (!CivetServer::getParam(conn, "apiHost", inHost)) {
    return false;
  }

  if (!CivetServer::getParam(conn, "apiPort", inPort)) {
    return false;
  }

  uint16_t port = 0;
  try {
    port = (uint16_t)std::stoul(inPort, nullptr);
  } catch (...) {
    return false;
  }

  // Make sure we specified a valid mode
  std::string configPath;
  if (inMode == "lcs") {
    configPath = GetRelativeFilePath("Config/League/lcs.ini");
  } else if (inMode == "spec") {
    return false;
  } else if (inMode == "game") {
    return false; // not yet implemented
  } else {
    return false;
  }

  // At this point we should have a stream URL and the game mode which is enough for the
  // dispatcher to begin analyzing.
  std::shared_ptr<Dispatch> ds = Dispatch::Get();
#ifdef _DEBUG
  bool bIsDebug = false;
  std::string tmpStore;
  bIsDebug = CivetServer::getParam(conn, "debug", tmpStore);
  ds->BeginNewDispatch(std::string("league"), configPath, inStream, inHost, port, bIsDebug);
#else
  ds->BeginNewDispatch(std::string("league"), configPath, inStream, inHost, port, false);
#endif

  
  mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n");
  mg_printf(conn, "OKAY");  
  return true;
}
