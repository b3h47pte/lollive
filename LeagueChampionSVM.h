#pragma once
#ifndef _LEAGUECHAMPIONSVM_H
#define _LEAGUECHAMPIONSVM_H
#include "common.h"
#include "ImageSVM.h"

class LeagueChampionSVM : public ImageSVM {
public:
  LeagueChampionSVM(bool performTraining);
  virtual ~LeagueChampionSVM();

protected:
  virtual std::string ConvertLabelToString(int label);
  virtual void LoadLabelMapping();
private:
  void LoadTrainingData();
  void CreateTrainingData();
  virtual void PerformPostTrainingVerification();

  std::vector<std::string> labelToChampion;
};

#endif