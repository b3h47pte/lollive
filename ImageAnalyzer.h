#pragma once
#ifndef _IMAGEANALYZER_H
#define _IMAGEANALYZER_H
#include "common.h"
#include "opencv2/core.hpp"
#include "tesseract/baseapi.h"

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

  // Whether or not analysis has finished
  bool IsAnalysisFinished() const { return bIsFinished; }

protected:
  // Set to true when we are done analyzing the image
  bool bIsFinished;

  // Path to Image
  std::string mImagePath;

  // Actual Image
  cv::Mat mImage;
};

#endif