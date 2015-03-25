#include "LeagueChampionSVM.h"
#include "LeagueChampionDatabase.h"

LeagueChampionSVM::LeagueChampionSVM(bool performTraining):
  ImageSVM("LeagueChampion", performTraining) {

}

LeagueChampionSVM::~LeagueChampionSVM() {

}

void LeagueChampionSVM::LoadTrainingData() {
  LoadTraining();
}

void LeagueChampionSVM::CreateTrainingData() {
  std::shared_ptr<const LeagueChampionDatabase> ldb = LeagueChampionDatabase::Get();
  const std::map<std::string, PtrLeagueChampionData>* mapping = ldb->GetDatabase();

  // Calculate the number of items we want to put into the training set.
  // Resize a champion image at multiple resolutions and put that into the SVM'
  const int resolutionStep = 5;
  int resolutionLevels = (int)((float)ldb->GetImageSizeX() * 2 / 3) / resolutionStep;
  InitializeTrainingDataset((mapping->size() - 1) * resolutionLevels, ldb->GetImageSizeX(), ldb->GetImageSizeY());
  int count = 0;
  int label = 0;
  for (auto data : *mapping) {
    if (data.second->shortName == "None") continue;
    cv::Mat originalImage = data.second->image;
    for (int i = 0; i < resolutionLevels; ++i) {
      int res = ldb->GetImageSizeX() / 3 + i * resolutionStep;
      cv::Mat resizedImage;
      cv::Size newSize;
      cv::resize(originalImage, resizedImage, newSize, (double)res / originalImage.rows, (double)res / originalImage.cols);

      cv::Mat finalImage;
      cv::Size finalSize;
      cv::resize(resizedImage, finalImage, finalSize, (double)ldb->GetImageSizeX() / res, (double)ldb->GetImageSizeY() / res);

      SetupImageTrainingData(count, finalImage, label);
      ++count;
    }
    ++label;
  }
  PerformTraining();
}

std::string LeagueChampionSVM::ConvertLabelToString(int label) {
  return labelToChampion[label];
}

void LeagueChampionSVM::LoadLabelMapping() {
  std::shared_ptr<const LeagueChampionDatabase> ldb = LeagueChampionDatabase::Get();
  const std::map<std::string, PtrLeagueChampionData>* mapping = ldb->GetDatabase();
  for (auto data : *mapping) {
    if (data.second->shortName == "None") continue;
    labelToChampion.push_back(data.first);
  }
}

void LeagueChampionSVM::PerformPostTrainingVerification() {
  std::shared_ptr<const LeagueChampionDatabase> ldb = LeagueChampionDatabase::Get();
  const std::map<std::string, PtrLeagueChampionData>* mapping = ldb->GetDatabase();

  int accurateCount = 0;
  int totalCount = 0;
  for (auto data : *mapping) {
    if (data.second->shortName == "None") continue;
    cv::Mat originalImage = data.second->image;
    std::string label = PredictImage(originalImage);
    if (label == data.first) {
      ++accurateCount;
    } else {
      std::cout << "ERROR: Got " << label << " instead of " << data.first << std::endl;
    }
    ++totalCount;
  }
  std::cout << "  LEAGUE CHAMPIONS ACCURACY: " << (double)accurateCount / totalCount << std::endl;
}