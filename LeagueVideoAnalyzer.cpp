#include "LeagueVideoAnalyzer.h"
#include "ImageAnalyzer.h"
#include "LeagueImageAnalyzer.h"
#include "LeagueTeamData.h"
#include "PropertyManager.h"
#include "cjson/cJSON.h"

LeagueVideoAnalyzer::LeagueVideoAnalyzer(const std::string& configPath, bool isRemoteConfigPath) : 
  VideoAnalyzer(configPath, isRemoteConfigPath), continuousInvalidFrameCount(0), isMatchOver(false), isDraftPhase(false) {

  // TODO: Make this load properties based on input.
  relevantProperties = PropertyManager::Get()->GetProperties(EGI_League);
}

LeagueVideoAnalyzer::~LeagueVideoAnalyzer() {

}

/*
 * From the image, see what properties make sense to update.
 * Update properties into the data store that already exists.
 * 
 * no need to lock/unlock because the callee does that for us.
 * Return value is kinda useless here. TODO: Remove return value?
 */
bool LeagueVideoAnalyzer::StoreData(std::shared_ptr<ImageAnalyzer> img) {
  std::shared_ptr<GenericDataStore> newData = img->GetData();
  std::shared_ptr<LeagueImageAnalyzer> limg = std::static_pointer_cast<LeagueImageAnalyzer>(img);

  if (limg->GetIsReplayFrame()) {
    return false;
  }

  isDraftPhase = false;
  if (!img->IsValidFrame()) {
    // There are a couple situations where the frame isn't valid
    // 1) The game ended (and we're waiting for the next one to start)
    // 2) The players are currently in the pick/ban stage
    // 3) The players are finished with picks and bans and we're just waiting for the game to start

    bool newDraftPhase = limg->GetIsDraftBan();
    // If we are currently drafting, we want to keep updating our data so we know who the bans and champions are.
    if (!newDraftPhase) {
      // If we were just drafting then this means we are waiting for the game to start.
      // In this case there's nothing to update so we can just return.
      // Of course, if we're already waiting to start, that state should remain the same.
      if (isWaitingToStart || isDraftPhase) {
        isWaitingToStart = true;
        isDraftPhase = false;
        return false;
      }
      
      ++continuousInvalidFrameCount;
      // TODO: Should be configurable
      isMatchOver = (continuousInvalidFrameCount >= GetMatchOverThreshold());

      // We operate under the assumption that the camera is at the losing team's nexus at the end of the match.
      if (isMatchOver) {
        winningTeam = GetTeamFromMapLocation(lastMapPositionX, lastMapPositionY);
        // The winning team isn't the one we're looking at , its' the other team
        winningTeam = GetOtherTeam(winningTeam);
      }

      // Once we determine the match is over, clear our data. If they're ever needed again, we'll have to reanalyze.
      mData = NULL;

      return false;
    } else {
      isDraftPhase = newDraftPhase; // We are now currently drafting!
    }
  } else {
    // We can't depend on us catching the champion switch during the draft phase. So when we enter the game for the first time,
    // clear the data.
    if (isWaitingToStart || isDraftPhase) {
      mData = NULL;
    }

    // In this case, we are no longer waiting for the game to start, waiting for the next game, or in pick/ban
    continuousInvalidFrameCount = 0;
    isMatchOver = false;
    isDraftPhase = false;
    isWaitingToStart = false;

    // Only valid frames have the timestamp on it so only update that here.
    limg->GetMapLocation(lastMapPositionX, lastMapPositionY);
  }

  // Not much to do if this is the first frame. Just do a straight copy.
  if (!mData) {
    mData = newData;
    return true;
  }

  // There's always going to be a time stamp. Check to make sure this time stamp is more recent,
  // otherwise it's probably a replay.
  // IF YOU EVER DEALLOCATE THESE POINTERS I WILL FIND YOU.
  int* curTimeStamp = &std::static_pointer_cast<GenericData<int>>((*newData)["CurrentTime"])->value;
  int* oldTimeStamp = &std::static_pointer_cast<GenericData<int>>((*mData)["CurrentTime"])->value;

  // Need to take time stamp as is. Don't really have a way around it
  // TODO: Make this more accurate somehow? Need to predict values more or less.
  if (*curTimeStamp != -1) {
    *oldTimeStamp = *curTimeStamp;
  }

  // Only in the case where the time stamp is correct, go into details.
  // The only thing we can assume to know are the champion kills and tower kills
  PtrLeagueTeamData blueNew = std::static_pointer_cast<GenericData<PtrLeagueTeamData>>((*newData)["BlueTeam"])->value;
  PtrLeagueTeamData blueOld = std::static_pointer_cast<GenericData<PtrLeagueTeamData>>((*mData)["BlueTeam"])->value;
  blueOld->Update(blueNew, *oldTimeStamp, mDataHistory);

  PtrLeagueTeamData purpleNew = std::static_pointer_cast<GenericData<PtrLeagueTeamData>>((*newData)["PurpleTeam"])->value;
  PtrLeagueTeamData purpleOld = std::static_pointer_cast<GenericData<PtrLeagueTeamData>>((*mData)["PurpleTeam"])->value;
  purpleOld->Update(purpleNew, *oldTimeStamp, mDataHistory);

  // Update Events
  PtrLeagueEvent announceEventNew = std::static_pointer_cast<GenericData<PtrLeagueEvent>>((*newData)["Announcement"])->value;
  PtrLeagueEvent announceEventOld = std::static_pointer_cast<GenericData<PtrLeagueEvent>>((*mData)["Announcement"])->value;
  if (announceEventNew->GetIdentifier() == announceEventOld->GetIdentifier()) {
    announceEventOld->Update(announceEventNew, *curTimeStamp);
  } else {
    announceEventOld->Copy(announceEventNew, *curTimeStamp);
  }

  MapPtrLeagueEvent* minibarEventsNew = &std::static_pointer_cast<GenericData<MapPtrLeagueEvent>>((*newData)["MinibarEvents"])->value;
  MapPtrLeagueEvent* minibarEventsOld = &std::static_pointer_cast<GenericData<MapPtrLeagueEvent>>((*newData)["MinibarEvents"])->value;
  // Iterate through the old events to see which ones to remove
  for (auto& e : *minibarEventsOld) {
    auto it = minibarEventsNew->find(e.first);
    if (it == minibarEventsNew->end()) {
      minibarEventsOld->erase(it);
    }
  }

  for (auto& e : *minibarEventsNew) {
    // Can't find the event? Add the event.
    if (minibarEventsOld->find(e.first) == minibarEventsOld->end()) {
      (*minibarEventsOld)[e.first] = e.second;
    } 
    // If we did find the event, then we should update
    else {
      (*minibarEventsOld)[e.first]->Update(e.second, *curTimeStamp);
    }
  }

  // Save this piece of data in our history. This allows us to fix our data retroactively.
  std::shared_ptr<GenericDataStore> copyData(new GenericDataStore(*newData));
  mDataHistory.push_back(copyData);

  return true;
}

