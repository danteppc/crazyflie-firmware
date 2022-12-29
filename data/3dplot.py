#!/usr/bin/env python3

from numpy import random
import matplotlib as mpl
import matplotlib.pyplot as plt
from mpl_toolkits import mplot3d 
import numpy as np
import pandas as pd
import scipy.stats as stats
import scipy

normal = pd.read_csv('/Users/mahyar/Developer/crazyflie/data/Pose-20221122T10-39-41_nodisrupt_noimu.csv')
canceled = pd.read_csv('/Users/mahyar/Developer/crazyflie/data/Pose-20221122T10-49-44_disrupt_noimu.csv')
attack = pd.read_csv('/Users/mahyar/Developer/crazyflie/data/Pose-20221122T11-18-31_disrupt_active_noimu.csv')



normal_xs = normal['stateEstimate.x']
normal_ys = normal['stateEstimate.y']
normal_zs = normal['stateEstimate.z']

normal_xs_centered = normal_xs - np.median(normal_xs)
normal_ys_centered = normal_ys - np.median(normal_ys)
normal_zs_centered = normal_zs - np.median(normal_zs)

attack_xs = attack['stateEstimate.x']
attack_ys = attack['stateEstimate.y']
attack_zs = attack['stateEstimate.z']

attack_xs_centered = attack_xs - np.median(attack_xs)
attack_ys_centered = attack_ys - np.median(attack_ys)
attack_yz_centered = attack_zs - np.median(attack_zs)

canceled_xs = canceled['stateEstimate.x']
canceled_ys = canceled['stateEstimate.y']

canceled_xs_centered = canceled_xs - np.median(canceled_xs)
canceled_ys_centered = canceled_ys - np.median(canceled_ys)


fig = plt.figure(figsize=(12,12))
ax = plt.axes(projection ="3d") 

for xx, yy, zz in zip(attack_xs_centered, attack_ys_centered, attack_yz_centered):
	ax.plot3D([0, xx], [0, yy], [0, zz], color = "blue")
	
ax.scatter3D(attack_xs_centered, attack_ys_centered, attack_yz_centered, color = "green", s = 10)

plt.show()