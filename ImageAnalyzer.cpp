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
cv::Mat ImageAnalyzer::FilterImage_Section_Channel_Resize(cv::Mat& inImage, cv::Rect& section, int channel, float threshold, float resX, float resY) {
  cv::Mat newImage = inImage(section);

  // Grab the RBG channel that we want (B = 0, R = 2)
  cv::Mat filterImage(newImage.rows, newImage.cols, CV_8UC1);
  int fromTo[] = { channel, 0 };
  cv::mixChannels(&newImage, 1, &filterImage, 1, fromTo, 1);

  // Apply threshold.
  cv::threshold(filterImage, filterImage, threshold, 255.0, cv::THRESH_BINARY);

  // Tesseract needs big text. TODO: This may need to change depending on the resolution?
  cv::Size newSize;
  cv::resize(filterImage, filterImage, newSize, 2.0, 2.0);
  return filterImage;
}