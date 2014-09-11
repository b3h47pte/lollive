#pragma once
#ifndef _LEAGUECHAMPIONDATABASE_H
#define _LEAGUECHAMPIONDATABASE_H

#include "common.h"
#include "LeagueChampionData.h"

/*
 * Contains a mapping of all the champions in League of Legends and any information that we 
 * might need with regards to those champions. It loads this database from an XML file.
 */
class LeagueChampionDatabase {
public:
  static std::shared_ptr<const LeagueChampionDatabase> Get();

  // Retrieves a constant pointer to database
  const std::map<std::string, PtrLeagueChampionData>* GetDatabase() const { return &mData; }

  // TODO: Make this configurable.
  const int GetImageSizeX() const { return 120; }
  const int GetImageSizeY() const { return 120; }

  ~LeagueChampionDatabase();
private:
  // Load all the data about the champion into memory. This includes name/images.
  void LoadChampionDatabase(std::string& dir, std::string& fileName);

  // Common Image Postfix String.
  std::string mImagePostfix;

  // Database
  std::map<std::string, PtrLeagueChampionData> mData;

  LeagueChampionDatabase();

  // Config File Constants
  const std::string ChampionDataSection = "Champions";
  const std::string DataDirectoryName = "ChampionDataDirectory";
  const std::string DatabaseFilenameName = "ChampionDatabaseFilename";
  const std::string ImageDirectoryName = "ImageDirectory";
};

#endif