import matplotlib.pyplot as pyp
import numpy as np
data = np.loadtxt("graph.txt")
points = np.loadtxt("points.txt")
pyp.plot(data[:, 0], data[:, 1])
pyp.scatter(points[:, 0], points[:, 1], color = 'red')
pyp.grid()
pyp.show()


