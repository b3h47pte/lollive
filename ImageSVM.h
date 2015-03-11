#pragma once
#ifndef _IMAGESVM_H
#define _IMAGESVM_H
#include "common.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/features2d.hpp"
#include "svm.h"

class ImageSVM {
public:
  ImageSVM(const std::string& datasetName, bool performTraining);
  virtual ~ImageSVM();
  virtual void Execute();

  std::string PredictImage(const cv::Mat& inImage);

protected:
  int imageX;
  int imageY;

  // SVM
  svm_problem problem;
  svm_parameter svmParams;
  svm_model svm;

  // Training SVM
  void InitializeTrainingDataset(int numImages, int xSize, int ySize);
  void SetupImageTrainingData(int imageIndex, cv::Mat image, int label);
  void PerformTraining();
  virtual void SetupSVMParameters();

  // Spatial Pyramid
  void GenerateSpatialPyramidDataHierarchy(cv::Mat image, std::vector<cv::KeyPoint>& points, std::vector<cv::DMatch>& matches, int maxLevel, svm_node* nodes);
  void GenerateSpatialPyramidData(cv::Mat image, std::vector<cv::KeyPoint>& points, std::vector<cv::DMatch>& matches, int level, svm_node* nodes);
  int TotalHistogramBins(int maxLevel);
  int TotalHistogramBinsLevel(int level);

  // Loading SVM
  void LoadTraining();

  virtual std::string ConvertLabelToString(int label) = 0;

private:
  virtual void LoadTrainingData() = 0;
  virtual void CreateTrainingData() = 0;

  std::string datasetName;
  bool isTraining;

  cv::Ptr<cv::ORB> orb;

  // Bag of Visual Words
  cv::Mat completeFeatureSet; // TRAINING ONLY
  std::vector<cv::Mat> completeImageSet; // TRAINING ONLY
  cv::Mat dictionary;

  int kClusters;
  int maxSpatialPyramidLevel;
};

#endif
