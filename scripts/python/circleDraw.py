import matplotlib.pyplot as plt
import numpy as np

fig, ax = plt.subplots()

# First circle (no fill, black border)
M1 = (0, 0)
r1 = 150
circle1 = plt.Circle(M1, r1, color='black', fill=False, linewidth=2)
ax.add_patch(circle1)

# Second circle (filled, blue)
M2 = (60.187431523810069, 122.16987032736733)
r2 = 15
v2 = (730.50388463697072, -259.62602644440216)  # velocity vector

circle2 = plt.Circle(M2, r2, color='blue', fill=True)
ax.add_patch(circle2)

# Scale velocity vector for display
scale = 0.05  # adjust to taste
v2_scaled = np.array(v2) * scale

# Draw arrow with proper head
ax.arrow(
    M2[0], M2[1],
    v2_scaled[0], v2_scaled[1],
    head_width=5, head_length=8,
    fc='blue', ec='blue', length_includes_head=True
)

# Equal aspect ratio and limits to fit everything
ax.set_aspect('equal', 'box')
ax.set_xlim(-r1 - 20, r1 + 200)
ax.set_ylim(-r1 - 20, r1 + 200)

plt.show()
