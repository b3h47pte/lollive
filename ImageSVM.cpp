#include "ImageSVM.h"
#include "FileUtility.h"

ImageSVM::ImageSVM(const std::string& datasetName, bool performTraining):
  datasetName(datasetName), isTraining(performTraining) {
  svmParams.svmType = cv::ml::SVM::C_SVC;
  svmParams.kernelType = cv::ml::SVM::POLY;
  svmParams.gamma = 3;
}

ImageSVM::~ImageSVM() {

}

void ImageSVM::Execute() {
  svm = cv::ml::SVM::create(svmParams);
  if (isTraining) {
    CreateTrainingData();
  } else {
    LoadTrainingData();
  }
}

void ImageSVM::InitializeTrainingDataset(int numImages, int xSize, int ySize) {
  trainingImage.create(numImages, xSize * ySize, CV_32FC3);
  trainingLabels.create(numImages, 1, CV_32S);

  imageX = xSize;
  imageY = ySize;
}

void ImageSVM::SetupImageTrainingData(int imageIndex, cv::Mat image, short label) {
  int count = 0;
  for (int i = 0; i < imageX; ++i) {
    for (int j = 0; j < imageY; ++j) {
      trainingImage.at<float>(imageIndex, count++) = image.at<unsigned char>(i, j);
    }
  }
  trainingLabels.at<short>(imageIndex, 0) = label;
}

void ImageSVM::PerformTraining() {
  cv::Ptr<cv::ml::TrainData> data = cv::ml::TrainData::create(trainingImage, cv::ml::ROW_SAMPLE, trainingLabels);
  svm->trainAuto(data);
  svm->save(GetRelativeFilePath("SVM/" + datasetName));
}

void ImageSVM::LoadTraining() {
  svm = cv::ml::SVM::load<cv::ml::SVM>(GetRelativeFilePath("SVM/" + datasetName));
}

std::string ImageSVM::PredictImage(const cv::Mat& inImage) {
  short label = (short)svm->predict(inImage);
  return ConvertLabelToString(label);
}