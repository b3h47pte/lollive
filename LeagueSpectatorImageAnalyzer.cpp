#include "LeagueSpectatorImageAnalyzer.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <algorithm>
#include "LeagueConstants.h"

LeagueSpectatorImageAnalyzer::LeagueSpectatorImageAnalyzer(IMAGE_PATH_TYPE ImagePath): LeagueImageAnalyzer(ImagePath) {
}

LeagueSpectatorImageAnalyzer::~LeagueSpectatorImageAnalyzer() {

}

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
bool LeagueSpectatorImageAnalyzer::IsValidFrame() {
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
  for (int i = 0; i < 5; ++i) {
    PtrLeaguePlayerData player = bt->players[i];
    CHECK_INVALID_INT_COUNT(player->kills, validItems, totalItems);
    CHECK_INVALID_INT_COUNT(player->deaths, validItems, totalItems);
    CHECK_INVALID_INT_COUNT(player->assists, validItems, totalItems);
    CHECK_INVALID_INT_COUNT(player->cs, validItems, totalItems);
  }

  PtrLeagueTeamData pt = RetrieveData<PtrLeagueTeamData>(mData, std::string("PurpleTeam"));
  CHECK_INVALID_INT_COUNT(pt->kills, validItems, totalItems);
  CHECK_INVALID_INT_COUNT(pt->gold, validItems, totalItems);
  CHECK_INVALID_INT_COUNT(pt->towerKills, validItems, totalItems);
  for (int i = 0; i < 5; ++i) {
    PtrLeaguePlayerData player = pt->players[i];
    CHECK_INVALID_INT_COUNT(player->kills, validItems, totalItems);
    CHECK_INVALID_INT_COUNT(player->deaths, validItems, totalItems);
    CHECK_INVALID_INT_COUNT(player->assists, validItems, totalItems);
    CHECK_INVALID_INT_COUNT(player->cs, validItems, totalItems);
  }

  double pass = (double)validItems / totalItems;
  // TODO: Configurable threshold
  return (pass >= GetValidFrameThreshold());
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
  cv::Rect section = cv::Rect((int)(mImage.cols * (GetMatchTimeX() / GetSmallRefImageX())), 
    (int)(mImage.rows * (GetMatchTimeY() / GetSmallRefImageY())),
    (int)(mImage.cols * (GetMatchTimeWidth() / GetSmallRefImageX())),
    (int)(mImage.rows * (GetMatchTimeHeight() / GetSmallRefImageY())));

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
    rect = cv::Rect((int)(mImage.cols * (GetTeamKillsBlueX() / GetSmallRefImageX())),
      (int)(mImage.rows * (GetTeamKillsY() / GetSmallRefImageY())),
      (int)(mImage.cols * (GetTeamKillsWidth() / GetSmallRefImageX())),
      (int)(mImage.rows * (GetTeamKillsHeight() / GetSmallRefImageY())));
  } else {
    rect = cv::Rect((int)(mImage.cols * (GetTeamKillsPurpleX() / GetSmallRefImageX())),
      (int)(mImage.rows * (GetTeamKillsY() / GetSmallRefImageY())),
      (int)(mImage.cols * (GetTeamKillsWidth() / GetSmallRefImageX())),
      (int)(mImage.rows * (GetTeamKillsHeight() / GetSmallRefImageY())));
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
    GetTeamGoldThreshold(), GetTeamGoldResizeX(), GetTeamGoldResizeY());
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
    rect = cv::Rect((int)(mImage.cols * (GetTeamGoldBlueX() / GetSmallRefImageX())),
      (int)(mImage.rows * (GetTeamGoldY() / GetSmallRefImageY())),
      (int)(mImage.cols * (GetTeamGoldWidth() / GetSmallRefImageX())),
      (int)(mImage.rows * (GetTeamGoldHeight() / GetSmallRefImageY())));
  } else {
    rect = cv::Rect((int)(mImage.cols * (GetTeamGoldPurpleX() / GetSmallRefImageX())),
      (int)(mImage.rows * (GetTeamGoldY() / GetSmallRefImageY())),
      (int)(mImage.cols * (GetTeamGoldWidth() / GetSmallRefImageX())),
      (int)(mImage.rows * (GetTeamGoldHeight() / GetSmallRefImageY())));
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
    GetTowerKillsThreshold(), 
    GetTowerKillsResizeX(), 
    GetTowerKillsResizeY());
  std::string towerKills = GetTextFromImage(filterImage, LeagueIdent, std::string("0123456789"), tesseract::PSM_SINGLE_BLOCK);
  std::string towerKills2 = GetTextFromImage(filterImage, LeagueIdent, std::string("0123456789"), tesseract::PSM_SINGLE_CHAR);
  try {
    return std::stoi(towerKills, NULL);
  } catch (...) {
    try {
      return std::stoi(towerKills2, NULL);
    } catch (...) {
      return -1;
    }
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
    rect = cv::Rect((int)(mImage.cols * (GetTowerKillsBlueX() / GetSmallRefImageX())),
      (int)(mImage.rows * (GetTowerKillsY() / GetSmallRefImageY())),
      (int)(mImage.cols * (GetTowerKillsWidth() / GetSmallRefImageX())),
      (int)(mImage.rows * (GetTowerKillsHeight() / GetSmallRefImageY())));
  } else {
    rect = cv::Rect((int)(mImage.cols * (GetTowerKillsPurpleX() / GetSmallRefImageX())),
      (int)(mImage.rows * (GetTowerKillsY() / GetSmallRefImageY())),
      (int)(mImage.cols * (GetTowerKillsWidth() / GetSmallRefImageX())),
      (int)(mImage.rows * (GetTowerKillsHeight() / GetSmallRefImageY())));
  }
  return rect;
}

