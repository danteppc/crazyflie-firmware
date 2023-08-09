#!/usr/bin/env python3

from mpl_toolkits import mplot3d
import matplotlib.pyplot as plt	

class Position:
	def __init__(self ,x, y, z):
		self.x = x
		self.y = y	
		self.z = z	
	def __str__(self):
		return f"\n({self.x},\n {self.y},\n {self.z})"
	
class Anchor(Position):
	def __init__(self, anchorID, x, y, z, isAttacker=False):
		super(Anchor, self).__init__(x,y,z)
		self.anchorID = anchorID
		self.isAttacker = isAttacker
	def __str__(self):
		if not self.isAttacker:
			return "A"+self.anchorID+" "+super().__str__()
		else:
			return "A"+self.anchorID+" "+super().__str__()

class Tag(Position):
	def __str__(self):
		return "Crazyflie "+super().__str__()

class Mal(Position):
	def __str__(self):
		return "Mal "+super().__str__()
	
	
X_TEXTPAD = 0.1
Z_TEXTPAD = 0.05

fig = plt.figure(figsize=(8,8))

ax = fig.add_subplot(111, projection='3d')
ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_zlabel('Z')

# sys embed
anchors = [Anchor('0',-2.2087,1.5101,0.8548),
		Anchor('1',2.5019,1.3475,2.8275),
		Anchor('2',2.5036,-0.8358,0.8356),
		Anchor('3',-2.2377,-1.9738,2.8627),
		Anchor('4',-2.2186,1.5145,2.8380),
		Anchor('5',2.4980,1.4397,0.8378),
		Anchor('6',2.5015,-0.7755,2.8275),
		Anchor('7',-2.2272,-1.9646,0.8558)]

# sys new
#anchors = [Anchor('0',-2.1230,-3.6221,0.3586),
#		    Anchor('1',-2.0414,4.0449,3.0240),
#		    Anchor('2',2.6391,3.4173,0.3626),
#		    Anchor('3',2.5791,-3.4709,3.1023),
#		    Anchor('4',-1.9667,-3.6053,2.9467),
#		    Anchor('5',-2.2824,3.4097,0.3657),
#		    Anchor('6',2.6057,3.9284,2.8900),
#		    Anchor('7',2.6056,-2.9680,0.3632)]

#sys old
#anchors = [Anchor('0',-2.250,-3.180,0.239),
#		Anchor('1',-2.039,4.199,3.029),
#		Anchor('2',2.519,3.539,0.230),
#		Anchor('3',2.619,-3.319,3.109),
#		Anchor('4',-1.919,-3.440,2.940),
#		Anchor('5',-2.039,4.079,0.239),
#		Anchor('6',2.609,4.099,2.849),
#		Anchor('7',2.480,-2.819,0.259)]

for index, a in enumerate(anchors):
	ax.scatter(a.x,a.y,a.z, c="red" if a.isAttacker else "blue", marker="x" if a.isAttacker else "o")
	ax.text(a.x+X_TEXTPAD,a.y,a.z+Z_TEXTPAD if not a.isAttacker else 0, str(a), color='blue' if not a.isAttacker else 'red', weight='bold')
	

tag = Tag(0,0,0.025)

ax.scatter(tag.x,tag.y,tag.z,s=50,c="black",marker="^")
ax.text(tag.x+X_TEXTPAD,tag.y,tag.z+Z_TEXTPAD,str(tag))
ax.margins(0.3)

plt.savefig('coordinates_embed.pdf')

#plt.show()