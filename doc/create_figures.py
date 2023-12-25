#!/usr/bin/env python3

import math
import matplotlib.pyplot as plt
import matplotlib.patches as patches
from dataclasses import dataclass

@dataclass
class Coordinates:
    x: float
    y: float

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


def create_empty_plot(figsize):
    fig, ax = plt.subplots(figsize=figsize)

    # Do not plot axis
    plt.axis('off')

    # Keep aspect ratio
    plt.gca().set_aspect('equal', adjustable='box')

    # Shrink plot area to fit all the objects
    plt.tight_layout()

    return fig, ax


def create_figure_of_coordinate_system_a(figsize, color):
    fig, ax = create_empty_plot(figsize)

    # Coordinate system A
    ax.arrow(A.x, A.y, axis_length*e_Ax.x, axis_length*e_Ax.y, width=arrow_width, head_width=arrow_head_width, head_length=arrow_head_length, fc=color, ec=color, length_includes_head=True)
    ax.arrow(A.x, A.y, axis_length*e_Ay.x, axis_length*e_Ay.y, width=arrow_width, head_width=arrow_head_width, head_length=arrow_head_length, fc=color, ec=color, length_includes_head=True)
    ax.text(A.x - 0.2, A.y - 0.2, 'A', fontsize=20, c=color)
    ax.text(axis_length*e_Ax.x - 0.1, axis_length*e_Ax.y - 0.3, '$\\vec{x}_A$', fontsize=14, c=color)
    ax.text(axis_length*e_Ay.x - 0.3, axis_length*e_Ay.y - 0.1, '$\\vec{y}_A$', fontsize=14, c=color)

    return fig, ax


def create_figure_of_passive_proper_rigid_transformation():
    fig, ax = create_figure_of_coordinate_system_a((10, 10), 'b')

    # Rotated coordinate system A
    ax.arrow(A.x, A.y, axis_length*e_Bx.x, axis_length*e_Bx.y, width=arrow_width, head_width=arrow_head_width, head_length=arrow_head_length, fc='lightgrey', ec='lightgrey', length_includes_head=True)
    ax.arrow(A.x, A.y, axis_length*e_By.x, axis_length*e_By.y, width=arrow_width, head_width=arrow_head_width, head_length=arrow_head_length, fc='lightgrey', ec='lightgrey', length_includes_head=True)

    # Coordinate system B
    ax.arrow(B.x, B.y, axis_length*e_Bx.x, axis_length*e_Bx.y, width=arrow_width, head_width=arrow_head_width, head_length=arrow_head_length, fc='g', ec='g', length_includes_head=True)
    ax.arrow(B.x, B.y, axis_length*e_By.x, axis_length*e_By.y, width=arrow_width, head_width=arrow_head_width, head_length=arrow_head_length, fc='g', ec='g', length_includes_head=True)
    ax.text(B.x, B.y - 0.25, 'B', fontsize=20, c='g')
    ax.text(B.x + axis_length*e_Bx.x, B.y + axis_length*e_Bx.y - 0.3, '$\\vec{x}_B$', fontsize=14, c='g')
    ax.text(B.x + axis_length*e_By.x - 0.3, B.y + axis_length*e_By.y - 0.2, '$\\vec{y}_B$', fontsize=14, c='g')

    # Rotation from A to B
    style = "Simple, tail_width=0.5, head_width=6, head_length=8"
    kw = dict(arrowstyle=style, color="k")
    a3 = patches.FancyArrowPatch((e_Ax.x, e_Ax.y), (e_Bx.x, e_Bx.y),
                                connectionstyle=f"arc3,rad={rotation / math.pi}", **kw)
    ax.add_patch(a3)
    ax.text(0.5*(e_Ax.x + e_Bx.x) + 0.10, 0.5*(e_Ax.y + e_Bx.y), '$\\alpha_{AB}$', fontsize=14, c='k')

    # Draw a 1 in a circle
    circle1 = Coordinates(0.5*(e_Ax.x + e_Bx.x) + 0.5, 0.5*(e_Ax.y + e_Bx.y) + 0.05)
    ax.add_patch(patches.Circle((circle1.x, circle1.y), 0.1, color='k', fill=False))
    ax.text(circle1.x, circle1.y, '1', fontsize=14, c='k', horizontalalignment='center', verticalalignment='center')

    # Translation from A to B
    ax.arrow(A.x, A.y, B.x - A.x, B.y - A.y, width=arrow_width, head_width=arrow_head_width, head_length=arrow_head_length, fc='k', ec='k', length_includes_head=True)
    ax.text(0.5*(A.x + B.x) - 0.15, 0.5*(A.y + B.y) + 0.15, '$\\vec{r}_{AB}$', fontsize=14, c='k', horizontalalignment='center', verticalalignment='center')

    # Draw a 2 in a circle
    circle2 = Coordinates(0.5*(A.x + B.x) - 0.45, 0.5*(A.y + B.y) + 0.15)
    ax.add_patch(patches.Circle((circle2.x, circle2.y), 0.1, color='k', fill=False))
    ax.text(circle2.x, circle2.y, '2', fontsize=14, c='k', horizontalalignment='center', verticalalignment='center')

    return fig, ax


