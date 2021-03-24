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
offCurve = np.genfromtxt('./CMS_data/CMS-SUS-16-032_Figure_005.csv',
                        delimiter=',', names=['msb','mlsp'])


# %% Get data from CheckMate results
resultFolder = '../validation_results/cms_sus_16_032'
slhaFolder = '../validation_slha/'
recastData = []
srRecast = []
for slhaFile in glob.glob(slhaFolder+'/T2bb*.slha'):
    slhaData = pyslha.readSLHAFile(slhaFile)
    msb = slhaData.blocks['MASS'][1000005]
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
    recastData.append([msb,mlsp,robs,robscons])
    srRecast.append([msb,mlsp,bestSR])

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
    for i,curve in enumerate(curves):
        if i == 0:
            plt.plot(curve[:,0],curve[:,1],label='Recast (r = %s)' %str(level),
                linestyle='--',linewidth=4)
        else:
            # continue
            plt.plot(curve[:,0],curve[:,1],
                linestyle='--',linewidth=4)
plt.plot(offCurve['msb'],offCurve['mlsp'],linewidth=4,
            color='black',label='CMS-SUS-16-032')
plt.xlabel(r'$m_{\tilde{b}}$ (GeV)')
plt.ylabel(r'$m_{\tilde{\chi}_1^0}$ (GeV)')
# for pt in recastData:
    # if 0.5 < pt[2] < 0.9:
        # plt.annotate('%1.1f'%pt[2],(pt[0],pt[1]),
                    # fontsize=10)
cb.set_label("r")
plt.legend()
plt.title(r'$\tilde{b} \tilde{b}, \tilde{b} \to b + \tilde{\chi}_1^0$ (Combined Exclusion)')
plt.savefig("cms_sus_16_032_T2bb.png")
plt.show()



# %% Get data from CheckMate results
resultFolder = '../validation_results/cms_sus_16_032'
slhaFolder = '../validation_slha/'
recastDataTrim = []
srRecastTrim = []
for slhaFile in glob.glob(slhaFolder+'/T2bb*.slha'):
    slhaData = pyslha.readSLHAFile(slhaFile)
    msb = slhaData.blocks['MASS'][1000005]
    mlsp = slhaData.blocks['MASS'][1000022]
    resDir = os.path.splitext(os.path.basename(slhaFile))[0]
    resFile = os.path.join(resultFolder,resDir,'evaluation',
                'total_results.txt')
    if not os.path.isfile(resFile):
        continue
    data = np.genfromtxt(resFile,names=True,
            usecols=(1,2,3,4,5,6,7,8,9,10,11,12),
            dtype=None,encoding=None)
    data = np.delete(data,np.where(data['sr'] == 'Combined'))
    ibest = np.argmax(data['rexp'])
    bestSR = data['sr'][ibest]
    robs = data['robs'][ibest]
    robscons = (1.0-1.64*0.2)*robs
    recastDataTrim.append([msb,mlsp,robs,robscons])
    srRecastTrim.append([msb,mlsp,bestSR])

recastDataTrim = np.array(recastDataTrim)

# %%
## %% Get exclusion contours for best signal region results
contoursBest = getContour(recastDataTrim[:,0],recastDataTrim[:,1],recastDataTrim[:,2],levels=[0.8,1.0,1.2])


# %% plot result
fig = plt.figure(figsize=(18,12))
ax = plt.scatter(recastDataTrim[:,0],recastDataTrim[:,1],
    c=recastDataTrim[:,3],cmap=cm,vmin=0.0,vmax=2.0,s=70)
cb = plt.colorbar(ax)
for level,curves in contoursBest.items():
    if level != 1.0: continue
    for curve in curves:
        plt.plot(curve[:,0],curve[:,1],label='r = '+str(level),
        linestyle='--',linewidth=4)
plt.plot(offCurve['msb'],offCurve['mlsp'],linewidth=4,
            color='black',label='CMS-SUS-16-032')
plt.xlabel(r'$m_{\tilde{b}}$ (GeV)')
plt.ylabel(r'$m_{\tilde{\chi}_1^0}$ (GeV)')
for i,pt in enumerate(recastDataTrim):
    if 0.5 < pt[2] < 1.2:
        # plt.annotate('%1.1f'%pt[2],(pt[0],pt[1]),
                    # fontsize=10)
        plt.annotate('%s'%srRecastTrim[i][2],(pt[0],pt[1]),
                    fontsize=8)

cb.set_label("r")
plt.legend()
plt.title("Best SR Exclusion")
plt.show()
