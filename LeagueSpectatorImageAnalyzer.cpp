#include "LeagueSpectatorImageAnalyzer.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <algorithm>

LeagueSpectatorImageAnalyzer::LeagueSpectatorImageAnalyzer(IMAGE_PATH_TYPE ImagePath): LeagueImageAnalyzer(ImagePath) {

}

LeagueSpectatorImageAnalyzer::~LeagueSpectatorImageAnalyzer() {

}

/*
 * Grabs the match time from the header bar in the upper center part of the image. 
 * A value of 0 is returned if no value can be parsed.
 */
int LeagueSpectatorImageAnalyzer::AnalyzeMatchTime() {
  // We only want a certain part of the image (the section that has the time on it).
  // So extract that into a separate image. 
  // TODO: Make sure the rectangle calculation works even when on 1080p since my only data right now is in 720p.
  // TODO: Make sure this works when it hits hours.
  cv::Rect section = cv::Rect((int)(mImage.cols * (620.0f / 1280.0f)), 
    (int)(mImage.rows * (50.0f / 720.0f)), 
    (int)(mImage.cols * (40.0f / 1280.0f)), 
    (int)(mImage.rows * (15.0f / 720.0f)));
  cv::Mat timeImage = mImage(section);

  // Process the image to make the text clearer for the Tesseract OCR engine.
  // Make sure it's black and white only
  cv::cvtColor(timeImage, timeImage, cv::COLOR_RGB2GRAY);

  // Make it black and white, no need for noise that we don't need.
  // TODO: Make these options configurable.
  cv::threshold(timeImage, timeImage, 105.0, 255.0, cv::THRESH_BINARY);

  // Tesseract needs big text. TODO: This may need to change depending on the resolution?
  cv::Size newSize;
  cv::resize(timeImage, timeImage, newSize, 2.0, 2.0);
  
  std::string result = GetTextFromImage(timeImage, EnglishIdent);

  // Now parse the result so that it is just a number of seconds.
  int secondsSinceGameStart = -1;
  // Look for the ':', if somehow tesseract wasn't able to find an ':', then try to infer based on the text given.
  // If no inference can be made, then just give up and fail. 
  size_t splitIdx = result.find(':');
  
  // No semi-colon found. The seconds number should always be two digits long so 
  // just try to manually stick a semi-colon in the right spot.
  if (splitIdx == std::string::npos) {
    try {
      result.replace(result.end() - 3, result.end() - 2, 1, ':');
    } catch (...) {
      // If for some reason the replacement failed, then assume that we have failed to read in the time.
      return -1;
    }
    splitIdx = result.find(':');
  }

  std::string strMinutes = result.substr(0, splitIdx);
  std::string strSeconds = result.substr(splitIdx + 1);
  // If seconds isn't two digits long, something screwed up. Failure.
  if (strSeconds.length() != 2) {
    return -1;
  }
  
  secondsSinceGameStart = std::stoi(strMinutes, NULL) * 60 + std::stoi(strSeconds, NULL);
  return secondsSinceGameStart;
}

/*
 * Retrieve the team's kills from the header bar at the top.
 * However, since we want to be able to read the team's kills separately, 
 * we must apply color filters and grab the right image section based on the team property.
 * Offload those tasks to utility functions.
 */
int LeagueSpectatorImageAnalyzer::AnalyzeTeamKills(ELeagueTeams team) {
  cv::Rect section = GetTeamKillsSection(team);
  cv::Mat killsImage = mImage(section);

  // Grab the BLUE channel for the blue team and the RED channel for the purple team.
  cv::Mat filterImage(killsImage.rows, killsImage.cols, CV_8UC1);
  int fromTo[] = { 0, 0 }; // Default to blue team
  if (team == ELT_PURPLE) {
    fromTo[0] = 2; // 2 is the red channel. 
  }
  cv::mixChannels(&killsImage, 1, &filterImage, 1, fromTo, 1);

  // Now make the image purely black and white.
  // TODO: Configurable.
  cv::threshold(filterImage, filterImage, 115.0, 255.0, cv::THRESH_BINARY);

  // Tesseract needs big text. TODO: This may need to change depending on the resolution?
  cv::Size newSize;
  cv::resize(filterImage, filterImage, newSize, 2.0, 2.0);

  std::string killsText = GetTextFromImage(filterImage, LeagueIdent, true);
  // In the case where the killsText that Tesseract returns isn't actually a number, just ignore it. Can't read it.
  try {
    return std::stoi(killsText, NULL);
  } catch (...) {
    return -1;
  }
  return -1;
}

/*
 * Gets the appropriate box that contains the number of kills the team has. 
 * TODO: Make sure this works on 1080p as well.
 */
cv::Rect LeagueSpectatorImageAnalyzer::GetTeamKillsSection(ELeagueTeams team) {
  cv::Rect rect;
  if (team == ELT_BLUE) {
    rect = cv::Rect((int)(mImage.cols * (595.0f / 1280.0f)),
      (int)(mImage.rows * (17.0f / 720.0f)),
      (int)(mImage.cols * (32.0f / 1280.0f)),
      (int)(mImage.rows * (25.0f / 720.0f)));
  } else {
    rect = cv::Rect((int)(mImage.cols * (656.0f / 1280.0f)),
      (int)(mImage.rows * (17.0f / 720.0f)),
      (int)(mImage.cols * (32.0f / 1280.0f)),
      (int)(mImage.rows * (25.0f / 720.0f)));
  }
  return rect;
}