/*
 * Determine which champion the player is playing. There's some other auxiliary information that we can pick up here as well.
 * Most notably, the champion icon can tell us if the champion is dead, is low on health, and its level.
 * 
 * Champion Identification:
 *  For our purposes comparing color histograms is sufficient for properly identifying a champion, since we have all the possible images and the only
 *  modification to it is the small box of black that holds the level information and some scale transformations. Therefore, if we split the input image into
 *  25 chunks and compare them to their corresponding chunks in the database image, then the database image that matches should be the champion that we are 
 *  looking at.
 *  
 *  However, there are some kinks to this process. There is an occurrence where, when the champion is low on health, its image flashes red. Sometimes, 
 *  we may be fed an image like this. As a result, in addition to analyzing the normal RGB image, we must also analyze the GB image (red channel set to 0). 
 *  In the cases where the champion isn't low on health, analyzing the RGB and GB images should give the same result. However, when the champion is flashing red,
 *  the results should differ and in that case, the GB image's analysis is more likely to be correct.
 *  
 * Level Identification: This process is rather simple. It takes the level section, identifies the text, and returns the text. Quite simple!
 */
std::string LeagueSpectatorImageAnalyzer::AnalyzePlayerChampion(uint idx, ELeagueTeams team, bool* isDead, bool* isLowOnHealth, int* championLevel) {
  cv::Mat filterImage = FilterImage_Section(mImage, GetPlayerChampionSection(idx, team));
  // Champion Hint. This will force us to choose a certain champion and save us a lot of computation. THIS HINT BETTER BE RIGHT.
  std::string hintKey = CreateLeagueChampionHint(idx, team);
  std::string championHint = mHints[hintKey];
  std::vector<std::string> allHints;
  if (championHint != "") {
    allHints.push_back(championHint);
  }

  bool tmpIsDead;
  bool tmpIsLowOnHealth;
  std::string championMatch = FindMatchingChampion(filterImage, allHints, tmpIsLowOnHealth, tmpIsDead);
  if (isDead) {
    *isDead = tmpIsDead;
  }

  if (isLowOnHealth) {
    *isLowOnHealth = tmpIsLowOnHealth;
  }
  
  // Get the champion level. Since there's a possibility of the image being red, go to HSV and just use the 'value.' :)
  if (bIs1080p) {
    cv::Mat hsvFilter;
    cv::cvtColor(filterImage, hsvFilter, cv::COLOR_BGR2HSV);
    cv::Mat levelImage = FilterImage_Section_Channel_BasicThreshold_Resize(hsvFilter, cv::Rect((int)(filterImage.cols * (GetPlayerChampLevelX() / GetPlayerChampImageWidth())),
      (int)(filterImage.rows * (GetPlayerChampLevelY() / GetPlayerChampImageHeight())),
      (int)(filterImage.cols * (GetPlayerChampLevelWidth() / GetPlayerChampImageWidth())),
      (int)(filterImage.rows * (GetPlayerChampLevelHeight() / GetPlayerChampImageHeight()))), 2, 
      GetPlayerChampLevelThreshold(),
      GetPlayerChampLevelResizeX(),
      GetPlayerChampLevelResizeY());
    // Dilate the image to make the text a bit clearer.
    std::string levelStr = "";
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::dilate(levelImage, levelImage, element);

    if (championLevel) {
      levelStr = GetTextFromImage(levelImage, LeagueIdent, std::string("012345679"));
      try {
        *championLevel = std::stoi(levelStr, NULL);
        if (*championLevel > 18) *championLevel = -1;
      } catch (...) {
        *championLevel = -1;
      }
    }
  } else if (championLevel) {
    *championLevel = -1;
  }
    
  return championMatch;
}

