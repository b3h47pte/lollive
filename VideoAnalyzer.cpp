#include "VideoAnalyzer.h"
#include "ImageAnalyzer.h"
#include "ConfigManager.h"

VideoAnalyzer::VideoAnalyzer(const std::string& configPath, bool isRemoteConfigPath) :
  mFrameCount(0), mDataLock(mDataMutex), mData(NULL), configPath(configPath) {
    ConfigManager::Get()->LoadExternalConfig(configPath, isRemoteConfigPath);
    LoadImagePropertyFile();
}

VideoAnalyzer::~VideoAnalyzer() {

}

/*
 * Create an image analyzer and analyze the frame.
 * Leave what happens to the data up to the subclasses.
 */
void VideoAnalyzer::NotifyNewFrame(IMAGE_PATH_TYPE path, IMAGE_FRAME_COUNT_TYPE frame) {
  std::shared_ptr<class ImageAnalyzer> imgAnalyzer = CreateImageAnalyzer(path, configPath);
  mDataCV.wait(mDataLock, [&]() {return mFrameCount == frame; });
  PostCreateImageAnalyzer(imgAnalyzer);
  try {
    imgAnalyzer->Analyze();
  } catch (...) {
    std::cerr << "ANALYZE UNCAUGHT EXCEPTION" << std::endl;
    return;
  }

  try {
    if (imgAnalyzer->IsAnalysisFinished()) {
      StoreData(imgAnalyzer);
    }
  } catch (...) {
    std::cerr << "ANALYZE UNCAUGHT EXCEPTION 2" << std::endl;
  }
  mDataJSON = ParseJSON(); // update the JSON because there's no better place to do it than here.
  NotifyAllJsonCallbacks(mDataJSON);
  ++mFrameCount;
  mDataCV.notify_all();  
}

void VideoAnalyzer::AddJsonCallback(std::function<void(const std::string&)> f) {
  jsonCallbacks.push_back(f);
}

void VideoAnalyzer::NotifyAllJsonCallbacks(const std::string& json) {
  for (size_t i = 0; i < jsonCallbacks.size(); ++i) {
    jsonCallbacks[i](json);
  }
}