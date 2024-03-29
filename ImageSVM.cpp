#include "ImageSVM.h"
#include "FileUtility.h"
#include "CvWrapper.h"
#include "CommandParser.h"
#include <fstream>
#include <algorithm>
#include <assert.h>

#define REUSE_DICTIONARY_TRAINING 0
#define USE_SIFT 0
#define USE_SURF 0

#define VERIFY_SVM_PARAM_EQ(x, y, var) if(x->var != y->var) { std::cout << "  PARAM UNEQ -- " << x->var << ": " << y->var << " " << #var << std::endl;}
#define VERIFY_SVM_PARAM_APPROX_EQ(x, y, var) if(fabs(x->var - y->var) > 1e-6 || (isnan(x->var) ^ isnan(y->var))) { std::cout << "  PARAM UNEQ -- " << x->var << ": " << y->var << " " << #var << std::endl;}

void svmNullPrint(const char *s) {}

ImageSVM::ImageSVM(const std::string& datasetName, bool performTraining):
  datasetName(datasetName), isTraining(performTraining), kClusters(300), maxSpatialPyramidLevel(1), imageType(TIT_GRAYSCALE) {
  svmParams.svm_type = C_SVC;
  svmParams.kernel_type = RBF;
  svmParams.eps = 0.001;
  svmParams.nr_weight = 0;
  svmParams.weight_label = NULL;
  svmParams.weight = NULL;
  svmParams.shrinking = 1;
  svmParams.probability = 0;
  svmParams.degree = 1;
  svm = NULL;

  problem.x = NULL;
  problem.y = NULL;
}

void ImageSVM::ParseOptions(CommandParser* parser) {
  // SVM Type - C_SVC only
  std::string parsedSvmType = parser->GetValue("svm_type", "svc");
  std::transform(parsedSvmType.begin(), parsedSvmType.end(), parsedSvmType.begin(), ::toupper);
  if (parsedSvmType == "SVC") {
    svmParams.svm_type = C_SVC;
  }

  // Kernel Type -- RBF, LINEAR
  std::string parsedKernelType = parser->GetValue("kernel_type", "rbf");
  std::transform(parsedKernelType.begin(), parsedKernelType.end(), parsedKernelType.begin(), ::toupper);
  if (parsedKernelType == "RBF") {
    svmParams.kernel_type = RBF;
  } else if (parsedKernelType == "LINEAR") {
    svmParams.kernel_type = LINEAR;
  }

  // K (Number Clusters)
  kClusters = parser->GetIntValue("clusters", 300);

  // Max Spatial Pyramid Level
  maxSpatialPyramidLevel = parser->GetIntValue("spatial", 1);

  // Image Type
  imageType = (TRAINER_IMAGE_TYPE)parser->GetIntValue("image", 0);
}

void ImageSVM::CreateOrb() {
#if USE_SIFT
#elif USE_SURF
#else
  featureDetector = cv::ORB::create();
#endif
}

ImageSVM::~ImageSVM() {
  if (svm) {
    svm_free_and_destroy_model(&svm);
  }
  svm_destroy_param(&svmParams);

  if (problem.x) {
    for (int i = 0; i < problem.l; ++i) {
      delete[] problem.x[i];
    }
    delete[] problem.x;
  }

  if (problem.y) {
    delete[] problem.y;
  }
}

void ImageSVM::SaveAuxiliaryData() {
  // Save K-Clusters, Max Spatial Pyramid Level, and Image Type
  std::ofstream outFile(CreateAuxiliaryFilename());
  outFile << kClusters << std::endl;
  outFile << maxSpatialPyramidLevel << std::endl;
  outFile << (int)imageType << std::endl;
  outFile.close();
}

void ImageSVM::LoadAuxiliaryData() {
  std::ifstream inFile(CreateAuxiliaryFilename());
  inFile >> kClusters;
  inFile >> maxSpatialPyramidLevel;

  int tmpImageType = 0;
  inFile >> tmpImageType;
  imageType = (TRAINER_IMAGE_TYPE)tmpImageType;

  inFile.close();
}

