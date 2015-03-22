#include "CvWrapper.h"

// Generic Function to analyze a part of an image. 
// Will take an image and cut out a section. From that section, we will use ONE channel. Then we will resize the image.
cv::Mat CvWrapper::FilterImage_Section_Channel_BasicThreshold_Resize(cv::Mat inImage, const cv::Rect& section, int channel, double threshold, double resX, double resY) {
  cv::Mat newMat = FilterImage_Section(inImage, section);
  newMat = FilterImage_Channel(newMat, channel);
  newMat = FilterImage_BasicThreshold(newMat, threshold);
  newMat = FilterImage_Resize(newMat, resX, resY);
  return newMat;
}

cv::Mat CvWrapper::FilterImage_Section_Grayscale_BasicThreshold_Resize(cv::Mat inImage, const cv::Rect& section, double threshold, double resX, double resY) {
  cv::Mat newMat = FilterImage_Section(inImage, section);
  newMat = FilterImage_Grayscale(newMat);
  newMat = FilterImage_BasicThreshold(newMat, threshold);
  newMat = FilterImage_Resize(newMat, resX, resY);
  return newMat;
}

// Basic OpenCV operations on images.
cv::Mat CvWrapper::FilterImage_Section(cv::Mat inImage, const cv::Rect& section) {
  cv::Rect toUseSection = section;
  if (section.x + section.width > inImage.cols) {
    toUseSection.width = inImage.cols - section.x;
  }
  if (section.y + section.height > inImage.rows) {
    toUseSection.height = inImage.rows - section.y;
  }
  return inImage(toUseSection);
}

cv::Mat CvWrapper::FilterImage_Channel(cv::Mat inImage, int channel) {
  cv::Mat filterImage(inImage.rows, inImage.cols, CV_8UC1);
  int fromTo[] = { channel, 0 };
  cv::mixChannels(&inImage, 1, &filterImage, 1, fromTo, 1);
  return filterImage;
}

cv::Mat CvWrapper::FilterImage_2Channel(cv::Mat inImage, int channel1, int channel2, double fillValue) {
  cv::Mat filterImage(inImage.rows, inImage.cols, CV_8UC3, fillValue);
  int fromTo[] = { channel1, channel1, channel2, channel2 };
  cv::mixChannels(&inImage, 1, &filterImage, 1, fromTo, 2);
  return filterImage;
}

cv::Mat CvWrapper::FilterImage_BasicThreshold(cv::Mat inImage, double threshold) {
  cv::Mat newImage;
  cv::threshold(inImage, newImage, threshold, 255.0, cv::THRESH_BINARY);
  return newImage;
}

cv::Mat CvWrapper::FilterImage_Resize(cv::Mat inImage, double resX, double resY) {
  cv::Size newSize;
  cv::Mat newImage;
  cv::resize(inImage, newImage, newSize, resX, resY);
  return newImage;
}

cv::Mat CvWrapper::FilterImage_Grayscale(cv::Mat inImage) {
  cv::Mat newImage;
  cv::cvtColor(inImage, newImage, cv::COLOR_RGB2GRAY);
  return newImage;
}

void CvWrapper::ShowImage(cv::Mat& image) {
  cv::namedWindow("Window", cv::WINDOW_AUTOSIZE);
  cv::imshow("Window", image);
  cv::waitKey(0);
  cv::destroyWindow("Window");
}

void CvWrapper::ShowImageNoPause(cv::Mat& image, const char* name) {
  cv::namedWindow(name, cv::WINDOW_AUTOSIZE);
  cv::imshow(name, image);
}