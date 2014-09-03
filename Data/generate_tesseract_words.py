import sys
import os

output = open("lol.user-words", "w")
teams = ["C9", "TSM", "XDG", "DIG", "LMQ", "CLG", "EG", "T8"]
for t in teams:
	output.write(t + "\n")


output.close()