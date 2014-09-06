import sys
import os
import xml.etree.ElementTree as ET
import re
from xml.dom import minidom

items_image_path = os.getcwd() + "/Images/"
item_regex = re.compile("(\d+)_ImagePack_items_Embeds__e_(\d+)_(.+).png")

database = ET.Element("database")
allItems = ET.SubElement(database, "items")

for root, dirs, files in os.walk(items_image_path):
	for name in files:
		res = re.match(item_regex, name)
		if res == None:
			continue
		
		itemCount = res.group(1)
		itemId = res.group(2)
		itemName = res.group(3)

		newItem = ET.SubElement(allItems, "item")
		itemCountEle = ET.SubElement(newItem, "count")
		itemCountEle.text = itemCount

		itemIdEle = ET.SubElement(newItem, "id")
		itemIdEle.text = itemId

		itemNameEle = ET.SubElement(newItem, "name")
		itemNameEle.text = itemName

xmlText = ET.tostring(database, 'utf-8')
xmlTextParse = minidom.parseString(xmlText)
xmlTextPretty = xmlTextParse.toprettyxml(indent="  ")

output = open("database.xml", "w")
output.write(xmlTextPretty)
output.close()