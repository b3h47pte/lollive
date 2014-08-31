#include "LeagueLCSVideoAnalyzer.h"
#include "LeagueLCSImageAnalyzer.h"

std::shared_ptr<class ImageAnalyzer> LeagueLCSVideoAnalyzer::CreateImageAnalyzer(std::string& path) {
  return std::shared_ptr<ImageAnalyzer>(new LeagueLCSImageAnalyzer(path));
}