/*
 * Get the image of the champion on the side bar.
 */
cv::Rect LeagueSpectatorImageAnalyzer::GetPlayerChampionSection(uint idx, ELeagueTeams team) {
  cv::Rect rect;
  double x; // x -- determined by the team
  double y; // y -- determined by the player index 
  y = GetPlayerChampY() + idx * GetPlayerChampYIncr();
  if (team == ELT_BLUE) {
    x = GetPlayerChampBlueX();
  } else {
    x = GetPlayerChampPurpleX();
  }
  rect = cv::Rect((int)(mImage.cols * (x / GetLargeRefImageX())),
    (int)(mImage.rows * (y / GetLargeRefImageY())),
    (int)(mImage.cols * (GetPlayerChampImageWidth() / GetLargeRefImageX())),
    (int)(mImage.rows * (GetPlayerChampImageHeight() / GetLargeRefImageY())));
  return rect;
}

std::string LeagueSpectatorImageAnalyzer::AnalyzePlayerName(uint idx, ELeagueTeams team) {
  if (!bIs1080p) return "";
  cv::Mat filterImage = FilterImage_Section_Grayscale_BasicThreshold_Resize(mImage,
    GetPlayerNameSection(idx, team), 
    GetPlayerNameThreshold(), 
    GetPlayerNameResizeX(),
    GetPlayerNameResizeY());
  std::vector<std::string> keys;
  std::vector<std::string> vals;
  const char* toDisable[4] = { "load_system_dawg", "load_punc_dawg", "load_number_dawg", "load_bigram_dawg" };
  for (int i = 0; i < 4; ++i) {
    keys.push_back(std::string(toDisable[i]));
    vals.push_back(std::string("false"));
  }

  std::string playerText = GetTextFromImage(filterImage, EnglishIdent, std::string("abcdefghijklmnopqrstuvzwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"), tesseract::PSM_SINGLE_BLOCK, &keys, &vals);
  return playerText;
}

/*
 * Get the name of the champion from the sidebar.
 */
cv::Rect LeagueSpectatorImageAnalyzer::GetPlayerNameSection(uint idx, ELeagueTeams team) {
  cv::Rect rect;
  double x; // x -- determined by the team
  double y; // y -- determined by the player index 
  y = GetPlayerNameY() + idx * GetPlayerNameYIncr();
  if (team == ELT_BLUE) {
    x = GetPlayerNameBlueX();
  } else {
    x = GetPlayerNamePurpleX();
  }
  rect = cv::Rect((int)(mImage.cols * (x / GetLargeRefImageX())),
    (int)(mImage.rows * (y / GetLargeRefImageY())),
    (int)(mImage.cols * (GetPlayerNameWidth() / GetLargeRefImageX())),
    (int)(mImage.rows * (GetPlayerNameHeight() / GetLargeRefImageY())));
  return rect;
}

std::string LeagueSpectatorImageAnalyzer::AnalyzePlayerScore(uint idx, ELeagueTeams team, int* kills, int* deaths, int* assists, int* cs) {
  // Initialize all variables
  if (kills) {
    *kills = -1;
  }

  if (deaths) {
    *deaths = -1;
  }

  if (assists) {
    *assists = -1;
  }

  if (cs) {
    *cs = -1;
  }

  cv::Mat filterImage = FilterImage_Section_Grayscale_BasicThreshold_Resize(mImage,
    GetPlayerKDASection(idx, team), GetPlayerKDAThreshold(), GetPlayerKDAResizeX(), GetPlayerKDAResizeY());

  std::string score = GetTextFromImage(filterImage, LeagueIdent, std::string("/0123456789"), tesseract::PSM_SINGLE_BLOCK);
  score = FixScore(score);
  
  try {
    size_t pos;
    int k, d, a;
    k = std::stoi(score, &pos);
    if (kills) *kills = k;
    score = score.substr(pos+1);

    d = std::stoi(score, &pos);
    if (deaths) *deaths = d;
    score = score.substr(pos+1);

    a = std::stoi(score, &pos);
    if (assists) *assists = a;
  } catch (...) {
  }

  filterImage = FilterImage_Section_Grayscale_BasicThreshold_Resize(mImage,
    GetPlayerCSSection(idx, team), GetPlayerCSThreshold(), GetPlayerCSResizeX(), GetPlayerCSResizeY());
  score = GetTextFromImage(filterImage, LeagueIdent, std::string("0123456789"));
  try {
    if (cs) {
      *cs = std::stoi(score, NULL);
    }
  } catch (...) {}

  return "";
}