def draw_perpendicular_line_from_point_to_line(ax, point, line_point, line_direction, color):
    if line_direction.x == 0:
        # The line is parallel to the y-axis.
        ax.plot([point.x, line_point.x], [point.y, point.y], c=color, linestyle='--')

    elif line_direction.y == 0:
        # The line is parallel to the x-axis.
        ax.plot([point.x, point.x], [point.y, line_point.y], c=color, linestyle='--')
        pass

    else:
        # Line through the point: y = a*x + c
        a = line_direction.y / line_direction.x
        c = line_point.y - a*line_point.x

        # Perpendicular line to the line through the point: y = b*x + d
        b = -1/a
        d = point.y - b*point.x

        # Intersection of the two lines
        x = (d - c) / (a - b)
        y = a*x + c

        # Plot the perpendicular line
        ax.plot([point.x, x], [point.y, y], c=color, linestyle='--')


def create_figure_of_passive_proper_rigid_transformation_of_point():
    fig, ax = create_figure_of_passive_proper_rigid_transformation()

    # Point P
    P = Coordinates(2, 2)
    ax.scatter(P.x, P.y, c='k', s=100)
    ax.text(P.x + 0.1, P.y + 0.1, 'P', fontsize=14, c='k')

    # Draw perpendicular lines from P to the axis of the coordinate system A
    draw_perpendicular_line_from_point_to_line(ax, P, A, e_Ax, 'b')
    draw_perpendicular_line_from_point_to_line(ax, P, A, e_Ay, 'b')

    # Draw perpendicular lines from P to the axis of the coordinate system B
    draw_perpendicular_line_from_point_to_line(ax, P, B, e_Bx, 'g')
    draw_perpendicular_line_from_point_to_line(ax, P, B, e_By, 'g')

    # To svg
    fig.savefig('passive_proper_rigid_transformation_of_point.svg', format='svg', dpi=1200, bbox_inches='tight')


def create_figure_of_passive_proper_rigid_transformation_of_vector():
    fig, ax = create_figure_of_passive_proper_rigid_transformation()

    # Point P
    P = Coordinates(1.7, 1.5)
    ax.scatter(P.x, P.y, c='k', s=100)
    ax.text(P.x + 0.15, P.y - 0.05, 'P', fontsize=14, c='k')

    # Point Q
    Q = Coordinates(2.5, 2.5)
    ax.scatter(Q.x, Q.y, c='k', s=100)
    ax.text(Q.x + 0.15, Q.y - 0.05, 'Q', fontsize=14, c='k')

    # Vector from P to Q
    ax.arrow(P.x, P.y, Q.x - P.x, Q.y - P.y, width=arrow_width, head_width=arrow_head_width, head_length=arrow_head_length, fc='k', ec='k', length_includes_head=True)
    ax.text(0.5*(P.x + Q.x), 0.5*(P.y + Q.y) - 0.15, '$\\vec{r}_{PQ}$', fontsize=14, c='k')

    # Draw perpendicular lines from P to the axis of the coordinate system A
    draw_perpendicular_line_from_point_to_line(ax, P, A, e_Ax, 'b')
    draw_perpendicular_line_from_point_to_line(ax, P, A, e_Ay, 'b')

    # Draw perpendicular lines from P to the axis of the coordinate system B
    draw_perpendicular_line_from_point_to_line(ax, P, B, e_Bx, 'g')
    draw_perpendicular_line_from_point_to_line(ax, P, B, e_By, 'g')

    # Draw perpendicular lines from Q to the axis of the coordinate system A
    draw_perpendicular_line_from_point_to_line(ax, Q, A, e_Ax, 'b')
    draw_perpendicular_line_from_point_to_line(ax, Q, A, e_Ay, 'b')

    # Draw perpendicular lines from Q to the axis of the coordinate system B
    draw_perpendicular_line_from_point_to_line(ax, Q, B, e_Bx, 'g')
    draw_perpendicular_line_from_point_to_line(ax, Q, B, e_By, 'g')

    # To svg
    fig.savefig('passive_proper_rigid_transformation_of_vector.svg', format='svg', dpi=1200, bbox_inches='tight')