cv::Mat ImageSVM::PreprocessImage(cv::Mat inImage) {
  assert(inImage.channels() == 1);
  
  cv::Mat typedImage;
  inImage.convertTo(typedImage, CV_32F);

  cv::Mat logImage;
  cv::GaussianBlur(typedImage, logImage, cv::Size(3, 3), 1.0);
  cv::Laplacian(logImage, logImage, CV_32F, 3);
  
  double tmax = 0.0;
  double tmin = 0.0;
  cv::minMaxLoc(logImage, &tmin, &tmax);
  for (int c = 0; c < logImage.cols; ++c) {
    for (int r = 0; r < logImage.rows; ++r) {
      logImage.at<float>(r, c) = (logImage.at<float>(r, c) - (float)tmin) * 255.f / ((float)tmax - (float)tmin);
    }
  }
  logImage.convertTo(logImage, CV_32F);

  cv::Mat retImage;
  cv::add(typedImage, logImage, retImage);
  cv::minMaxLoc(retImage, &tmin, &tmax);
  for (int c = 0; c < retImage.cols; ++c) {
    for (int r = 0; r < retImage.rows; ++r) {
      retImage.at<float>(r, c) = (retImage.at<float>(r, c) - (float)tmin) * 255.f / ((float)tmax - (float)tmin);
    }
  }
  retImage.convertTo(retImage, CV_8U);

  CvWrapper::ShowImage(inImage);

  cv::Mat tmpImage;
  logImage.convertTo(tmpImage, CV_8U);
  CvWrapper::ShowImage(tmpImage);

  CvWrapper::ShowImage(retImage);

  return retImage;
}

