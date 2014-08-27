#include "LeagueSpectatorImageAnalyzer.h"
#include "common.h"

int main() {
  std::string path = "Images/faker/frame16.png";
  LeagueSpectatorImageAnalyzer* img = new LeagueSpectatorImageAnalyzer(path);
  img->Analyze();
  return 0;
}