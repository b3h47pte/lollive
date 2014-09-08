#include "LeagueImageAnalyzer.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/superres.hpp"
#include <ctime>
#include <algorithm>
#include <thread>
#include "LeagueConstants.h"
#include "LeagueItemData.h"

LeagueImageAnalyzer::LeagueImageAnalyzer(IMAGE_PATH_TYPE ImagePath): ImageAnalyzer(ImagePath) {
  ChampionDatabase = LeagueChampionDatabase::Get();
  ItemDatabase = LeagueItemDatabase::Get();

  bIs1080p = (mImage.cols == 1920 && mImage.rows == 1080);

  // Everything is much better in 1080p
  cv::resize(mImage, mImage, cv::Size(1920, 1080), 0.0, 0.0, cv::INTER_LANCZOS4);
}

LeagueImageAnalyzer::~LeagueImageAnalyzer() {

}

/*
 * Call functions to get various pieces of data that are of interest.
 * If too many fail [threshold TBD], then fail.
 */
bool LeagueImageAnalyzer::Analyze() {
  bool ret = true;
  if (mImage.empty()) {
    return false;
  }
  
  std::cout << "LoL Frame Data: " << std::endl;
  std::clock_t begin = std::clock();

  // Whether we're currently in the draft ban stage.
  bIsDraftBan = AnalyzeIsDraftBan();

  // Current match time.
  int time = AnalyzeMatchTime();
  std::shared_ptr<GenericData<int>> timeProp(new GenericData<int>(time));
  mDataMutex.lock();
  (*mData)["CurrentTime"] = timeProp;
  mDataMutex.unlock();
  std::cout << "Current Time: " << time << std::endl;

  PtrLeagueTeamData blueTeam;
  // Team Data.
  std::thread btThread(std::bind([&]() {
    blueTeam = AnalyzeTeamData(ELT_BLUE);
  }));

  PtrLeagueTeamData purpleTeam;
  std::thread ptThread(std::bind([&]() {
    purpleTeam = AnalyzeTeamData(ELT_PURPLE);
  }));

  btThread.join();
  ptThread.join();

  std::shared_ptr<GenericData<PtrLeagueTeamData>> blueTeamProp(new GenericData<PtrLeagueTeamData>(blueTeam));
  (*mData)["BlueTeam"] = blueTeamProp;
  blueTeam->Print();

  std::shared_ptr<GenericData<PtrLeagueTeamData>> purpleTeamProp(new GenericData<PtrLeagueTeamData>(purpleTeam));
  (*mData)["PurpleTeam"] = purpleTeamProp;
  purpleTeam->Print();

  std::clock_t end = std::clock();
  double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
  std::cout << "Elapsed Seconds: " << elapsed_secs << std::endl;

  bIsFinished = true;
  return ret;
}

/*
 * Get all the information we can about this team (gold ,kills, who's on the team, etc.)
 * It is perfectly fine if some of that information is unavailable. Pass off the duty of
 * identifying invalid values to the user of the data.
 */
PtrLeagueTeamData LeagueImageAnalyzer::AnalyzeTeamData(ELeagueTeams team) {
  PtrLeagueTeamData newTeam(new LeagueTeamData);
  newTeam->team = team;
  newTeam->kills = AnalyzeTeamKills(team);
  newTeam->gold = AnalyzeTeamGold(team);
  newTeam->towerKills = AnalyzeTeamTowerKills(team);
  for (uint i = 0; i < 5; ++i) {
    newTeam->players[i] = AnalyzePlayerData(i, team);
  }
  return newTeam;
}

/*
 * Gets all the information we can gather about this player. Obviously the index is kind of vague
 * so it is up to subclasses to determine what this index should mean. But we can guarantee that 
 * we will call AnalyzePlayerData with indexes 0 to 4.
 */
