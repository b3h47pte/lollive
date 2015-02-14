#include "LeagueImageAnalyzer.h"
#include "ConversionUtility.h"
#include "MultiRectangle.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

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
std::string LeagueImageAnalyzer::AnalyzePlayerChampion(uint idx, ELeagueTeams team, bool* isDead, bool* isLowOnHealth, int* championLevel) {
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
  std::string championMatch = FindMatchingChampion(filterImage, allHints, tmpIsLowOnHealth, tmpIsDead, false);
  if (isDead) {
    *isDead = tmpIsDead;
  }

  if (isLowOnHealth) {
    *isLowOnHealth = tmpIsLowOnHealth;
  }
  
  // Get the champion level. Since there's a possibility of the image being red, go to HSV and just use the 'value.' :)
  if (bIs1080p) {
    cv::Mat hsvFilter;
    cv::cvtColor(mImage, hsvFilter, cv::COLOR_BGR2HSV);
    cv::Mat levelImage = FilterImage_Section_Channel_BasicThreshold_Resize(hsvFilter, 
      GetPlayerChampLevelSection(team, idx), 2, 
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

cv::Rect LeagueImageAnalyzer::
GetPlayerChampLevelSection(ELeagueTeams team, uint idx) {
  MultiRectangle rect;
  GetCastedPropertyValue<MultiRectangle>((team == ELT_BLUE) ? LEAGUE_PLAYER_CHAMP_LEVEL_BLUE : LEAGUE_PLAYER_CHAMP_LEVEL_RED , rect, CreateMultiRectFromString);
  return GetRealRectangle(rect.GetRectangle(idx));
}

/*
 * Get the image of the champion on the side bar.
 */
cv::Rect LeagueImageAnalyzer::GetPlayerChampionSection(uint idx, ELeagueTeams team) {
  MultiRectangle rect;
  if (GetIsDraftBan()) {
    GetCastedPropertyValue<MultiRectangle>((team == ELT_BLUE) ? LEAGUE_DRAFT_PICK_CHAMPIONS_BLUE : LEAGUE_DRAFT_PICK_CHAMPIONS_RED, rect, CreateMultiRectFromString);
  } else {
    GetCastedPropertyValue<MultiRectangle>((team == ELT_BLUE) ? LEAGUE_PLAYER_CHAMP_IMAGE_BLUE : LEAGUE_PLAYER_CHAMP_IMAGE_RED , rect, CreateMultiRectFromString);
  }
  return GetRealRectangle(rect.GetRectangle(idx));
}

std::string LeagueImageAnalyzer::AnalyzePlayerName(uint idx, ELeagueTeams team) {
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
cv::Rect LeagueImageAnalyzer::GetPlayerNameSection(uint idx, ELeagueTeams team) {
  MultiRectangle rect;
  GetCastedPropertyValue<MultiRectangle>((team == ELT_BLUE) ? LEAGUE_PLAYER_NAME_BLUE : LEAGUE_PLAYER_NAME_RED , rect, CreateMultiRectFromString);
  return GetRealRectangle(rect.GetRectangle(idx));
}

std::string LeagueImageAnalyzer::AnalyzePlayerScore(uint idx, ELeagueTeams team, int* kills, int* deaths, int* assists, int* cs) {
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

cv::Rect LeagueImageAnalyzer::GetPlayerKDASection(uint idx, ELeagueTeams team) {
  MultiRectangle rect;
  GetCastedPropertyValue<MultiRectangle>((team == ELT_BLUE) ? LEAGUE_PLAYER_KDA_BLUE : LEAGUE_PLAYER_KDA_RED , rect, CreateMultiRectFromString);
  return GetRealRectangle(rect.GetRectangle(idx));
}

cv::Rect LeagueImageAnalyzer::GetPlayerCSSection(uint idx, ELeagueTeams team) {
  MultiRectangle rect;
  GetCastedPropertyValue<MultiRectangle>((team == ELT_BLUE) ? LEAGUE_PLAYER_CS_BLUE : LEAGUE_PLAYER_CS_RED , rect, CreateMultiRectFromString);
  return GetRealRectangle(rect.GetRectangle(idx));
}

// Basic Assumptions for Correcting the Score 
//  1) The numbers that the OCR returns are either supposed to be a backslash or are actually part of the score
//  2) The number of kills/deaths/assists will never reach 3 digits
//  3) A single digit number (0-9) will never be made up of two digits (i.e 08 instead of just 8).
// Since I have to allow the reading in of the backslash, there is a chance that the OCR gets it wrong so I need to make sure
// that the two backslashes are registered.

/*
 * Fixes the score. An invalid score resulting from our recursion will net us an empty string.
 * TODO: Have some model that returns the most likely result.
 */
std::string LeagueImageAnalyzer::FixScore(std::string inScore) {
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

bool LeagueImageAnalyzer::IsValidScore(std::string& inScore) {
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

cv::Rect LeagueImageAnalyzer::GetPlayerItemSection(uint playerIdx, ELeagueTeams team, uint itemIdx) {
  MultiRectangle rect;
  GetCastedPropertyValue<MultiRectangle>((team == ELT_BLUE) ? LEAGUE_PLAYER_ITEMS_BLUE : LEAGUE_PLAYER_ITEMS_RED , rect, CreateMultiRectFromString);
  return GetRealRectangle(rect.GetRectangle(playerIdx, itemIdx));
}
