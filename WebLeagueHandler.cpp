#include "WebLeagueHandler.h"
#include "civetweb/civetweb.h"
#include "Dispatch.h"
#include "FileUtility.h"
#include "CompileTimeSettings.h"

WebLeagueHandler::WebLeagueHandler() {

}

WebLeagueHandler::~WebLeagueHandler() {

}

bool WebLeagueHandler::handleGet(CivetServer *server, struct mg_connection *conn) {
  std::string inMode;
  std::string inStream;
  std::string inHost;
  std::string inPort;
  std::string eventId;

  if (!CivetServer::getParam(conn, "stream", inStream)) {
    return false;
  }

  if (!CivetServer::getParam(conn, "apiHost", inHost)) {
    return false;
  }

  if (!CivetServer::getParam(conn, "apiPort", inPort)) {
    return false;
  }

  if (!CivetServer::getParam(conn, "eventId", eventId)) {
    return false;
  }

  uint16_t port = 0;
  try {
    port = (uint16_t)std::stoul(inPort, nullptr);
  } catch (...) {
    return false;
  }

  // TODO: Pull config path from the specified config path in the URL parameters.
  std::string configPath = GetRelativeFilePath("Config/League/lcs.ini");

  // At this point we should have a stream URL and the game mode which is enough for the
  // dispatcher to begin analyzing.
  std::shared_ptr<Dispatch> ds = Dispatch::Get();
  if (ALLOW_DEBUG_VIDEO_FETCH) {
    bool bIsDebug = false;
    std::string tmpStore;
    bIsDebug = CivetServer::getParam(conn, "debug", tmpStore);
    ds->BeginNewDispatch(eventId, std::string("league"), configPath, inStream, inHost, port, bIsDebug);
  } else {
    ds->BeginNewDispatch(eventId, std::string("league"), configPath, inStream, inHost, port, false);
  }
  
  mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n");
  mg_printf(conn, "OKAY");  
  return true;
}