/*
 * Gets the data in JSON form.
 */
std::string LeagueVideoAnalyzer::ParseJSON() {
  if (!mData) {
    return "";
  }

  cJSON* newJson = cJSON_CreateObject();
  // Game status field to let the user know which part of the game we're in}
  if (isMatchOver) {
    cJSON_AddNumberToObject(newJson, "mode", 2);
  } else if (isDraftPhase) {
    cJSON_AddNumberToObject(newJson, "mode", 1);
  } else if (isWaitingToStart) {
    cJSON_AddNumberToObject(newJson, "mode", 5);
  } else {
    cJSON_AddNumberToObject(newJson, "mode", 0);
  }
  
  cJSON* globalObject = cJSON_CreateObject();
  if (DataExists(mData, std::string("CurrentTime"))) {
    cJSON_AddNumberToObject(globalObject, "time", RetrieveData<int>(mData, std::string("CurrentTime")));
  } else {
    cJSON_AddNumberToObject(globalObject, "time", -1);
  }
  // TODO: TEMPORARY UNTIL THESE STATS IS TRACKED
  cJSON_AddNumberToObject(globalObject, "time", 0);
  cJSON_AddNumberToObject(globalObject, "time", 0);
  cJSON_AddItemToObject(newJson, "global", globalObject);

  cJSON* teamArray = cJSON_CreateArray();
  if (DataExists(mData, std::string("BlueTeam"))) {
    cJSON_AddItemToArray(teamArray, RetrieveData<PtrLeagueTeamData>(mData, std::string("BlueTeam"))->CreateJSON());
  } else {
    cJSON_AddItemToArray(teamArray, cJSON_CreateObject());
  }

  if (DataExists(mData, std::string("PurpleTeam"))) {
    cJSON_AddItemToArray(teamArray, RetrieveData<PtrLeagueTeamData>(mData, std::string("PurpleTeam"))->CreateJSON());
  } else {
    cJSON_AddItemToArray(teamArray, cJSON_CreateObject());
  }
  cJSON_AddItemToObject(newJson, "teams", teamArray);

  cJSON* eventsArray = cJSON_CreateArray();
  if (DataExists(mData, std::string("Announcement"))) {
    cJSON_AddItemToArray(newJson, RetrieveData<PtrLeagueEvent>(mData, std::string("Announcement"))->CreateJSON());
  }

  if (DataExists(mData, std::string("MinibarEvents"))) {
    MapPtrLeagueEvent eventMapping = RetrieveData<MapPtrLeagueEvent>(mData, std::string("MinibarEvents"));
    for (auto& e : eventMapping) {
      cJSON_AddItemToArray(eventsArray, e.second->CreateJSON());
    }
  }
  cJSON_AddItemToObject(newJson, "events", eventsArray);

  char* retChar = cJSON_PrintUnformatted(newJson);
  cJSON_Delete(newJson);
  std::string newRet(retChar);
  delete[] retChar;
  return newRet;
}

std::shared_ptr<class ImageAnalyzer> LeagueVideoAnalyzer::CreateImageAnalyzer(std::string& path, const std::string& configPath) {
  return std::shared_ptr<ImageAnalyzer>(new LeagueImageAnalyzer(path, configPath, relevantProperties));
}

/*
 * Use the hint system to speed up the image analysis process by 
 * telling the image analyzer who all the champions are.
 */
void LeagueVideoAnalyzer::PostCreateImageAnalyzer(std::shared_ptr<class ImageAnalyzer> img) {
  if (!mData) {
    return; 
  }

  std::string teams[] = { "BlueTeam", "PurpleTeam" };
  for (auto& team : teams) {
    PtrLeagueTeamData t = RetrieveData<PtrLeagueTeamData>(mData, team);
    for (int i = 0; i < 5; ++i) {
      PtrLeaguePlayerData p = t->players[i];
      std::string hintKey = CreateLeagueChampionHint(i, t->team);
      img->SetHint(hintKey, p->champion);
    }
  }
}

void LeagueVideoAnalyzer::
LoadImagePropertyFile() {

}
