#include "LeagueSpectatorVideoAnalyzer.h"
#include "LeagueSpectatorImageAnalyzer.h"
#include "Data.h"
#include "LeagueTeamData.h"
#include "LeagueConstants.h"

std::shared_ptr<class ImageAnalyzer> LeagueSpectatorVideoAnalyzer::CreateImageAnalyzer(std::string& path) {
  return std::shared_ptr<ImageAnalyzer>(new LeagueSpectatorImageAnalyzer(path));
}

/*
 * Use the hint system to speed up the image analysis process by 
 * telling the image analyzer who all the champions are.
 */
void LeagueSpectatorVideoAnalyzer::PostCreateImageAnalyzer(std::shared_ptr<class ImageAnalyzer> img) {
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