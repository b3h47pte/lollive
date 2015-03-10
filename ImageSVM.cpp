#include "ImageSVM.h"
#include "FileUtility.h"

#define USE_CROSS_VALIDATION 1
#define SAVE_LIBSVM_DATA 0
#if SAVE_LIBSVM_DATA
#include <fstream>
#endif

void svmNullPrint(const char *s) {}

ImageSVM::ImageSVM(const std::string& datasetName, bool performTraining):
  svm(NULL), currentProblem(NULL), svmParams(NULL), datasetName(datasetName), isTraining(performTraining) {
}

ImageSVM::~ImageSVM() {
  free_and_destroy_model(&svm);
  destroy_param(svmParams);

  delete currentProblem->x;
  delete currentProblem->y;
  delete currentProblem;
}

void ImageSVM::Execute() {
  set_print_string_function(&svmNullPrint);
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
  // Construct Problem
  currentProblem = new problem;
  currentProblem->l = trainingImage.rows;
  currentProblem->n = trainingImage.cols + 1;
  currentProblem->bias = 1.0;

  // Simultaneously copy over the data and scale it. Benefit of having color data is I can just scale 0-255 to 0-1.
  currentProblem->x = new feature_node*[currentProblem->l];
  currentProblem->y = new double[currentProblem->l];
  for (int r = 0; r < trainingImage.rows; ++r) {
    currentProblem->y[r] = trainingLabels.at<int>(r, 0);

    // Count number of non-zero elements
    int totalNonzero = 0;
    std::vector<int> nonzeroIdx;
    for (int c = 0; c < trainingImage.cols; ++c) {
      if (trainingImage.at<float>(r, c) > 1e-6) {
        ++totalNonzero;
        nonzeroIdx.push_back(c);
      }
    }

    // Add 1 for the bias, add another for the -1 end of data indicator.
    currentProblem->x[r] = new feature_node[totalNonzero + 2];
    for (size_t c = 0; c < nonzeroIdx.size(); ++c) {
      int index = nonzeroIdx[c];
      currentProblem->x[r][c].index = index + 1;
      currentProblem->x[r][c].value = trainingImage.row(r).at<float>(index) / 255.0f;
    }
    currentProblem->x[r][totalNonzero].index = trainingImage.cols + 1;
    currentProblem->x[r][totalNonzero].value = currentProblem->bias;
    currentProblem->x[r][totalNonzero + 1].index = -1;
  }

  SetupSVMParameters();

  const char* error = check_parameter(currentProblem, svmParams);
  if (error != NULL) {
    std::cout << "SVM ERROR: " << error << std::endl;
    return;
  }

  svm = train(currentProblem, svmParams);
  if (save_model(("SVM_" + datasetName + ".svm").c_str(), svm) != 0) {
    std::cout << "SVM ERROR: Could not save." << std::endl;
  }

#if SAVE_LIBSVM_DATA
  // Save data in LibSVM format so that I can do cross validation externally (OpenCV SVM sucks...)
  std::ofstream libsvmFile;
  libsvmFile.open("LIBSVM_" + datasetName + ".data");
  for (int j = 0; j < trainingImage.rows; ++j) {
    int label = trainingLabels.at<int>(j, 0);
    libsvmFile << label;
    for (int i = 0; i < trainingImage.cols; ++i)  {
      libsvmFile << " " << (i + 1) << ":" << trainingImage.at<float>(j, i) / 255.0f;
    }
    libsvmFile << std::endl;
  }
  libsvmFile.close();
#endif
}

void ImageSVM::LoadTraining() {
}

std::string ImageSVM::PredictImage(const cv::Mat& inImage) {
  cv::Mat query;
  cv::Mat imageGray;
  cv::cvtColor(inImage, imageGray, cv::COLOR_BGR2GRAY);

  imageGray.reshape(1, 1).convertTo(query, CV_32F);
  int label = 0;

  return ConvertLabelToString(label);
}

void ImageSVM::SetupSVMParameters() {
  // Perform Cross-Validation to find the best parameters for the model
  svmParams = new parameter;
  svmParams->solver_type = L2R_L2LOSS_SVC;
  svmParams->C = 1;
  svmParams->eps = 0.01;
  svmParams->p = 0.1;
  svmParams->nr_weight = 0;
  svmParams->weight_label = NULL;
  svmParams->weight = NULL;

  double maxC = 0.0;
#if USE_CROSS_VALIDATION
  // Cross validate the problem's bias and the parameters C and EPS.
  double maxAccuracy = 0.0;
  double* target = new double[currentProblem->l];
  for (int c = -5; c < 5; c += 1) {
    svmParams->C = pow(2, (double)c);

    cross_validation(currentProblem, svmParams, 5, target);
    int accurate_count = 0;
    for (int i = 0; i < currentProblem->l; ++i) {
      if ((int)target[i] == (int)currentProblem->y[i]) {
        ++accurate_count;
      }
    }
    double accuracy = (double)accurate_count / currentProblem->l;
    std::cout << "ACCURACY: " << accuracy << " " << svmParams->C << std::endl;
    if (accuracy > maxAccuracy) {
      maxAccuracy = accuracy;
      maxC = c;
    }
  }
  delete target;
#else
#endif
  svmParams->C = pow(2, maxC);
}