cv::Rect LeagueSpectatorImageAnalyzer::GetPlayerKDASection(uint idx, ELeagueTeams team) {
  cv::Rect rect;
  double x; // x -- determined by the team
  double y; // y -- determined by the player index 
  y = GetPlayerKDAY() + idx * GetPlayerKDAYIncr();
  if (team == ELT_BLUE) {
    x = GetPlayerKDABlueX();
  } else {
    x = GetPlayerKDAPurpleX();
  }
  rect = cv::Rect((int)(mImage.cols * (x / GetLargeRefImageX())),
    (int)(mImage.rows * (y / GetLargeRefImageY())),
    (int)(mImage.cols * (GetPlayerKDAWidth() / GetLargeRefImageX())),
    (int)(mImage.rows * (GetPlayerKDAHeight() / GetLargeRefImageY())));
  return rect;
}

cv::Rect LeagueSpectatorImageAnalyzer::GetPlayerCSSection(uint idx, ELeagueTeams team) {
  cv::Rect rect;
  double x; // x -- determined by the team
  double y; // y -- determined by the player index 
  y = GetPlayerCSY() + idx * GetPlayerCSYIncr();
  if (team == ELT_BLUE) {
    x = GetPlayerCSBlueX();
  } else {
    x = GetPlayerCSPurpleX();
  }
  rect = cv::Rect((int)(mImage.cols * (x / GetLargeRefImageX())),
    (int)(mImage.rows * (y / GetLargeRefImageY())),
    (int)(mImage.cols * (GetPlayerCSWidth() / GetLargeRefImageX())),
    (int)(mImage.rows * (GetPlayerCSHeight() / GetLargeRefImageY())));
  return rect;
}

// Basic Assumptions for Correcting the Score 
//  1) The numbers that the OCR returns are either supposed to be a backslash or are actually part of the score
//  2) The number of kills/deaths/assists will never reach 3 digits
//  3) A single digit number (0-9) will never be made up of two digits (i.e 08 instead of just 8).
// Since I have to allow the reading in of the backslash, there is a chance that the OCR gets it wrong so I need to make sure
// that the two backslashes are registered.

/*
 * Fixes the score. An invalid score resulting from our recursion will net us an empty string.
 * TODO: Return all possible results?
 */
std::string LeagueSpectatorImageAnalyzer::FixScore(std::string inScore) {
  size_t bs1 = inScore.find('/');
  size_t bs2 = inScore.find('/', bs1 + 1); 
  size_t start = 0;

  if (bs1 != std::string::npos && bs2 != std::string::npos) {
    if (IsValidScore(inScore)) {
      return inScore;
    }
    return "";
  }

  // Try adding one in various positions in the string
  for (size_t j = start; j < inScore.size(); ++j) {
    if (j == bs1 || j == bs2) continue;

    std::string tmp = inScore;

    // Try replacing first
    tmp.replace(tmp.begin() + j, tmp.begin() + j + 1, "/");
    std::string res = FixScore(tmp);
    if (res != "" && IsValidScore(res)) {
      return res;
    }

    // Then try inserting
    tmp = inScore;
    tmp.insert(tmp.begin() + j, '/');
    res = FixScore(tmp);
    if (res != "" && IsValidScore(res)) {
      return res;
    }
  }
  return "";
}

