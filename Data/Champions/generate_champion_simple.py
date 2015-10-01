import sys
import os
import xml.etree.ElementTree as ET
from xml.dom import minidom

champion_image_path = os.getcwd() + "/Images/"
champion_image_postfix = "_Square_0.png"
output = open("database.txt", "w")

for root, dirs, files in os.walk(champion_image_path):
    for name in files:
        filename = os.path.join(root, name)
        name = name.replace(champion_image_postfix, "")
        output.write(filename + "\t" + name + "\n")

output.close()
