#include "LeagueLCSImageAnalyzer.h"
#include "WebFrontend.h"
#include "VideoFetcher.h"
#include "common.h"

void test(IMAGE_PATH_TYPE t, IMAGE_FRAME_COUNT_TYPE t1) {

}

int main() {
  std::string path1 = "Images/drafttest/frame44.png";
  std::string path2 = "Images/drafttest/frame65.png";
  std::string path3 = "Images/2014-09-01-league-lcs2/frame28.png";
  std::string path4 = "Images/2014-09-01-league-lcs2/frame29.png";
  std::string path5 = "Images/2014-09-01-league-lcs2/frame30.png";

  //VideoFetcher* vf = new VideoFetcher("drafttest", "twitch.tv/riotgames", test);
  //vf->BeginFetch();

  WebFrontend* wf = new WebFrontend();
  
  //LeagueLCSImageAnalyzer* img = new LeagueLCSImageAnalyzer(path1);
  //img->Analyze();
  /*
  img = new LeagueLCSImageAnalyzer(path2);
  img->Analyze();

  img = new LeagueLCSImageAnalyzer(path3);
  img->Analyze();

  img = new LeagueLCSImageAnalyzer(path4);
  img->Analyze();

  img = new LeagueLCSImageAnalyzer(path5);
  img->Analyze();
  */
  int exit; std::cin >> exit;
  return 0;
}