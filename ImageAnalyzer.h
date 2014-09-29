#pragma once
#ifndef _IMAGEANALYZER_H
#define _IMAGEANALYZER_H
#include "common.h"
#include "opencv2/core.hpp"
#include "tesseract/baseapi.h"
#include "Data.h"
#include <mutex>

/*
 * Takes one frame of the video and analyzes it. 
 * When analysis is finished, we will get a data structure
 * that is filled with all relevant data.
 * 
 * This class MUST be resilient to resolution changes since
 * I am not guaranteed any resolution from the stream. Some streams 
 * are 1080p, some others are 720p. Sad. 
 */
class ImageAnalyzer {
public:
  ImageAnalyzer(IMAGE_PATH_TYPE ImagePath);
  virtual ~ImageAnalyzer();

  // Starts Analysis of the image. 
  // In the future, we'll have multiple subclasses of ImageAnalyzer that will
  // be able to parse various screens. There should be a spectator mode, normal mode, and an LCS mode (LCS
  // should probably be based off of spectator). And looking even further ahead, we can
  // expand this to analyze other game streams as well. This should return false
  // if we are able to extract very little data from this image.
  virtual bool Analyze() = 0;

  // Whether or not this frame is a valid frame or not. 
  // A valid frame would be a frame from the game. 
  virtual bool IsValidFrame() = 0;

  // Whether or not analysis has finished
  bool IsAnalysisFinished() const { return bIsFinished; }

  // Histogram Functions
  static cv::MatND CreateHSHistogram(cv::Mat inImage, int hue_bins, int sat_bins);
  static cv::MatND CreateVHistogram(cv::Mat inImage, int value_bins);

  // Get Data
  std::shared_ptr<GenericDataStore> GetData() { return mData; }

  // Hint System. Only strings can be used here.
  void SetHint(std::string& key, std::string& value) { mHints[key] = value; }

protected:
  // Set to true when we are done analyzing the image
  bool bIsFinished;

  // Path to Image
  std::string mImagePath;

  // Actual Image
  cv::Mat mImage;

  // Holds all the information about this image. 
  std::mutex mDataMutex;
  std::shared_ptr<GenericDataStore> mData;

  // Utility Function to get text.
  std::string GetTextFromImage(cv::Mat& inImage, std::string& language, std::string& whitelist = std::string(""), tesseract::PageSegMode mode = tesseract::PSM_SINGLE_WORD, std::vector<std::string>* keys = NULL, std::vector<std::string>* values = NULL, bool useUserWords = false);

  // Debug Function to show an image
  void ShowImage(cv::Mat& image);
  void ShowImageNoPause(cv::Mat& image, const char* name);

  // Split image into many multiple images. These images are stored in a 2-dimensional array which we receive a pointer to
  void SplitImage(cv::Mat& inImage, int x_dim, int y_dim, cv::Mat** out);

  // Language Identifiers
  static std::string EnglishIdent;
  static std::string LeagueIdent;

  // Generic Functions to analyze a part of an image.
  cv::Mat FilterImage_Section_Grayscale_BasicThreshold_Resize(cv::Mat inImage, cv::Rect& section, double threshold, double resX, double resY);
  cv::Mat FilterImage_Section_Channel_BasicThreshold_Resize(cv::Mat inImage, cv::Rect& section, int channel, double threshold, double resX, double resY);

  cv::Mat FilterImage_Section(cv::Mat inImage, cv::Rect& section);
  cv::Mat FilterImage_Channel(cv::Mat inImage, int channel); 
  cv::Mat FilterImage_2Channel(cv::Mat inImage, int channel1, int channel2, double fillValue);
  cv::Mat FilterImage_BasicThreshold(cv::Mat inImage, double threshold);
  cv::Mat FilterImage_Resize(cv::Mat inImage, double resX, double resY);
  cv::Mat FilterImage_Grayscale(cv::Mat inImage);

  // Template Matching
  double SobelTemplateMatching(cv::Mat templateImage, cv::Mat sourceImage, cv::Vec3b bgColor, cv::Point& matchPoint);

  // Image Hint System. What we do with these hints is totally up to the subclasses.
  std::map<std::string, std::string> mHints;
};

#endif