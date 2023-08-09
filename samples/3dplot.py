import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

# Load data from CSV file
data = pd.read_csv("posesample-Apr-14-2023_1757.csv")

# Determine color range based on X, Y, or Z axis
color_range = data['pose.x'] if 'pose.x' in data.columns else data['pose.y'] if 'pose.y' in data.columns else data['pose.z']
#color_scale = sns.color_palette("#69d", as_cmap=True)
color_scale = sns.dark_palette("#E8A13D", as_cmap=True)


# Create a 3D scatter plot with automatic colors
fig = plt.figure(figsize=(10, 8))
ax = fig.add_subplot(111, projection='3d')

ax.scatter(data['pose.x'],
		   data['pose.y'],
		   data['pose.z'], 
	s=0.01,c=color_range, cmap=color_scale)

#ax.xaxis._axinfo["grid"]['linewidth'] = 0.2
#ax.yaxis._axinfo["grid"]['linewidth'] = 0.2
#ax.zaxis._axinfo["grid"]['linewidth'] = 0.2

# Set axis labels and title
ax.set_xlabel('x')
ax.set_ylabel('y')
ax.set_zlabel('z')

#plt.savefig('inner.png')
plt.show()