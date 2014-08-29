#include "LeagueSpectatorImageAnalyzer.h"
#include "common.h"

int main() {
  std::string path = "Images/test/Screen01.png";
  LeagueSpectatorImageAnalyzer* img = new LeagueSpectatorImageAnalyzer(path);
  img->Analyze();

  int exit; std::cin >> exit;
  return 0;
}