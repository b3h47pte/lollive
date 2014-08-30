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
  mg_printf(conn, ds->GetJSONResponse(std::string("league"), inMode, inStream).c_str());
  

  return true;
}