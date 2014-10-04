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

enum ELeagueKillType {
  ELKT_KILL,
  ELKT_MULTI,
  ELKT_SHUTDOWN,
  ELKT_MAX
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
typedef std::shared_ptr<struct LeagueEvent> PtrLeagueEvent;
typedef std::map<std::size_t, PtrLeagueEvent> MapPtrLeagueEvent;
struct LeagueEvent {
  LeagueEvent() : RelevantTeam(ELT_UNKNOWN), EventId(ELEI_UNKNOWN), AdditionalInfo(""), PlayerInstigator(""), Timestamp(-1) {
  }


  // Which team this event is for (the team that killed the thing/champion, not the team that just lost a turret).
  ELeagueTeams RelevantTeam;
  // What just happened.
  ELeagueEventId EventId;
  // Sometimes we need more information. Which turret? Which inhib? Which champion?
  std::string AdditionalInfo;
  // All of these events have a source (some player landed the final blow).
  std::string PlayerInstigator;
  // Sometimes other people help out (like in the case of a champion kill).
  std::vector<std::string> SupportingPlayers;
  // Time stamp. However, this is not used as identifying information for this event. So let's say we see 
  // Tristana kills Janna in Frame 1 and Frame 2. If the time stamp were used, these two events would be different; however,
  // by necessity, they are obviously the same since they're so close together. The timestamp should only be used later
  // to determine when this event actually happened.
  int Timestamp;
  // Kill Type Information. Can't encode this in any other section.
  ELeagueKillType KillType;

  // Gets a unique identifier for this event.
  // Supporting players is not used since the only real identifying information is the team + event type.
  std::size_t GetIdentifier() const {
    std::string strIdent = std::to_string(RelevantTeam) + "-" + std::to_string(EventId) + "-" + AdditionalInfo + "-" + PlayerInstigator;
    std::hash<std::string> hash;
    return hash(strIdent);
  }

  // Spits out alll the information into stdout
  void Print() const {
    std::cout << "EVENT INFORMATION" << std::endl;
    std::cout << " - Team: " << RelevantTeam << std::endl;
    std::cout << " - Id: " << EventId << std::endl;
    std::cout << " - Add. Info: " << AdditionalInfo << std::endl;
    std::cout << " - Instigator: " << PlayerInstigator << std::endl;
    std::cout << " - Supporting Players: ";
    for (auto& s : SupportingPlayers) {
      std::cout << s << " ";
    }
    std::cout << std::endl << " - Kill Type: " << KillType << std::endl;
  }

  void Update(PtrLeagueEvent newEvent, int timestamp);

  void Copy(PtrLeagueEvent newEvent, int timestamp) {
    RelevantTeam = newEvent->RelevantTeam;
    EventId = newEvent->EventId;
    AdditionalInfo = newEvent->AdditionalInfo;
    PlayerInstigator = newEvent->PlayerInstigator;
    SupportingPlayers = newEvent->SupportingPlayers;
    Timestamp = timestamp;
    KillType = newEvent->KillType;
  }
};

#endif