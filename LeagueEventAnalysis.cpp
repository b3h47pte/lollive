#include "LeagueImageAnalyzer.h"
#include "MultiRectangle.h"
#include "ConversionUtility.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/superres.hpp"

cv::Rect LeagueImageAnalyzer::GetMinibarSection(uint idx) {
  MultiRectangle rect;
  GetCastedPropertyValue<MultiRectangle>(LEAGUE_EVENT_MINIBAR, rect, CreateMultiRectFromString);
  return GetRealRectangle(rect.GetRectangle(idx));
}

cv::Rect LeagueImageAnalyzer::GetAnnouncementSection() {
  cv::Rect output;
  GetCastedPropertyValue<cv::Rect>(LEAGUE_EVENT_ANNOUNCE, output, CreateRectFromString);
  return GetRealRectangle(output);
}

cv::Rect LeagueImageAnalyzer::GetMinibarOriginalResolution() {
  cv::Rect newRect = cv::Rect(0, 0, (int)GetMinibarEventWidth(), (int)GetMinibarEventHeight());
  return GetRealRectangle(newRect);
}

cv::Rect LeagueImageAnalyzer::GetMinibarObjectiveIconOriginalResolution() {
  cv::Rect output;
  GetCastedPropertyValue<cv::Rect>(LEAGUE_EVENT_MINIBAR_MAIN_ICON, output, CreateRectFromString);
  return GetRealRectangle(output);
}

cv::Rect LeagueImageAnalyzer::GetMinibarSupportingIconOriginalResolution() {
  cv::Rect output;
  GetCastedPropertyValue<cv::Rect>(LEAGUE_EVENT_MINIBAR_SUPPORTING_ICON, output, CreateRectFromString);
  return GetRealRectangle(output);
}

/*
 * Get the announcement for the inhibitor kill and attribute it to a team.
 * Sadly we can't attribute it to an actual player since we don't know player names usually.
 */
PtrLeagueEvent LeagueImageAnalyzer::GetAnnouncementEvent() {
  cv::Mat filterImage, mask;
  cv::cvtColor(mImage, filterImage, cv::COLOR_BGR2HSV);
  cv::Rect section = GetAnnouncementSection();
  filterImage = FilterImage_Section(filterImage, section);
  cv::inRange(filterImage, cv::Scalar(GetAnnouncementLowH(), GetAnnouncementLowS(), GetAnnouncementLowV()), 
    cv::Scalar(GetAnnouncementHighH(), GetAnnouncementHighS(), GetAnnouncementHighV()), filterImage);
  std::string result = GetTextFromImage(filterImage, EnglishIdent, std::string("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvxywz!"), tesseract::PSM_SINGLE_BLOCK);
  std::transform(result.begin(), result.end(), result.begin(), ::toupper);

  // This text shouldn't be that intelligible since we have a bunch of other stuff in here, like the background and any images.
  // The best we can do is hope to find keywords in here. Luckily we're only depending on the announcement to get the inhibitor announcement
  // right.
  PtrLeagueEvent newEvent(new LeagueEvent());
  
  // The only thing we're looking for is 'inhibitor'.
  if (result.find("INHIBITOR") != std::string::npos) {
    newEvent->EventId = ELEI_INHIB;
    // Then we want to know if we killed the red or blue inhibitor.
    if (result.find("RED") != std::string::npos) {
      newEvent->RelevantTeam = ELT_BLUE;
    } else if (result.find("BLUE") != std::string::npos) {
      newEvent->RelevantTeam = ELT_PURPLE;
    } else {
      // Can't really do anything with this, sadly.
      newEvent->EventId = ELEI_UNKNOWN;
    }
  }

  return newEvent;
}

/*
 * The rest of the live events [besides inhibitor kills] will get picked up here.
 * Assume that there is some max number of events at any given time.
 */
