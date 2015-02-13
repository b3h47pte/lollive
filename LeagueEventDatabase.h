#pragma once
#ifndef _LEAGUEEVENTDATABASE_H
#define _LEAGUEEVENTDATABASE_H

#include "common.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"


#define GENERATE_OBJECTIVE_CONFIG_STRINGS() \
  DECLARE_CONFIG_VARIABLE(BaronObjective, std::string, ConfigManager::CONFIG_LEAGUE_FILENAME, "Objectives", "")\
  DECLARE_CONFIG_VARIABLE(DragonObjective, std::string, ConfigManager::CONFIG_LEAGUE_FILENAME, "Objectives", "")\
  DECLARE_CONFIG_VARIABLE(PurpleOuterTurretObjective, std::string, ConfigManager::CONFIG_LEAGUE_FILENAME, "Objectives", "")\
  DECLARE_CONFIG_VARIABLE(PurpleInnerTurretObjective, std::string, ConfigManager::CONFIG_LEAGUE_FILENAME, "Objectives", "")\
  DECLARE_CONFIG_VARIABLE(PurpleInhibTurretObjective, std::string, ConfigManager::CONFIG_LEAGUE_FILENAME, "Objectives", "")\
  DECLARE_CONFIG_VARIABLE(PurpleNexusTurretObjective, std::string, ConfigManager::CONFIG_LEAGUE_FILENAME, "Objectives", "")\
  DECLARE_CONFIG_VARIABLE(PurpleInhibitor, std::string, ConfigManager::CONFIG_LEAGUE_FILENAME, "Objectives", "")\
  DECLARE_CONFIG_VARIABLE(BlueOuterTurretObjective, std::string, ConfigManager::CONFIG_LEAGUE_FILENAME, "Objectives", "")\
  DECLARE_CONFIG_VARIABLE(BlueInnerTurretObjective, std::string, ConfigManager::CONFIG_LEAGUE_FILENAME, "Objectives", "")\
  DECLARE_CONFIG_VARIABLE(BlueInhibTurretObjective, std::string, ConfigManager::CONFIG_LEAGUE_FILENAME, "Objectives", "")\
  DECLARE_CONFIG_VARIABLE(BlueNexusTurretObjective, std::string, ConfigManager::CONFIG_LEAGUE_FILENAME, "Objectives", "")\
  DECLARE_CONFIG_VARIABLE(BlueInhibitor, std::string, ConfigManager::CONFIG_LEAGUE_FILENAME, "Objectives", "")\

/*
 * Holds common images for the events.
 */
class LeagueEventDatabase {
public:
  static std::shared_ptr<const LeagueEventDatabase> Get();
  cv::Mat GetObjectiveImage(std::string id) const { return mImageMapping.at(id); }

  ~LeagueEventDatabase();
private:
  // Loads the objective database and stores the images in our private mapping
  void LoadEventDatabase(std::string& dir, std::string& fileName);

  // Mapping from an 'identifier' string to the actual image.
  std::map<std::string, cv::Mat> mImageMapping;

  LeagueEventDatabase();

  // Config variables
  DECLARE_CONFIG_VARIABLE(ObjectiveDataDirectory, std::string, ConfigManager::CONFIG_LEAGUE_FILENAME, "Objectives", "")
  DECLARE_CONFIG_VARIABLE(ObjectiveDatabaseFilename, std::string, ConfigManager::CONFIG_LEAGUE_FILENAME, "Objectives", "")
  DECLARE_CONFIG_VARIABLE(ImageDirectory, std::string, ConfigManager::CONFIG_LEAGUE_FILENAME, "Objectives", "")
};

#endif
