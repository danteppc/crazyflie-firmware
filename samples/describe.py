#!/usr/bin/env python3

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import scipy.stats as stats
import scipy
from scipy.interpolate import make_interp_spline, BSpline

notesla_normal = pd.read_csv('posesample-Jan-11-2023_1646-normal.csv')
notesla_lspoof  = pd.read_csv('posesample-Jan-11-2023_1803-8vs2t+.csv')
notesla_spoof = pd.read_csv('posesample-Jan-11-2023_1809-8vs2t0.csv')
tesla_normal = pd.read_csv('posesample-Jan-12-2023_1107-SYNC 8vs0.csv')
tesla_lspoof  = pd.read_csv('posesample-Jan-12-2023_1114-SYNC8vs2t0.csv')
tesla_spoof = pd.read_csv('posesample-Jan-12-2023_1205-SYNCPERFECT-8vs2t+134.csv')

all = [("no tesla normal",notesla_normal),
	("no tesla late spoof",notesla_lspoof),
	("no tesla spoof",notesla_spoof),
	("tesla normal",tesla_normal),
	("tesla late spoof",tesla_lspoof),
	("tesla spoof",tesla_spoof)]

for str, data in all:
	print("------"+str+"--------")
	t = data['timestamp']
	x = data['pose.x']
	y = data['pose.y']
	#print(t.describe(include='all').apply("{0:.4f}".format))
	#print("----")
	print(x.describe(include='all').apply("{0:.4f}".format))
	x.describe(include='all').to_csv("x-desc-"+str+".csv")
	print("----")
	print(y.describe(include='all').apply("{0:.4f}".format))
	y.describe(include='all').to_csv("y-desc-"+str+".csv")
	
	print("----")
	print("-------------------------------")
	