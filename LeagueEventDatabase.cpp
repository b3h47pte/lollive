#include "LeagueEventDatabase.h"
#pragma warning(push)
#pragma warning(disable: 4244) // possible loss of data
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#pragma warning(pop)

#define LOAD_EVENT_DATABASE_ERR(chk, err) \
if (!chk) {\
  std::cerr << "Failed to find the " #err " node." << std::endl; \
  exit(1); \
}

LeagueEventDatabase::LeagueEventDatabase() {
  // Load all the data on construction. 
  std::string dir = GetObjectiveDataDirectory();
  std::string fileName = GetObjectiveDatabaseFilename();
  LoadEventDatabase(dir, fileName);
}

LeagueEventDatabase::~LeagueEventDatabase() {

}

std::shared_ptr<const LeagueEventDatabase> LeagueEventDatabase::Get() {
  static std::shared_ptr<const LeagueEventDatabase> lcd = []() {
    std::shared_ptr<const LeagueEventDatabase> rr(new LeagueEventDatabase);
    return rr;
  }();
  return lcd;
}

void LeagueEventDatabase::LoadEventDatabase(std::string& dir, std::string& fileName) {
  // Read in the XML file
  std::string envDir(getenv("LLLDB_DIR"));
  std::string loc = std::string(envDir) + dir + fileName;
  rapidxml::file<> xmlFile(loc.c_str());
  rapidxml::xml_document<> xmlDoc;
  xmlDoc.parse<0>(xmlFile.data());

  // Get the base node ('database')
  rapidxml::xml_node<>* baseNode = xmlDoc.first_node("database");
  LOAD_EVENT_DATABASE_ERR(baseNode, "database");

  // Get all items
  rapidxml::xml_node<>* allObjsNode = baseNode->first_node("objectives");
  LOAD_EVENT_DATABASE_ERR(allObjsNode, "objectives");

  rapidxml::xml_node<>* objNode = allObjsNode->first_node("objective");
  std::string imageDir = GetImageDirectory();
  while (objNode) {
    std::string pathToImage = std::string(envDir) + dir + imageDir + objNode->first_node("image")->value();
    cv::Mat newImage = cv::imread(pathToImage, cv::IMREAD_UNCHANGED);
    mImageMapping[objNode->first_node("id")->value()] = newImage;
    objNode = objNode->next_sibling("objective");
  }
}
