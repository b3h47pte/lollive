import sys
import os

output = open("lol.user-words", "w")

# Write out all common numbers (0-500)
for i in range(0, 501):
	output.write(str(i)+"\n")

# Write out all common scores XX/XX/XX
# Assume score is always never more than two digits
for k in range(0, 10):
	for d in range(0, 10):
		for a in range(0, 10):
			output.write(str(k)+"/"+str(d)+"/"+str(a)+"\n")

# Write out all common times
for m in range(0, 61):
	for s in range(0, 60):
		output.write(str(m)+ ":" + str(s).zfill(2) + "\n")

# Write out common gold values from 2.4k [starting value] to 100.0k
for t in range(2,101):
	for h in range(0, 10):
		output.write(str(t) + "." + str(h) + "k" + "\n")

teams = ["C9", "TSM", "XDG", "DIG", "LMQ", "CLG", "EG", "T8"]
for t in teams:
	output.write(t + "\n")


output.close()