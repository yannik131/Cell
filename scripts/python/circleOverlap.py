from sympy import symbols, Eq, solve, simplify, sqrt

rx, ry, vx, vy, R1, R2, t = symbols("rx ry vx vy R1 R2 t")
eq = Eq(sqrt((rx + t*vx)**2+(ry+t*vy)**2), R1+R2)
solution = solve(eq, t)

simplified = [simplify(sol) for sol in solution]

for sol in simplified:
	print(sol)