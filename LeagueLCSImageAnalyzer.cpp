#include "LeagueLCSImageAnalyzer.h"
#include "LeagueTeamData.h"
#include <algorithm>
#include <string>

LeagueLCSImageAnalyzer::LeagueLCSImageAnalyzer(IMAGE_PATH_TYPE ImagePath) : LeagueSpectatorImageAnalyzer(ImagePath) {
}

LeagueLCSImageAnalyzer::~LeagueLCSImageAnalyzer() {

}

PtrLeagueTeamData LeagueLCSImageAnalyzer::AnalyzeTeamData(ELeagueTeams team) {
  PtrLeagueTeamData newTeam = LeagueSpectatorImageAnalyzer::AnalyzeTeamData(team);
  newTeam->teamName = GetTeamName(team);
  newTeam->teamScore = GetTeamGamesWon(team);
  if (bIsDraftBan) {
    GetBans(newTeam->bans, team);
  }
  return newTeam;
}

/*
 * Determine whether or not we are on the draft/ban screen.
 */
bool LeagueLCSImageAnalyzer::AnalyzeIsDraftBan() {
  cv::Mat versusImage = FilterImage_Section_Grayscale_BasicThreshold_Resize(mImage,
    GetVersusTextSection(),
    GetVersusThreshold(),
    GetVersusResizeX(),
    GetVersusResizeY());

  // Make sure versus text is there
  std::string versusText = GetTextFromImage(versusImage, LeagueIdent, std::string("vsVS"));
  std::transform(versusText.begin(), versusText.end(), versusText.begin(), ::toupper);
  if (versusText != GetVersusText()) {
    return false;
  }

  // Make sure bans text is there
  double BansThreshold = GetBansThreshold();
  double BansResizeX = GetBansResizeX();
  double BansResizeY = GetBansResizeY();

  cv::Mat btBans = FilterImage_Section_Channel_BasicThreshold_Resize(mImage,
    GetBansTextSection(ELT_BLUE), 0, BansThreshold, BansResizeX, BansResizeY);
  cv::Mat ptBans = FilterImage_Section_Channel_BasicThreshold_Resize(mImage,
    GetBansTextSection(ELT_PURPLE), 2, BansThreshold, BansResizeX, BansResizeY);
  std::string btBansText = GetTextFromImage(btBans, LeagueIdent, std::string("BANS"));
  std::string ptBansText = GetTextFromImage(ptBans, LeagueIdent, std::string("BANS"));

  std::string BansTextCheck = GetBansText();
  if (btBansText != BansTextCheck || ptBansText != BansTextCheck) {
    return false;
  }

  return true;
}

cv::Rect LeagueLCSImageAnalyzer::GetBansTextSection(ELeagueTeams team) {
  cv::Rect rect;
  if (team == ELT_BLUE) {
    rect = cv::Rect((int)(mImage.cols * (GetBansBlueXStart() / GetRefImageXSize())),
      (int)(mImage.rows * (GetBansYStart() / GetRefImageYSize())),
      (int)(mImage.cols * (GetBansWidth() / GetRefImageXSize())),
      (int)(mImage.rows * (GetBansHeight() / GetRefImageYSize())));
  } else {
    rect = cv::Rect((int)(mImage.cols * (GetBansPurpleXStart() / GetRefImageXSize())),
      (int)(mImage.rows * (GetBansYStart() / GetRefImageYSize())),
      (int)(mImage.cols * (GetBansWidth() / GetRefImageXSize())),
      (int)(mImage.rows * (GetBansHeight() / GetRefImageYSize())));
  }
  return rect;
}

cv::Rect LeagueLCSImageAnalyzer::GetVersusTextSection() {
  return cv::Rect((int)(mImage.cols * (GetVersusXStart() / GetRefImageXSize())),
    (int)(mImage.rows * (GetVersusYStart() / GetRefImageYSize())),
    (int)(mImage.cols * (GetVersusWidth() / GetRefImageXSize())),
    (int)(mImage.rows * (GetVersusHeight() / GetRefImageYSize())));
}

/*
 * Get series score from the header bar up top.
 */
