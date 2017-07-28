# purpose -- display image from file of line segments

import glob
import numpy as np
import pylab as pl
from matplotlib import collections as mc

def selector(path):
    files = []
    for file in glob.glob(path + '*'):
        file = file[file.rfind('/') + 1:]
        files.append(file)
        print ' : [{}] {}'.format(len(files) - 1, file)
    id = eval(raw_input('Select a file to view: '))
    while id not in range(len(files)):
        id = eval(input('File not valid. Please select a valid file number: '))
    return path + files[id]

def show(lines, freq):
    l = []
    for i in xrange(len(lines) / freq):
        id = freq * i
        l.append([(lines[id], lines[id + 1]), (lines[id + 2], lines[id + 3])])
    lc = mc.LineCollection(l)
    fig, ax = pl.subplots()
    ax.add_collection(lc)
    ax.autoscale()
    pl.show()

def main():
    path = raw_input('Path to tests: ')
    while True:
        f = open(selector(path))
        lines = f.readlines()
        f.close()
        show(lines, 4)
        quit = raw_input('Continue? (Y/n) ')
        if quit not in ['Y', 'y', 'Yes', 'yes']:
            break

if __name__ == '__main__':
    main()

