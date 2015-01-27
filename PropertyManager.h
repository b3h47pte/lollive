#pragma once
#ifndef _PROPERTYMANAGER_H
#define _PROPERTYMANAGER_H

#include "common.h"
#include "GameConstants.h"
#include <unordered_map>
#include <memory>
#include <vector>

/*
 * Handles the storage of image analysis property key values.
 */
class PropertyManager {
public:
  static std::string PROPERTY_DIRECTORY;
  static std::string PROPERTY_LEAGUE_FILENAME;

  virtual ~PropertyManager();

  static std::shared_ptr<PropertyManager> Get();
  std::shared_ptr<std::unordered_map<std::string, T_EMPTY>> GetProperties(EGameId id);
private:
  static std::shared_ptr<PropertyManager> mSingleton;
  PropertyManager();

  void LoadProperties(EGameId id, const std::string& filename);
  
  std::unordered_map<EGameId, std::shared_ptr<std::unordered_map<std::string, T_EMPTY>>, std::hash<int>> gameProperties;
};

#endif
