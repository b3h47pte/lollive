#include "LeagueImageAnalyzer.h"
#include "ConversionUtility.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#define CHECK_INVALID_INT_COUNT(num, validCount, totalCount) \
if (num != -1) {\
  ++validCount; \
}\
++totalCount;

/*
 * If all a majority of data numbers are things like -1 then we assume
 * that this frame is invalid. Note that I don't check for strings since
 * that's a bit harder to detect whether or not I got a trash value.
 */
bool LeagueImageAnalyzer::IsValidFrame() {
  int totalItems = 0;
  int validItems = 0;

  // Check time
  CHECK_INVALID_INT_COUNT(RetrieveData<int>(mData, std::string("CurrentTime")), validItems, totalItems);

  // Check the team data
  // TODO: Clean up 
  PtrLeagueTeamData bt = RetrieveData<PtrLeagueTeamData>(mData, std::string("BlueTeam"));
  CHECK_INVALID_INT_COUNT(bt->kills, validItems, totalItems);
  CHECK_INVALID_INT_COUNT(bt->gold, validItems, totalItems);
  CHECK_INVALID_INT_COUNT(bt->towerKills, validItems, totalItems);

  PtrLeagueTeamData pt = RetrieveData<PtrLeagueTeamData>(mData, std::string("PurpleTeam"));
  CHECK_INVALID_INT_COUNT(pt->kills, validItems, totalItems);
  CHECK_INVALID_INT_COUNT(pt->gold, validItems, totalItems);
  CHECK_INVALID_INT_COUNT(pt->towerKills, validItems, totalItems);

  double pass = (double)validItems / totalItems;
  // TODO: Configurable threshold
  return (pass >= GetValidFrameThreshold());
}

/*
 * Grabs the match time from the header bar in the upper center part of the image. 
 * A value of 0 is returned if no value can be parsed.
 */
int LeagueImageAnalyzer::AnalyzeMatchTime() {
  // We only want a certain part of the image (the section that has the time on it).
  // So extract that into a separate image. 
  // TODO: Make sure this works when it hits hours.
  cv::Rect section = GetMatchTimeSection();
  cv::Mat timeImage = FilterImage_Section_Grayscale_BasicThreshold_Resize(mImage, section, 
    GetMatchTimeThreshold(), GetMatchTimeResizeX(), GetMatchTimeResizeY());
  std::string result = GetTextFromImage(timeImage, LeagueIdent, std::string("012345679:"));
  if (result.size() < 3) {
    return -1;
  }

  // Now parse the result so that it is just a number of seconds.
  int secondsSinceGameStart = -1;
  // Look for the ':', if somehow tesseract wasn't able to find an ':', then try to infer based on the text given.
  // If no inference can be made, then just give up and fail. 
  size_t splitIdx = result.find(':');
  
  // No semi-colon found. The seconds number should always be two digits long so 
  // just try to manually stick a semi-colon in the right spot.
  if (splitIdx == std::string::npos) {
    try {
      if (result.size() == 5) {
        result.replace(result.end() - 3, result.end() - 2, 1, ':');
      } else if (result.size() < 5) {
        result.insert(result.end() - 2, ':');
      } else {
        return -1; // wtf is this time. no idea.
      }
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
  
  try {
    secondsSinceGameStart = std::stoi(strMinutes, NULL) * 60 + std::stoi(strSeconds, NULL);
  } catch (...) {
    return -1;
  }
  return secondsSinceGameStart;
}

cv::Rect LeagueImageAnalyzer::
GetMatchTimeSection() {
  cv::Rect output;
  GetCastedPropertyValue<cv::Rect>(LEAGUE_GAMESTATE_MATCH_TIME, output, CreateRectFromString);
  return GetRealRectangle(output);
}

/*
 * Determine where the camera is. The camera is a rectangle on the minimap so we say
 * that the camera's position is the very center of the rectangle (since that is what should
 * be at the center of the screen).
 */
void LeagueImageAnalyzer::AnalyzeMapPosition(double& xPos, double& yPos) {
  cv::Mat mapImg = FilterImage_Section_Grayscale_BasicThreshold_Resize(mImage, GetMapSection(), 
    GetMapThreshold(), GetMapResizeX(), GetMapResizeY());

  // Find the contours in this image to find the giant rectangle which indicates the location of the camera.
  std::vector<std::vector<cv::Point> > contours;
  cv::findContours(mapImg, contours, cv::RETR_CCOMP, cv::CHAIN_APPROX_NONE);

  int largestContourIdx = -1;
  double largestContourArea = 0.0f;

  for (size_t i = 0; i < contours.size(); ++i) {
    // Find the largest contours in a rectangle that is also a valid camera rectangle.
    // The height of the box can't be larger than the width. 
    cv::Rect r = cv::boundingRect(contours[i]);
    if (r.width <= r.height) {
      continue;
    }

    double a = r.width * r.height;
    if (a > largestContourArea) {
      largestContourArea = a;
      largestContourIdx = i;
    }
  }

  cv::Rect cameraRectangle = cv::boundingRect(contours[largestContourIdx]);
  double x = (2 * cameraRectangle.x + cameraRectangle.width) / 2.0;
  double y = (2 * cameraRectangle.y + cameraRectangle.height) / 2.0;
  xPos = x / mapImg.cols;
  yPos = y / mapImg.rows;
}

cv::Rect LeagueImageAnalyzer::GetMapSection() {
  cv::Rect output;
  GetCastedPropertyValue<cv::Rect>(LEAGUE_GAMESTATE_MAP, output, CreateRectFromString);
  return GetRealRectangle(output);
}


