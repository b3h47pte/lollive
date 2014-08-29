#include "LeagueLCSImageAnalyzer.h"
#include "common.h"

int main() {
  std::string path = "Images/twitch/frame0.png";
  LeagueLCSImageAnalyzer* img = new LeagueLCSImageAnalyzer(path);
  img->Analyze();

  int exit; std::cin >> exit;
  return 0;
}