from sympy import symbols, Eq, solve, simplify, sqrt, factor_terms

Mx, My, vx, vy, t, Rc, Rm = symbols("Mx My vx vy t Rc Rm")
eq = Eq(sqrt((Mx-t*vx)**2+(My-t*vy)**2)+Rc-Rm, 0)
solution = solve(eq, t)

simplified = [simplify(sol) for sol in solution]

for sol in simplified:
	print(sol)