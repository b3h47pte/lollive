#include "LeagueItemDatabase.h"
#pragma warning(push)
#pragma warning(disable: 4244) // possible loss of data
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#pragma warning(pop)
#include "math.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"


#define LOAD_ITEM_DATABASE_ERR(chk, err) \
if (!chk) {\
  std::cerr << "Failed to find the " #err " node." << std::endl; \
  exit(1); \
}

LeagueItemDatabase::LeagueItemDatabase() {
  // Load all the data on construction. 
  std::string dir = ConfigManager::Get()->GetStringFromINI(ConfigManager::CONFIG_LEAGUE_FILENAME, ItemDataSection, DataDirectoryName, std::string(""));
  std::string fileName = ConfigManager::Get()->GetStringFromINI(ConfigManager::CONFIG_LEAGUE_FILENAME, ItemDataSection, DatabaseFilenameName, std::string(""));
  LoadItemDatabase(dir, fileName);

  // Load in database image (table of all the items)
  std::string imgName = ConfigManager::Get()->GetStringFromINI(ConfigManager::CONFIG_LEAGUE_FILENAME, ItemDataSection, DatabaseImageFilenameName, std::string(""));
  char* llldb_dir = getenv("LLLDB_DIR");
  std::string envDir = llldb_dir ? std::string(llldb_dir) : "";
  mDatabaseImage = cv::imread(std::string(envDir) + dir + imgName, cv::IMREAD_COLOR);
  if (mDatabaseImage.data == NULL) {
    mDatabaseImage = CreateDatabaseImage(dir);
    cv::imwrite(std::string(envDir) + dir + imgName, mDatabaseImage);
  }
  LoadItemDatabaseImage();
}

std::shared_ptr<const LeagueItemDatabase> LeagueItemDatabase::Get() {
  static std::shared_ptr<const LeagueItemDatabase> lcd = []() {
    std::shared_ptr<const LeagueItemDatabase> rr(new LeagueItemDatabase);
    return rr;
  }();
  return lcd;
}


LeagueItemDatabase::~LeagueItemDatabase() {

}

/*
 * Creates a mapping between the item ID (that matches with the League of Legends API) and our
 * internal representation of the item.
 */
void LeagueItemDatabase::LoadItemDatabase(std::string& dir, std::string& fileName) {
  // Read in the XML file
  char* llldb_dir = getenv("LLLDB_DIR");
  std::string envDir = llldb_dir ? std::string(llldb_dir) : "";
  std::string loc = std::string(envDir) + dir + fileName;
  rapidxml::file<> xmlFile(loc.c_str());
  rapidxml::xml_document<> xmlDoc;
  xmlDoc.parse<0>(xmlFile.data());

  // Get the base node ('database')
  rapidxml::xml_node<>* baseNode = xmlDoc.first_node("database");
  LOAD_ITEM_DATABASE_ERR(baseNode, "database");

  // Get all items
  rapidxml::xml_node<>* allItemsNode = baseNode->first_node("items");
  LOAD_ITEM_DATABASE_ERR(allItemsNode, "items");

  rapidxml::xml_node<>* itemNode = allItemsNode->first_node("item");
  std::string imageDir = ConfigManager::Get()->GetStringFromINI(ConfigManager::CONFIG_LEAGUE_FILENAME, ItemDataSection, ImageDirectoryName, std::string(""));
  while (itemNode) {
    PtrLeagueItemData itemData(new LeagueItemData);
    itemData->itemID = itemNode->first_node("id")->value();
    itemData->itemImage = cv::imread(envDir + dir + imageDir + itemNode->first_node("path")->value(), cv::IMREAD_UNCHANGED);
    // TODO: Remake item images to all be consistent.
    if (itemData->itemImage.channels() == 4) {
      cv::cvtColor(itemData->itemImage, itemData->itemImage, cv::COLOR_BGRA2BGR);
    }
    // If a conflict exists, choose the one with the most recent.
    mData[itemData->itemID] = itemData;

    itemNode = itemNode->next_sibling("item");
  }
}

/*
 * Generates a giant image with all the items lined up next to each other.
 */
cv::Mat LeagueItemDatabase::CreateDatabaseImage(std::string& dir) {
  // Calculate how large the resulting image is going to be.
  int x_imgs = 0;
  int y_imgs = 0;
  
  int sqr_imgs = (int)floor(sqrt((double)mData.size()));
  if (mData.size() % sqr_imgs != 0) {
    sqr_imgs += 1;
  }
  x_imgs = y_imgs = sqr_imgs;

  // Now put all the images together into this one giant image
  cv::Mat newImage(x_imgs * LEAGUE_ITEM_SQUARE_SIZE, y_imgs * LEAGUE_ITEM_SQUARE_SIZE, CV_8UC3 );
  int count = 0;
  for (auto& pair : mData) {
    // Calculate X/Y index in terms of pixels
    int yIdx = (int)((float)count / y_imgs);
    int xIdx = (int)(count % x_imgs);

    // Load the image
    cv::Mat itemImage = pair.second->itemImage;
    
    // Copy it into the big image.
    cv::Rect section(xIdx * LEAGUE_ITEM_SQUARE_SIZE, yIdx * LEAGUE_ITEM_SQUARE_SIZE, LEAGUE_ITEM_SQUARE_SIZE, LEAGUE_ITEM_SQUARE_SIZE);
    itemImage.copyTo(newImage(section));

    ++count;
  }

  return newImage;
}

/*
 * After the image is made, make sure that we generate the mapping from image location to item name. 
 */
void LeagueItemDatabase::LoadItemDatabaseImage() {
  numImageDim = (int)((float)mDatabaseImage.cols / LEAGUE_ITEM_SQUARE_SIZE);
  for (auto& pair : mData) {
    mLocationData.push_back(pair.second->itemID);
  }
}

/*
 * Convert x and y image coordinate (not pixel) into an actual index.
 */
PtrLeagueItemData LeagueItemDatabase::GetItem(int x, int y) const {
  int idx = y * numImageDim + x;
  if ((size_t)idx >= mLocationData.size()) {
    return nullptr;
  }
  std::string id = mLocationData[idx];
  return mData.at(id);
}
