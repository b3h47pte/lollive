#pragma once
#ifndef _IMAGESVM_H
#define _IMAGESVM_H
#include "common.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/ml.hpp"

class ImageSVM {
public:
  ImageSVM(const std::string& datasetName, bool performTraining);
  virtual ~ImageSVM();

  std::string PredictImage(const cv::Mat& inImage);

protected:
  // Training Variables
  cv::Mat trainingImage;
  cv::Mat trainingLabels;

  int imageX;
  int imageY;

  // SVM
  cv::ml::SVM::Params svmParams;
  cv::Ptr<cv::ml::SVM> svm;

  // Training SVM
  void InitializeTrainingDataset(int numImages, int xSize, int ySize);
  void SetupImageTrainingData(int imageIndex, cv::Mat image, short label);
  void PerformTraining();

  // Loading SVM
  void LoadTraining();

  virtual std::string ConvertLabelToString(short label) = 0;

private:
  void Execute();
  virtual void LoadTrainingData() = 0;
  virtual void CreateTrainingData() = 0;

  std::string datasetName;
  bool isTraining;
};

#endif
