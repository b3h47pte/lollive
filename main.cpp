#include "common.h"
#include "LeagueImageAnalyzer.h"
#include "WebFrontend.h"
#include "VideoFetcher.h"
#include "LeagueItemDatabase.h"
#include "PropertyManager.h"

void test(IMAGE_PATH_TYPE t, IMAGE_FRAME_COUNT_TYPE t1) {
  std::cout << "CREATE IMAGE: " << t << std::endl;
}

int main() {
  std::string path1 = "Images/c9-vs-gv/frame100.png";
  std::string path2 = "Images/anc/Screen03.png";
  std::string path3 = "Images/2014-09-01-league-lcs2/frame28.png";
  std::string path4 = "Images/2014-09-01-league-lcs2/frame29.png";
  std::string path5 = "Images/2014-09-01-league-lcs2/frame30.png";

// VideoFetcher* vf = new VideoFetcher("c9-vs-gv", "http://media-cdn.twitch.tv/store69.media57/archives/2015-1-25/live_user_riotgames_1422230388.flv", test);
//  VideoFetcher* vf = new VideoFetcher("tsm-vs-tl", "twitch.tv/riotgames", test);
  VideoFetcher* vf = new VideoFetcher("imaqtpie", "twitch.tv/imaqtpie", test);
  vf->BeginFetch();
//
  //WebFrontend* wf = new WebFrontend();

  //std::shared_ptr<const LeagueItemDatabase> db = LeagueItemDatabase::Get();
 
//  LeagueImageAnalyzer* img = new LeagueImageAnalyzer(path1, "Config/League/lcs.ini", PropertyManager::Get()->GetProperties(EGI_League));
//  img->Analyze();
  int exit; std::cin >> exit;
  return 0;
}
