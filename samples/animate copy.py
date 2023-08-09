import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import matplotlib.image as mpimg

# Read xy positions from the CSV file
df = pd.read_csv('posesample-Jan-11-2023_1809-8vs2t0.csv')

img = mpimg.imread('image.png')

# Create the figure and axes
fig, ax = plt.subplots()

# Set the x and y limits based on the min and max values in the DataFrame
ax.set_xlim(df['posex'].min(), df['posex'].max())
ax.set_ylim(df['posey'].min(), df['posey'].max())

# Create the initial scatter plot
scat = ax.scatter(df['posex'][0], df['posey'][0])

# Define the update function for the animation
def update(frame):
    # Get the xy positions for the current row
    x = df['posex'][frame]
    y = df['posey'][frame]

    # Update the scatter plot with the new positions
    scat.set_offsets([[x, y]])

    return scat,

# Create the animation
ani = FuncAnimation(fig, update, frames=len(df), interval=1)

# Show the plot
plt.show()