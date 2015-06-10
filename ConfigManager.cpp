#include "ConfigManager.h"
#include "URLFetch.h"
#include <stdio.h>

std::string ConfigManager::CONFIG_FILE_DIRECTORY = "Config/";
// Some global constants for easy access to configurations
std::string ConfigManager::CONFIG_GENERAL_FILENAME = "general.ini";
std::string ConfigManager::CONFIG_LEAGUE_FILENAME = "league.ini";
std::string ConfigManager::CONFIG_SERVER_FILENAME = "server.ini";

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

  std::shared_ptr<INIReader> server = std::shared_ptr<INIReader>(new INIReader(GenerateFilePath(CONFIG_SERVER_FILENAME)));
  mINIMapping[CONFIG_SERVER_FILENAME] = server;
}

ConfigManager::~ConfigManager() {

}

bool ConfigManager::
Exists(const std::string& fileName, const std::string& section, const std::string& name) {
  if (mINIMapping.find(fileName) == mINIMapping.end()) {
    return false;
  }
  return mINIMapping[fileName]->Exists(section, name);
}

void ConfigManager::
LoadExternalConfig(const std::string& filename, bool isExternalFile) {
  if(filename == CONFIG_GENERAL_FILENAME || filename == CONFIG_LEAGUE_FILENAME) {
    std::cout << "ERROR: Load External Config has a filename conflict." << std::endl;
    return;
  }

  if(mINIMapping.find(filename) != mINIMapping.end()) {
    return;
  }

  std::shared_ptr<INIReader> reader;
  if(!isExternalFile) {
    reader = std::shared_ptr<INIReader>(new INIReader(filename));
  }
  else {
    // Download external file and use it for the INI Reader. -- This is a temporary file that gets deleted afterwards.
    FILE* externalConfig = tmpfile();
    std::string text;
    DownloadTextFile(filename, text);
    fprintf(externalConfig, text.c_str());
    reader = std::shared_ptr<INIReader>(new INIReader(externalConfig));
    fclose(externalConfig);
  }
  mINIMapping[filename] = reader;
}

/*
 * Perform a string concatenation between the LLLDB_DIR environment variable, the config file directory and the 
 * filename.
 */
std::string ConfigManager::GenerateFilePath(std::string& fileName) {
  char* llldb_dir = getenv("LLLDB_DIR");
  std::string envDir = llldb_dir ? std::string(llldb_dir) : "";
  return std::string(envDir) + CONFIG_FILE_DIRECTORY + fileName;
}

template<typename T> T ConfigManager::
GetGenericFromINI(const std::string& fileName, const std::string& section, const std::string& name, const std::string& defaultValue) {
  if (mINIMapping.find(fileName) == mINIMapping.end()) {
    return T(defaultValue);
  }
  return T(mINIMapping[fileName]->Get(section, name, defaultValue));
}

std::string ConfigManager::GetStringFromINI(const std::string& fileName, const std::string& section, const std::string& name, const std::string& defaultValue) {
  if (mINIMapping.find(fileName) == mINIMapping.end()) {
    return defaultValue;
  }
  return mINIMapping[fileName]->Get(section, name, defaultValue);
}

int ConfigManager::GetIntFromINI(const std::string& fileName, const std::string& section, const std::string& name, const int defaultValue) {
  if (mINIMapping.find(fileName) == mINIMapping.end()) {
    return defaultValue;
  }
  return mINIMapping[fileName]->GetInteger(section, name, defaultValue);
}

bool ConfigManager::GetBoolFromINI(const std::string& fileName, const std::string& section, const std::string& name, const bool defaultValue) {
  if (mINIMapping.find(fileName) == mINIMapping.end()) {
    return defaultValue;
  }
  return mINIMapping[fileName]->GetBoolean(section, name, defaultValue);
}

double ConfigManager::GetDoubleFromINI(const std::string& fileName, const std::string& section, const std::string& name, const double defaultValue) {
  if (mINIMapping.find(fileName) == mINIMapping.end()) {
    return defaultValue;
  }
  return mINIMapping[fileName]->GetReal(section, name, defaultValue);
}
