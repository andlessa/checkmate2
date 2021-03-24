#!/usr/bin/env python3

"""Simple code for get a contour from a 2D grid of points."""

import copy
from matplotlib import pyplot as plt


def getContour(xpts,ypts,zpts,levels):
    """
    Use pyplot tricontour method to obtain contour curves in a 2D plane.

    :return: A dictionary with a list of contours for each level
    """
    fig = plt.figure()
    x = copy.deepcopy(xpts)
    y = copy.deepcopy(ypts)
    z = copy.deepcopy(zpts)
    CS = plt.tricontour(x,y,z,levels=levels)
    levelPts = {}
    for il,level in enumerate(CS.levels):
        levelPts[level] = []
        c = CS.collections[il]
        paths = c.get_paths()
        for path in paths:
            levelPts[level].append(path.vertices)
    plt.close(fig)

    return levelPts
