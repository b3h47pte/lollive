#pragma once
#ifndef _CONFIGMANAGER_H
#define _CONFIGMANAGER_H

#include <iostream>
#include <string>
#include <memory>
#include <map>
#include "inih/INIReader.h"



/*
 * Manages all our config files. 
 */
class ConfigManager {
public:
  // Some global constants for easy access to configurations
  static std::string CONFIG_GENERAL_FILENAME;
  static std::string CONFIG_LEAGUE_FILENAME;

  ConfigManager();
  ~ConfigManager();

  static std::shared_ptr<ConfigManager> Get();

  // Accessor functions to get certain values in the config file. These are simple wraps over the INIReader class.
  // This config file must have been loaded already
  std::string GetStringFromINI(std::string& fileName, std::string& section, std::string& name, std::string& defaultValue);
  int GetIntFromINI(std::string& fileName, std::string& section, std::string& name, int defaultValue);
  bool GetBoolFromINI(std::string& fileName, std::string& section, std::string& name, bool defaultValue);
private:
  static std::shared_ptr<ConfigManager> mSingleton;

  // Config File Directory
  static std::string CONFIG_FILE_DIRECTORY;


  // Utility Functions
  // Generate the full path from a filename. The filename should just be something along the lines of 'xxx.ini'
  std::string GenerateFilePath(std::string& fileName);
  // 

  // Mapping from the config file identifier [filename] to the INIReader which manges it
  std::map<std::string, std::shared_ptr<INIReader>> mINIMapping;
};

#endif