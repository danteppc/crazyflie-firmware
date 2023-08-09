#!/usr/bin/env python3

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

df1 = pd.read_csv('posesample-Apr-14-2023_1757.csv')
#df2 = pd.read_csv('posesample-Jan-11-2023_1809-8vs2t0.csv')
#df3 = pd.read_csv('posesample-Jan-12-2023_1114-SYNC8vs2t0.csv')

x1 = df1['pose.x']
y1 = df1['pose.y']

#x2 = df2['pose.x']
#y2 = df2['pose.y']
#
#x3 = df3['pose.x']
#y3 = df3['pose.y']

#t = df1['timestamp']

fig, ax = plt.subplots()
#ax.set_xlim(-0.2, 0.2)
#ax.set_ylim(-0.2, 0.2)
#plt.axis("equal")

#plt.rcParams['figure.figsize'] = [5, 5]
#plt.scatter(x2, y2, s=1,color='white', marker='.')
#plt.scatter(x3, y3, s=1,color='white', marker='.')
plt.scatter(x1, y1, s=1,color='white', marker='.')

#plt.yticks(np.arange(-0.2, -0.2, 0.1))

#plt.figure().set_figwidth(10)
#plt.figure().set_figheight(10)
ax.set_aspect('equal', 'box')
#fig.tight_layout()

#plt.xticks(np.arange(-0.2, 0.2, 0.1))
#plt.yticks(np.arange(-0.2, 0.2, 0.1))
plt.savefig('org.png', transparent=True)
plt.show()