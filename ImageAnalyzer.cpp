#include "ImageAnalyzer.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
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

std::string ImageAnalyzer::GetTextFromImage(cv::Mat& inImage, std::string& language, bool bDigitsOnly) {
  // Get the actual text.
  tesseract::TessBaseAPI tessApi;
  char const* baseDir = getenv("TESSDATA_DIR");
  tessApi.Init(baseDir, language.c_str());
  if (bDigitsOnly) {
    tessApi.SetVariable("tessedit_char_whitelist", "0123456789");
  }
  tessApi.SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
  tessApi.SetImage((uchar*)inImage.data, inImage.cols, inImage.rows, 1, inImage.cols);
  std::string result = tessApi.GetUTF8Text();
  result.erase(std::remove_if(result.begin(), result.end(), isspace), result.end());
  return result;
}