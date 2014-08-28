import sys
import os
import xml.etree.ElementTree as ET
from xml.dom import minidom

champion_image_path = os.getcwd() + "/Images/"
champion_image_postfix = "_Square_0.png"

database = ET.Element("database")
common = ET.SubElement(database, "common")
imgpostfix = ET.SubElement(common, "imgpostfix")
imgpostfix.text = champion_image_postfix

allChamps = ET.SubElement(database, "champions")

for root, dirs, files in os.walk(champion_image_path):
	for name in files:
		name = name.replace(champion_image_postfix, "")

		champion = ET.SubElement(allChamps, "champion")
		longName = ET.SubElement(champion, "name")
		longName.text = name
		shortName = ET.SubElement(champion, "shortname")
		shortName.text = name

xmlText = ET.tostring(database, 'utf-8')
xmlTextParse = minidom.parseString(xmlText)
xmlTextPretty = xmlTextParse.toprettyxml(indent="  ")

output = open("database.xml", "w")
output.write(xmlTextPretty)
output.close()