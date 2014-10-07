import xml.etree.ElementTree as ET
import re
import json
from xml.dom import minidom

jsonInputFile = open("items.json0", "r")
jsonInput = json.loads(jsonInputFile.read())

database = ET.Element("database")
items = ET.SubElement(database, "items")

for itemObject in jsonInput["items"]:
  newItem = ET.SubElement(items, "item")
  idEle = ET.SubElement(newItem, "id")
  idEle.text = itemObject["id"]
  pathEle = ET.SubElement(newItem, "path")
  pathEle.text = itemObject["icon"].replace(".dds", ".png")

xmlText = ET.tostring(database, 'utf-8')
xmlTextParse = minidom.parseString(xmlText)
xmlTextPretty = xmlTextParse.toprettyxml(indent="  ")

output = open("database.xml", "w")
output.write(xmlTextPretty)
output.close()
