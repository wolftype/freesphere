from __future__ import print_function
import sys
import json

print(sys.version)
print("translation app")

pid = 11
j = json.loads(open("gr" + str(pid) + ".json", 'r').read())

active = 1 if j["active"] else 0
print("active: " + str(active))

projections = j["projections"]
num_projections = len(projections)
print(num_projections)

b = [0] * num_projections
h = [0] * num_projections
l = [0] * num_projections
w = [0] * num_projections

for i in range(num_projections):
    b[i] = projections[i]["viewport"]["b"]
    h[i] = projections[i]["viewport"]["h"]
    l[i] = projections[i]["viewport"]["l"]
    w[i] = projections[i]["viewport"]["w"]

t = ""
for i in range(num_projections):
    t += "id " + str(pid) + "\n"
    t += "b " + str(b[i]) + "\n"
    t += "h " + str(h[i]) + "\n"
    t += "l " + str(l[i]) + "\n"
    t += "w " + str(w[i]) + "\n"
    t += "active " + str(active) + "\n\n"

r = open("gr" + str(i) + ".txt", 'w')
r.write(t)
