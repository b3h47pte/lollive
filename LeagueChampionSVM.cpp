#include "LeagueChampionSVM.h"
#include "LeagueChampionDatabase.h"

LeagueChampionSVM::LeagueChampionSVM(bool performTraining):
  ImageSVM("LeagueChampion", performTraining) {

}

LeagueChampionSVM::~LeagueChampionSVM() {

}

void LeagueChampionSVM::LoadTrainingData() {
  LoadTraining();

  std::shared_ptr<const LeagueChampionDatabase> ldb = LeagueChampionDatabase::Get();
  const std::map<std::string, PtrLeagueChampionData>* mapping = ldb->GetDatabase();
  for (auto data : *mapping) {
    labelToChampion.push_back(data.first);
  }
}

void LeagueChampionSVM::CreateTrainingData() {
  std::shared_ptr<const LeagueChampionDatabase> ldb = LeagueChampionDatabase::Get();
  const std::map<std::string, PtrLeagueChampionData>* mapping = ldb->GetDatabase();

  // Calculate the number of items we want to put into the training set.
  // Resize a champion image at multiple resolutions and put that into the SVM'
  const int resolutionStep = 5;
  int resolutionLevels = (ldb->GetImageSizeX() * 2 / 3) / resolutionStep;
  InitializeTrainingDataset(mapping->size() * resolutionLevels, ldb->GetImageSizeX(), ldb->GetImageSizeY());
  int count = 0;
  int label = 0;
  for (auto data : *mapping) {
    cv::Mat originalImage = data.second->image;
    for (int res = ldb->GetImageSizeX() / 3; res < ldb->GetImageSizeX(); res += resolutionStep) {
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
