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
  // Resize a champion image at multiple resolutions and put that into the SVM
  InitializeTrainingDataset(mapping->size(), ldb->GetImageSizeX(), ldb->GetImageSizeY());
  int count = 0;
  for (auto data : *mapping) {
    cv::Mat originalImage = data.second->image;
    for (int res = ldb->GetImageSizeX() / 2; res < ldb->GetImageSizeX(); res += 5) {
      cv::Mat resizedImage;
      cv::Size newSize;
      cv::resize(originalImage, resizedImage, newSize, (double)res / originalImage.rows, (double)res / originalImage.cols);
      SetupImageTrainingData(count, resizedImage, (short)count);
    }
    ++count;
  }
  PerformTraining();
}

std::string LeagueChampionSVM::ConvertLabelToString(short label) {
  return labelToChampion[label];
}
