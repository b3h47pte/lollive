#pragma once

#ifndef _CVWRAPPER_H
#define _CVWRAPPER_H

#include "common.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

class CvWrapper {
public:
  static cv::Mat FilterImage_Section_Grayscale_BasicThreshold_Resize(cv::Mat inImage, const cv::Rect& section, double threshold, double resX, double resY);
  static cv::Mat FilterImage_Section_Channel_BasicThreshold_Resize(cv::Mat inImage, const cv::Rect& section, int channel, double threshold, double resX, double resY);

  static cv::Mat FilterImage_Section(cv::Mat inImage, const cv::Rect& section);
  static cv::Mat FilterImage_Channel(cv::Mat inImage, int channel);
  static cv::Mat FilterImage_2Channel(cv::Mat inImage, int channel1, int channel2, double fillValue);
  static cv::Mat FilterImage_BasicThreshold(cv::Mat inImage, double threshold);
  static cv::Mat FilterImage_Resize(cv::Mat inImage, double resX, double resY);
  static cv::Mat FilterImage_Grayscale(cv::Mat inImage);
private:
};

#endif