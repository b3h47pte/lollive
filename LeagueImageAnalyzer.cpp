#include "LeagueImageAnalyzer.h"

LeagueImageAnalyzer::LeagueImageAnalyzer(IMAGE_PATH_TYPE ImagePath): ImageAnalyzer(ImagePath) {

}

LeagueImageAnalyzer::~LeagueImageAnalyzer() {

}

/*
 * Call functions to get various pieces of data that are of interest.
 * If too many fail [threshold TBD], then fail.
 */
bool LeagueImageAnalyzer::Analyze() {
  bool ret = true;
  if (mImage.empty()) {
    return false;
  }
  
  // Current match time.
  int time = AnalyzeMatchTime();
  std::shared_ptr<GenericData<int>> timeProp(new GenericData<int>(time));
  (*mData)["CurrentTime"] = timeProp;

  return ret;
}