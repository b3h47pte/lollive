import xml.etree.ElementTree as ET
import re
import json
from xml.dom import minidom
from collections import defaultdict

jsonInputFile = open("items.json0", "r")
jsonInput = json.loads(jsonInputFile.read())

# Remove duplicate elements (ID's that point to the same image)
# In the case of duplicates, use the one that has the same ID in the image.
# If none of them have the same ID, then cry and just use the first one
itemGrouping = defaultdict(list)
itemList = jsonInput["items"]
for itemObject in itemList:
  itemGrouping[itemObject["icon"]].append(itemObject)
  
for icon in itemGrouping:
  items = itemGrouping[icon]
  iconId = ""
  try:
    idx = icon.index("_")
    iconId = icon[0:idx]
  except:
    iconId = ""
  foundItem = False
  chosenItem = None
  if len(items) > 1:
    for item in items:
      if item["id"] == iconId:
        foundItem = True
        chosenItem = item
        break
    if not foundItem:
      chosenItem = items[0]
    
    for item in items:
      if item != chosenItem:
        itemList.remove(item)

database = ET.Element("database")
items = ET.SubElement(database, "items")

for itemObject in itemList:
  newItem = ET.SubElement(items, "item")
  
  idEle = ET.SubElement(newItem, "id")
  idEle.text = itemObject["id"]
  
  pathEle = ET.SubElement(newItem, "path")
  pathEle.text = itemObject["icon"].replace(".dds", ".png").replace(".DDS", ".png")
  
  altEle = ET.SubElement(newItem, "alt")
  baseEle = ET.SubElement(newItem, "base")
  if "Enchantment" in itemObject["name"]:
    altEle.text = "true"
    baseEle.text = itemObject["from"][0]
  else:
    altEle.text = "false"
    baseEle.text = "9999"
  
  if "Bonetooth_Necklace" in itemObject["icon"]:
    altEle.text = "true"
    baseEle.text = "3166"

xmlText = ET.tostring(database, 'utf-8')
xmlTextParse = minidom.parseString(xmlText)
xmlTextPretty = xmlTextParse.toprettyxml(indent="  ")

output = open("database.xml", "w")
output.write(xmlTextPretty)
output.close()