bool LeagueSpectatorImageAnalyzer::IsValidScore(std::string& inScore) {
  size_t bs1 = inScore.find('/');
  size_t bs2 = inScore.find('/', bs1 + 1);
  // A valid score can't have the backslash as its first or last character
  if (bs1 == 0 || bs2 == inScore.size() - 1) {
    return false;
  }

  // A valid score can't have a triple digit number. 
  // It also can't have a number that starts with a 0 [unless it is zero].
  if (bs1 != std::string::npos) {
    std::string num = inScore.substr(0, bs1);
    if (num.size() > 2) {
      return false;
    }

    try {
      int n1 = std::stoi(num, NULL);
      if (n1 < 10 && num.size() == 2) {
        return false;
      }
    } catch (...) {
      // not a number, not a valid string
      return false;
    }

    if (bs2 != std::string::npos) {
      std::string num2 = inScore.substr(bs1 + 1, bs2 - bs1 - 1);
      if (num2.size() > 2) {
        return false;
      }

      try {
        int n2 = std::stoi(num2, NULL);
        if (n2 < 10 && num2.size() == 2) {
          return false;
        }
      } catch (...) {
        // not a number, not a valid string
        return false;
      }

      std::string num3 = inScore.substr(bs2 + 1);
      if (num3.size() > 2) {
        return false;
      }

      try {
        int n3 = std::stoi(num3, NULL);
        if (n3 < 10 && num3.size() == 2) {
          return false;
        }
      } catch (...) {
        // not a number, not a valid string
        return false;
      }

      // At this point the score should be valid.
      return true;
    }
  }
  return false;
}

/*
 * Using 1080p as a reference.
 */
cv::Rect LeagueSpectatorImageAnalyzer::GetPlayerItemSection(uint playerIdx, ELeagueTeams team, uint itemIdx) {
  double x = 0.0f;
  double y = 0.0f;
  if (team == ELT_BLUE) {
    x = GetPlayerItemBlueX();
  } else {
    x = GetPlayerItemPurpleX();
  }
  x += itemIdx * GetPlayerItemXIncr();
  y = GetPlayerItemY() + playerIdx * GetPlayerItemYIncr();
  cv::Rect newRect = cv::Rect((int)(mImage.cols * (x / GetLargeRefImageX())),
    (int)(mImage.rows * (y / GetLargeRefImageY())),
    (int)(mImage.cols * (GetPlayerItemWidth() / GetLargeRefImageX())),
    (int)(mImage.rows * (GetPlayerItemHeight() / GetLargeRefImageY())));

  return newRect;
}

/*
 * Determine where the camera is. The camera is a rectangle on the minimap so we say
 * that the camera's position is the very center of the rectangle (since that is what should
 * be at the center of the screen).
 */
void LeagueSpectatorImageAnalyzer::AnalyzeMapPosition(double& xPos, double& yPos) {
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

cv::Rect LeagueSpectatorImageAnalyzer::GetMapSection() {
  cv::Rect newRect = cv::Rect((int)(mImage.cols * (GetMapX() / GetLargeRefImageX())),
    (int)(mImage.rows * (GetMapY() / GetLargeRefImageY())),
    (int)(mImage.cols * (GetMapWidth() / GetLargeRefImageX())),
    (int)(mImage.rows * (GetMapHeight() / GetLargeRefImageY())));

  return newRect;
}

cv::Rect LeagueSpectatorImageAnalyzer::GetMinibarSection(uint idx) {
  double x = GetMinibarEventX();
  double y = GetMinibarEventY();
  y += idx * GetMinibarEventYIncr();

  cv::Rect newRect = cv::Rect((int)(mImage.cols * (x / GetLargeRefImageX())),
    (int)(mImage.rows * (y / GetLargeRefImageY())),
    (int)(mImage.cols * (GetMinibarEventWidth() / GetLargeRefImageX())),
    (int)(mImage.rows * (GetMinibarEventHeight() / GetLargeRefImageY())));

  return newRect;
}

cv::Rect LeagueSpectatorImageAnalyzer::GetAnnouncementSection() {
  cv::Rect newRect = cv::Rect((int)(mImage.cols * (GetAnnouncementX() / GetLargeRefImageX())),
    (int)(mImage.rows * (GetAnnouncementY() / GetLargeRefImageY())),
    (int)(mImage.cols * (GetAnnouncementWidth() / GetLargeRefImageX())),
    (int)(mImage.rows * (GetAnnouncementHeight() / GetLargeRefImageY())));

  return newRect;
}

cv::Rect LeagueSpectatorImageAnalyzer::GetMinibarOriginalResolution() {
  cv::Rect newRect = cv::Rect(0,0,(int)GetLargeRefImageX(),(int)GetLargeRefImageY());
  return newRect;
}

cv::Rect LeagueSpectatorImageAnalyzer::GetMinibarObjectiveIconOriginalResolution() {
  cv::Rect newRect = cv::Rect(0, 0, (int)GetMinibarObjectiveIconTargetX(), (int)GetMinibarObjectiveIconTargetY());
  return newRect;
}