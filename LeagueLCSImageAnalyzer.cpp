#include "LeagueLCSImageAnalyzer.h"
#include "LeagueTeamData.h"

LeagueLCSImageAnalyzer::LeagueLCSImageAnalyzer(IMAGE_PATH_TYPE ImagePath) : LeagueSpectatorImageAnalyzer(ImagePath) {
}

LeagueLCSImageAnalyzer::~LeagueLCSImageAnalyzer() {

}

PtrLeagueTeamData LeagueLCSImageAnalyzer::AnalyzeTeamData(ELeagueTeams team) {
  PtrLeagueTeamData newTeam = LeagueSpectatorImageAnalyzer::AnalyzeTeamData(team);
  newTeam->teamName = GetTeamName(team);
  return newTeam;
}

std::string LeagueLCSImageAnalyzer::GetTeamName(ELeagueTeams team) {
  cv::Mat filterImage = FilterImage_Section_Channel_BasicThreshold_Resize(mImage,
    GetTeamNameSection(team),
    (team == ELT_BLUE) ? 0 : 2,
    115.0, 2.0, 2.0);
  std::string teamText = GetTextFromImage(filterImage, LeagueIdent, std::string("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"), tesseract::PSM_SINGLE_WORD, NULL, NULL, true);
  return teamText;
}

cv::Rect LeagueLCSImageAnalyzer::GetTeamNameSection(ELeagueTeams team) {
  cv::Rect rect;
  if (team == ELT_BLUE) {
    rect = cv::Rect((int)(mImage.cols * (313.0f / 1280.0f)),
      (int)(mImage.rows * (2.0f / 720.0f)),
      (int)(mImage.cols * (84.0f / 1280.0f)),
      (int)(mImage.rows * (31.0f / 720.0f)));
  } else {
    rect = cv::Rect((int)(mImage.cols * (877.0f / 1280.0f)),
      (int)(mImage.rows * (2.0f / 720.0f)),
      (int)(mImage.cols * (84.0f / 1280.0f)),
      (int)(mImage.rows * (31.0f / 720.0f)));
  }
  return rect;
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


cv::Rect LeagueLCSImageAnalyzer::GetPlayerKDASection(uint idx, ELeagueTeams team) {
  cv::Rect rect;
  float x; // x -- determined by the team
  float y; // y -- determined by the player index 
  y = 623.0f + idx * 20.0f;
  if (team == ELT_BLUE) {
    x = 532.0f;
  } else {
    x = 700.0f;
  }
  rect = cv::Rect((int)(mImage.cols * (x / 1280.0f)),
    (int)(mImage.rows * (y / 720.0f)),
    (int)(mImage.cols * (48.0f / 1280.0f)),
    (int)(mImage.rows * (14.0f / 720.0f)));
  return rect;
}

cv::Rect LeagueLCSImageAnalyzer::GetPlayerCSSection(uint idx, ELeagueTeams team) {
  cv::Rect rect;
  float x; // x -- determined by the team
  float y; // y -- determined by the player index 
  y = 623.0f + idx * 20.0f;
  if (team == ELT_BLUE) {
    x = 595.0f;
  } else {
    x = 662.0f;
  }
  rect = cv::Rect((int)(mImage.cols * (x / 1280.0f)),
    (int)(mImage.rows * (y / 720.0f)),
    (int)(mImage.cols * (23.0f / 1280.0f)),
    (int)(mImage.rows * (14.0f / 720.0f)));
  return rect;
}