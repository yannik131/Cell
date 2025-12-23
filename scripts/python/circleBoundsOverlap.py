from sympy import symbols, Eq, solve, simplify, sqrt, factor_terms

Mx, My, rx, ry, vx, vy, t, Rc, Rm = symbols("Mx My rx ry vx vy t Rc Rm", real=True)
eq = Eq(sqrt((Mx-(rx+t*vx))**2+(My-(ry+t*vy))**2)+Rc-Rm, 0)
solution = solve(eq, t)

simplified = [simplify(sol) for sol in solution]

for sol in simplified:
	print(sol)