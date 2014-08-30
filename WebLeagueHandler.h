#pragma once
#ifndef _WEBLEAGUEHANDLER_H
#define _WEBLEAGUEHANDLER_h

#include "common.h"
#include "civetweb/CivetServer.h"

/*
 * Handles URI for any requests related to League of Legends.
 */
class WebLeagueHandler : public CivetHandler {
public:
  WebLeagueHandler();
  virtual ~WebLeagueHandler();

  // At this point we should be at /league?mode={mode}&stream={uri} in the URI.
  // League Mode Options:
  // -lcs(LCS Spectator)
  // -spec(Regular Spectator)
  // -game(Regular Game)
  virtual bool handleGet(CivetServer *server, struct mg_connection *conn);
};

#endif