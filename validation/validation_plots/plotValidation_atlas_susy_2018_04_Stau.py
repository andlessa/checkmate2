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
np.warnings.filterwarnings('ignore', category=np.VisibleDeprecationWarning)

pd.options.mode.chained_assignment = None #Disable copy warnings
#Define plotting style:
sns.set() #Set style
sns.set_style('ticks',{'font.family':'serif', 'font.serif':'Times New Roman'})
sns.set_context('paper', font_scale=1.8)
cm = plt.cm.get_cmap('RdYlBu')

# %% Load data
effOfficialHigh = np.genfromtxt('./ATLAS_data/HEPData-ins1765529-v1-Eff_Acc_table_SRhigh.csv',
                                comments='#', delimiter=',', names=['mstau','mlsp','effATLAS'],
                                skip_header=10)

effOfficialLow = np.genfromtxt('./ATLAS_data/HEPData-ins1765529-v1-Eff_Acc_table_SRlow.csv',
                                comments='#', delimiter=',', names=['mstau','mlsp','effATLAS'],
                                skip_header=10)

effOfficialHigh['effATLAS'] = effOfficialHigh['effATLAS']/100.
effOfficialLow['effATLAS'] = effOfficialLow['effATLAS']/100.

# %% Get data from CheckMate results
resultFolder = '../validation_results/atlas_susy_2018_04'
slhaFolder = '../validation_slha/'
recastDataLow = []
recastDataHigh = []
rData = []
for slhaFile in glob.glob(slhaFolder+'/*.slha'):
    slhaData = pyslha.readSLHAFile(slhaFile)
    mstau = slhaData.blocks['MASS'][1000013]
    mlsp = slhaData.blocks['MASS'][1000022]
    resDir = os.path.splitext(os.path.basename(slhaFile))[0]
    resFile = os.path.join(resultFolder,resDir,'evaluation',
                'total_results.txt')
    if not os.path.isfile(resFile):
        continue
    data = np.genfromtxt(resFile,names=True,dtype=None,encoding=None)
    ilow = np.where(data['sr'] == 'SR-lowMass')[0]
    ihigh = np.where(data['sr'] == 'SR-highMass')[0]
    recastDataLow.append([mstau,mlsp,data[ilow]])
    recastDataHigh.append([mstau,mlsp,data[ihigh]])
    rData.append([mstau,mlsp,data[ihigh]['robs'][0],data[ilow]['robs'][0]])
recastDataLow = np.array(recastDataLow)
recastDataHigh = np.array(recastDataHigh)
rData = np.array(rData)


# %% Compute relative difference
effsDiffLow = []
for pt in recastDataLow:
    diff = 0.0
    effOff = 0.0
    ioff = np.where((effOfficialLow['mstau'] == pt[0]) & (effOfficialLow['mlsp'] == pt[1]))
    if ioff:
        effOff = effOfficialLow['effATLAS'][ioff][0]
    if effOff:
        diff = (pt[2]['eff']-effOff)/effOff
    effsDiffLow.append([pt[0],pt[1],diff])
effsDiffLow = np.array(effsDiffLow)
effsDiffHigh = []
for pt in recastDataHigh:
    diff = 0.0
    effOff = 0.0
    ioff = np.where((effOfficialHigh['mstau'] == pt[0]) & (effOfficialHigh['mlsp'] == pt[1]))
    if ioff:
        effOff = effOfficialHigh['effATLAS'][ioff][0]
    if effOff:
        diff = (pt[2]['eff']-effOff)/effOff
    effsDiffHigh.append([pt[0],pt[1],diff])
effsDiffHigh = np.array(effsDiffHigh)

# %% plot result High mass
fig, axes = plt.subplots(nrows=1, ncols=2, figsize=(18,8))
ax = axes[0].scatter(effsDiffHigh[:,0],effsDiffHigh[:,1],
    c=effsDiffHigh[:,2],cmap=cm,vmin=-1.0,vmax=1.0,s=120)
for pt in effsDiffHigh:
    if abs(pt[2]) > 0.3:
        axes[0].annotate('%1.1f'%pt[2],(pt[0],pt[1]+3.),
                    fontsize=11)
