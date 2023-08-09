#!/usr/bin/env python3

import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

# Read the CSV file into a pandas dataframe
df = pd.read_csv('posesample-Apr-14-2023_1856.csv')

# Define a custom color palette
colors = ['#FFA07A', '#90EE90','#87CEFA']

# Create a figure with 3 subplots
fig, axes = plt.subplots(1, 3, figsize=(12, 4), sharey=True)

# Create the histogram plots with KDE lines using the "pose.x", "pose.y", and "pose.z" columns
sns.histplot(data=df, x='pose.x', kde=True, alpha=0.5, color=colors[0], edgecolor='white', linewidth=1, bins=100, ax=axes[0])
sns.histplot(data=df, x='pose.y', kde=True, alpha=0.5, color=colors[1], edgecolor='white', linewidth=1, bins=100, ax=axes[1])
sns.histplot(data=df, x='pose.z', kde=True, alpha=0.5, color=colors[2], edgecolor='white', linewidth=1, bins=100, ax=axes[2])

# Display the mean and standard deviation on each plot
for i, col in enumerate(['pose.x', 'pose.y', 'pose.z']):
	axes[i].axvline(x=df[col].mean(), color='black', linestyle='--', linewidth=1)
	#axes[i].axvline(x=df[col].std(), color='black', linestyle=':', linewidth=2, label='Std Dev')
	
# Remove spines from the plots
sns.despine()

# Set the x-axis labels for each plot
axes[0].set_xlabel('pose.x')
axes[1].set_xlabel('pose.y')
axes[2].set_xlabel('pose.z')

# Set the y-axis label for the first plot
axes[0].set_ylabel('Count')

# Adjust the spacing between subplots
plt.subplots_adjust(wspace=0.1)
plt.savefig('hist2.pdf')
# Display the plot
plt.show()
