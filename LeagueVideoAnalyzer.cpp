#include "LeagueVideoAnalyzer.h"
#include "ImageAnalyzer.h"
#include "LeagueTeamData.h"

LeagueVideoAnalyzer::LeagueVideoAnalyzer() {

}

LeagueVideoAnalyzer::~LeagueVideoAnalyzer() {

}

/*
 * From the image, see what properties make sense to update.
 * Update properties into the data store that already exists.
 */
bool LeagueVideoAnalyzer::StoreData(std::shared_ptr<ImageAnalyzer> img) {
  std::shared_ptr<GenericDataStore> newData = img->GetData();

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

  if (*curTimeStamp > *oldTimeStamp) {
    *oldTimeStamp = *curTimeStamp;

    // Only in the case where the time stamp is correct, go into details.
    // The only thing we can assume to know are the champion kills and tower kills
    PtrLeagueTeamData blueNew = std::static_pointer_cast<GenericData<PtrLeagueTeamData>>((*newData)["BlueTeam"])->value;
    PtrLeagueTeamData blueOld = std::static_pointer_cast<GenericData<PtrLeagueTeamData>>((*mData)["BlueTeam"])->value;
    blueOld->Update(blueNew, *oldTimeStamp);

    PtrLeagueTeamData purpleNew = std::static_pointer_cast<GenericData<PtrLeagueTeamData>>((*newData)["PurpleTeam"])->value;
    PtrLeagueTeamData purpleOld = std::static_pointer_cast<GenericData<PtrLeagueTeamData>>((*mData)["PurpleTeam"])->value;
    purpleOld->Update(purpleNew, *oldTimeStamp);

    return true;
  }

  return false;
}