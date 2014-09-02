#include "LeagueImageAnalyzer.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/superres.hpp"
#include <ctime>

LeagueImageAnalyzer::LeagueImageAnalyzer(IMAGE_PATH_TYPE ImagePath): ImageAnalyzer(ImagePath) {
  ChampionDatabase = LeagueChampionDatabase::Get();

  bIs1080p = (mImage.cols == 1920 && mImage.rows == 1080);

  // Everything is much better in 1080p
  cv::resize(mImage, mImage, cv::Size(1920, 1080), 0.0, 0.0, cv::INTER_LANCZOS4);
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
  
  //std::cout << "LoL Frame Data: " << std::endl;
  //std::clock_t begin = std::clock();

  // Current match time.
  int time = AnalyzeMatchTime();
  std::shared_ptr<GenericData<int>> timeProp(new GenericData<int>(time));
  (*mData)["CurrentTime"] = timeProp;
  //std::cout << "Current Time: " << time << std::endl;

  // Team Data.
  PtrLeagueTeamData blueTeam = AnalyzeTeamData(ELT_BLUE);
  std::shared_ptr<GenericData<PtrLeagueTeamData>> blueTeamProp(new GenericData<PtrLeagueTeamData>(blueTeam));
  (*mData)["BlueTeam"] = blueTeamProp;
  //blueTeam->Print();

  PtrLeagueTeamData purpleTeam = AnalyzeTeamData(ELT_PURPLE);
  std::shared_ptr<GenericData<PtrLeagueTeamData>> purpleTeamProp(new GenericData<PtrLeagueTeamData>(purpleTeam));
  (*mData)["PurpleTeam"] = purpleTeamProp;
  //purpleTeam->Print();

  //std::clock_t end = std::clock();
  //double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
  //std::cout << "Elapsed Seconds: " << elapsed_secs << std::endl;

  bIsFinished = true;
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
  for (uint i = 0; i < 5; ++i) {
    newTeam->players[i] = AnalyzePlayerData(i, team);
  }
  return newTeam;
}

/*
 * Gets all the information we can gather about this player. Obviously the index is kind of vague
 * so it is up to subclasses to determine what this index should mean. But we can guarantee that 
 * we will call AnalyzePlayerData with indexes 0 to 4.
 */
PtrLeaguePlayerData LeagueImageAnalyzer::AnalyzePlayerData(uint idx, ELeagueTeams team) {
  PtrLeaguePlayerData newPlayer(new LeaguePlayerData);
  newPlayer->team = team;
  newPlayer->playerIdx = idx;
  newPlayer->champion = AnalyzePlayerChampion(idx, team, &newPlayer->isDead, &newPlayer->isLowHealth, &newPlayer->level);
  newPlayer->name = AnalyzePlayerName(idx, team);
  AnalyzePlayerScore(idx, team, &newPlayer->kills, &newPlayer->deaths, &newPlayer->assists, &newPlayer->cs);
  return newPlayer;
}