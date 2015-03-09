#include "ImageSVM.h"
#include "FileUtility.h"

#define SAVE_LIBSVM_DATA 0
#if SAVE_LIBSVM_DATA
#include <fstream>
#endif

ImageSVM::ImageSVM(const std::string& datasetName, bool performTraining):
  datasetName(datasetName), isTraining(performTraining) {
  svmParams.svmType = cv::ml::SVM::C_SVC;
  svmParams.kernelType = cv::ml::SVM::LINEAR;
  svmParams.C = 10.0;

}

ImageSVM::~ImageSVM() {

}

void ImageSVM::Execute() {
  SetupSVMParameters();
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
  cv::Mat inputGray;
  cv::cvtColor(image, inputGray, cv::COLOR_BGR2GRAY);

  cv::Mat inputImage;
  inputGray.reshape(1, 1).convertTo(inputImage, CV_32F);
  inputImage.row(0).copyTo(trainingImage.row(imageIndex));
  trainingLabels.row(imageIndex).col(0) = label;
}

void ImageSVM::PerformTraining() {
  cv::Ptr<cv::ml::TrainData> data = cv::ml::TrainData::create(trainingImage, cv::ml::ROW_SAMPLE, trainingLabels);
  svm->train(data);
  svm->save("SVM_" + datasetName + ".xml");

#if SAVE_LIBSVM_DATA
  // Save data in LibSVM format so that I can do cross validation externally (OpenCV SVM sucks...)
  std::ofstream libsvmFile;
  libsvmFile.open("LIBSVM_" + datasetName + ".data");
  for (int j = 0; j < trainingImage.rows; ++j) {
    int label = trainingLabels.at<int>(j, 0);
    libsvmFile << label;
    for (int i = 0; i < trainingImage.cols; ++i)  {
      libsvmFile << " " << (i + 1) << ":" << trainingImage.at<float>(j, i);
    }
    libsvmFile << std::endl;
  }
  libsvmFile.close();
#endif
}

void ImageSVM::LoadTraining() {
  svm = cv::ml::SVM::load<cv::ml::SVM>(GetRelativeFilePath("SVM_" + datasetName + ".xml", true));
}

std::string ImageSVM::PredictImage(const cv::Mat& inImage) {
  cv::Mat query;
  cv::Mat imageGray;
  cv::cvtColor(inImage, imageGray, cv::COLOR_BGR2GRAY);

  imageGray.reshape(1, 1).convertTo(query, CV_32F);
  int label = (int)svm->predict(query);

  return ConvertLabelToString(label);
}