void ImageSVM::Execute() {
  CreateOrb();

#ifdef NDEBUG
  svm_set_print_string_function(&svmNullPrint);
#endif

  LoadLabelMapping();
  if (isTraining) {
    dictionary.resize(GetTotalChannels());
    completeFeatureSet.resize(GetTotalChannels());

    CreateTrainingData();
    SetupSVMParameters();

    const char* error = svm_check_parameter(&problem, &svmParams);
    if (error != NULL) {
      std::cout << "SVM TRAINING ERROR: " << error << std::endl;
      return;
    }

    svm = svm_train(&problem, &svmParams);
    if (svm_save_model(CreateSVMName().c_str(), svm)) {
      std::cout << "SVM SAVE MODEL ERROR" << std::endl;
    }

    SaveAuxiliaryData();

    std::cout << "ORIGINAL MODEL VERIFICATION: " << std::endl;
    PerformPostTrainingVerification();
    std::cout << "LOAD FROM FILE MODEL VERIFICATION: " << std::endl;
    svm_model* oldModel = svm;
    svm = svm_load_model(CreateSVMName().c_str());
    PerformPostTrainingVerification();

    // Model Verification
    VERIFY_SVM_PARAM_EQ(svm, oldModel, param.svm_type);
    VERIFY_SVM_PARAM_EQ(svm, oldModel, param.kernel_type);
    VERIFY_SVM_PARAM_EQ(svm, oldModel, nr_class);
    VERIFY_SVM_PARAM_EQ(svm, oldModel, l);
    if (svm->nr_class == oldModel->nr_class) {
      for (int i = 0; i < svm->nr_class * (svm->nr_class - 1) / 2; ++i) {
        VERIFY_SVM_PARAM_APPROX_EQ(svm, oldModel, rho[i]);
      }

      for (int i = 0; i < svm->nr_class; ++i) {
        VERIFY_SVM_PARAM_EQ(svm, oldModel, label[i]);
      }

      for (int i = 0; i < svm->nr_class; ++i) {
        VERIFY_SVM_PARAM_EQ(svm, oldModel, nSV[i]);
      }

      if (svm->l == oldModel->l) {
        for (int i = 0; i < svm->l; ++i) {
          for (int j = 0; j < svm->nr_class - 1; ++j) {
            VERIFY_SVM_PARAM_APPROX_EQ(svm, oldModel, sv_coef[j][i]);
          }
        }
      }
    }

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

  for (int i = 0; i < GetTotalChannels(); ++i) {
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat features;
    BatchComputeImageFeatures(image, keypoints, features, i);

    completeFeatureSet[i].push_back(features);
  }
  completeImageSet.push_back(image);
}

void ImageSVM::BatchComputeImageFeatures(cv::Mat image, std::vector<cv::KeyPoint>& keypoints, cv::Mat& features, int channel) {
  if (imageType == TIT_GRAYSCALE) {
    cv::Mat grayImage = CvWrapper::FilterImage_Grayscale(image);
    cv::Mat sharpenedImage = PreprocessImage(grayImage);
    ComputeImageFeatures(sharpenedImage, keypoints, features);
  } else {
    cv::Mat channeledImage;
    if (imageType == TIT_RGB) {
      channeledImage = image;
    } else if (imageType == TIT_HSV) {
      cv::cvtColor(image, channeledImage, cv::COLOR_BGR2HSV_FULL);
    }

    for (int i = 0; i < 3; ++i) {
      if (channel != -1 && i != channel) continue;
      std::vector<cv::KeyPoint> channelKeypoints;
      cv::Mat channelFeatures;
      ComputeImageFeatures(PreprocessImage(CvWrapper::FilterImage_Channel(channeledImage, i)), channelKeypoints, channelFeatures); 

      keypoints.insert(keypoints.end(), channelKeypoints.begin(), channelKeypoints.end());
      features.push_back(channelFeatures);
    }
  }
}

void ImageSVM::ComputeImageFeatures(cv::Mat image, std::vector<cv::KeyPoint>& keypoints, cv::Mat& features) {
  featureDetector->detectAndCompute(image, cv::Mat(), keypoints, features);
}

std::string ImageSVM::CreateDictionaryName(int channel) {
  std::stringstream ss;
  ss << "DICTIONARY_" << datasetName << "_" << channel << ".png";
  std::cout << "CREATE DICTIONARY NAME: " << ss.str() << std::endl;
  return ss.str();
}

std::string ImageSVM::CreateSVMName() {
  std::stringstream ss;
  ss << "SVM_" << datasetName << ".svm";
  return ss.str();
}

std::string ImageSVM::CreateAuxiliaryFilename() {
  std::stringstream ss;
  ss << "AUX_" << datasetName << ".txt";
  return ss.str();
}

int ImageSVM::GetTotalChannels() const {
  return (imageType == TIT_GRAYSCALE) ? 1 : 3;
}

void ImageSVM::PerformTraining() {
  int totalChannels = GetTotalChannels();
  for (int i = 0; i < totalChannels; ++i) {
#if !REUSE_DICTIONARY_TRAINING
    std::cout << "START KMEANS CLUSTERING" << std::endl;
    // Create the bag of visual words dictionary using K-Means clustering on all the features
    cv::Mat floatFeatureSet;
    completeFeatureSet[i].convertTo(floatFeatureSet, CV_32F);
    std::cout << "  Number Features: " << floatFeatureSet.rows << " " << floatFeatureSet.cols << std::endl;

    cv::Mat featureLabels;
    cv::TermCriteria criteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 10000, 0.0001);
    cv::kmeans(floatFeatureSet, kClusters, featureLabels, criteria, 5, cv::KMEANS_PP_CENTERS, dictionary[i]);

    std::cout << "FINISH KMEANS CLUSTERING" << std::endl;
    cv::imwrite(CreateDictionaryName(i), dictionary[i]);
#else
    cv::imread(CreateDictionaryName(i), cv::IMREAD_GRAYSCALE).convertTo(dictionary[i], CV_32F);
#endif 
  }

  // Now go back through the images and calculate their feature vector histograms to by finding the closest match in the dictionary  
  matcher.resize(totalChannels);
  for (int i = 0; i < totalChannels; ++i) {
    matcher[i].add(dictionary[i]);
    matcher[i].train();
  }

  std::cout << "PERFORM NEAREST NEIGHBOR MATCH" << std::endl;
  int singleChannelBinCount = TotalHistogramBins(maxSpatialPyramidLevel);
  int totalBins = singleChannelBinCount * totalChannels;

  std::cout << "  Total Features: " << totalBins << std::endl;
  std::cout << "  Total Samples: " << problem.l << std::endl;

  for (size_t i = 0; i < completeImageSet.size(); ++i) {
    problem.x[i] = new svm_node[totalBins + 1];
    GenerateSVMDataForImage(problem.x[i], completeImageSet[i], matcher);
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
  int totalBins = TotalHistogramBins(maxLevel);
  
  for (int level = 0; level <= maxLevel; ++level) {
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
ImageSVM::GenerateSVMDataForImage(svm_node* inProblem, cv::Mat inImage, std::vector<cv::FlannBasedMatcher>& matcher) {
  int totalChannels = (imageType == TIT_GRAYSCALE) ? 1 : 3;
  for (int j = 0; j < totalChannels; ++j) {
    svm_node* problemPointer = inProblem + j * TotalHistogramBins(maxSpatialPyramidLevel);
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat features;
    BatchComputeImageFeatures(inImage, keypoints, features, j);

    cv::Mat floatFeatures;
    features.convertTo(floatFeatures, CV_32F);

    std::vector<cv::DMatch> matches;
    matcher[j].match(floatFeatures, matches);

    GenerateSpatialPyramidDataHierarchy(inImage, keypoints, matches, maxSpatialPyramidLevel, problemPointer);
  }
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
  LoadAuxiliaryData();

  dictionary.resize(GetTotalChannels());
  completeFeatureSet.resize(GetTotalChannels());
  matcher.resize(GetTotalChannels());

  for (int i = 0; i < GetTotalChannels(); ++i) {
    cv::imread(CreateDictionaryName(i), cv::IMREAD_GRAYSCALE).convertTo(dictionary[i], CV_32F);
    matcher[i].add(dictionary[i]);
    matcher[i].train();
  }
  svm = svm_load_model(CreateSVMName().c_str());
}

std::string ImageSVM::PredictImage(const cv::Mat& inImage) {
  cv::Mat newImage;

  switch (imageType) {
  case TIT_GRAYSCALE:
    cv::cvtColor(inImage, newImage, cv::COLOR_BGR2GRAY);
    break;
  case TIT_HSV:
    cv::cvtColor(inImage, newImage, cv::COLOR_BGR2HSV_FULL);
    break;
  }

  int totalNodes = TotalHistogramBins(maxSpatialPyramidLevel) * GetTotalChannels() + 1;
  svm_node* queryNode = new svm_node[totalNodes];
  GenerateSVMDataForImage(queryNode, inImage, matcher);
  
  double label = svm_predict(svm, queryNode);
  delete[] queryNode;
  return ConvertLabelToString((int)label);
}

void ImageSVM::SetupSVMParameters() {

  double maxAccuracy = 0.0;
  double bestC = 0.0;
  double bestG = 0.0;
#ifdef NDEBUG
#pragma omp parallel for num_threads(3)
#endif
  for (int c = -5; c <= 15; c += 2) {
    for (int g = 3; g >= 3; g -= 2) {
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
#ifdef NDEBUG
#pragma omp critical
#endif
      if (accuracy > maxAccuracy + 1e-6) {
        maxAccuracy = accuracy;
        bestC = tempParam.C;
        bestG = tempParam.gamma;
      }
    }
  }

  std::cout << "FINISH CROSS VALIDATION: " << kClusters << " " << maxSpatialPyramidLevel << " " << svmParams.kernel_type << std::endl;
  std::cout << "  Accuracy: " << maxAccuracy << std::endl;
  svmParams.C = bestC;
  svmParams.gamma = bestG;
}