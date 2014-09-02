#include "ImageAnalyzer.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include <stdlib.h>
#include <assert.h>
#include <locale>

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
  cv::imwrite("test.png", image);
  cv::waitKey(0);
  cv::destroyWindow("Window");
}

void ImageAnalyzer::ShowImageNoPause(cv::Mat& image, const char* name) {
  cv::namedWindow(name, cv::WINDOW_AUTOSIZE);
  cv::imshow(name, image);
}

std::string ImageAnalyzer::GetTextFromImage(cv::Mat& inImage, std::string& language, std::string& whitelist, tesseract::PageSegMode mode, std::vector<std::string>* keys, std::vector<std::string>* values) {
  // Get the actual text.
  tesseract::TessBaseAPI tessApi;
  char const* baseDir = getenv("TESSDATA_DIR");

  // Copy keys and values
  GenericVector<STRING> gKeys;
  GenericVector<STRING> gValues;
  if (keys && values) {
    for (auto& s : *keys) {
      gKeys.push_back(STRING(s.c_str()));
    }
    for (auto& s : *values) {
      gValues.push_back(STRING(s.c_str()));
    }
  }
  gKeys.push_back(STRING("user_words_suffix"));
  gValues.push_back(STRING("user-words"));

  tessApi.Init(baseDir, language.c_str(), tesseract::OEM_DEFAULT, NULL, 0, &gKeys, &gValues, false);
  if (!whitelist.empty()) {
    tessApi.SetVariable("tessedit_char_whitelist", whitelist.c_str());
  }
  tessApi.SetPageSegMode(mode);
  tessApi.SetImage((uchar*)inImage.data, inImage.cols, inImage.rows, 1, inImage.cols);
  std::string result = tessApi.GetUTF8Text();
  result.erase(std::remove_if(result.begin(), result.end(), iswspace), result.end());
  return result;
}

// Create a histogram for the given image.
// TODO: Cleanup this and the other create HS histogram functions.
cv::MatND ImageAnalyzer::CreateHSHistogram(cv::Mat inImage, int hue_bins, int sat_bins) {
  cv::Mat hsvImage;
  cv::cvtColor(inImage, hsvImage, cv::COLOR_BGR2HSV);

  // Histogram properties
  // Source: http://docs.opencv.org/doc/tutorials/imgproc/histograms/histogram_comparison/histogram_comparison.html
  int histSize[] = { hue_bins, sat_bins };

  float h_ranges[] = { 0, 180 };
  float s_ranges[] = { 0, 256 };

  const float* ranges[] = { h_ranges, s_ranges };

  int channels[] = { 0, 1 };

  cv::MatND retHist;
  cv::calcHist(&hsvImage, 1, channels, cv::Mat(), retHist, 2, histSize, ranges, true, false);
  cv::normalize(retHist, retHist, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
  return retHist;
}

cv::MatND ImageAnalyzer::CreateVHistogram(cv::Mat inImage, int value_bins) {
  cv::Mat hsvImage;
  cv::cvtColor(inImage, hsvImage, cv::COLOR_BGR2HSV);

  // Histogram properties
  // Source: http://docs.opencv.org/doc/tutorials/imgproc/histograms/histogram_comparison/histogram_comparison.html
  int histSize[] = { value_bins };

  float v_ranges[] = { 0, 256 };

  const float* ranges[] = { v_ranges };

  int channels[] = { 2 };

  cv::MatND retHist;
  cv::calcHist(&hsvImage, 1, channels, cv::Mat(), retHist, 1, histSize, ranges, true, false);
  cv::normalize(retHist, retHist, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
  return retHist;
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

cv::Mat ImageAnalyzer::FilterImage_2Channel(cv::Mat inImage, int channel1, int channel2, double fillValue) {
  cv::Mat filterImage (inImage.rows, inImage.cols, CV_8UC3, fillValue);
  int fromTo[] = { channel1, channel1, channel2, channel2 };
  cv::mixChannels(&inImage, 1, &filterImage, 1, fromTo, 2);
  return filterImage;
}

cv::Mat ImageAnalyzer::FilterImage_BasicThreshold(cv::Mat inImage, double threshold) {
  cv::Mat newImage;
  cv::threshold(inImage, newImage, threshold, 255.0, cv::THRESH_BINARY);
  return newImage;
}

cv::Mat ImageAnalyzer::FilterImage_Resize(cv::Mat inImage, float resX, float resY) {
  cv::Size newSize;
  cv::Mat newImage;
  cv::resize(inImage, newImage, newSize, resX, resY);
  return newImage;
}

cv::Mat ImageAnalyzer::FilterImage_Grayscale(cv::Mat inImage) {
  cv::Mat newImage;
  cv::cvtColor(inImage, newImage, cv::COLOR_RGB2GRAY);
  return newImage;
}

// Split an Image into many parts. 
// It does its best to keep the same number of pixels in each section but once you reach the edge
// it will resize as appropriate. No matter what, there won't be duplicated pixels.
void ImageAnalyzer::SplitImage(cv::Mat& inImage, int x_dim, int y_dim, cv::Mat** out) {
  cv::Mat* res = *out;
  assert(res != NULL);
  int x_pos = 0;
  int y_pos = 0;
  bool y_perf = inImage.rows % y_dim == 0;
  bool x_perf = inImage.cols % x_dim == 0;
  for (int y = 0; y < y_dim; ++y) {
    x_pos = 0;
    int y_width = inImage.rows / y_dim + (y_perf ? 0 : 1);
    if (y == y_dim - 1 && inImage.rows % y_dim != 0) {
      y_width -= (y_width * y_dim - inImage.rows);
    }

    for (int x = 0; x < x_dim; ++x) {
      int x_width = inImage.cols / x_dim + (x_perf ? 0 : 1);
      if (x == x_dim - 1 && inImage.cols % x_dim != 0) {
        x_width -= (x_width * x_dim - inImage.cols);
      }

      res[y * y_dim + x] = FilterImage_Section(inImage, cv::Rect(x_pos, y_pos, x_width, y_width));
      x_pos += x_width;
    }
    y_pos += y_width;
  }
  *out = res;
}