axes[0].set_xlabel(r'$m_{\tilde{\tau}}$ (GeV)')
axes[0].set_ylabel(r'$m_{\tilde{\chi}_1^0}$ (GeV)')
axes[0].set_title(r'$\tilde{\tau} \tilde{\tau}, \tilde{\tau} \to \tau + \tilde{\chi}_1^0$ (SR-HighMass Efficiencies)')

ax = axes[1].scatter(effsDiffLow[:,0],effsDiffLow[:,1],
    c=effsDiffLow[:,2],cmap=cm,vmin=-1.0,vmax=1.0,s=120)
for pt in effsDiffLow:
    if abs(pt[2]) > 0.3:
        axes[1].annotate('%1.1f'%pt[2],(pt[0],pt[1]+3.),
                    fontsize=11)
axes[1].set_xlabel(r'$m_{\tilde{\tau}}$ (GeV)')
# axes[1].set_ylabel(r'$m_{\tilde{\chi}_1^0}$ (GeV)')
axes[1].set_title(r'$\tilde{\tau} \tilde{\tau}, \tilde{\tau} \to \tau + \tilde{\chi}_1^0$ (SR-lowMass Efficiencies)')

cb = fig.colorbar(ax, ax=axes.ravel().tolist())
cb.set_label(r'$\Delta\epsilon/\epsilon_{ATLAS}$')
plt.savefig("atlas_susy_2018_04_StauEffs.png")




## %% Get exclusion contours for signal
contoursHigh = getContour(rData[:,0],rData[:,1],rData[:,2],levels=[1.0])
contoursLow = getContour(rData[:,0],rData[:,1],rData[:,3],levels=[1.0])

# %% Load data
offCurveComb = np.genfromtxt('./ATLAS_data/HEPData-ins1765529-v2-Exclusion_contour_1_Obs.csv',
                        delimiter=',', names=True, skip_header=10)
offCurveLow = np.genfromtxt('./ATLAS_data/HEPData-ins1765529-v2-Exclusion_contour_aux_1_Obs.csv',
                        delimiter=',', names=True, skip_header=10)
offCurveHigh = np.genfromtxt('./ATLAS_data/HEPData-ins1765529-v2-Exclusion_contour_aux_2_Obs.csv',
                        delimiter=',', names=True, skip_header=10)

# %% Plot exclusion curve
fig, axes = plt.subplots(nrows=1, ncols=2, figsize=(18,8))
ax = axes[0].scatter(rData[:,0],rData[:,1],
    c=rData[:,2],cmap=cm,vmin=0.0,vmax=2.0,s=70)
for level,curves in contoursHigh.items():
    for curve in curves:
        axes[0].plot(curve[:,0],curve[:,1],label='Recast (r = %s)' %str(level),linestyle='--',linewidth=4)
axes[0].plot(offCurveHigh['MSTAU_GeV'],offCurveHigh['MNEUTRALINO1_GeV'],linewidth=4,
        color='black',label='ATLAS-SUSY-2018-04 (High Mass)')

ax = axes[1].scatter(rData[:,0],rData[:,1],
    c=rData[:,3],cmap=cm,vmin=0.0,vmax=2.0,s=70)
for level,curves in contoursLow.items():
    for curve in curves:
        axes[1].plot(curve[:,0],curve[:,1],label='Recast (r = %s)' %str(level),linestyle='--',linewidth=4)
axes[1].plot(offCurveLow['MSTAU_GeV'],offCurveLow['MNEUTRALINO1_GeV'],linewidth=4,
        color='black',label='ATLAS-SUSY-2018-04 (Low Mass)')
axes[0].legend()
axes[0].set_xlabel(r'$m_{\tilde{\tau}}$ (GeV)')
axes[0].set_ylabel(r'$m_{\tilde{\chi}_1^0}$ (GeV)')
axes[0].set_title(r'$\tilde{\tau} \tilde{\tau}, \tilde{\tau} \to \tau + \tilde{\chi}_1^0$ (SR-HighMass)')
axes[1].legend()
axes[1].set_xlabel(r'$m_{\tilde{\tau}}$ (GeV)')
axes[1].set_title(r'$\tilde{\tau} \tilde{\tau}, \tilde{\tau} \to \tau + \tilde{\chi}_1^0$ (SR-lowMass)')

cb = fig.colorbar(ax, ax=axes.ravel().tolist())
cb.set_label(r'$r = \sigma/\sigma_{UL}^{95}$')
plt.savefig("atlas_susy_2018_04_Stau.png")
plt.show()
