#include "ImageAnalyzer.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"

ImageAnalyzer::ImageAnalyzer(IMAGE_PATH_TYPE ImagePath): bIsFinished(false), mImagePath(ImagePath), mData(new GenericDataStore) {
  // Load image immediately
  mImage = cv::imread(mImagePath, cv::IMREAD_UNCHANGED);
}

ImageAnalyzer::~ImageAnalyzer() {
}