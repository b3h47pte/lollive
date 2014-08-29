#include "LeagueLCSImageAnalyzer.h"

LeagueLCSImageAnalyzer::LeagueLCSImageAnalyzer(IMAGE_PATH_TYPE ImagePath) : LeagueSpectatorImageAnalyzer(ImagePath) {
}

LeagueLCSImageAnalyzer::~LeagueLCSImageAnalyzer() {

}

cv::Rect LeagueLCSImageAnalyzer::GetPlayerChampionSection(uint idx, ELeagueTeams team) {
  cv::Rect rect;
  float x; // x -- determined by the team
  float y; // y -- determined by the player index 
  y = 155.0f + idx * 70.0f;
  if (team == ELT_BLUE) {
    x = 26.0f;
  } else {
    x = 1223.0f;
  }
  rect = cv::Rect((int)(mImage.cols * (x / 1280.0f)),
    (int)(mImage.rows * (y / 720.0f)),
    (int)(mImage.cols * (32.0f / 1280.0f)),
    (int)(mImage.rows * (32.0f / 720.0f)));
  return rect;
}

cv::Rect LeagueLCSImageAnalyzer::GetPlayerNameSection(uint idx, ELeagueTeams team) {
  cv::Rect rect;
  float x; // x -- determined by the team
  float y; // y -- determined by the player index 
  y = 142.0f + idx * 70.0f;
  if (team == ELT_BLUE) {
    x = 2.0f;
  } else {
    x = 1215.0f;
  }
  rect = cv::Rect((int)(mImage.cols * (x / 1280.0f)),
    (int)(mImage.rows * (y / 720.0f)),
    (int)(mImage.cols * (65.0f / 1280.0f)),
    (int)(mImage.rows * (8.0f / 720.0f)));
  return rect;
}