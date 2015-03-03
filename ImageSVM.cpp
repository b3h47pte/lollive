#include "ImageSVM.h"
#include "FileUtility.h"

ImageSVM::ImageSVM(const std::string& datasetName, bool performTraining):
  datasetName(datasetName), isTraining(performTraining) {
  svmParams.svmType = cv::ml::SVM::C_SVC;
  svmParams.kernelType = cv::ml::SVM::POLY;
  svmParams.gamma = 3;
  svmParams.degree = 1.0;
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
  trainingImage.create(numImages, xSize * ySize, CV_32F);
  trainingLabels.create(numImages, 1, CV_32S);

  imageX = xSize;
  imageY = ySize;
}

void ImageSVM::SetupImageTrainingData(int imageIndex, cv::Mat image, int label) {
  cv::Mat inputImage = image.reshape(0, 1);
  trainingImage.row(imageIndex) = inputImage;
  trainingLabels.row(imageIndex).col(0) = label;
}

void ImageSVM::PerformTraining() {
  cv::Ptr<cv::ml::TrainData> data = cv::ml::TrainData::create(trainingImage, cv::ml::ROW_SAMPLE, trainingLabels);
  svm->train(data);
  svm->save("SVM_" + datasetName);
}

void ImageSVM::LoadTraining() {
  svm = cv::ml::SVM::load<cv::ml::SVM>("SVM_" + datasetName);
}

std::string ImageSVM::PredictImage(const cv::Mat& inImage) {
  int label = (int)svm->predict(inImage);
  return ConvertLabelToString(label);
}