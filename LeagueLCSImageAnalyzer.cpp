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
  double x; // x -- determined by the team
  double y; // y -- determined by the player index 
  if (bIsDraftBan) {
    y = GetPlayerChampDraftYStart() + idx * GetPlayerChampDraftYIncr();
    if (team == ELT_BLUE) {
      x = GetPlayerChampDraftBlueXStart();
    } else {
      x = GetTeamGamesPurpleXStart();
    }
    rect = cv::Rect((int)(mImage.cols * (x / GetRefImageXSize())),
      (int)(mImage.rows * (y / GetRefImageYSize())),
      (int)(mImage.cols * (GetPlayerChampDraftWidth() / GetRefImageXSize())),
      (int)(mImage.rows * (GetPlayerChampDraftHeight() / GetRefImageYSize())));
  } else {
    y = GetPlayerChampYStart() + idx * GetPlayerChampYIncr();
    if (team == ELT_BLUE) {
      x = GetPlayerChampBlueXStart();
    } else {
      x = GetPlayerChampPurpleXStart();
    }
    rect = cv::Rect((int)(mImage.cols * (x / GetRefImageXSize())),
      (int)(mImage.rows * (y / GetRefImageYSize())),
      (int)(mImage.cols * (GetPlayerChampWidth() / GetRefImageXSize())),
      (int)(mImage.rows * (GetPlayerChampHeight() / GetRefImageYSize())));
  }
  return rect;
}

cv::Rect LeagueLCSImageAnalyzer::GetPlayerNameSection(uint idx, ELeagueTeams team) {
  cv::Rect rect;
  double x; // x -- determined by the team
  double y; // y -- determined by the player index 
  y = GetPlayerNameYStart() + idx * GetPlayerNameYIncr();
  if (team == ELT_BLUE) {
    x = GetPlayerNameBlueXStart();
  } else {
    x = GetPlayerNamePurpleXStart();
  }
  rect = cv::Rect((int)(mImage.cols * (x / GetRefImageXSize())),
    (int)(mImage.rows * (y / GetRefImageYSize())),
    (int)(mImage.cols * (GetPlayerNameWidth() / GetRefImageXSize())),
    (int)(mImage.rows * (GetPlayerNameHeight() / GetRefImageYSize())));
  return rect;
}


cv::Rect LeagueLCSImageAnalyzer::GetPlayerKDASection(uint idx, ELeagueTeams team) {
  cv::Rect rect;
  double x; // x -- determined by the team
  double y; // y -- determined by the player index 
  y = GetPlayerKDAYStart() + idx * GetPlayerKDAYIncr();
  if (team == ELT_BLUE) {
    x = GetPlayerKDABlueXStart();
  } else {
    x = GetPlayerKDAPurpleXStart();
  }
  rect = cv::Rect((int)(mImage.cols * (x / GetRefImageXSize())),
    (int)(mImage.rows * (y / GetRefImageYSize())),
    (int)(mImage.cols * (GetPlayerKDAWidth() / GetRefImageXSize())),
    (int)(mImage.rows * (GetPlayerKDAHeight() / GetRefImageYSize())));
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
    cv::Mat percImg = FilterImage_Section_Grayscale_BasicThreshold_Resize(mImage, banPercentSection, 
      GetBanChampThreshold(), 
      GetBanChampResizeX(), 
      GetBanChampResizeY());
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
  double x; // x -- determined by the index AND the team
  double y;
  y = GetBanChampYStart();
  if (team == ELT_BLUE) {
    x = GetBanChampBlueXStart();
  } else {
    x = GetBanChampPurpleXStart();
  }
  x += idx * GetBanChampXIncrement();

  rect = cv::Rect((int)(mImage.cols * (x / GetRefImageXSize())),
    (int)(mImage.rows * (y / GetRefImageYSize())),
    (int)(mImage.cols * (GetBanChampWidth() / GetRefImageXSize())),
    (int)(mImage.rows * (GetBanChampHeight() / GetRefImageYSize())));
  return rect;
}

/*
 * Takes in the result of 'GetBansSection' and changes the height and y-position
 * to get the appropriate image for the ban percentage.
 */
cv::Rect LeagueLCSImageAnalyzer::GetBansPercentageSection(cv::Rect banRect) {
  cv::Rect newRect = banRect;
  newRect.y += (int)(mImage.rows * GetBanPercentYAdjust() / GetRefImageYSize());
  newRect.height = (int)(mImage.rows * GetBanPercentHeightAdjust() / GetRefImageYSize());
  return newRect;
}

cv::Rect LeagueLCSImageAnalyzer::GetMinibarObjectiveIconOriginalResolution() {
  cv::Rect newRect = cv::Rect(0, 0, (int)GetMinibarObjectiveIconTargetX(), (int)GetMinibarObjectiveIconTargetY());
  return newRect;
}

cv::Rect LeagueLCSImageAnalyzer::GetMinibarSupportingIconOriginalResolution() {
  cv::Rect newRect = cv::Rect(0, 0, (int)GetMinibarSupportingIconTargetX(), (int)GetMinibarSupportingIconTargetY());
  return newRect;
}

/*
 * Get the item section.
 */
cv::Rect LeagueLCSImageAnalyzer::GetPlayerItemSection(uint playerIdx, ELeagueTeams team, uint itemIdx) {
  double x = 0.0f;
  double y = 0.0f;
  if (team == ELT_BLUE) {
    x = GetPlayerItemBlueX();
  } else {
    x = GetPlayerItemPurpleX();
  }
  x += itemIdx * GetPlayerItemXIncr();
  y = GetPlayerItemY() + playerIdx * GetPlayerItemYIncr();
  cv::Rect newRect = cv::Rect((int)(mImage.cols * (x / GetRefImageXSize())),
    (int)(mImage.rows * (y / GetRefImageYSize())),
    (int)(mImage.cols * (GetPlayerItemWidth() / GetRefImageXSize())),
    (int)(mImage.rows * (GetPlayerItemHeight() / GetRefImageYSize())));

  return newRect;
}