import numpy as np
from vispy import app, visuals, gloo

# Load the data
data = np.genfromtxt('posesample-Jan-11-2023_1809-8vs2t0.csv', delimiter=',', names=['timestamp', 'x', 'y'], dtype=None)

# Load the image
img = visuals.ImageVisual('crazyflieimg.png')

# Create a canvas
canvas = app.Canvas(keys='interactive')

# Create a scatter plot
scatter = visuals.MarkersVisual()

# Set up the color and size of the scatter plot markers
scatter.set_data(data['x'], data['y'], edge_color=None, face_color=(0.2, 0.2, 1, 1), size=15)

# Set up the path plot
path = visuals.LineVisual()
path_color = np.array([0.2, 0.2, 1, 0.8])
path_data = np.empty((0, 2))
path.set_data(path_data, color=path_color, width=5)

# Set up the view and add the scatter plot and path plot
view = visuals.transforms.STTransform(scale=(1, 1), translate=(-0.5, -0.5))
scatter.transform = view
path.transform = view
scatter.z_index = 1
path.z_index = 0
canvas.add_visual(scatter)
canvas.add_visual(path)
canvas.add_visual(img)

# Set up the animation function
def update(frame):
    # Update the scatter plot data
    scatter.set_data(data['x'][frame], data['y'][frame])
    
    # Update the path plot data
    global path_data
    path_data = np.append(path_data, [[data['x'][frame], data['y'][frame]]], axis=0)
    path.set_data(path_data[-10:], color=np.concatenate((path_color[:3], np.linspace(0, path_color[3], len(path_data[-10:])))), width=5)
    
    return scatter, path

# Create the animation
animation = app.Animation(update, len(data), interval=1/60, blit=True)

# Run the application
canvas.show()
animation.start()
app.run()