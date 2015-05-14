#include "TestVideoFetch.h"
#include <fstream>

TestVideoFetch::TestVideoFetch(std::string Id, std::string Url, std::function<void(IMAGE_PATH_TYPE, IMAGE_FRAME_COUNT_TYPE)> Callback): VideoFetcher(Id, Url, Callback) {

}

TestVideoFetch::~TestVideoFetch() {

}

void TestVideoFetch::BeginFetch() {
  std::string streamUrl = GetStreamURL();
  if (!BeginStreamPlayback(streamUrl)) {
    return;
  }
}


std::string TestVideoFetch::GetStreamURL() {
  return mURL;
}

bool TestVideoFetch::BeginStreamPlayback(std::string& streamUrl) {
  // 'streamUrl' should just be a path to images 
  // so the image names should be streamUrl/frameXX.png
  // Since this is just debug, I assume this folder already exists
  bool bFoundInitial = false;
  int frameCount = 0;
  for (int i = 0;; ++i) {
    std::string path = streamUrl + "/frame_" + std::to_string(i) + ".png";
    std::cout << "ANALYZE " << path << std::endl;
    std::ifstream infile(path);
    if (!infile.good()) {
      if (bFoundInitial) {
        break;
      } else {
        continue;
      }
    }
    mCallback(path, frameCount++);
  }

  return true;
}
