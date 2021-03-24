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
offCurve = np.genfromtxt('./CMS_data/CMS-SUS-16-032_Figure_006.csv',
                        delimiter=',', names=['mstop','mlsp'])

# %% Get data from CheckMate results
resultFolder = '../validation_results/cms_sus_16_032'
slhaFolder = '../validation_slha/'
recastData = []
for slhaFile in glob.glob(slhaFolder+'/T2cc*.slha'):
    slhaData = pyslha.readSLHAFile(slhaFile)
    mstop = slhaData.blocks['MASS'][1000006]
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
    robscons = (1.0-1.64*0.2)*robs
    recastData.append([mstop,mlsp,robs,robscons])

recastData = np.array(recastData)

## %% Get exclusion contours for signal +- 20%
contours = getContour(recastData[:,0],recastData[:,1],recastData[:,2],levels=[0.8,1.0,1.2])

# %% plot result
fig = plt.figure(figsize=(12,8))
ax = plt.scatter(recastData[:,0],recastData[:,1],
    c=recastData[:,2],cmap=cm,vmin=0.0,vmax=2.0,s=70)
for level,curves in contours.items():
    if level != 1.0: continue
    npts = [len(c) for c in curves]
    for curve in curves:
        if len(curve) != max(npts): continue
        plt.plot(curve[:,0],curve[:,1],label='Recast (r = %s)' %str(level),linestyle='--',linewidth=4)
plt.plot(offCurve['mstop'],offCurve['mlsp'],linewidth=4,
        color='black',label='CMS-SUS-16-032')
plt.xlabel(r'$m_{\tilde{\t}}$ (GeV)')
plt.ylabel(r'$m_{\tilde{\chi}_1^0}$ (GeV)')
cb = plt.colorbar(ax)
for pt in recastData:
    if pt[2] < 1.2:
        plt.annotate('%1.1f'%pt[2],(pt[0],pt[1]),
                    fontsize=10)
cb.set_label("r")
plt.title(r'$\tilde{t} \tilde{t}, \tilde{t} \to c + \tilde{\chi}_1^0$ (Combined Exclusion)')
plt.savefig("cms_sus_16_032_Stop.png")
plt.show()
