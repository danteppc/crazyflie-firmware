#!/usr/bin/env python3

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import scipy.stats as stats
import scipy
from scipy.interpolate import make_interp_spline, BSpline
from sklearn.metrics import mean_squared_error
import math

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

all = [("embed", pd.read_csv('posesample-Apr-11-2023_0744_embed.csv')),
	("outward", pd.read_csv('posesample-Apr-11-2023_0756.csv')),
	("old", pd.read_csv('posesample-Apr-11-2023_0946_old.csv'))]	


df = pd.read_csv('posesample-Apr-17-2023_1038.csv')
df.replace([np.inf, -np.inf, np.nan], 0, inplace=True)

df2 = pd.read_csv('posesample-Apr-17-2023_1051.csv')
df2.replace([np.inf, -np.inf, np.nan], 0, inplace=True)

all = [("1",df),
	("2",df2),
	("3", pd.read_csv('posesample-Apr-17-2023_1105.csv')),
	("4", pd.read_csv('posesample-Apr-17-2023_1113.csv')),
	("5", pd.read_csv('posesample-Apr-17-2023_1118.csv')),
	('reset all', pd.read_csv('posesample-Apr-17-2023_1123.csv')),
	('shake', pd.read_csv('posesample-Apr-17-2023_1129.csv')),
	('battery', pd.read_csv('posesample-Apr-17-2023_1136.csv')),
	('battery+reset', pd.read_csv('posesample-Apr-17-2023_1142.csv'))]



for str, data in all:
	print("------"+str+"--------")
	t = data['timestamp']
	x = data['pose.x']
	y = data['pose.y']
	z = data['pose.z']
	
	#print(t.describe(include='all').apply("{0:.4f}".format))
	#print("----")
	print(x.describe(include='all').apply("{0:.4f}".format))
	x.describe(include='all').to_csv("x-desc-"+str+".csv")
	print("----")
	print(y.describe(include='all').apply("{0:.4f}".format))
	y.describe(include='all').to_csv("y-desc-"+str+".csv")
	
	print(z.describe(include='all').apply("{0:.4f}".format))
	z.describe(include='all').to_csv("z-desc-"+str+".csv")
	
	
	print("----")
	
	true_x = np.zeros(len(x), dtype = float)
	true_y = np.zeros(len(y), dtype = float)
	true_z = np.repeat(0.025, len(z))
	true_pos = np.tile(np.array([0.0, 0.0, 0.025]), (len(x), 1))
	
	RMSE_x = math.sqrt(mean_squared_error(true_x, x))
	RMSE_y = math.sqrt(mean_squared_error(true_y, y))
	RMSE_z = math.sqrt(mean_squared_error(true_z, z))
	
	

	d = []
	for i,pos in enumerate(true_pos):
		d.append((math.dist([x[i],y[i],z[i]], pos)))


	d_s = pd.Series(d)
	print(d_s.describe(include='all').apply("{0:.4f}".format))
	d_s.describe(include='all').to_csv("z-desc-"+str+".csv")
	
	RMSE_d = math.sqrt(mean_squared_error(true_x, d))
	
	
	print("----")
	
	print("RMSE x: " , RMSE_x)
	print("RMSE y: " , RMSE_y)
	print("RMSE z: " , RMSE_z)
	print("RMSE d: " , RMSE_d)
	
	
	print("-------------------------------")
	