#include "LeagueImageAnalyzer.h"

LeagueImageAnalyzer::LeagueImageAnalyzer(IMAGE_PATH_TYPE ImagePath): ImageAnalyzer(ImagePath) {

}

LeagueImageAnalyzer::~LeagueImageAnalyzer() {

}

/*
 * Call functions to get various pieces of data that are of interest.
 * If too many fail [threshold TBD], then fail.
 */
bool LeagueImageAnalyzer::Analyze() {
  bool ret = true;
  if (mImage.empty()) {
    return false;
  }
  
  // Current match time.
  int time = AnalyzeMatchTime();
  std::shared_ptr<GenericData<int>> timeProp(new GenericData<int>(time));
  (*mData)["CurrentTime"] = timeProp;

  // Team Data.
  PtrLeagueTeamData blueTeam = AnalyzeTeamData(ELT_BLUE);
  std::shared_ptr<GenericData<PtrLeagueTeamData>> blueTeamProp(new GenericData<PtrLeagueTeamData>(blueTeam));
  (*mData)["BlueTeam"] = blueTeamProp;

  PtrLeagueTeamData purpleTeam = AnalyzeTeamData(ELT_PURPLE);
  std::shared_ptr<GenericData<PtrLeagueTeamData>> purpleTeamProp(new GenericData<PtrLeagueTeamData>(purpleTeam));
  (*mData)["PurpleTeam"] = purpleTeamProp;

  return ret;
}

/*
 * Get all the information we can about this team (gold ,kills, who's on the team, etc.)
 * It is perfectly fine if some of that information is unavailable. Pass off the duty of
 * identifying invalid values to the user of the data.
 */
PtrLeagueTeamData LeagueImageAnalyzer::AnalyzeTeamData(ELeagueTeams team) {
  PtrLeagueTeamData newTeam(new LeagueTeamData);
  newTeam->team = team;
  newTeam->kills = AnalyzeTeamKills(team);
  newTeam->gold = AnalyzeTeamGold(team);
  newTeam->towerKills = AnalyzeTeamTowerKills(team);

  return newTeam;
}