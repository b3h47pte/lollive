#include "ImageSVM.h"
#include "FileUtility.h"

#define REUSE_DICTIONARY_TRAINING 0

void svmNullPrint(const char *s) {}

ImageSVM::ImageSVM(const std::string& datasetName, bool performTraining):
  datasetName(datasetName), isTraining(performTraining), kClusters(200), maxSpatialPyramidLevel(1) {
  svmParams.svm_type = C_SVC;
  svmParams.kernel_type = RBF;
  svmParams.eps = 0.001;
  svmParams.nr_weight = 0;
  svmParams.weight_label = NULL;
  svmParams.weight = NULL;
  svmParams.shrinking = 1;
  svmParams.probability = 0;

  orb = cv::ORB::create();
}

ImageSVM::~ImageSVM() {
  for (int i = 0; i < problem.l; ++i) {
    delete problem.x[i];
  }
  delete problem.x;
  delete problem.y;

  svm_destroy_param(&svmParams);
  svm_free_and_destroy_model(&svm);
  delete orb;
}

void ImageSVM::Execute() {
  svm_set_print_string_function(&svmNullPrint);

  if (isTraining) {
    CreateTrainingData();
    SetupSVMParameters();

    char* error = svm_check_parameter(&problem, &svmParams);
    if (error != NULL) {
      std::cout << "SVM TRAINING ERROR: " << error << std::endl;
      return
    }

    svm = svm_train(&problem, &svmParams);
    svm_save_model("SVM_" + datasetName + ".svm", svm);

  } else {
    LoadTrainingData();
  }
}

void ImageSVM::InitializeTrainingDataset(int numImages, int xSize, int ySize) {
  imageX = xSize;
  imageY = ySize;

  problem.l = numImages;
  problem.y = new double[numImages];
  problem.x = new svm_node*[numImages];
}

void ImageSVM::SetupImageTrainingData(int imageIndex, cv::Mat image, int label) {
  problem.y[imageIndex] = (double)label;

  cv::Mat grayImage;
  cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);

  std::vector<cv::KeyPoint> keypoints;
  cv::Mat features;
  orb->detectAndCompute(grayImage, cv::Mat(), keypoints, features);
  completeFeatureSet.push_back(features);
  completeImageSet.push_back(grayImage);
}

void ImageSVM::PerformTraining() {
  cv::Mat featureClusters;
#if !REUSE_DICTIONARY_TRAINING
  std::cout << "START KMEANS CLUSTERING" << std::endl;
  // Create the bag of visual words dictionary using K-Means clustering on all the features
  cv::Mat floatFeatureSet;
  completeFeatureSet.convertTo(floatFeatureSet, CV_32F);

  cv::Mat featureLabels;
  cv::TermCriteria criteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 10000, 0.0001);
  cv::kmeans(floatFeatureSet, kClusters, featureLabels, criteria, 5, cv::KMEANS_PP_CENTERS, featureClusters);

  std::cout << "FINISH KMEANS CLUSTERING" << std::endl;
  cv::imwrite("DICTIONARY_" + datasetName + ".png", featureClusters);
#else
  cv::imread("DICTIONARY_" + datasetName + ".png", cv::IMREAD_GRAYSCALE).convertTo(featureClusters, CV_32F);
#endif 

  // Now go back through the images and calculate their feature vector histograms to by finding the closest match in the dictionary  
  cv::FlannBasedMatcher matcher;
  matcher.add(featureClusters);
  matcher.train();

  std::cout << "PERFORM NEAREST NEIGHBOR MATCH" << std::endl;
  int totalBins = TotalHistogramBins(maxSpatialPyramidLevel);
  std::cout << "  Total Features: " << totalBins << std::endl;
  std::cout << "  Total Samples: " << problem.l << std::endl;

  for (size_t i = 0; i < completeImageSet.size(); ++i) {
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat features;
    orb->detectAndCompute(completeImageSet[i], cv::Mat(), keypoints, features);

    cv::Mat floatFeatures;
    features.convertTo(floatFeatures, CV_32F);

    std::vector<cv::DMatch> matches;
    matcher.match(floatFeatures, matches);
    
    problem.x[i] = new svm_node[totalBins + 1];
    GenerateSpatialPyramidDataHierarchy(completeImageSet[i], keypoints, matches, maxSpatialPyramidLevel, problem.x[i]);
  }

  std::cout << "FINISH NEAREST NEIGHBOR AND DATA CREATION" << std::endl;  
}

