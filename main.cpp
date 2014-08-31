#include "LeagueLCSImageAnalyzer.h"
#include "WebFrontend.h"
#include "VideoFetcher.h"
#include "common.h"

void test(IMAGE_PATH_TYPE t, IMAGE_FRAME_COUNT_TYPE t1) {

}

int main() {
  std::string path = "Images/twitch/frame1.png";

  //VideoFetcher* vf = new VideoFetcher("tsmVSlmq", "twitch.tv/riotgames", test);
  //vf->BeginFetch();

  WebFrontend* wf = new WebFrontend();
  
  //LeagueLCSImageAnalyzer* img = new LeagueLCSImageAnalyzer(path);
  //img->Analyze();

  int exit; std::cin >> exit;
  return 0;
}