#pragma once
#ifndef _LEAGUEITEMDATABASE_H
#define _LEAGUEITEMDATABASE_H

#include "common.h"
#include "LeagueItemData.h"
#include "opencv2/core.hpp"

/*
 * Contains mapping of the item ID to the item data. 
 * It also contains the mapping of the location on the database image to the item ID.
 */
class LeagueItemDatabase {
public:
  static std::shared_ptr<const LeagueItemDatabase> Get();

  ~LeagueItemDatabase();
  
  cv::Mat GetDatabaseImage() const { return mDatabaseImage; }

  // Retrieves a constant pointer to database
  const std::map<std::string, PtrLeagueItemData>* GetDatabase() const { return &mData; }

  PtrLeagueItemData GetItem(int x, int y) const;

private:
  // Load in the database file to creating the mapping of item ID to item data -- also loads in the image as well
  void LoadItemDatabase(std::string& dir, std::string& fileName);

  // Load in the database IMAGE to create the mapping of image location to string ID.
  void LoadItemDatabaseImage();

  // Utility function to make our database image. This concatenates all the images into a 
  // giant image (it tries to make it as square as possible). 
  cv::Mat CreateDatabaseImage(std::string& dir);

  // Database (Two Parts)
  // Part 1 - From ID to Item Data
  // Part 2 - From Image Location to ID
  // Why? Because we use template matching to find the item and therefore will have a match at some point in the image and we want
  // to translate that point into the right item.
  std::map<std::string, PtrLeagueItemData> mData;
  // How to use? When we get a pixel location like x: 100 y: 100, then we want to translate that to the 'image coordinate' which for a 10x10 item image
  // would be x:10 , y:10. Then we translate that into a single index (y * x_width + x) to index into mLocationData. Easy!
  std::vector<std::string> mLocationData;

  // Database Image.
  cv::Mat mDatabaseImage;
  int numImageDim;

  LeagueItemDatabase();

  const std::string ItemDataSection = "Items";
  const std::string DataDirectoryName = "ItemDataDirectory";
  const std::string DatabaseFilenameName = "ItemDatabaseFilename";
  const std::string DatabaseImageFilenameName = "ItemDatabaseImageFilename";
  const std::string ImageDirectoryName = "ImageDirectory";
};

#endif