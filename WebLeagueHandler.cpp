#include "WebLeagueHandler.h"
#include "civetweb/civetweb.h"
#include "Dispatch.h"

WebLeagueHandler::WebLeagueHandler() {

}

WebLeagueHandler::~WebLeagueHandler() {

}

bool WebLeagueHandler::handleGet(CivetServer *server, struct mg_connection *conn) {
  std::string inMode;
  std::string inStream;
  if (!CivetServer::getParam(conn, "mode", inMode)) {
    return false;
  }

  if (!CivetServer::getParam(conn, "stream", inStream)) {
    return false;
  }

  // Make sure we specified a valid mode
  if (inMode == "lcs") {
  } else if (inMode == "spec") {
  } else if (inMode == "game") {
    return false; // not yet implemented
  } else {
    return false;
  }

  // At this point we should have a stream URL and the game mode which is enough for the
  // dispatcher to begin analyzing.
  std::shared_ptr<Dispatch> ds = Dispatch::Get();
  
  mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n");

#ifdef _DEBUG
  bool bIsDebug = false;
  std::string tmpStore;
  bIsDebug = CivetServer::getParam(conn, "debug", tmpStore);
  mg_printf(conn, ds->GetJSONResponse(std::string("league"), inMode, inStream, bIsDebug).c_str());
#else
  mg_printf(conn, ds->GetJSONResponse(std::string("league"), inMode, inStream, false).c_str());
#endif
  
  return true;
}