int ImageSVM::TotalHistogramBins(int maxLevel) {
  int total = 0;
  for (int i = 0; i <= maxLevel; ++i) {
    total += TotalHistogramBinsLevel(i);
  }
  return total;
}

int ImageSVM::TotalHistogramBinsLevel(int level) {
  return (int)pow(4, level) * kClusters;
}

void 
ImageSVM::GenerateSpatialPyramidDataHierarchy(cv::Mat image, std::vector<cv::KeyPoint>& points, std::vector<cv::DMatch>& matches, int maxLevel, svm_node* nodes) {
  int totalBins = TotalHistogramBins(maxSpatialPyramidLevel);
  for (int level = 0; level <= maxSpatialPyramidLevel; ++level) {
    svm_node* problemXPointer = nodes + TotalHistogramBins(level - 1);
    GenerateSpatialPyramidData(image, points, matches, level, problemXPointer);
  }

  // Fill in indices and do a final normalization pass
  double total = 0.0;
  for (int j = 0; j < totalBins; ++j) {
    nodes[j].index = j + 1;
    total += nodes[j].value;
  }

  for (int j = 0; j < totalBins; ++j) {
    nodes[j].value /= total;
  }

  nodes[totalBins].index = -1;
}

void
ImageSVM::GenerateSpatialPyramidData(cv::Mat image, std::vector<cv::KeyPoint>& points, std::vector<cv::DMatch>& matches, int level, svm_node* nodes) {
  int bins = TotalHistogramBinsLevel(level);
  int sideCount = (int)pow(2, level);

  for (int i = 0; i < bins; ++i) {
    nodes[i].value = 0.0;
  }

  for (size_t i = 0; i < matches.size(); ++i) {
    int x = (int)(points[i].pt.x / image.cols) * (sideCount - 1);
    int y = (int)(points[i].pt.y / image.rows) * (sideCount - 1);
    int index = y * sideCount + x + matches[i].trainIdx;
    nodes[index].value += 1.0;
  }

  // Normalize each histogram separately
  for (int r = 0; r < sideCount; ++r) {
    for (int c = 0; c < sideCount; ++c) {
      double total = 0.0;
      for (int i = 0; i < kClusters; ++i) {
        int index = r * sideCount + c + i;
        total += nodes[index].value;
      }

      for (int i = 0; i < kClusters; ++i) {
        int index = r * sideCount + c + i;
        nodes[index].value /= total;
      }
    }
  }
}

void ImageSVM::LoadTraining() {
  cv::imread("DICTIONARY_" + datasetName + ".png", cv::IMREAD_GRAYSCALE).convertTo(dictionary, CV_32F);
  svm = svm_load_model("SVM_" + datasetName + ".svm");
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

  double maxAccuracy = 0.0;
  double bestC = 0.0;
  double bestG = 0.0;
  for (int c = -5; c <= 15; c += 2) {
#pragma omp parallel for
    for (int g = 3; g >= -15; g -= 2) {
      svm_parameter tempParam = svmParams;
      tempParam.C = pow(2, c);
      tempParam.gamma = pow(2, g);

      double* target = new double[problem.l];
      svm_cross_validation(&problem, &tempParam, 5, target);
      int accurateCount = 0;
      for (int i = 0; i < problem.l; ++i) {
        if (target[i] == problem.y[i]) {
          ++accurateCount;
        }
      }
      delete target;

      double accuracy = (double)accurateCount / problem.l;
      std::cout << "Cross Validation Step: " << accuracy << " " << c << " " << g << std::endl;
#pragma omp critical
      if (accuracy > maxAccuracy) {
        maxAccuracy = accuracy;
        bestC = tempParam.C;
        bestG = tempParam.gamma;
      }
    }
  }

  std::cout << "FINISH CROSS VALIDATION: " << std::endl;
  std::cout << "  Accuracy: " << maxAccuracy << std::endl;
  std::cout << "  Best C: " << bestC << std::endl;
  std::cout << "  Best G: " << bestG << std::endl;
  svmParams.C = bestC;
  svmParams.gamma = bestG;
}