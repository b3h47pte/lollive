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
  virtual std::string ConvertLabelToString(short label);
private:
  void LoadTrainingData();
  void CreateTrainingData();

  std::vector<std::string> labelToChampion;
};

#endif