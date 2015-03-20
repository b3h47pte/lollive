#pragma once
#ifndef _LEAGUEITEMSVM_H
#define _LEAGUEITEMSVM_H
#include "common.h"
#include "ImageSVM.h"

class LeagueItemSVM : public ImageSVM {
public:
  LeagueItemSVM(bool performTraining);
  virtual ~LeagueItemSVM();

protected:
  virtual std::string ConvertLabelToString(int label);
private:
  void LoadTrainingData();
  void CreateTrainingData();

  std::vector<std::string> labelToItem;
  virtual void CreateOrb();
};

#endif