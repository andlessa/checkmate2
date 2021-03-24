#!/bin/python3

import numpy as np

fname = "/home/lessa/RDM/myCheckMateFiles/HEPData-ins1765529-v1-Cutflow_for_Direct_Stau.csv"
with open(fname,'r') as f:
    data120 = np.array([eval(l) for l in f.readlines()[11:31]])
# data120

acc120 = np.array([[data120[i+1,0], eval(data120[i+1, 1])/eval(data120[i, 1])] for i,_ in enumerate(data120[:-1])])
# acc120

fname = "/home/lessa/RDM/CheckMATE2/results/validation_atlas_susy_2018_04_120_1/analysis/StauPairProd_atlas_susy_2018_04_cutflow.dat"
with open(fname,'r') as f2:
    data120recast = np.array([[l[:30],eval(l[46:55])] for l in f2.readlines()[15:31]])
acc120recast = np.array([[data120recast[i+1,0], eval(data120recast[i+1, 1])/eval(data120recast[i, 1])] for i,_ in enumerate(data120recast[:-1])])
acc120recast
acc120


fname = "/home/lessa/RDM/myCheckMateFiles/HEPData-ins1765529-v1-Cutflow_for_Direct_Stau.csv"
with open(fname,'r') as f:
    data280 = np.array([eval(l) for l in f.readlines()[59:79]])
# data120

acc280 = np.array([[data280[i+1,0], eval(data280[i+1, 1])/eval(data280[i, 1])] for i,_ in enumerate(data280[:-1])])
# acc120

fname = "/home/lessa/RDM/CheckMATE2/results/validation_atlas_susy_2018_04_280_1/analysis/StauPairProd_atlas_susy_2018_04_cutflow.dat"
with open(fname,'r') as f2:
    data280recast = np.array([[l[:30],eval(l[46:55])] for l in f2.readlines()[15:31]])
acc280recast = np.array([[data280recast[i+1,0], eval(data280recast[i+1, 1])/eval(data280recast[i, 1])] for i,_ in enumerate(data280recast[:-1])])
acc280recast
acc280
