#include "LeagueItemSVM.h"
#include "LeagueItemDatabase.h"
#include "LeagueConstants.h"

LeagueItemSVM::LeagueItemSVM(bool performTraining) :
  ImageSVM("LeagueItem", performTraining) {
  kClusters = 350;
  maxSpatialPyramidLevel = 1;
  imageType = TIT_HSV;
}

LeagueItemSVM::~LeagueItemSVM() {

}

void LeagueItemSVM::LoadTrainingData() {
  LoadTraining();

  std::shared_ptr<const LeagueItemDatabase> ldb = LeagueItemDatabase::Get();
  const std::map<std::string, PtrLeagueItemData>* mapping = ldb->GetDatabase();
  for (auto data : *mapping) {
    labelToItem.push_back(data.first);
  }
}

void LeagueItemSVM::CreateTrainingData() {
  std::shared_ptr<const LeagueItemDatabase> ldb = LeagueItemDatabase::Get();
  const std::map<std::string, PtrLeagueItemData>* mapping = ldb->GetDatabase();

  // Calculate the number of items we want to put into the training set.
  // Resize a champion image at multiple resolutions and put that into the SVM'
  const int resolutionStep = 3;
  int resolutionLevels = (int)((float)LEAGUE_ITEM_SQUARE_SIZE * 3 / 4) / resolutionStep;
  InitializeTrainingDataset((mapping->size() - 1) * resolutionLevels, LEAGUE_ITEM_SQUARE_SIZE, LEAGUE_ITEM_SQUARE_SIZE);
  int count = 0;
  int label = 0;
  for (auto data : *mapping) {
    if (data.second->itemID == "9999") continue;
    cv::Mat originalImage = data.second->itemImage;
    for (int i = 0; i < resolutionLevels; ++i) {
      int res = LEAGUE_ITEM_SQUARE_SIZE / 4 + i * resolutionStep;
      cv::Mat resizedImage;
      cv::Size newSize;
      cv::resize(originalImage, resizedImage, newSize, (double)res / originalImage.rows, (double)res / originalImage.cols);

      cv::Mat finalImage;
      cv::Size finalSize;
      cv::resize(resizedImage, finalImage, finalSize, (double)LEAGUE_ITEM_SQUARE_SIZE / res, (double)LEAGUE_ITEM_SQUARE_SIZE / res);

      SetupImageTrainingData(count, finalImage, label);
      ++count;
    }
    ++label;
  }
  PerformTraining();
}

std::string LeagueItemSVM::ConvertLabelToString(int label) {
  return labelToItem[label];
}

void LeagueItemSVM::CreateOrb() {
  orb = cv::ORB::create(250, 1.2f, 8, 15, 0, 2, cv::ORB::HARRIS_SCORE, 15, 20);
}