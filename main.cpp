#include "common.h"
#include "LeagueImageAnalyzer.h"
#include "WebFrontend.h"
#include "VideoFetcher.h"
#include "LeagueItemDatabase.h"
#include "LeagueChampionDatabase.h"
#include "PropertyManager.h"
#include "ConfigManager.h"
#include "FileUtility.h"

#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/superres.hpp"

void test(IMAGE_PATH_TYPE t, IMAGE_FRAME_COUNT_TYPE t1) {
  std::cout << "CREATE IMAGE: " << t << std::endl;
}

int main() {
  std::string path1 = "Images/c9-vs-gv/frame0.png";
  std::string path2 = "Images/tl-vs-tsm/draft1.png";
  std::string path3 = "Images/fnc-vs-gmb/frame0.png";
  std::string path4 = "Images/2014-09-01-league-lcs2/frame29.png";
  std::string path5 = "Images/2014-09-01-league-lcs2/frame30.png";
  std::string path6 = "Images/tip-vs-t8/frame129.png";

//  VideoFetcher* vf = new VideoFetcher("fnc-vs-gmb", "http://media-cdn.twitch.tv/store156.media103/archives/2015-1-30/live_user_riotgames_1422640803.flv", test);
//  VideoFetcher* vf = new VideoFetcher("tsm-vs-tl", "twitch.tv/riotgames", test);
//  VideoFetcher* vf = new VideoFetcher("imaqtpie", "twitch.tv/imaqtpie", test);
//  vf->BeginFetch();
//
  //WebFrontend* wf = new WebFrontend();


  //std::shared_ptr<const LeagueItemDatabase> db = LeagueItemDatabase::Get();
  ConfigManager::Get()->LoadExternalConfig(GetRelativeFilePath("Config/League/lcs.ini"), false);
  LeagueImageAnalyzer* img = new LeagueImageAnalyzer(path1, GetRelativeFilePath("Config/League/lcs.ini"), PropertyManager::Get()->GetProperties(EGI_League));
  img->Analyze();
  int exit; std::cin >> exit;
  return 0;
}
