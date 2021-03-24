#!/usr/bin/env python3

"""Plot validation results."""

# %% Imports
import os,glob
import pyslha
import numpy as np
import pandas as pd
from matplotlib import pyplot as plt
import seaborn as sns
from getContour import getContour

pd.options.mode.chained_assignment = None #Disable copy warnings
#Define plotting style:
sns.set() #Set style
sns.set_style('ticks',{'font.family':'serif', 'font.serif':'Times New Roman'})
sns.set_context('paper', font_scale=1.8)
cm = plt.cm.get_cmap('RdYlBu')

# %% Load data
offCurve = np.genfromtxt('./CMS_data/CMS-SUS-19-005_Figure_011-a.csv',
                        delimiter=',', names=['mg','mlsp'])

# %% Get data from CheckMate results
resultFolder = '../validation_results/cms_sus_19_005'
slhaFolder = '../validation_slha/'
recastData = []
srRecast = []
for slhaFile in glob.glob(slhaFolder+'T1*.slha'):
    slhaData = pyslha.readSLHAFile(slhaFile)
    mg = slhaData.blocks['MASS'][1000021]
    mlsp = slhaData.blocks['MASS'][1000022]
    resDir = os.path.splitext(os.path.basename(slhaFile))[0]
    resFile = os.path.join(resultFolder,resDir,'evaluation',
                'total_results.txt')
    if not os.path.isfile(resFile):
        continue
    data = np.genfromtxt(resFile,names=True,
            usecols=(1,2,3,4,5,6,7,8,9,10,11,12),
            dtype=None,encoding=None)
    ibest = np.argmax(data['rexp'])
    bestSR = data['sr'][ibest]
    robs = data['robs'][ibest]
    recastData.append([mg,mlsp,robs])

recastData = np.array(recastData)

## %% Get exclusion contours for combined results (signal +- 20%)
contours = getContour(recastData[:,0],recastData[:,1],recastData[:,2],levels=[0.8,1.0,1.2])


# %% plot result
fig = plt.figure(figsize=(12,8))
ax = plt.scatter(recastData[:,0],recastData[:,1],
    c=recastData[:,2],cmap=cm,vmin=0.0,vmax=2.0,s=70)
cb = plt.colorbar(ax)
for level,curves in contours.items():
    if level != 1.0: continue
    for i,curve in enumerate(sorted(curves, key = lambda c: len(c),reverse=True)):
        if i == 0:
            plt.plot(curve[:,0],curve[:,1],label='Recast (r = %s)' %str(level),
                linestyle='--',linewidth=4)
        else:
            plt.plot(curve[:,0],curve[:,1],
                linestyle='--',linewidth=4)
plt.plot(offCurve['mg'],offCurve['mlsp'],linewidth=4,
            color='black',label='CMS-SUS-19-005')
plt.xlabel(r'$m_{\tilde{g}}$ (GeV)')
plt.ylabel(r'$m_{\tilde{\chi}_1^0}$ (GeV)')
# for pt in recastData:
    # if pt[2] < 1.0:
        # plt.annotate('%1.1f'%pt[2],(pt[0],pt[1]),
                    # fontsize=10)
cb.set_label("r")
plt.legend()
plt.title(r'$\tilde{g} \tilde{g}, \tilde{g} \to \bar{q} q + \tilde{\chi}_1^0$ (Best SR Exclusion)')
plt.savefig("cms_sus_19_005_T1.png")
plt.show()
