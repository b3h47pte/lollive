#include "VideoAnalyzer.h"
#include "ImageAnalyzer.h"

VideoAnalyzer::VideoAnalyzer() :mFrameCount(0), mDataLock(mDataMutex), mData(NULL) {

}

VideoAnalyzer::~VideoAnalyzer() {

}

/*
 * Create an image analyzer and analyze the frame.
 * Leave what happens to the data up to the subclasses.
 */
void VideoAnalyzer::NotifyNewFrame(IMAGE_PATH_TYPE path, IMAGE_FRAME_COUNT_TYPE frame) {
  std::shared_ptr<class ImageAnalyzer> imgAnalyzer = CreateImageAnalyzer(path);
  imgAnalyzer->Analyze();
 
  mDataCV.wait(mDataLock, [&]() {return mFrameCount == frame; });
  StoreData(imgAnalyzer);
  ++mFrameCount;
  mDataJSON = ParseJSON(); // update the JSON because there's no better place to do it than here.
  mDataCV.notify_all();  
}