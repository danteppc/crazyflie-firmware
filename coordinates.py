#!/usr/bin/env python3

from mpl_toolkits import mplot3d
import matplotlib.pyplot as plt	

class Position:
	def __init__(self ,x, y, z):
		self.x = x
		self.y = y	
		self.z = z	
	def __str__(self):
		return f"({self.x}, {self.y}, {self.z})"
	
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

anchors = [Anchor('0',-1,-1,0),
		   Anchor('1',-1,+1,1),
		   Anchor('2',+1,+1,0),
		   Anchor('3',+1,-1,1),
		   Anchor('4',-1,-1,1),
		   Anchor('5',-1,+1,0),
		   Anchor('6',+1,+1,1),
		   Anchor('7',+1,-1,0),
		   Anchor('2',+1,0.1,0, isAttacker=True),
		   Anchor('7',+1,-0.1,0, isAttacker=True),]

 
for index, a in enumerate(anchors):
	ax.scatter(a.x,a.y,a.z, c="red" if a.isAttacker else "blue", marker="x" if a.isAttacker else "o")
	ax.text(a.x+X_TEXTPAD,a.y,a.z+Z_TEXTPAD if not a.isAttacker else 0, str(a), color='blue' if not a.isAttacker else 'red', weight='bold')
	

tag = Tag(0,0,0)

ax.scatter(tag.x,tag.y,tag.z,s=50,c="black",marker="^")
ax.text(tag.x+X_TEXTPAD,tag.y,tag.z+Z_TEXTPAD,str(tag))
ax.margins(0.3)

plt.savefig('coordinates.pdf')

plt.show()