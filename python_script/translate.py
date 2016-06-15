# Python 2.7
# tested with OSX built in Python

from __future__ import print_function
import sys
import json

print(sys.version)
print("translation app")

large_width = 1920
large_height = 1200
small_width = 1400
small_height = 1050

# no tilda for c++!
# sphere_filepath = "~/calibration-current/"
sphere_filepath = "/home/sphere/calibration-current/"

mid = "14" # machine id
j = json.loads(open("gr" + mid + ".json", 'r').read())

active = 1 if j["active"] else 0
print("active: " + str(active))

projections = j["projections"]
num_projections = len(projections)
print(num_projections)

b = [0] * num_projections
h = [0] * num_projections
l = [0] * num_projections
w = [0] * num_projections

pid = [0] * num_projections # projector id

for i in range(num_projections):
    b[i] = projections[i]["viewport"]["b"]
    h[i] = projections[i]["viewport"]["h"]
    l[i] = projections[i]["viewport"]["l"]
    w[i] = projections[i]["viewport"]["w"]
    pid[i] = projections[i]["blend"]["file"][5:-4]


t = ""
for i in range(num_projections):
    t += "id " + str(pid[i]) + "\n"
    t += "filepath " + sphere_filepath + "proj" + str(pid[i]) + ".bin" + "\n"
    if (pid[i] == 9 or pid[i] == 10 or pid[i] == 11 or pid[i] == 12):
        t += "width " + str(large_width) + "\n"
        t += "height " + str(large_height) + "\n"
    else:
        t += "width " + str(small_width) + "\n"
        t += "height " + str(small_height) + "\n"
    t += "b " + str(b[i]) + "\n"
    t += "h " + str(h[i]) + "\n"
    t += "l " + str(l[i]) + "\n"
    t += "w " + str(w[i]) + "\n"
    t += "active " + str(active) + "\n"
    t += "\n"

r = open("gr" + str(mid) + ".txt", 'w')
r.write(t)