PtrLeaguePlayerData LeagueImageAnalyzer::AnalyzePlayerData(uint idx, ELeagueTeams team) {
  PtrLeaguePlayerData newPlayer(new LeaguePlayerData);
  newPlayer->team = team;
  newPlayer->playerIdx = idx;
  newPlayer->champion = AnalyzePlayerChampion(idx, team, &newPlayer->isDead, &newPlayer->isLowHealth, &newPlayer->level);
  newPlayer->name = AnalyzePlayerName(idx, team);
  AnalyzePlayerScore(idx, team, &newPlayer->kills, &newPlayer->deaths, &newPlayer->assists, &newPlayer->cs);
  for (uint i = 0; i < 7; ++i) {
    newPlayer->items[i] = AnalyzePlayerItem(idx, team, i);
  }
  return newPlayer;
}

/*
 * From the given image, we want to give the champion that is the best match as well as whether or not the champion is low on health or is dead.
 * Optionally, we may provide this function a list of 'hints.' When that is the case, we will only look at champions that are contained within the
 * hints.
 */
std::string LeagueImageAnalyzer::FindMatchingChampion(cv::Mat filterImage, std::vector<std::string>& championHints, bool& isLowOnHealth, bool& isDead) {
  std::clock_t begin = std::clock();

  // Split the image into x_dim * y_dim parts (generally want to have ~25 solid pieces to compare).
  // TODO: Make this configurable
  int x_dim = 6;
  int y_dim = 6;

  int h_buckets = 30;
  int s_buckets = 32;

  int totalEle = x_dim * y_dim;
  cv::Mat* filterSubImages = new cv::MatND[x_dim * y_dim];
  cv::Mat* filterSubImagesNoRed = new cv::MatND[x_dim * y_dim];

  cv::MatND* filterSubHSHists = new cv::MatND[x_dim * y_dim]; // Hue and Saturation. Used to determine who the champion actually is.
  cv::MatND* filterSubHSHistNoRed = new cv::MatND[x_dim * y_dim]; // Hue and Saturation. Used to determine who the champion is when the champion is low on health.
  cv::MatND* filterSubVHists = new cv::MatND[x_dim * y_dim]; // Value (from HSV). Used to determine if the champion be dead.
  SplitImage(filterImage, x_dim, y_dim, &filterSubImages);
  int cc = 0;
  std::for_each(filterSubImages, filterSubImages + totalEle, [&](cv::Mat inImg) {
    filterSubHSHists[cc] = CreateHSHistogram(inImg, h_buckets, s_buckets);
    filterSubVHists[cc] = CreateVHistogram(inImg, 10);

    // Filter out red channel
    filterSubImagesNoRed[cc] = FilterImage_2Channel(filterSubImages[cc], 0, 1, 0.0);
    filterSubHSHistNoRed[cc] = CreateHSHistogram(filterSubImagesNoRed[cc], h_buckets, s_buckets);

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
    if (championHints.size() != 0 && std::find(championHints.begin(), championHints.end(), pair.second->shortName) == championHints.end()) {
      continue;
    }
    // Make this image as close to the input image as possible
    baseImage = FilterImage_Resize(pair.second->image, (float)filterImage.cols / pair.second->image.cols, (float)filterImage.rows / pair.second->image.rows);
    cv::GaussianBlur(baseImage, baseImage, cv::Size(3, 3), 0.0); // Blur the image since the input is probably blurry too.
    SplitImage(baseImage, x_dim, y_dim, &baseSubImages);
    std::for_each(baseSubImages, baseSubImages + totalEle, [&](cv::Mat inImg) {
      baseSubHSHists[cc] = CreateHSHistogram(inImg, h_buckets, s_buckets);
      baseSubVHists[cc] = CreateVHistogram(inImg, 10);

      // Filter out red channel
      baseSubImagesNoRed[cc] = FilterImage_2Channel(baseSubImages[cc], 0, 1, 0.0);
      baseSubHSHistsNoRed[cc] = CreateHSHistogram(baseSubImagesNoRed[cc], h_buckets, s_buckets);
      ++cc;
    });
    cc = 0;

    // Now compare the histograms to see if the champion matches & and if the champion is dead.
    double totalScore = 0.0; // A higher score indicates an increased likeliness that the champion is correct.
    double deadScore = 0.0;  // A higher score indicates an increased likeliness that the champion is alive.
    double noRedScore = 0.0;
    int count = 0;
    for (int i = 0; i < totalEle; ++i) {
      // Skip the bottom right corner
      if (i / y_dim >= y_dim - 2 && i % x_dim >= x_dim - 2) continue;

      totalScore += cv::compareHist(filterSubHSHists[i], baseSubHSHists[i], cv::HISTCMP_CORREL);
      deadScore += cv::compareHist(filterSubVHists[i], baseSubVHists[i], cv::HISTCMP_CORREL);
      noRedScore += cv::compareHist(filterSubHSHistNoRed[i], baseSubHSHistsNoRed[i], cv::HISTCMP_CORREL);
      ++count;
    }
    totalScore /= count;
    deadScore /= count;
    noRedScore /= count;

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

  // TODO: Configurable
  isDead = (championDeadScore < 0.5);

  // At this point, there are two possibilities:
  //  a) The two outputs match (for the RGB image and the GB image). In this case, it should be clear that the character
  //     isn't low on health, and that whatever the RGB image's result is should be taken.
  //  b) The two outputs don't match! In this case we should take whichever gives a higher score. If the no red image
  //     has the high score, then we know the champion is low on health 

  // Set this to a default value first
  isLowOnHealth = false;

  if (championMatch != championNoRedMatch) {
    championMatch = (closestMatch > bestNoRedScore) ? championMatch : championNoRedMatch;

    // No red score is higher, which means taking red out of the image helped our analysis => LOW ON HEALTH. RED FLASHES. LIGHTS. EVERYWHERE. RUN.
    if (closestMatch < bestNoRedScore) {
      isLowOnHealth = true;

      // But we can't be dead if we're low on health
      isDead = false;
    }
  }

  if (championMatch == LEAGUE_NO_CHAMPION) {
    championMatch = "";
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

  std::clock_t end = std::clock();
  double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
  std::cout << "Find Champion Elapsed Seconds: " << elapsed_secs << std::endl;

  return championMatch;
}

/*
 * Given an item image, we need to use template matching to find the item's image in the
 * giant image with all the items. 
 * 
 * However, since template matching doesn't do too well with scaling, we rescale the 
 * base image to match the template's size.
 * 
 * TODO: Use an image pyramid to speed this up. Also use multiple indicators to increase accuracy.
 */
std::string LeagueImageAnalyzer::AnalyzePlayerItem(uint playerIdx, ELeagueTeams team, uint itemIdx) {
  std::clock_t begin = std::clock();
  cv::Mat itemImage = FilterImage_Section(mImage, GetPlayerItemSection(playerIdx, team, itemIdx));
  cv::Mat baseImage = ItemDatabase->GetDatabaseImage().clone();

  // Resize the base image so that each individual cell matches the size of the item image.
  float factor = (float)itemImage.cols / LEAGUE_ITEM_SQUARE_SIZE;
  baseImage = FilterImage_Resize(baseImage, factor, factor);

  // Do template matching to find where we have a match
  cv::Mat matchResult;
  cv::matchTemplate(baseImage, itemImage, matchResult, cv::TM_SQDIFF_NORMED);
  cv::normalize(matchResult, matchResult, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

  // Now find the minimum and maximum results
  double minVal;
  double maxVal;
  cv::Point minPoint; // WHILE WE USE TM_SQDIFF_NORMED, MINPOINT IS THE ACTUAL POINT WE WANT TO USE.
  cv::Point maxPoint;
  cv::minMaxLoc(matchResult, &minVal, &maxVal, &minPoint, &maxPoint, cv::Mat());

  std::clock_t end = std::clock();
  double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
  std::cout << " Find Item Elapsed Seconds: " << elapsed_secs << std::endl;

  // Convert the point to an actual index.
  int y_idx = minPoint.y / itemImage.rows;
  int x_idx = minPoint.x / itemImage.cols;
  PtrLeagueItemData item = ItemDatabase->GetItem(x_idx, y_idx);
  if (!item || item->IsInvalid()) {
    return "";
  }

  return item->itemID;
}