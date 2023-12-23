#!/usr/bin/env python3

import math
import matplotlib.pyplot as plt
import matplotlib.patches as patches
from dataclasses import dataclass

@dataclass
class Coordinates:
    x: float
    y: float


# Plot points
fig, ax = plt.subplots(figsize=(10, 10))

# Do not plot axis
plt.axis('off')

# Keep aspect ratio
plt.gca().set_aspect('equal', adjustable='box')

# Drawing parameters
axis_length = 3
arrow_head_width = 0.075
arrow_head_length = 0.1
arrow_width = 0.01

# Transformation
translation = Coordinates(1.5, 1)
rotation = math.pi / 8

# Coordinate system A
A = Coordinates(0, 0)
e_Ax = Coordinates(1, 0)
e_Ay = Coordinates(0, 1)

# Coordinate system B
B = translation
e_Bx = Coordinates(math.cos(rotation), math.sin(rotation))
e_By = Coordinates(-math.sin(rotation), math.cos(rotation))

# Point P
P = Coordinates(2, 2)

# Coordinate system A
ax.arrow(A.x, A.y, axis_length*e_Ax.x, axis_length*e_Ax.y, width=arrow_width, head_width=arrow_head_width, head_length=arrow_head_length, fc='b', ec='b', length_includes_head=True)
ax.arrow(A.x, A.y, axis_length*e_Ay.x, axis_length*e_Ay.y, width=arrow_width, head_width=arrow_head_width, head_length=arrow_head_length, fc='b', ec='b', length_includes_head=True)

# Label the origin of the coordinate system A
ax.text(A.x - 0.2, A.y - 0.2, 'A', fontsize=14, c='b')

# Label the axis of the coordinate system A
ax.text(axis_length*e_Ax.x - 0.1, axis_length*e_Ax.y - 0.3, '$\\vec{x}_A$', fontsize=14, c='b')
ax.text(axis_length*e_Ay.x - 0.3, axis_length*e_Ay.y - 0.1, '$\\vec{y}_A$', fontsize=14, c='b')

# Rotated coordinate system A
ax.arrow(A.x, A.y, axis_length*e_Bx.x, axis_length*e_Bx.y, width=arrow_width, head_width=arrow_head_width, head_length=arrow_head_length, fc='lightgrey', ec='lightgrey', length_includes_head=True)
ax.arrow(A.x, A.y, axis_length*e_By.x, axis_length*e_By.y, width=arrow_width, head_width=arrow_head_width, head_length=arrow_head_length, fc='lightgrey', ec='lightgrey', length_includes_head=True)

# Coordinate system B, green with a green head
ax.arrow(B.x, B.y, axis_length*e_Bx.x, axis_length*e_Bx.y, width=arrow_width, head_width=arrow_head_width, head_length=arrow_head_length, fc='g', ec='g', length_includes_head=True)
ax.arrow(B.x, B.y, axis_length*e_By.x, axis_length*e_By.y, width=arrow_width, head_width=arrow_head_width, head_length=arrow_head_length, fc='g', ec='g', length_includes_head=True)

# Label the origin of the coordinate system B
ax.text(B.x, B.y - 0.25, 'B', fontsize=14, c='g')

# Label the axis of the coordinate system B
ax.text(B.x + axis_length*e_Bx.x, B.y + axis_length*e_Bx.y - 0.3, '$\\vec{x}_B$', fontsize=14, c='g')
ax.text(B.x + axis_length*e_By.x - 0.3, B.y + axis_length*e_By.y - 0.2, '$\\vec{y}_B$', fontsize=14, c='g')

# Translation from A to B
ax.arrow(A.x, A.y, B.x - A.x, B.y - A.y, head_width=arrow_head_width, head_length=arrow_head_length, fc='k', ec='k', length_includes_head=True)
ax.text(0.5*(A.x + B.x) - 0.15, 0.5*(A.y + B.y) + 0.15, '$\\vec{r}_{AB}$', fontsize=14, c='k')

# Rotation from A to B
style = "Simple, tail_width=0.5, head_width=6, head_length=8"
kw = dict(arrowstyle=style, color="k")
a3 = patches.FancyArrowPatch((e_Ax.x, e_Ax.y), (e_Bx.x, e_Bx.y),
                             connectionstyle=f"arc3,rad={rotation / math.pi}", **kw)
ax.add_patch(a3)
ax.text(0.5*(e_Ax.x + e_Bx.x) + 0.10, 0.5*(e_Ax.y + e_Bx.y), '$\\alpha_{AB}$', fontsize=14, c='k')

# Draw point P
ax.scatter(P.x, P.y, c='k')

# Label point P
ax.text(P.x + 0.1, P.y + 0.1, 'P', fontsize=14, c='k')

# Draw vertical lines from P to the axis of the coordinate system A
ax.plot([P.x, P.x], [A.y, P.y], c='b', linestyle='--')
ax.plot([A.x, P.x], [P.y, P.y], c='b', linestyle='--')

# x-axis of B through B: y = a1*x + c1
a1 = e_Bx.y / e_Bx.x
c1 = B.y - a1*B.x

# Perpendicular line to the x-axis of B through P: y = b1*x + d1
b1 = -1/a1
d1 = P.y - b1*P.x

# Intersection of the two lines
x1 = (d1 - c1) / (a1 - b1)
y1 = a1*x1 + c1

ax.plot([P.x, x1], [P.y, y1], c='g', linestyle='--')

# y-axis of B through B: y = a2*x + c2
a2 = e_By.y / e_By.x
c2 = B.y - a2*B.x

# Perpendicular line to the y-axis of B through P: y = b2*x + d2
b2 = -1/a2
d2 = P.y - b2*P.x

# Intersection of the two lines
x2 = (d2 - c2) / (a2 - b2)
y2 = a2*x2 + c2

ax.plot([P.x, x2], [P.y, y2], c='g', linestyle='--')

plt.show()