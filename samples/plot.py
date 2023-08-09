#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt
from scipy.ndimage import gaussian_filter
import pandas as pd

df = pd.read_csv('posesample-Apr-17-2023_1201.csv')
#df2 = pd.read_csv('posesample-Jan-11-2023_1809-8vs2t0.csv')
#df3 = pd.read_csv('posesample-Jan-12-2023_1114-SYNC8vs2t0.csv')

x = df['pose.x'].values
y = df['pose.y'].values
# Create a 2D histogram of the data
bins = 50
hist, xedges, yedges = np.histogram2d(x, y, bins=bins)

# Define a range of sigma values to experiment with
sigma_values = [0, 0.5, 1, 1.5]

# Plot the filled contour density plot with different sigma values
plt.figure(figsize=(12, 8))

for i, sigma in enumerate(sigma_values):
	# Smooth the histogram using a Gaussian filter
	smooth_hist = gaussian_filter(hist, sigma=sigma)
	
	# Define the extent of the plot
	#extent = [xedges[0], xedges[-1], yedges[0], yedges[-1]]
	extent = [-0.05, 0.05, -0.05, 0.05]
	
	# Create a subplot for each sigma value
	plt.subplot(2, 2, i + 1)
	plt.contourf(smooth_hist.T, extent=extent, cmap='inferno')
	plt.colorbar()
	plt.xlabel('X')
	plt.ylabel('Y')
	plt.title(f'Smoothed (Sigma = {sigma})')
	
plt.tight_layout()
plt.show()
