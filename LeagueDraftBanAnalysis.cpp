#include "LeagueImageAnalyzer.h"
#include "ConversionUtility.h"
#include "MultiRectangle.h"

/*
 * Determine whether or not we are on the draft/ban screen.
 */
bool LeagueImageAnalyzer::
AnalyzeIsDraftBan() {
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

cv::Rect LeagueImageAnalyzer::
GetBansTextSection(ELeagueTeams team) {
  cv::Rect output;
  if(team == ELT_BLUE) {
    GetCastedPropertyValue<cv::Rect>(LEAGUE_DRAFT_BANS_TEXT_BLUE, output, CreateRectFromString);
  } else {
    GetCastedPropertyValue<cv::Rect>(LEAGUE_DRAFT_BANS_TEXT_RED, output, CreateRectFromString);
  }
  return GetRealRectangle(output);
}

cv::Rect LeagueImageAnalyzer::
GetVersusTextSection() {
  cv::Rect output;
  GetCastedPropertyValue<cv::Rect>(LEAGUE_DRAFT_VERSUS_TEXT, output, CreateRectFromString);
  return GetRealRectangle(output);
}

/*
 * Get Bans! Woo. We assume that the draft screen takes up the entire screen.
 * TODO: Maybe provide the ban percentage as well?
 */
void LeagueImageAnalyzer::
GetBans(std::string* outArray, ELeagueTeams team) {
  // There are always at most three bans.
  std::vector<std::string> tmpHints;
  bool tmpB1, tmpB2;
  for (uint i = 0; i < 3; ++i) {
    cv::Rect banSection = GetBansSection(team, i);
    cv::Rect banPercentSection = GetBansPercentageSection(team, i);
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

cv::Rect LeagueImageAnalyzer::
GetBansSection(ELeagueTeams team, uint idx) {
  MultiRectangle rect;
  GetCastedPropertyValue<MultiRectangle>((team == ELT_BLUE) ? LEAGUE_DRAFT_BANS_CHAMPIONS_BLUE : LEAGUE_DRAFT_BANS_CHAMPIONS_RED, rect, CreateMultiRectFromString);
  return GetRealRectangle(rect.GetRectangle(idx));
}

/*
 * Takes in the result of 'GetBansSection' and changes the height and y-position
 * to get the appropriate image for the ban percentage.
 */
cv::Rect LeagueImageAnalyzer::
GetBansPercentageSection(ELeagueTeams team, uint idx) {
  MultiRectangle rect;
  GetCastedPropertyValue<MultiRectangle>((team == ELT_BLUE) ? LEAGUE_DRAFT_BANS_PERCENT_BLUE : LEAGUE_DRAFT_BANS_PERCENT_RED, rect, CreateMultiRectFromString);
  return GetRealRectangle(rect.GetRectangle(idx));
}



