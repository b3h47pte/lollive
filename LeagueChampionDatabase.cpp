#include "LeagueChampionDatabase.h"
#pragma warning(push)
#pragma warning(disable: 4244) // possible loss of data
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#pragma warning(pop)
#include <fstream>
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

#define LOAD_CHAMPION_DATABASE_ERR(chk, err) \
if (!chk) {\
  std::cerr << "Failed to find the " #err " node." << std::endl;\
  exit(1);\
}

LeagueChampionDatabase::LeagueChampionDatabase() {
  // Load all the data on construction. 
  // TODO: Make the location of the database configurable.
  std::string dir = ConfigManager::Get()->GetStringFromINI(ConfigManager::CONFIG_LEAGUE_FILENAME, ChampionDataSection, DataDirectoryName, std::string(""));
  std::string fileName = ConfigManager::Get()->GetStringFromINI(ConfigManager::CONFIG_LEAGUE_FILENAME, ChampionDataSection, DatabaseFilenameName, std::string(""));
  LoadChampionDatabase(dir, fileName);
}

LeagueChampionDatabase::~LeagueChampionDatabase() {
}

std::shared_ptr<const LeagueChampionDatabase> LeagueChampionDatabase::Get() {
  static std::shared_ptr<const LeagueChampionDatabase> lcd = []() {
    std::shared_ptr<const LeagueChampionDatabase> rr(new LeagueChampionDatabase);
    return rr;
  }();
  return lcd;
}

/*
 * The database.xml file contains everything we need to know about champions.
 * The database.xml file is structured as follows:
 *  1) The entire file is surrounded by <database></database>.
 *  2) The first section is "common" (<common></common). Within this section contains various things that help us note similarities between all champions.
 *        a) "imgpostfix" (<imgpostfix></imgpostfix>). All images end with a common suffix (because Riot has it that way) so this saves us some keystrokes later.
 *  3) The next section is "champions" which contains all the champions League of Legends has.
 *        a) "champion" specifies a champion's data.
 *          - "name": The champion's full name (i.e. 'Dr. Mundo')
 *          - "shortname" : The champion's short name which is used for all identification purpose (i.e. 'DrMundo'). The imgpostfix is attached to this name.
 */
void LeagueChampionDatabase::LoadChampionDatabase(std::string& dir, std::string& fileName) {
  // Read in the XML file
  char* llldb_dir = getenv("LLLDB_DIR");
  std::string envDir = llldb_dir ? std::string(llldb_dir) : "";
  std::string loc = envDir + dir + fileName;
  rapidxml::file<> xmlFile(loc.c_str());
  rapidxml::xml_document<> xmlDoc;
  xmlDoc.parse<0>(xmlFile.data());

  // Get the base node ('database')
  rapidxml::xml_node<>* baseNode = xmlDoc.first_node("database");
  LOAD_CHAMPION_DATABASE_ERR(baseNode, "database");
  
  // Get the common node
  rapidxml::xml_node<>* commonNode = baseNode->first_node("common");
  LOAD_CHAMPION_DATABASE_ERR(commonNode, "common");

  // Get common values.
  // Get the imgpostfix
  rapidxml::xml_node<>* imgPostFixNode = commonNode->first_node("imgpostfix");
  LOAD_CHAMPION_DATABASE_ERR(imgPostFixNode, "imgpostfix");
  mImagePostfix = imgPostFixNode->value();

  // Get all champions
  rapidxml::xml_node<>* championsNode = baseNode->first_node("champions");
  LOAD_CHAMPION_DATABASE_ERR(championsNode, "champions");

  rapidxml::xml_node<>* championNode = championsNode->first_node("champion");
  std::string imageDir = ConfigManager::Get()->GetStringFromINI(ConfigManager::CONFIG_LEAGUE_FILENAME, ChampionDataSection, ImageDirectoryName, std::string(""));
  while (championNode) {
    PtrLeagueChampionData data(new LeagueChampionData);
    data->longName = championNode->first_node("name")->value();
    data->shortName = championNode->first_node("shortname")->value();
    data->image = cv::imread(envDir + dir + imageDir + data->shortName + mImagePostfix, cv::IMREAD_UNCHANGED);
    mData[data->shortName] = data;
    championNode = championNode->next_sibling("champion");
  }
}
