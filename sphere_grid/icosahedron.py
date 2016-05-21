#!/usr/bin/env python

import math

phi = (1 + math.sqrt(5.0)) / 2

vertices = [
    (0, 1, phi),
    (0, 1, -phi),
    (0, -1, phi),
    (0, -1, -phi),

    (1, phi, 0),
    (1, -phi, 0),
    (-1, phi, 0),
    (-1, -phi, 0),

    (phi, 0, 1),
    (phi, 0, -1),
    (-phi, 0, 1),
    (-phi, 0, -1)
]

norm = math.sqrt(1 + phi*phi)
for i in range(12):
    vertices[i] = (vertices[i][0]/norm, vertices[i][1]/norm, vertices[i][2]/norm)

edges = set()

def dist(u, v):
    dx = u[0] - v[0]
    dy = u[1] - v[1]
    dz = u[2] - v[2]
    return math.sqrt(dx*dx + dy*dy + dz*dz)

for i in range(12):
    v = vertices[i]
    dist_arr = []
    for j in range(12):
        if j == i:
            continue
        u = vertices[j]
        d = dist(u, v)
        dist_arr += (d, j),
    dist_arr.sort()
    for k in range(5):
        j = dist_arr[k][1]
        a = min(i, j)
        b = max(i, j)
        edges.add((a, b))

# print(edges)

if __name__ == "__main__":
    from mpl_toolkits.mplot3d import Axes3D
    import matplotlib.pyplot as plt
    fig = plt.figure()
    ax = Axes3D(fig)
    for e in edges:
        xs = [vertices[e[0]][0], vertices[e[1]][0]]
        ys = [vertices[e[0]][1], vertices[e[1]][1]]
        zs = [vertices[e[0]][2], vertices[e[1]][2]]
        ax.plot(xs, ys, zs, "k")

    ax.plot([0,0],[-1,1],[0,0],'o-b')
    ax.plot([-1,1],[0,0],[0,0],'o-r')
    ax.plot([0,0],[0,0],[-1,1],'o-g')
    plt.show()
