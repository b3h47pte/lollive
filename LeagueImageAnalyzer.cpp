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
  bool ret = false;
  if (mImage.empty()) {
    return false;
  }
  
  AnalyzeMatchTime();

  return ret;
}