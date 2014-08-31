#include "VideoAnalyzer.h"
#include "ImageAnalyzer.h"

VideoAnalyzer::VideoAnalyzer(): mData(NULL) {

}

VideoAnalyzer::~VideoAnalyzer() {

}

/*
 * Create an image analyzer and analyze the frame.
 * Leave what happens to the data up to the subclasses.
 */
void VideoAnalyzer::NotifyNewFrame(IMAGE_PATH_TYPE path, IMAGE_TIMESTAMP_TYPE time) {
  std::shared_ptr<class ImageAnalyzer> imgAnalyzer = CreateImageAnalyzer(path);

  try {
    imgAnalyzer->Analyze();
  } catch (...) {
    std::cout << "Unhandled exception in Image Analyzer." << std::endl;
  }
  mDataMutex.lock();
  try {
    StoreData(imgAnalyzer);
  } catch (...) {
    std::cout << "Unhandled exception in Store Data." << std::endl;
  }
  mDataMutex.unlock();
}