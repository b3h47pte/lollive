#pragma once
#ifndef _LEAGUEVENT_H
#define _LEAGUEVENT_H

#include "common.h"
#include "LeaguePlayerData.h"

enum ELeagueEventId {
  ELEI_TURRET,
  ELEI_INHIB,
  ELEI_CHAMP,
  ELEI_BARON,
  ELEI_DRAGON,
  ELEI_UNKNOWN
};

/*
 * Provides functionality to store and hold an event. An event is any one of the following:
 *  - Turret Destruction
 *  - Inhibitor Destruction
 *  - Champion Kill
 *  - Baron/Dragon Kill
 * The thing with events is that they'll sometimes last through multiple images and it'll be up to the video
 * analyzer to make sure it doesn't account for these events more than once. This structure will provide
 * identifying utility functions to make that easier.
 */
struct LeagueEvent {
  LeagueEvent() : RelevantTeam(ELT_UNKNOWN), EventId(ELEI_UNKNOWN), AdditionalInfo(""), PlayerInstigator(NULL), Timestamp(-1) {
  }


  // Which team this event is for (the team that killed the thing/champion, not the team that just lost a turret).
  ELeagueTeams RelevantTeam;
  // What just happened.
  ELeagueEventId EventId;
  // Sometimes we need more information. Which turret? Which inhib? Which champion?
  std::string AdditionalInfo;
  // All of these events have a source (some player landed the final blow).
  PtrLeaguePlayerData PlayerInstigator;
  // Sometimes other people help out (like in the case of a champion kill).
  std::vector<PtrLeaguePlayerData> SupportingPlayers;
  // Time stamp. However, this is not used as identifying information for this event. So let's say we see 
  // Tristana kills Janna in Frame 1 and Frame 2. If the time stamp were used, these two events would be different; however,
  // by necessity, they are obviously the same since they're so close together. The timestamp should only be used later
  // to determine when this event actually happened.
  int Timestamp;

  // Gets a unique identifier for this event.
  std::size_t GetIdentifier() const {
    std::string strIdent = std::to_string(RelevantTeam) + "-" + std::to_string(EventId) + "-" + AdditionalInfo + "-" +
      std::to_string(PlayerInstigator->playerIdx);
    for (auto& player : SupportingPlayers) {
      strIdent += ("-" + std::to_string(player->playerIdx));
    }
    std::hash<std::string> hash;
    return hash(strIdent);
  }
};
typedef std::shared_ptr<struct LeagueEvent> PtrLeagueEvent;
typedef std::vector<PtrLeagueEvent> VectorPtrLeagueEvent;

#endif