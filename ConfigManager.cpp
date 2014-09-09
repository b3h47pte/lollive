#include "ConfigManager.h"

std::string ConfigManager::CONFIG_FILE_DIRECTORY = "Config/";
// Some global constants for easy access to configurations
std::string ConfigManager::CONFIG_GENERAL_FILENAME = "general.ini";
std::string ConfigManager::CONFIG_LEAGUE_FILENAME = "league.ini";

std::shared_ptr<ConfigManager> ConfigManager::mSingleton = std::shared_ptr<ConfigManager>(new ConfigManager);

std::shared_ptr<ConfigManager> ConfigManager::Get() {
  return mSingleton;
}

ConfigManager::ConfigManager() {
  // Initialize the mapping with the config files which should always exist.
  std::shared_ptr<INIReader> general = std::shared_ptr<INIReader>(new INIReader(GenerateFilePath(CONFIG_GENERAL_FILENAME)));
  mINIMapping[CONFIG_GENERAL_FILENAME] = general;

  std::shared_ptr<INIReader> league = std::shared_ptr<INIReader>(new INIReader(GenerateFilePath(CONFIG_LEAGUE_FILENAME)));
  mINIMapping[CONFIG_LEAGUE_FILENAME] = league;
}

ConfigManager::~ConfigManager() {

}

/*
 * Perform a string concatenation between the LLLDB_DIR environment variable, the config file directory and the 
 * filename.
 */
std::string ConfigManager::GenerateFilePath(std::string& fileName) {
  char* envDir = getenv("LLLDB_DIR");
  if (envDir == NULL) envDir = "";
  return std::string(envDir) + CONFIG_FILE_DIRECTORY + fileName;
}

std::string ConfigManager::GetStringFromINI(std::string& fileName, std::string& section, std::string& name, std::string& defaultValue) {
  if (mINIMapping.find(fileName) == mINIMapping.end()) {
    return defaultValue;
  }
  return mINIMapping[fileName]->Get(section, name, defaultValue);
}

int ConfigManager::GetIntFromINI(std::string& fileName, std::string& section, std::string& name, int defaultValue) {
  if (mINIMapping.find(fileName) == mINIMapping.end()) {
    return defaultValue;
  }
  return mINIMapping[fileName]->GetInteger(section, name, defaultValue);
}

bool ConfigManager::GetBoolFromINI(std::string& fileName, std::string& section, std::string& name, bool defaultValue) {
  if (mINIMapping.find(fileName) == mINIMapping.end()) {
    return defaultValue;
  }
  return mINIMapping[fileName]->GetBoolean(section, name, defaultValue);
}