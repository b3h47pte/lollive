#include "LeagueImageAnalyzer.h"
#include "ConversionUtility.h"

/*
 * Retrieve the team's kills from the header bar at the top.
 * However, since we want to be able to read the team's kills separately, 
 * we must apply color filters and grab the right image section based on the team property.
 * Offload those tasks to utility functions.
 */
int LeagueImageAnalyzer::AnalyzeTeamKills(ELeagueTeams team) {
  // TODO: Configurable
  cv::Mat filterImage = FilterImage_Section_Channel_BasicThreshold_Resize(mImage,
    GetTeamKillsSection(team),
    (team == ELT_BLUE) ? 0 : 2,
    GetTeamKillsThreshold(), GetTeamKillsResizeX(), GetTeamKillsResizeY());
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
cv::Rect LeagueImageAnalyzer::GetTeamKillsSection(ELeagueTeams team) {
  cv::Rect rect;
  GetCastedPropertyValue<cv::Rect>((team == ELT_BLUE) ? LEAGUE_TEAM_KILLS_BLUE : LEAGUE_TEAM_KILLS_RED, rect, CreateRectFromString);
  return GetRealRectangle(rect);
}

/*
 * Get the team's gold. League of Legends displays gold as follows:
 * XX.Xk OR X.Xk. Players start with 2.4k gold.
 */
int LeagueImageAnalyzer::AnalyzeTeamGold(ELeagueTeams team) {
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
*/
cv::Rect LeagueImageAnalyzer::GetTeamGoldSection(ELeagueTeams team) {
  cv::Rect rect;
  GetCastedPropertyValue<cv::Rect>((team == ELT_BLUE) ? LEAGUE_TEAM_GOLD_BLUE : LEAGUE_TEAM_GOLD_RED, rect, CreateRectFromString);
  return GetRealRectangle(rect);
}

/*
 * Gets the number of tower kills each team has from the spectator header bar.
 */
int LeagueImageAnalyzer::AnalyzeTeamTowerKills(ELeagueTeams team) {
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
*/
cv::Rect LeagueImageAnalyzer::GetTeamTowerKillSection(ELeagueTeams team) {
  cv::Rect rect;
  GetCastedPropertyValue<cv::Rect>((team == ELT_BLUE) ? LEAGUE_TEAM_TOWERS_BLUE : LEAGUE_TEAM_TOWERS_RED, rect, CreateRectFromString);
  return GetRealRectangle(rect);
}

/*
 * Get series score from the header bar up top.
 */
int LeagueImageAnalyzer::GetTeamGamesWon(ELeagueTeams team) {
  cv::Mat filterImage = FilterImage_Section_Channel_BasicThreshold_Resize(mImage,
    GetTeamGamesWonSection(team),
    (team == ELT_BLUE) ? 0 : 2,
    GetTeamGamesThreshold(), GetTeamGamesResizeX(), GetTeamGamesResizeY());
  std::string teamText = GetTextFromImage(filterImage, LeagueIdent, std::string("0123456789"), tesseract::PSM_SINGLE_CHAR);
  try {
    return std::stoi(teamText, NULL);
  } catch (...) {
    return -1;
  }
}

cv::Rect LeagueImageAnalyzer::GetTeamGamesWonSection(ELeagueTeams team) {
  cv::Rect rect;
  GetCastedPropertyValue<cv::Rect>((team == ELT_BLUE) ? LEAGUE_TEAM_GAMES_BLUE : LEAGUE_TEAM_GAMES_RED, rect, CreateRectFromString);
  return GetRealRectangle(rect);
}

/*
 * Retrieve team name. We have to use the user words since we store all the 
 * LCS team names in there.
 */
std::string LeagueImageAnalyzer::GetTeamName(ELeagueTeams team) {
  cv::Mat filterImage = FilterImage_Section_Channel_BasicThreshold_Resize(mImage,
    GetTeamNameSection(team),
    (team == ELT_BLUE) ? 0 : 2,
    GetTeamNameThreshold(), GetTeamNameResizeX(), GetTeamNameResizeY());
  std::string teamText = GetTextFromImage(filterImage, LeagueIdent, std::string("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"), tesseract::PSM_SINGLE_WORD, NULL, NULL, true);
  return teamText;
}

cv::Rect LeagueImageAnalyzer::GetTeamNameSection(ELeagueTeams team) {
  cv::Rect rect;
  GetCastedPropertyValue<cv::Rect>((team == ELT_BLUE) ? LEAGUE_TEAM_NAME_BLUE : LEAGUE_TEAM_NAME_RED, rect, CreateRectFromString);
  return GetRealRectangle(rect);
}
