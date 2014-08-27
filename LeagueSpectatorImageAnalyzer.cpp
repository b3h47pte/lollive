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
  // TODO: Make this adapt to non 16 : 9 resolutions.
  // TODO: Make sure this works when it hits hours.
  cv::Rect section = cv::Rect((int)(mImage.cols * (620.0f / 1280.0f)), 
    (int)(mImage.rows * (50.0f / 720.0f)), 
    (int)(mImage.cols * (40.0f / 1280.0f)), 
    (int)(mImage.rows * (15.0f / 720.0f)));

  cv::Mat timeImage = FilterImage_Section_Grayscale_BasicThreshold_Resize(mImage, section, 105.0, 2.0, 2.0);
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
  // TODO: Configurable
  cv::Mat filterImage = FilterImage_Section_Channel_BasicThreshold_Resize(mImage,
    GetTeamKillsSection(team),
    (team == ELT_BLUE) ? 0 : 2,
    115.0, 2.0, 2.0);
  std::string killsText = GetTextFromImage(filterImage, LeagueIdent, std::string("0123456789"));
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
 * TODO: Make this adapt to non 16:9 resolutions.
 * TODO: Make this configurable.
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

/*
 * Get the team's gold. League of Legends displays gold as follows:
 * XX.Xk OR X.Xk. Players start with 2.4k gold.
 */
int LeagueSpectatorImageAnalyzer::AnalyzeTeamGold(ELeagueTeams team) {
  // TODO: Configurable
  cv::Mat filterImage = FilterImage_Section_Channel_BasicThreshold_Resize(mImage,
    GetTeamGoldSection(team),
    (team == ELT_BLUE) ? 0 : 2,
    100.0, 2.0, 2.0);
  std::string goldText = GetTextFromImage(filterImage, LeagueIdent, std::string("0123456789.k"));
  // Need to parse the number to return the actual amount of gold.
  // For example 51.7k should return 51700.
  try {
    size_t separator = goldText.find('.');
    size_t end = goldText.find('k');
    if (separator == std::string::npos && end == std::string::npos) {
      return -1; // This case is impossible to recover from. Can't make any educated guesses.
    } else if (separator == std::string::npos) {
      // Since we know where k is, we can infer where the period should be and replace whatever text that has.
      goldText.replace(end - 2, end - 1, 1, '.');
      separator = end - 2;
    } else if (end == std::string::npos) {
      // Since know where the separator is, we can infer where the end should be.
      goldText.replace(separator + 2, separator + 3, 1, 'k');
      end = separator + 2;
    }
    std::string thousands = goldText.substr(0, separator);
    std::string hundreds = goldText.substr(separator + 1, end - separator - 1);
    return std::stoi(thousands, NULL) * 1000 + std::stoi(hundreds, NULL) * 100;
  } catch (...) {
    return -1;
  }
  return -1;
}

/*
* Gets the appropriate box that contains the amount of gold the team has.
* TODO: Make this adapt to non 16:9 resolutions.
* TODO: Make this configurable.
*/
cv::Rect LeagueSpectatorImageAnalyzer::GetTeamGoldSection(ELeagueTeams team) {
  cv::Rect rect;
  if (team == ELT_BLUE) {
    rect = cv::Rect((int)(mImage.cols * (509.0f / 1280.0f)),
      (int)(mImage.rows * (19.0f / 720.0f)),
      (int)(mImage.cols * (45.0f / 1280.0f)),
      (int)(mImage.rows * (14.0f / 720.0f)));
  } else {
    rect = cv::Rect((int)(mImage.cols * (744.0f / 1280.0f)),
      (int)(mImage.rows * (19.0f / 720.0f)),
      (int)(mImage.cols * (45.0f / 1280.0f)),
      (int)(mImage.rows * (14.0f / 720.0f)));
  }
  return rect;
}

/*
 * Gets the number of tower kills each team has from the spectator header bar.
 */
int LeagueSpectatorImageAnalyzer::AnalyzeTeamTowerKills(ELeagueTeams team) {
  // TODO: Configurable
  cv::Mat filterImage = FilterImage_Section_Channel_BasicThreshold_Resize(mImage,
    GetTeamTowerKillSection(team),
    (team == ELT_BLUE) ? 0 : 2,
    100.0, 2.0, 2.0);
  std::string towerKills = GetTextFromImage(filterImage, LeagueIdent, std::string("0123456789"));
  try {
    return std::stoi(towerKills, NULL);
  } catch (...) {
    return -1;
  }
  return -1;
}

/*
* Gets the appropriate box that contains the amount of tower kills the team has.
* TODO: Make this adapt to non 16:9 resolutions.
* TODO: Make this configurable.
*/
cv::Rect LeagueSpectatorImageAnalyzer::GetTeamTowerKillSection(ELeagueTeams team) {
  cv::Rect rect;
  if (team == ELT_BLUE) {
    rect = cv::Rect((int)(mImage.cols * (455.0f / 1280.0f)),
      (int)(mImage.rows * (19.0f / 720.0f)),
      (int)(mImage.cols * (20.0f / 1280.0f)),
      (int)(mImage.rows * (14.0f / 720.0f)));
  } else {
    rect = cv::Rect((int)(mImage.cols * (821.0f / 1280.0f)),
      (int)(mImage.rows * (19.0f / 720.0f)),
      (int)(mImage.cols * (20.0f / 1280.0f)),
      (int)(mImage.rows * (14.0f / 720.0f)));
  }
  return rect;
}