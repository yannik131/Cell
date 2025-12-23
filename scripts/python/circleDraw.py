import matplotlib.pyplot as plt
import numpy as np
import re

def parse_xy(s: str) -> tuple[float, float]:
    x = float(re.search(r"x=([+-]?\d+(?:\.\d+)?)", s).group(1))
    y = float(re.search(r"y=([+-]?\d+(?:\.\d+)?)", s).group(1))
    return (x, y)

fig, ax = plt.subplots()

# First circle (no fill, black border)
M1 = parse_xy("{x=-3000.0000000000000 y=-3000.0000000000000 }")
r1 = 500
circle1 = plt.Circle(M1, r1, color='black', fill=False, linewidth=2)
ax.add_patch(circle1)

# Second circle (filled, blue)
M2 = parse_xy("{x=-3772.7090022480993 y=-2957.4260550139793 }")
r2 = 12
v2 = parse_xy("{x=832.14699767767570 y=726.35701326998037 }")

circle2 = plt.Circle(M2, r2, color='blue', fill=True)
ax.add_patch(circle2)

# Scale velocity vector for display
scale = 1  # adjust to taste
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

plt.show()