std::shared_ptr<MapPtrLeagueEvent> LeagueImageAnalyzer::GetMinibarEvents() {
  std::shared_ptr<MapPtrLeagueEvent> RetArray(new MapPtrLeagueEvent());
  std::string killObjectives[ELKT_MAX];
  killObjectives[ELKT_KILL] = GetKillObjective();
  killObjectives[ELKT_MULTI] = GetKillStreakObjective();
  killObjectives[ELKT_SHUTDOWN] = GetKillShutdownObjective();
  std::string targetObjective[12] = { GetDragonObjective(), GetPurpleOuterTurretObjective(), GetBlueOuterTurretObjective(), GetBaronObjective(),
    GetPurpleInnerTurretObjective(), GetBlueInnerTurretObjective(), GetPurpleInhibTurretObjective(), GetBlueInhibTurretObjective(),
    GetPurpleNexusTurretObjective(), GetBlueNexusTurretObjective(), GetPurpleInhibitor(), GetBlueInhibitor() };

  // Do the same event analysis for each event. 
  // Each subclass will tell us where to look for this minibar.
  for (int i = 0; i < GetMinibarEventMax(); ++i) {
    cv::Rect eventSection = GetMinibarSection(i);
    cv::Mat filterImage = FilterImage_Section(mImage, eventSection);

    // What's the background color?
    cv::Vec3b bgColor = filterImage.at<cv::Vec3b>(cv::Point((int)(filterImage.cols*GetBackgroundColorXPercentage()), GetBackgroundColorY()));
    cv::Point killIconLocation;
    // Determine which team performed this action based on the background color
    ELeagueTeams instigatorTeam = ELT_UNKNOWN;
    if (bgColor[GetMinibarBackgroundAllyChannel()] > bgColor[GetMinibarBackgroundEnemyChannel()]) {
      // Green = Blue Team
      instigatorTeam = ELT_BLUE;
    } else {
      instigatorTeam = ELT_PURPLE;
    }

    // First orient ourselves by finding the 'kill' icon. 
    // This can either be a regular kill, multi-kill, or shutdown icon.
    bool bFoundMatch = false;
    int j = 0;
    for (j = 0; j < ELKT_MAX; ++j) {
      cv::Mat origKillImg = LeagueEventDatabase::Get()->GetObjectiveImage(killObjectives[j]);
      cv::Mat killImg;
      origKillImg.copyTo(killImg);
      
      double maxVal = SobelTemplateMatching(killImg, filterImage, bgColor, killIconLocation);

      if (maxVal >= GetKillIconSobelThreshold()) {
        bFoundMatch = true;
        break;
      }
    }

    // If we don't find a kill symbol, nothing happened here.
    if (!bFoundMatch) {
      continue;
    }

    PtrLeagueEvent newEvent(new LeagueEvent());
    newEvent->RelevantTeam = instigatorTeam;
    newEvent->KillType = (ELeagueKillType)j;

    // Otherwise, we know what kind of kill we did.
    // Now figure out what we did the kill on. This should be a turret, baron, dragon, or a champion.
    // But we don't want to figure out what the champion is until we determine it's not anything else (that'd be expensive!).
    bFoundMatch = false;

    cv::Rect origMinibarResolution = GetMinibarOriginalResolution();
    cv::Rect origIconResolution = GetMinibarObjectiveIconOriginalResolution();

    double minibarScaleX = 1.0;
    double minibarScaleY = 1.0;
    for (j = 0; j < 10; ++j) {
      // DID WE FIND IT?
      cv::Mat targetImg = LeagueEventDatabase::Get()->GetObjectiveImage(targetObjective[j]);
      ShowImage(targetImg);
      // Need to resize the image to become the appropriate resolution.
      // We know what size the image is at the original resolution. We know what the current resolution is so we can figure out
      // what we need the size of the objective image.
      minibarScaleX = ((double)filterImage.cols * origIconResolution.width / origMinibarResolution.width) / targetImg.cols;
      minibarScaleY = ((double)filterImage.rows * origIconResolution.height / origMinibarResolution.height) / targetImg.rows;

      cv::Mat useTargetImg = FilterImage_Resize(targetImg, 
        minibarScaleX, minibarScaleY);

      cv::Point objIconLocation;
      double maxVal = TemplateMatching(useTargetImg, filterImage, bgColor, objIconLocation);
      if (maxVal < GetObjectiveIconSobelThreshold()) {
        continue;
      } else {
        break;
      }
    }

    // If we find the match, then we want to remember what we killed, which will determine the event ID as long as any
    // 'AdditionalInfo'
    switch (j) {
    case 0: // Dragon
      newEvent->EventId = ELEI_DRAGON;
      break;
    case 3: // Baron
      newEvent->EventId = ELEI_BARON;
      break;
    case 1: // Purple Outer Turret
    case 2: // Blue Outer Turret 
    case 4: // Purple Inner
    case 5: // Blue Inner
    case 6: // Purple Inhib Turret
    case 7: // Blue Inhib Turret
    case 8: // Purple Nexus Turret
    case 9: // Blue Nexus Turret
      newEvent->EventId = ELEI_TURRET;
      newEvent->AdditionalInfo = targetObjective[j];
      break;
    case 10: // Purple Inhibitor
    case 11: // Blue Inhibitor
      newEvent->EventId = ELEI_INHIB;
      newEvent->AdditionalInfo = targetObjective[j];
      break;
    default: // Champion Kill
      newEvent->EventId = ELEI_CHAMP;
      break;
    }

    // Now we need to find all relevant champions related to this event by doing a reverse search. Given the list of champions we think could exist
    // and the size that we would expect them to exist at, we try to find it anywhere on the minibar.
    std::vector<std::string> relevantChampions;
    cv::Point outputRect;
    PtrLeagueTeamData bt = RetrieveData<PtrLeagueTeamData>(mData, std::string("BlueTeam"));
    PtrLeagueTeamData pt = RetrieveData<PtrLeagueTeamData>(mData, std::string("PurpleTeam"));
    for (int i = 0; i < 5; ++i) {
      relevantChampions.push_back(bt->players[i]->champion);
      relevantChampions.push_back(pt->players[i]->champion);
    }

    // Get main champions
    double iconScaleX = origMinibarResolution.width / filterImage.cols;
    double iconScaleY = origMinibarResolution.height / filterImage.rows;

    for (int i = 0; i < 10; ++i) {
      int targetX = (int)(GetMinibarObjectiveIconOriginalResolution().width * iconScaleX);
      int targetY = (int)(GetMinibarObjectiveIconOriginalResolution().height * iconScaleY);
      if (ReverseMatchChampion(filterImage, relevantChampions[i], targetX, targetY, outputRect)) {
        // We know which team the champion is on based on which side of the 'kill' image it is.
        if (outputRect.x < killIconLocation.x) { 
          newEvent->PlayerInstigator = relevantChampions[i];
        } else if (newEvent->EventId == ELEI_CHAMP) {
          newEvent->AdditionalInfo = relevantChampions[i];
        }
      }
    }

    // Get supporting champions
    for (int i = 0; i < 10; ++i) {
      int targetX = (int)(GetMinibarSupportingIconOriginalResolution().width * iconScaleX);
      int targetY = (int)(GetMinibarSupportingIconOriginalResolution().height * iconScaleY);
      if (ReverseMatchChampion(filterImage, relevantChampions[i], targetX, targetY, outputRect)) {
        newEvent->SupportingPlayers.push_back(relevantChampions[i]);
      }
    }

    newEvent->Print();

    if (newEvent->EventId == ELEI_UNKNOWN) {
      continue;
    }

    (*RetArray)[newEvent->GetIdentifier()] = newEvent;
  }
  return RetArray;
}
