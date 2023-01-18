#!/usr/bin/env python3

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import scipy.stats as stats
import scipy
from scipy.interpolate import make_interp_spline, BSpline

set1 = pd.read_csv('posesample-Jan-11-2023_1646-normal.csv')
set2 = pd.read_csv('posesample-Jan-11-2023_1809-8vs2t0.csv')
set3 = pd.read_csv('posesample-Jan-12-2023_1114-SYNC8vs2t0.csv')

t1 = (set1['timestamp']-min(set1['timestamp'])) / 1000
x1 = set1['pose.x']
y1 = set1['pose.y']

t2 = (set2['timestamp']-min(set2['timestamp'])) / 1000
x2 = set2['pose.x']
y2 = set2['pose.y']

t3 = (set3['timestamp']-min(set3['timestamp'])) / 1000
x3 = set3['pose.x']
y3 = set3['pose.y']

plt.figure(figsize=(6, 6))

mymodel1 = np.poly1d(np.polyfit(t1, x1, 3))
myline1 = np.linspace(min(t1), max(t1), 10)

print(mymodel1)

#plt.plot(t1, x1, alpha=0.1)
plt.plot(myline1, mymodel1(myline1), marker='.',c='gray')


mymodel2 = np.poly1d(np.polyfit(t2, x2, 3))
myline2 = np.linspace(min(t2), max(t2), 10)

print(mymodel2)

#plt.plot(t1, x2, alpha=0.1)
plt.plot(myline2, mymodel2(myline2), marker = 'x', c='r')


mymodel3 = np.poly1d(np.polyfit(t3, x3, 3))
myline3 = np.linspace(min(t3), max(t3), 10)

#plt.plot(t1, x3, alpha=0.1)
plt.plot(myline3, mymodel3(myline3), marker ='^', c='green')

print(mymodel3)

#plt.scatter(canceled_xs_centered, canceled_ys_centered, c ="green",alpha=0.5,s=2)
#plt.scatter(tesla_x, tesla_y, c ="orange",s=1)
#plt.scatter(normal_xs_centered, normal_ys_centered, c ="green",s=1)
#plt.set_xlabel('time [s]')
#plt.set_ylabel('x-estimates')

plt.xlabel("time [s]")
plt.ylabel("estimates [m]")

plt.legend(['no-spoofing regression for x-estimates', 'spoofing regression for x-estimates', 'spoofing regression for x-estimates under MI-TESLA'])

plt.savefig('regx.pdf')

#plt.ylabel("polynomial function")
# To show the plot
plt.show()
