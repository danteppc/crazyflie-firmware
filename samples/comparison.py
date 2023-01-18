#!/usr/bin/env python3

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import scipy.stats as stats
import scipy

def every1000(array):
	return  array#[array[index] for index in range(1, len(array), 50)]

set1 = pd.read_csv('posesample-Jan-12-2023_1107-SYNC 8vs0.csv')
set2 = pd.read_csv('posesample-Jan-12-2023_1130-SYNC 8vs2t+127.csv')
set3 = pd.read_csv('posesample-Jan-12-2023_1114-SYNC8vs2t0.csv')

t1 = every1000((set1['timestamp']-min(set1['timestamp'])) / 1000)
x1 = every1000(set1['pose.x'])
y1 = every1000(set1['pose.y'])


t2 = every1000((set2['timestamp']-min(set2['timestamp'])) / 1000)
x2 = every1000(set2['pose.x'])
y2 = every1000(set2['pose.y'])
t3 = every1000((set3['timestamp']-min(set3['timestamp'])) / 1000)
x3 = every1000(set3['pose.x'])
y3 = every1000(set3['pose.y'])


plt.figure(figsize=(10, 10))

plt1 = plt.subplot(431, aspect=1)
plt1.scatter(x1, y1,s=0.5,c='black')
plt1.title.set_text('(a) normal pose\n under MI-TESLA')
plt1.set_xlabel('x-estimates [m]')
plt1.set_ylabel('y-estimates [m]')

plt2 = plt.subplot(432, aspect=1)
plt2.scatter(x2, y2,s=0.5,c='black')
plt2.title.set_text('(b) late spoof-attempted pose\n under MI-TESLA')
plt2.set_xlabel('x-estimates [m]')
plt2.set_ylabel('y-estimates [m]')

plt3 = plt.subplot(433, aspect=1)
plt3.scatter(x3, y3,s=0.5,c='black')
plt3.title.set_text('(c) spoof-attempted pose\n under MI-TESLA')
plt3.set_xlabel('x-estimates [m]')
plt3.set_ylabel('y-estimates [m]')

plt4 = plt.subplot(412)
plt4.plot(t1, x1,c='r')
plt4.plot(t1, y1,c='g')
plt4.title.set_text('(d) normal pose under MI-TESLA over time')
plt4.legend(['x', 'y'])
plt4.set_xlabel('time [s]')
plt4.set_ylabel('estimates [m]')

plt5 = plt.subplot(413)
plt5.plot(t2, x2,c='r')
plt5.plot(t2, y2,c='g')
plt5.title.set_text('(e) late spooft-attempted pose under MI-TESLA over time')
plt5.legend(['x', 'y'])
plt5.set_xlabel('time [s]')
plt5.set_ylabel('estimates [m]')


plt6 = plt.subplot(414)
plt6.plot(t3, x3,c='r')
plt6.plot(t3, y3,c='g')
plt6.title.set_text('(f) spooft-attempted pose under MI-TESLA over time')
plt6.legend(['x', 'y'])
plt6.set_xlabel('time [s]')
plt6.set_ylabel('estimates [m]')

#plt4.plot(t1, y1,c='g')
#m1, b1 = np.polyfit(t1, x1, 1)
#add linear regression line to scatterplot 
#plt4.plot(t1, m1*t1+b1,c='r')

plt.tight_layout()
plt.savefig('vis.pdf')

plt.show()

