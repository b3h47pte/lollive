#include "LeagueSpectatorVideoAnalyzer.h"
#include "LeagueSpectatorImageAnalyzer.h"

std::shared_ptr<class ImageAnalyzer> LeagueSpectatorVideoAnalyzer::CreateImageAnalyzer(std::string& path) {
  return std::shared_ptr<ImageAnalyzer>(new LeagueSpectatorImageAnalyzer(path));
}