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

  ~LeagueChampionDatabase();
private:
  // Load all the data about the champion into memory. This includes name/images.
  void LoadChampionDatabase(std::string& dir, std::string& fileName);

  // Common Image Postfix String.
  std::string mImagePostfix;

  // Database
  std::map<std::string, PtrLeagueChampionData> mData;

  LeagueChampionDatabase();
};

#endif