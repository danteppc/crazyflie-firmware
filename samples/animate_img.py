import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
from matplotlib.animation import FuncAnimation

# Read the xy positions from CSV file
df = pd.read_csv('posesample-Jan-11-2023_1809-8vs2t0.csv')

# Load the image from file
img = mpimg.imread('crazyflieimg.png')

# Create the figure and axis
fig, ax = plt.subplots()

# Set the axis limits based on the data range
x_min, x_max = df['pose.x'].min(), df['pose.x'].max()
y_min, y_max = df['pose.y'].min(), df['pose.y'].max()
ax.set_xlim(x_min, x_max)
ax.set_ylim(y_min, y_max)

# Draw the image on the plot
im = ax.imshow(img, extent=[x_min, x_max, y_min, y_max], aspect='auto', zorder=1)

# Initialize the scatter plot and line
scat = ax.scatter([], [], c='red', s=10, zorder=2)
line, = ax.plot([], [], c='red', alpha=1.0, zorder=2)

# Define the update function
def update(frame):
    # Get the current xy position
    x, y = df['pose.x'][frame], df['pose.y'][frame]
    
    # Update the scatter plot and line data
    scat.set_offsets(np.array([[x, y]]))
    #line.set_xdata(np.append(line.get_xdata(), x))
    #line.set_ydata(np.append(line.get_ydata(), y))
    
    # Gradually decrease the alpha of the previous trace
    alpha = max(line.get_alpha() - 1.0 / (1000), 0)
    #line.set_alpha(alpha)
    
    # Update the image position
    im.set_extent([x, x + 0.1, y, y + 0.1])
    
    # Return the updated plot elements
    return im, scat, line

# Create the animation
ani = FuncAnimation(fig, update, frames=len(df), interval=1, blit=True)

# Save the animation to file
#ani.save('animation.mp4', fps=fps, dpi=150)
# Save the animation to file
#ani.save('animation.mp4', fps=fps, dpi=150)
plt.show()