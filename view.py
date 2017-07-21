import numpy as np
import pylab as pl
from matplotlib import collections as mc
import sys

#f=open(raw_input("File: "), "r")
f=open(sys.argv[1],"r")
redlines = f.readlines()
f.close()

lines=[]
for i in range(0,len(redlines)/4):
	lines.append([( redlines[4*i], redlines[4*i+1] ), ( redlines[4*i+2], redlines[4*i+3] )])
lc = mc.LineCollection(lines)
fig, ax = pl.subplots()
ax.add_collection(lc)
ax.autoscale()
pl.show()
