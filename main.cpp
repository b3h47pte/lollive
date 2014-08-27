#include "LeagueSpectatorImageAnalyzer.h"
#include "common.h"

int main() {
  std::string path = "Images/test/Screen01.png";
  LeagueSpectatorImageAnalyzer* img = new LeagueSpectatorImageAnalyzer(path);
  img->Analyze();
  return 0;
}