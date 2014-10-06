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
  std::string envDir(getenv("LLLDB_DIR"));
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
  std::string envDir(getenv("LLLDB_DIR"));
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
    itemData->itemCount = std::stoi(itemNode->first_node("count")->value(), NULL);
    itemData->itemID = itemNode->first_node("id")->value();
    itemData->itemName = itemNode->first_node("name")->value();
    itemNode = itemNode->next_sibling("item");

    // If a conflict exists, choose the one with the higher item count.
    if (mData.find(itemData->itemID) != mData.end()) {
      PtrLeagueItemData oldItem = mData[itemData->itemID];
      if (oldItem->itemCount > itemData->itemCount) {
        continue;
      }
    }

    mData[itemData->itemID] = itemData;
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
    std::string envDir(getenv("LLLDB_DIR"));
    std::string fileName = std::string(envDir) + dir + + "Images/" + CreateLeagueItemFileName(pair.second->itemCount, pair.second->itemID, pair.second->itemName);
    cv::Mat itemImage = cv::imread(fileName, cv::IMREAD_COLOR);
    
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
  std::string id = mLocationData[idx];
  return mData.at(id);
}
