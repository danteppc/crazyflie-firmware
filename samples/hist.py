
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from scipy.optimize import curve_fit

# Read the csv file into a pandas dataframe
df = pd.read_csv('posesample-Apr-14-2023_1856.csv')

# Extract the "pose.x" column as a numpy array
x = df['pose.x'].values.squeeze()

# Create the histogram plot with 10 bins and step lines
counts, bins, _ = plt.hist(x, bins=20,edgecolor='black')

# Define the polynomial function to fit to the data
def func(x, a, b, c):
	return a * np.exp(-(x - b)**2 / (2 * c**2))

# Fit the function to the data
popt, _ = curve_fit(func, bins[:-1], counts)

# Create a line plot of the best fit line on top of the histogram
plt.plot(bins, func(bins, *popt), 'r-')

# Add labels, title, and legend to the plot
plt.xlabel('X Values')
plt.ylabel('Density')
plt.title('Histogram Plot with Best Fit Line')
plt.legend()

# Display the plot
plt.show()
