#include "LeagueImageAnalyzer.h"
#include "MultiRectangle.h"
#include "ConversionUtility.h"

cv::Rect LeagueImageAnalyzer::GetMinibarSection(uint idx) {
  MultiRectangle rect;
  GetCastedPropertyValue<MultiRectangle>(LEAGUE_EVENT_MINIBAR, rect, CreateMultiRectFromString);
  return rect.GetRectangle(idx);
}

cv::Rect LeagueImageAnalyzer::GetAnnouncementSection() {
  cv::Rect output;
  GetCastedPropertyValue<cv::Rect>(LEAGUE_EVENT_ANNOUNCE, output, CreateRectFromString);
  return output;
}

cv::Rect LeagueImageAnalyzer::GetMinibarOriginalResolution() {
  cv::Rect newRect = cv::Rect(0, 0, (int)GetMinibarEventWidth(), (int)GetMinibarEventHeight());
  return newRect;
}

cv::Rect LeagueImageAnalyzer::GetMinibarObjectiveIconOriginalResolution() {
  cv::Rect newRect = cv::Rect(0, 0, (int)GetMinibarObjectiveIconTargetX(), (int)GetMinibarObjectiveIconTargetY());
  return newRect;
}

cv::Rect LeagueImageAnalyzer::GetMinibarSupportingIconOriginalResolution() {
  cv::Rect newRect = cv::Rect(0, 0, (int)GetMinibarSupportingIconTargetX(), (int)GetMinibarSupportingIconTargetY());
  return newRect;
}
