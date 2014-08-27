#include "ImageAnalyzer.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include <stdlib.h>

// Language Identifiers
std::string ImageAnalyzer::EnglishIdent = "eng";
std::string ImageAnalyzer::LeagueIdent = "lol";

ImageAnalyzer::ImageAnalyzer(IMAGE_PATH_TYPE ImagePath): bIsFinished(false), mImagePath(ImagePath), mData(new GenericDataStore) {
  // Load image immediately
  mImage = cv::imread(mImagePath, cv::IMREAD_UNCHANGED);
}

ImageAnalyzer::~ImageAnalyzer() {
}

void ImageAnalyzer::ShowImage(cv::Mat& image) {
  cv::namedWindow("Window", cv::WINDOW_AUTOSIZE);
  cv::imshow("Window", image);
  cv::waitKey(0);
  cv::destroyWindow("Window");
  std::string path = "test.png";
  cv::imwrite(path, image);
}

std::string ImageAnalyzer::GetTextFromImage(cv::Mat& inImage, std::string& language, std::string& whitelist) {
  // Get the actual text.
  tesseract::TessBaseAPI tessApi;
  char const* baseDir = getenv("TESSDATA_DIR");
  tessApi.Init(baseDir, language.c_str());
  if (!whitelist.empty()) {
    tessApi.SetVariable("tessedit_char_whitelist", whitelist.c_str());
  }
  tessApi.SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
  tessApi.SetImage((uchar*)inImage.data, inImage.cols, inImage.rows, 1, inImage.cols);
  std::string result = tessApi.GetUTF8Text();
  result.erase(std::remove_if(result.begin(), result.end(), isspace), result.end());
  return result;
}

// Generic Function to analyze a part of an image. 
// Will take an image and cut out a section. From that section, we will use ONE channel. Then we will resize the image.
cv::Mat ImageAnalyzer::FilterImage_Section_Channel_BasicThreshold_Resize(cv::Mat inImage, cv::Rect& section, int channel, double threshold, float resX, float resY) {
  cv::Mat newMat = FilterImage_Section(inImage, section);
  newMat = FilterImage_Channel(newMat, channel);
  newMat = FilterImage_BasicThreshold(newMat, threshold);
  newMat = FilterImage_Resize(newMat, resX, resY);
  return newMat;
}

cv::Mat ImageAnalyzer::FilterImage_Section_Grayscale_BasicThreshold_Resize(cv::Mat inImage, cv::Rect& section, double threshold, float resX, float resY) {
  cv::Mat newMat = FilterImage_Section(inImage, section);
  newMat = FilterImage_Grayscale(newMat);
  newMat = FilterImage_BasicThreshold(newMat, threshold);
  newMat = FilterImage_Resize(newMat, resX, resY);
  return newMat;
}

// Basic OpenCV operations on images.
cv::Mat ImageAnalyzer::FilterImage_Section(cv::Mat inImage, cv::Rect& section) {
  return inImage(section);
}

cv::Mat ImageAnalyzer::FilterImage_Channel(cv::Mat inImage, int channel) {
  cv::Mat filterImage(inImage.rows, inImage.cols, CV_8UC1);
  int fromTo[] = { channel, 0 };
  cv::mixChannels(&inImage, 1, &filterImage, 1, fromTo, 1);
  return filterImage;
}

cv::Mat ImageAnalyzer::FilterImage_BasicThreshold(cv::Mat inImage, double threshold) {
  cv::threshold(inImage, inImage, threshold, 255.0, cv::THRESH_BINARY);
  return inImage;
}

cv::Mat ImageAnalyzer::FilterImage_Resize(cv::Mat inImage, float resX, float resY) {
  cv::Size newSize;
  cv::resize(inImage, inImage, newSize, resX, resY);
  return inImage;
}

cv::Mat ImageAnalyzer::FilterImage_Grayscale(cv::Mat inImage) {
  cv::cvtColor(inImage, inImage, cv::COLOR_RGB2GRAY);
  return inImage;
}