int LeagueLCSImageAnalyzer::GetTeamGamesWon(ELeagueTeams team) {
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

cv::Rect LeagueLCSImageAnalyzer::GetTeamGamesWonSection(ELeagueTeams team) {
  cv::Rect rect;
  if (team == ELT_BLUE) {
    rect = cv::Rect((int)(mImage.cols * (GetTeamGamesBlueXStart() / GetRefImageXSize())),
      (int)(mImage.rows * (GetTeamGamesYStart() / GetRefImageYSize())),
      (int)(mImage.cols * (GetTeamGamesWidth() / GetRefImageXSize())),
      (int)(mImage.rows * (GetTeamGamesHeight() / GetRefImageYSize())));
  } else {
    rect = cv::Rect((int)(mImage.cols * (GetTeamGamesPurpleXStart() / GetRefImageXSize())),
      (int)(mImage.rows * (GetTeamGamesYStart() / GetRefImageYSize())),
      (int)(mImage.cols * (GetTeamGamesWidth() / GetRefImageXSize())),
      (int)(mImage.rows * (GetTeamGamesHeight() / GetRefImageYSize())));
  }
  return rect;
}

/*
 * Retrieve team name. We have to use the user words since we store all the 
 * LCS team names in there.
 */
std::string LeagueLCSImageAnalyzer::GetTeamName(ELeagueTeams team) {
  cv::Mat filterImage = FilterImage_Section_Channel_BasicThreshold_Resize(mImage,
    GetTeamNameSection(team),
    (team == ELT_BLUE) ? 0 : 2,
    GetTeamNameThreshold(), GetTeamNameResizeX(), GetTeamNameResizeY());
  std::string teamText = GetTextFromImage(filterImage, LeagueIdent, std::string("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"), tesseract::PSM_SINGLE_WORD, NULL, NULL, true);
  return teamText;
}

cv::Rect LeagueLCSImageAnalyzer::GetTeamNameSection(ELeagueTeams team) {
  cv::Rect rect;
  if (team == ELT_BLUE) {
    rect = cv::Rect((int)(mImage.cols * (GetTeamNameBlueXStart() / GetRefImageXSize())),
      (int)(mImage.rows * (GetTeamNameYStart() / GetRefImageYSize())),
      (int)(mImage.cols * (GetTeamNameWidth() / GetRefImageXSize())),
      (int)(mImage.rows * (GetTeamNameHeight() / GetRefImageYSize())));
  } else {
    rect = cv::Rect((int)(mImage.cols * (GetTeamNamePurpleXStart() / GetRefImageXSize())),
      (int)(mImage.rows * (GetTeamNameYStart() / GetRefImageYSize())),
      (int)(mImage.cols * (GetTeamNameWidth() / GetRefImageXSize())),
      (int)(mImage.rows * (GetTeamNameHeight() / GetRefImageYSize())));
  }
  return rect;
}

cv::Rect LeagueLCSImageAnalyzer::GetPlayerChampionSection(uint idx, ELeagueTeams team) {
  cv::Rect rect;
  float x; // x -- determined by the team
  float y; // y -- determined by the player index 
  if (bIsDraftBan) {
    y = 118.0f + idx * 90.0f;
    if (team == ELT_BLUE) {
      x = 67.0f;
    } else {
      x = 1137.0f;
    }
    rect = cv::Rect((int)(mImage.cols * (x / 1280.0f)),
      (int)(mImage.rows * (y / 720.0f)),
      (int)(mImage.cols * (51.0f / 1280.0f)),
      (int)(mImage.rows * (51.0f / 720.0f)));
  } else {
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
  }
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
  double x; // x -- determined by the team
  double y; // y -- determined by the player index 
  y = GetCSYStart() + idx * GetCSYIncrement();
  if (team == ELT_BLUE) {
    x = GetCSXBlueStart();
  } else {
    x = GetCSXPurpleStart();
  }
  rect = cv::Rect((int)(mImage.cols * (x / GetRefImageXSize())),
    (int)(mImage.rows * (y / GetRefImageYSize())),
    (int)(mImage.cols * (GetCSXSize() / GetRefImageXSize())),
    (int)(mImage.rows * (GetCSYSize() / GetRefImageYSize())));
  return rect;
}

/*
 * Get Bans! Woo. We assume that the draft screen takes up the entire screen.
 * TODO: Maybe provide the ban percentage as well?
 */
void LeagueLCSImageAnalyzer::GetBans(std::string* outArray, ELeagueTeams team) {
  // There are always at most three bans.
  std::vector<std::string> tmpHints;
  bool tmpB1, tmpB2;
  for (uint i = 0; i < 3; ++i) {
    cv::Rect banSection = GetBansSection(team, i);
    cv::Rect banPercentSection = GetBansPercentageSection(banSection);
    cv::Mat percImg = FilterImage_Section_Grayscale_BasicThreshold_Resize(mImage, banPercentSection, 100.0f, 3.0f, 3.0f);
    std::string banPercentage = GetTextFromImage(percImg, LeagueIdent, std::string("0123456789.%"), tesseract::PSM_SINGLE_WORD);
    if (banPercentage == "") {
      continue;
    }
    cv::Mat img = FilterImage_Section(mImage, banSection);
    outArray[i] = FindMatchingChampion(img, tmpHints, tmpB1, tmpB2);
  }
}

cv::Rect LeagueLCSImageAnalyzer::GetBansSection(ELeagueTeams team, uint idx) {
  cv::Rect rect;
  float x; // x -- determined by the index AND the team
  float y;
  y = 631.0f;
  if (team == ELT_BLUE) {
    x = 17.0f;
  } else {
    x = 1092.0f;
  }
  x += idx * 63.0f;

  rect = cv::Rect((int)(mImage.cols * (x / 1280.0f)),
    (int)(mImage.rows * (y / 720.0f)),
    (int)(mImage.cols * (51.0f / 1280.0f)),
    (int)(mImage.rows * (51.0f / 720.0f)));
  return rect;
}

/*
 * Takes in the result of 'GetBansSection' and add just changes the height and y-position
 * to get the appropriate image for the ban percentage.
 */
cv::Rect LeagueLCSImageAnalyzer::GetBansPercentageSection(cv::Rect banRect) {
  cv::Rect newRect = banRect;
  newRect.y += (int)(mImage.rows * 52.0f / 720.0f);
  newRect.height = (int)(mImage.rows * 21.0f / 720.0f);
  return newRect;
}