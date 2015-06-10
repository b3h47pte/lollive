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
  static std::string CONFIG_SERVER_FILENAME;

  ConfigManager();
  ~ConfigManager();

  static std::shared_ptr<ConfigManager> Get();

  void LoadExternalConfig(const std::string& filename, bool isExternalFile);

  // Generic Accessor that make it easier for us to use macros to define variables that will be found in the config file
  template<typename T>
  inline T GetFromINI(const std::string& fileName, const std::string& section, const std::string& name, const T defaultValue) {
    return GetGenericFromINI<T>(fileName, section, name, defaultValue); 
  }

  bool Exists(const std::string& fileName, const std::string& section, const std::string& name);

  // Accessor functions to get certain values in the config file. These are simple wraps over the INIReader class.
  // This config file must have been loaded already
  template <typename T>
  T GetGenericFromINI(const std::string& fileName, const std::string& section, const std::string& name, const std::string& defaultValue);

  std::string GetStringFromINI(const std::string& fileName, const std::string& section, const std::string& name, const std::string& defaultValue);
  int GetIntFromINI(const std::string& fileName, const std::string& section, const std::string& name, const int defaultValue);
  bool GetBoolFromINI(const std::string& fileName, const std::string& section, const std::string& name, const bool defaultValue);
  double GetDoubleFromINI(const std::string& fileName, const std::string& section, const std::string& name, const double defaultValue);
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
template<> 
inline double ConfigManager::GetFromINI<double>(const std::string& fileName, const std::string& section, const std::string& name, const double defaultValue) {
  return GetDoubleFromINI(fileName, section, name, defaultValue);
}

template<>
inline bool ConfigManager::GetFromINI<bool>(const std::string& fileName, const std::string& section, const std::string& name, const bool defaultValue) {
  return GetBoolFromINI(fileName, section, name, defaultValue);
}

template<>
inline int ConfigManager::GetFromINI<int>(const std::string& fileName, const std::string& section, const std::string& name, const int defaultValue) {
  return GetIntFromINI(fileName, section, name, defaultValue);
}

template<>
inline std::string ConfigManager::GetFromINI<std::string>(const std::string& fileName, const std::string& section, const std::string& name, const std::string defaultValue) {
  return GetStringFromINI(fileName, section, name, defaultValue);
}


// Need to easily be able to define variables as being stored in the config file and then easily retrieve them
// Obviously the best way to do this would be to access them like variables, but the easier alternative is to just use functions
#define DECLARE_CONFIG_VARIABLE(var_name, var_type, config_file, config_section, def_value) \
  var_type _cached_value_##var_name; \
  bool _is_cached_##var_name = false;\
  inline var_type Get##var_name() { \
    if (!_is_cached_##var_name) {\
      _cached_value_##var_name = ConfigManager::Get()->GetFromINI<var_type>(config_file, std::string(config_section), #var_name, def_value); \
    }\
    return _cached_value_##var_name;\
  }
#endif
