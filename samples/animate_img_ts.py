import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.animation as animation

# Read in CSV file
df = pd.read_csv('posesample-Jan-11-2023_1809-8vs2t0.csv')

# Set up figure and axis
fig, ax = plt.subplots()

fig.set_size_inches((10, 10))
# Load image to represent current position
img = plt.imread('crazyflieimgshadowlow.png')
# Define list to store plotted points
points = []

# Define function to update the animation at each frame
def update(frame):
    # Clear axis
    ax.clear()
    plt.grid(color='#eee')
    
    # Get x and y values for current frame
    x = df.iloc[frame]['pose.x']
    y = df.iloc[frame]['pose.y']
    
    # Add current position to list of plotted points and keep only the last 10 points
    points.append((x, y))
        
    # Plot path trace with gradually fading transparency
    #for i, (px, py) in enumerate(points):
    ax.scatter(x, y, c='black',s=5, alpha=1,zorder=1)
        
    # Plot image at current position
    ax.imshow(img, extent=[x-0.2, x+0.2, y-0.2, y+0.2],zorder=2,alpha=0.9)
    
    x_margin = (df['pose.x'].max() - df['pose.x'].min())*0.1
    y_margin = (df['pose.y'].max() - df['pose.y'].min())*0.1
    ax.set_xlim(df['pose.x'].min()-x_margin, df['pose.x'].max()+x_margin)
    ax.set_ylim(df['pose.y'].min()-y_margin, df['pose.y'].max()+y_margin)
    
    # Set title to current timestamp
    ax.set_title('t='+str(int(df.iloc[frame]['timestamp']-df['timestamp'].min()))+'ms')
    
        
# Create animation
ani = animation.FuncAnimation(fig, update, frames=len(df), interval=1)
ax.set_aspect('equal', 'box')

# Show animation
plt.show()
#ani.save('animation.mp4', writer='ffmpeg', fps=60)