def create_figure_of_active_proper_rigid_transformation_of_point():
    fig, ax = create_figure_of_coordinate_system_a((7, 7), 'k')

    # Point P
    angle = math.pi / 6
    radius = 1.5
    P = Coordinates(radius*math.cos(angle), radius*math.sin(angle))
    ax.scatter(P.x, P.y, c='b', s=100)
    ax.text(P.x + 0.1, P.y, 'P', fontsize=14, c='b')

    # Rotated point
    angle2 = angle + rotation
    P2 = Coordinates(radius*math.cos(angle2), radius*math.sin(angle2))
    ax.scatter(P2.x, P2.y, c='lightgrey', s=100)

    # Translated point
    P3 = Coordinates(P2.x + translation.x, P2.y + translation.y)
    ax.scatter(P3.x, P3.y, c='g', s=100)
    ax.text(P3.x + 0.1, P3.y, 'P\'', fontsize=14, c='g')

    # Rotation
    style = "Simple, tail_width=1.5, head_width=10, head_length=14"
    kw = dict(arrowstyle=style, color="k")
    a3 = patches.FancyArrowPatch((P.x, P.y), (P2.x, P2.y),
                                connectionstyle=f"arc3,rad={rotation / math.pi}", **kw)
    ax.add_patch(a3)
    ax.text(0.5*(P.x + P2.x) + 0.1, 0.5*(P.y + P2.y) + 0.05, '$\\alpha$', fontsize=14, c='k')

    # Draw a 1 in a circle
    circle1 = Coordinates(0.5*(P.x + P2.x) + 0.4, 0.5*(P.y + P2.y) + 0.1)
    ax.add_patch(patches.Circle((circle1.x, circle1.y), 0.1, color='k', fill=False))
    ax.text(circle1.x, circle1.y, '1', fontsize=14, c='k', horizontalalignment='center', verticalalignment='center')

    # Translation
    ax.arrow(P2.x, P2.y, translation.x, translation.y, width=arrow_width, head_width=arrow_head_width, head_length=arrow_head_length, fc='k', ec='k', length_includes_head=True)
    ax.text(0.5*(P2.x + P3.x) - 0.1, 0.5*(P2.y + P3.y) + 0.1, '$\\vec{r}$', fontsize=14, c='k', horizontalalignment='center', verticalalignment='center')

    # Draw a 2 in a circle
    circle2 = Coordinates(0.5*(P2.x + P3.x) - 0.35, 0.5*(P2.y + P3.y) + 0.1)
    ax.add_patch(patches.Circle((circle2.x, circle2.y), 0.1, color='k', fill=False))
    ax.text(circle2.x, circle2.y, '2', fontsize=14, c='k', horizontalalignment='center', verticalalignment='center')

    # Draw perpendicular lines from P to the axis of the coordinate system A
    draw_perpendicular_line_from_point_to_line(ax, P, A, e_Ax, 'b')
    draw_perpendicular_line_from_point_to_line(ax, P, A, e_Ay, 'b')

    # Draw perpendicular lines from P3 to the axis of the coordinate system A
    draw_perpendicular_line_from_point_to_line(ax, P3, A, e_Ax, 'g')
    draw_perpendicular_line_from_point_to_line(ax, P3, A, e_Ay, 'g')

    # To svg
    fig.savefig('active_proper_rigid_transformation_of_point.svg', format='svg', dpi=1200, bbox_inches='tight')



create_figure_of_passive_proper_rigid_transformation_of_point()
create_figure_of_passive_proper_rigid_transformation_of_vector()
create_figure_of_active_proper_rigid_transformation_of_point()
