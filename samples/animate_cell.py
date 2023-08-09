import pandas as pd
import matplotlib.pyplot as plt
from celluloid import Camera

# Load data
df = pd.read_csv('posesample-Jan-11-2023_1809-8vs2t0.csv')

# Define image path
img_path = 'crazyflieimg.png'

# Set up figure
fig, ax = plt.subplots(figsize=(10, 6))

# Load image
img = plt.imread(img_path)

# Initialize camera
camera = Camera(fig)

# Define function to update scatter plot
for i in range(len(df)):
    # Get current position
    x = df['x'][i]
    y = df['y'][i]
    
    # Plot scatter
    ax.scatter(x, y, s=50, color='red', alpha=0.7)
    
    # Plot image overlay
    ax.imshow(img, extent=[df['x'].min()-10, df['x'].max()+10, df['y'].min()-10, df['y'].max()+10])
    
    # Set axis limits
    ax.set(xlim=(df['x'].min()-10, df['x'].max()+10), ylim=(df['y'].min()-10, df['y'].max()+10))
    
    # Capture frame with camera
    camera.snap()

# Animate scatter plot
animation = camera.animate(interval=10, blit=True)

plt.show()
