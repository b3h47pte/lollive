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
  std::string result = GetTextFromImage(timeImage, LeagueIdent, std::string("012345679:"));

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
  // Split the image into x_dim * y_dim parts (generally want to have ~25 solid pieces to compare).
  // TODO: Make this configurable
  int x_dim = 3;
  int y_dim = 3;
  int totalEle = x_dim * y_dim;
  cv::Mat* filterSubImages = new cv::MatND[x_dim * y_dim];
  cv::Mat* filterSubImagesNoRed = new cv::MatND[x_dim * y_dim];

  cv::MatND* filterSubHSHists = new cv::MatND[x_dim * y_dim]; // Hue and Saturation. Used to determine who the champion actually is.
  cv::MatND* filterSubHSHistNoRed = new cv::MatND[x_dim * y_dim]; // Hue and Saturation. Used to determine who the champion is when the champion is low on health.
  cv::MatND* filterSubVHists = new cv::MatND[x_dim * y_dim]; // Value (from HSV). Used to determine if the champion be dead.
  SplitImage(filterImage, x_dim, y_dim, &filterSubImages);
  int cc = 0;
  std::for_each(filterSubImages, filterSubImages + totalEle, [&](cv::Mat inImg) {
    filterSubHSHists[cc] = CreateHSHistogram(inImg, 10, 12);
    filterSubVHists[cc] = CreateVHistogram(inImg, 10);

    // Filter out red channel
    filterSubImagesNoRed[cc] = FilterImage_2Channel(filterSubImages[cc], 0, 1);
    filterSubHSHistNoRed[cc] = CreateHSHistogram(filterSubImagesNoRed[cc], 10, 12);

    ++cc;
  });
  cc = 0;

  // Go through the image database and determine which champion we look like most.
  const std::map<std::string, PtrLeagueChampionData>* db = ChampionDatabase->GetDatabase();

  // Find the image that is most similar
  double closestMatch = 0.0;
  std::string championMatch = "";
  double championDeadScore = 0.0;

  // Also find the image that is most similar in terms of its red score
  // In this case we can ignore the dead score because you can't be low on health and dead!
  double bestNoRedScore = 0.0;
  std::string championNoRedMatch = "";

  cv::Mat baseImage;
  cv::Mat* baseSubImages = new cv::MatND[x_dim * y_dim];
  cv::Mat* baseSubImagesNoRed = new cv::MatND[x_dim * y_dim];
  cv::MatND* baseSubHSHists = new cv::MatND[x_dim * y_dim];
  cv::MatND* baseSubHSHistsNoRed = new cv::MatND[x_dim * y_dim];
  cv::MatND* baseSubVHists = new cv::MatND[x_dim * y_dim];

  for (auto& pair : *db) {
    // Make this image as close to the input image as possible
    baseImage = FilterImage_Resize(pair.second->image, (float)filterImage.cols / pair.second->image.cols, (float)filterImage.rows / pair.second->image.rows);
    SplitImage(baseImage, x_dim, y_dim, &baseSubImages);
    std::for_each(baseSubImages, baseSubImages + totalEle, [&](cv::Mat inImg) {
      baseSubHSHists[cc] = CreateHSHistogram(inImg, 10, 12);
      baseSubVHists[cc] = CreateVHistogram(inImg, 10);

      // Filter out red channel
      baseSubImagesNoRed[cc] = FilterImage_2Channel(baseSubImages[cc], 0, 1);
      baseSubHSHistsNoRed[cc] = CreateHSHistogram(baseSubImagesNoRed[cc], 10, 12);
      ++cc;
    });
    cc = 0;

    // Now compare the histograms to see if the champion matches & and if the champion is dead.
    double totalScore = 0.0; // A higher score indicates an increased likeliness that the champion is correct.
    double deadScore = 0.0;  // A higher score indicates an increased likeliness that the champion is alive.
    double noRedScore = 0.0; 
    for (int i = 0; i < totalEle; ++i) {
      totalScore += cv::compareHist(filterSubHSHists[i], baseSubHSHists[i], cv::HISTCMP_CORREL);
      deadScore += cv::compareHist(filterSubVHists[i], baseSubVHists[i], cv::HISTCMP_CORREL);
      noRedScore += cv::compareHist(filterSubHSHistNoRed[i], baseSubHSHistsNoRed[i], cv::HISTCMP_CORREL);
    }
    totalScore /= totalEle;
    deadScore /= totalEle;
    noRedScore /= totalEle;

    if (totalScore > closestMatch) {
      closestMatch = totalScore;
      championMatch = pair.second->shortName;
      championDeadScore = deadScore;
    }

    if (noRedScore > bestNoRedScore) {
      bestNoRedScore = noRedScore;
      championNoRedMatch = pair.second->shortName;
    }
  }

  if (isDead) {
    // TODO: Configurable
    *isDead = (championDeadScore < 0.5);
  }

  // At this point, there are two possibilities:
  //  a) The two outputs match (for the RGB image and the GB image). In this case, it should be clear that the character
  //     isn't low on health, and that whatever the RGB image's result is should be taken.
  //  b) The two outputs don't match! In this case we should take whichever gives a higher score. If the no red image
  //     has the high score, then we know the champion is low on health
       
  // Set this to a default value first
  if (isLowOnHealth) {
    *isLowOnHealth = false; 
  }

  if (championMatch != championNoRedMatch) {
    championMatch = (closestMatch > bestNoRedScore) ? championMatch : championNoRedMatch;
    
    // No red score is higher, which means taking red out of the image helped our analysis => LOW ON HEALTH. RED FLASHES. LIGHTS. EVERYWHERE. RUN.
    if (closestMatch < bestNoRedScore) {
      if (isLowOnHealth) {
        *isLowOnHealth = true;
      }

      // But we can't be dead if we're low on health
      if (isDead) {
        *isDead = false;
      }
    }
    
  }
  
  // Get the champion level. Since there's a possibility of the image being red, go to HSV and just use the 'value.' :)
  cv::Mat hsvFilter;
  cv::cvtColor(filterImage, hsvFilter, cv::COLOR_BGR2HSV);
  cv::Mat levelImage = FilterImage_Section_Channel_BasicThreshold_Resize(hsvFilter, cv::Rect((int)(filterImage.cols * (36.5f / 52.0f)),
    (int)(filterImage.rows * (37.0f / 52.0f)),
    (int)(filterImage.cols * (14.0f / 52.0f)),
    (int)(filterImage.rows * (12.0f / 52.0f))), 2, 65.0f, 6.0f, 6.0f);
  
  // Dilate the image to make the text a bit clearer.
  std::string levelStr = "";
  cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4));
  cv::dilate(levelImage, levelImage, element);
  
  if (*championLevel) {
    levelStr = GetTextFromImage(levelImage, LeagueIdent, std::string("012345679"));
    try {
      *championLevel = std::stoi(levelStr, NULL);
    } catch (...) {
      *championLevel = -1;
    }
  }
  
  if (filterSubImages) delete[] filterSubImages;
  if (filterSubImagesNoRed) delete[] filterSubImagesNoRed;
  if (filterSubHSHists) delete[] filterSubHSHists;
  if (filterSubHSHistNoRed) delete[] filterSubHSHistNoRed;
  if (filterSubVHists) delete[] filterSubVHists;
  if (baseSubImages) delete[] baseSubImages;
  if (baseSubHSHists) delete[] baseSubHSHists;
  if (baseSubVHists) delete[] baseSubVHists;
  if (baseSubImagesNoRed) delete[] baseSubImagesNoRed;
  if (baseSubHSHistsNoRed) delete[] baseSubHSHistsNoRed;
  
  return championMatch;
}

/*
 * Get the image of the champion on the side bar.
 */
cv::Rect LeagueSpectatorImageAnalyzer::GetPlayerChampionSection(uint idx, ELeagueTeams team) {
  cv::Rect rect;
  float x; // x -- determined by the team
  float y; // y -- determined by the player index 
  y = 160.0f + idx * 106.0f;
  if (team == ELT_BLUE) {
    x = 38.0f;
  } else {
    x = 1830.0f;
  }
  rect = cv::Rect((int)(mImage.cols * (x / 1920.0f)),
    (int)(mImage.rows * (y / 1080.0f)),
    (int)(mImage.cols * (52.0f / 1920.0f)),
    (int)(mImage.rows * (52.0f / 1080.0f)));
  return rect;
}