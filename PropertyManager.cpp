#include "PropertyManager.h"
#include "FileUtility.h"
#include <iostream>
#include <fstream>

PropertyManager::PropertyManager() {
  LoadProperties(EGI_League, PROPERTY_DIRECTORY + "/" + PROPERTY_LEAGUE_FILENAME);
}

PropertyManager::~PropertyManager() {

}

std::string PropertyManager::PROPERTY_DIRECTORY = "Data/";
std::string PropertyManager::PROPERTY_LEAGUE_FILENAME = "LEAGUE_PROPERTIES.txt";

std::shared_ptr<PropertyManager> PropertyManager::mSingleton = std::shared_ptr<PropertyManager>(new PropertyManager);

std::shared_ptr<PropertyManager> PropertyManager::Get() {
  return mSingleton;
}

void PropertyManager::
LoadProperties(EGameId id, const std::string& filename) {
  if(gameProperties.find(id) == gameProperties.end()) {
    gameProperties[id] = std::shared_ptr<std::unordered_map<std::string, T_EMPTY>>(new std::unordered_map<std::string, T_EMPTY>);
  }

  std::ifstream propertyFile(GetRelativeFilePath(filename));
  if(!propertyFile.is_open()) {
    std::cout << "ERROR: Unable to open properties file." << std::endl;
    return;
  }

  std::string line;
  while(getline(propertyFile, line)) {
    (*gameProperties[id])[line] = T_EMPTY();
  }

  propertyFile.close();
}

std::shared_ptr<std::unordered_map<std::string, T_EMPTY>> PropertyManager::
GetProperties(EGameId id) {
  if(gameProperties.find(id) == gameProperties.end()) {
    return NULL;
  }
  return gameProperties[id];
}
