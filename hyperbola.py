#!/usr/bin/env python3

measurements = [
	[0.000001,0.000001,1],
	[-0.000001,0.000001,1],
	[0.000001,-0.000001,1]
]

import pymap3d as pm

lat0, lon0, t0 = measurements[0]
for i in range(len(measurements)):
	lat, lon, t = measurements[i]
	e, n, _ = pm.geodetic2enu(lat, lon, 0, lat0, lon0, 0)
	measurements[i] = [e, n, t]
	
speed = 299792458 # m/s

measurements = [
	[1,3,0],
	[6,2,0],
	[1,1,0]
]

#measurements = [
#	[87,62,0],
#	[87,764,0],
#	[1660,62,0],
#	[1660,764,0]
#	
#]


import numpy as np

def functions(x0, y0, x1, y1, x2, y2, d01, d02, d12):
	print(x2,y2)
	""" Given observers at (x0, y0), (x1, y1), (x2, y2) and TDOA between observers d01, d02, d12, this closure
		returns a function that evaluates the system of three hyperbolae for given event x, y.
	"""
	def fn(args):
		with np.printoptions(threshold=np.inf):
			print(args)
		x,y=args
		a = np.sqrt(np.power(x - x1, 2.) + np.power(y - y1, 2.)) - np.sqrt(np.power(x - x0, 2.) + np.power(y - y0, 2.)) - d01
		b = np.sqrt(np.power(x - x2, 2.) + np.power(y - y2, 2.)) - np.sqrt(np.power(x - x0, 2.) + np.power(y - y0, 2.)) - d02
		c = np.sqrt(np.power(x - x2, 2.) + np.power(y - y2, 2.)) - np.sqrt(np.power(x - x1, 2.) + np.power(y - y1, 2.)) - d12
		return [a, b, c]
	return fn

def jacobian(x0, y0, x1, y1, x2, y2, d01, d02, d12):
	def fn(args):
		x,y = args
		adx = (x - x1) / np.sqrt(np.power(x - x1, 2.) + np.power(y - y1, 2.)) - (x - x0) / np.sqrt(np.power(x - x0, 2.) + np.power(y - y0, 2.))
		bdx = (x - x2) / np.sqrt(np.power(x - x2, 2.) + np.power(y - y2, 2.)) - (x - x0) / np.sqrt(np.power(x - x0, 2.) + np.power(y - y0, 2.))
		cdx = (x - x2) / np.sqrt(np.power(x - x2, 2.) + np.power(y - y2, 2.)) - (x - x1) / np.sqrt(np.power(x - x1, 2.) + np.power(y - y1, 2.))
		ady = (y - y1) / np.sqrt(np.power(x - x1, 2.) + np.power(y - y1, 2.)) - (y - y0) / np.sqrt(np.power(x - x0, 2.) + np.power(y - y0, 2.))
		bdy = (y - y2) / np.sqrt(np.power(x - x2, 2.) + np.power(y - y2, 2.)) - (y - y0) / np.sqrt(np.power(x - x0, 2.) + np.power(y - y0, 2.))
		cdy = (y - y2) / np.sqrt(np.power(x - x2, 2.) + np.power(y - y2, 2.)) - (y - y1) / np.sqrt(np.power(x - x1, 2.) + np.power(y - y1, 2.))
		
		return [
			[adx, ady],
			[bdx, bdy],
			[cdx, cdy]
		]
	return fn

xp = np.mean([x for x,y,t in measurements])
yp = np.mean([y for x,y,t in measurements])

import scipy.optimize as opt

x0, y0, t0 = measurements[0]
x1, y1, t1 = measurements[1]
x2, y2, t2 = measurements[2]

F = functions(x0, y0, x1, y1, x2, y2, (t1 - t0) * speed, (t2 - t0) * speed, (t2 - t1) * speed)
J = jacobian(x0, y0, x1, y1, x2, y2, (t1 - t0) * speed, (t2 - t0) * speed, (t2 - t1) * speed)

val = opt.leastsq(F, x0=[xp, yp], Dfun=J)
x = val[0][0]
y = val[0][1]

(lat, lon, _) = pm.enu2geodetic(x, y, 0, lat0, lon0, 0)

import matplotlib.pyplot as plt

# Create reasonable x, y bounds for visualization
max_x = max(x0, x1, x2, x)
min_x = min(x0, x1, x2, x)
range_x = max_x - min_x
min_x -= range_x * .2
max_x += range_x * .2

max_y = max(y0, y1, y2, y)
min_y = min(y0, y1, y2, y)
range_y = max_y - min_y
min_y -= range_y * .2
max_y += range_y * .2

# Create a grid of input coordinates
xs = np.linspace(min_x, max_x, 100)
ys = np.linspace(min_y, max_y, 100)
val = np.meshgrid(xs, ys)
xs, ys = np.meshgrid(xs, ys)

# Evaluate the system across the grid
A = np.empty([100, 100])
B = np.empty([100, 100])
C = np.empty([100, 100])
for i in range(0,100):
	for j in range(0,100):
		A[i][j], B[i][j], C[i][j] = F(([xs[i][j], ys[i][j]]))

	


		
d0 = np.sqrt(np.power(x - x0, 2.) + np.power(y - y0, 2.))
d1 = np.sqrt(np.power(x - x1, 2.) + np.power(y - y1, 2.))
d2 = np.sqrt(np.power(x - x2, 2.) + np.power(y - y2, 2.))
t0 = d0 / speed
t1 = d1 / speed
t2 = d2 / speed


print("x0="+str(x))
print("y0="+str(y))

print("distances:")
print(d0)
print(d1)
print(d2)

print(t0)
print(t1)
print(t2)

plt.rcParams['figure.figsize'] = [6, 6]

# Plot the results
plt.plot([x0,x1,x2,x0],[y0,y1,y2,y0],c='black')
plt.contour(xs, ys, A, [0], colors='r',zorder=0)
plt.contour(xs, ys, B, [0], colors='g',zorder=0)
plt.contour(xs, ys, C, [0], colors='b',zorder=0)
plt.scatter(x, y, color='k', marker='^',s=200,zorder=1)
plt.scatter(x0, y0,s=200,c='darkorange',marker='*',zorder=1)
plt.scatter(x1, y1,s=200,c='gold',zorder=1)
plt.scatter(x2, y2,s=200,c='violet',zorder=1)

plt.savefig('hyperbola_spoofed.pdf')
plt.show()


