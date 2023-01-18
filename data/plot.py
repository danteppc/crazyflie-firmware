#!/usr/bin/env python3

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import scipy.stats as stats
import scipy

attack = pd.read_csv('/Users/mahyar/Developer/crazyflie/samples/posesample-Jan-11-2023_1809.csv')
#normal = pd.read_csv('/Users/mahyar/Developer/crazyflie/data/Pose-20221128T16-15-23_normal.csv')


#canceled = pd.read_csv('/Users/mahyar/Developer/crazyflie/data/Pose-20221122T10-49-44_disrupt_noimu.csv')
normal = pd.read_csv('/Users/mahyar/Developer/crazyflie/samples/posesample-Jan-11-2023_1715.csv')


tesla = pd.read_csv('/Users/mahyar/Developer/crazyflie/samples/posesample-Jan-12-2023_1114-SYNC8vs2t0.csv')

normal_xs = normal['pose.x']
normal_ys = normal['pose.y']

normal_xs_centered = normal_xs# - np.median(normal_xs)
normal_ys_centered = normal_ys# - np.median(normal_ys)

attack_xs = attack['pose.x']
attack_ys = attack['pose.y']

attack_xs_centered = attack_xs# - np.median(attack_xs)
attack_ys_centered = attack_ys# - np.median(attack_ys)

tesla_x = tesla['pose.x']
tesla_y = tesla['pose.y']

#canceled_xs = canceled['stateEstimate.x']
#canceled_ys = canceled['stateEstimate.y']

#canceled_xs_centered = canceled_xs - np.median(canceled_xs)
#canceled_ys_centered = canceled_ys - np.median(canceled_ys)

#plt.scatter(normal_xs, normal_ys, c ="blue")
#plt.scatter(attack_xs, attack_ys, c ="red")
plt.scatter(attack_xs_centered, attack_ys_centered, c ="red",s=1)
#plt.scatter(canceled_xs_centered, canceled_ys_centered, c ="green",alpha=0.5,s=2)
plt.scatter(tesla_x, tesla_y, c ="orange",s=1)
plt.scatter(normal_xs_centered, normal_ys_centered, c ="green",s=1)

# To show the plot
plt.show()

#result = stats.ttest_ind(attack_xs_centered, normal_xs_centered)
#print(result)


#print(scipy.stats.describe(attack_xs_centered))
#print(scipy.stats.describe(normal_xs_centered))
#print(scipy.stats.describe(canceled_xs_centered))

f = lambda x: abs(x)>0.1
outliers_attack = list(filter(f, attack_xs_centered))
outliers_normal = list(filter(f, normal_xs_centered))
outliers_canceled = list(filter(f, canceled_xs_centered))

print("attacked outliers = " + str(len(outliers_attack)))
print("normal outliers = " + str(len(outliers_normal)))
print("canceled outliers = " + str(len(outliers_canceled)))