from mpi4py import MPI
from dolfinx import mesh, fem
from dolfinx.fem.petsc import LinearProblem
import ufl
from ufl import dx, grad, inner, div
import numpy as np
import time

def solve_poisson(n, ksp_type, pc_type):
    msh = mesh.create_unit_cube(MPI.COMM_WORLD, n, n, n)
    V = fem.functionspace(msh, ("Lagrange", 1))

    def boundary(x):
        return np.logical_or(np.isclose(x[0], 0), np.isclose(x[0], 1))

    dofs = fem.locate_dofs_geometrical(V, boundary)
    bc = fem.dirichletbc(0.0, dofs, V)

    u = ufl.TrialFunction(V)
    v = ufl.TestFunction(V)
    f = fem.Constant(msh, 1.0)
    a = inner(grad(u), grad(v)) * dx
    L = inner(f, v) * dx

    options = build_options(ksp_type, pc_type)
    problem = LinearProblem(a, L, bcs=[bc], petsc_options=options)

    start = time.time()
    uh = problem.solve()
    elapsed = time.time() - start

    ksp = problem.solver
    its = ksp.getIterationNumber()
    vertices = msh.topology.index_map(0).size_global
    return vertices, its, elapsed

def solve_convdiff(n, ksp_type, pc_type):
    msh = mesh.create_unit_cube(MPI.COMM_WORLD, n, n, n)
    V = fem.functionspace(msh, ("Lagrange", 1))

    def boundary(x):
        return np.logical_or(np.isclose(x[0], 0), np.isclose(x[0], 1))

    dofs = fem.locate_dofs_geometrical(V, boundary)
    bc = fem.dirichletbc(0.0, dofs, V)

    u = ufl.TrialFunction(V)
    v = ufl.TestFunction(V)

    # Wind vector - makes matrix non-symmetric
    wind = fem.Constant(msh, np.array([10.0, 0.0, 0.0]))
    f = fem.Constant(msh, 1.0)

    # Diffusion + convection term
    a = inner(grad(u), grad(v)) * dx + inner(wind, grad(u)) * v * dx
    L = inner(f, v) * dx

    options = build_options(ksp_type, pc_type)
    problem = LinearProblem(a, L, bcs=[bc], petsc_options=options)

    start = time.time()
    try:
        uh = problem.solve()
        elapsed = time.time() - start
        ksp = problem.solver
        its = ksp.getIterationNumber()
        return msh.topology.index_map(0).size_global, its, elapsed
    except Exception as e:
        return msh.topology.index_map(0).size_global, -1, -1

def build_options(ksp_type, pc_type):
    options = {
        "ksp_type": ksp_type,
        "pc_type": pc_type,
        "ksp_rtol": 1e-8,
        "ksp_max_it": 1000,
    }
    if pc_type == "hypre":
        options["pc_hypre_type"] = "boomeramg"
    return options

configs = [
    ("cg",    "hypre", "AMG"),
    ("cg",    "ilu",   "ILU"),
    ("cg",    "asm",   "ASM"),
    ("gmres", "hypre", "AMG"),
    ("gmres", "ilu",   "ILU"),
    ("gmres", "asm",   "ASM"),
]

mesh_sizes = [20, 50]

for problem_name, solver_func in [("POISSON", solve_poisson), ("CONV-DIFF", solve_convdiff)]:
    print("=" * 70)
    print(f"  {problem_name}")
    print("=" * 70)
    print(f"{'Solver':<8} {'Precond':<8} {'Mesh':<6} {'Vertices':<12} {'Iters':<8} {'Time(s)'}")
    print("-" * 70)
    for n in mesh_sizes:
        for ksp, pc, pc_name in configs:
            verts, its, t = solver_func(n, ksp, pc)
            if its == -1:
                print(f"{ksp.upper():<8} {pc_name:<8} {n}^3    {verts:<12} FAILED   FAILED")
            else:
                print(f"{ksp.upper():<8} {pc_name:<8} {n}^3    {verts:<12} {its:<8} {t:.2f}")
        print("-" * 70)