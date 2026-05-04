# PETSc Benchmark: Parallel Sparse Linear Solvers

## Overview
Benchmarking Krylov solvers and preconditioners for large-scale PDE problems using FEniCS (DOLFINx) and PETSc, with CPU and GPU comparisons on AMD hardware.

## Problems
- 3D Poisson equation (symmetric) — 7-point stencil via FEniCS, 27-point stencil via PETSc bench
- 3D Convection-Diffusion equation (non-symmetric) — custom PETSc benchmark

## Methods Tested
- Solvers: CG, GMRES
- Preconditioners: AMG (BoomerAMG), ILU, Additive Schwarz (ASM)

## Hardware Environments
- AMD HP Z2 Mini — AMD Radeon 8060S GPU, 94GB unified memory (CPU + GPU share same RAM)
- UT HPC falcon2 — NVIDIA Tesla GPU node, 515GB RAM

## Repository Structure
- `benchmark.py` — FEniCS CPU benchmark (Poisson + Convection-Diffusion)
- `bench_kspsolve.c` — PETSc 3D Poisson benchmark (CPU + AMD GPU)
- `bench_convdiff.c` — PETSc Convection-Diffusion benchmark (CPU + AMD GPU)
- `results.md` — All benchmark results

---

## Results

### 1. Poisson Equation — FEniCS CPU Benchmark

| Solver | Precond | Mesh | Iters | AMD Z2 CPU(s) | HPC CPU(s) |
|--------|---------|------|-------|---------------|------------|
| CG | AMG | 20³ | 5 | 0.04 | 0.10 |
| CG | ILU | 20³ | 28 | 0.01 | 0.05 |
| CG | ASM | 20³ | 28 | 0.01 | 0.05 |
| GMRES | AMG | 20³ | 5 | 0.03 | 0.12 |
| GMRES | ILU | 20³ | 28 | 0.01 | 0.05 |
| GMRES | ASM | 20³ | 28 | 0.01 | 0.06 |
| CG | AMG | 50³ | 5 | 0.67 | 1.83 |
| CG | ILU | 50³ | 61 | 0.29 | 0.89 |
| CG | ASM | 50³ | 61 | 0.31 | 0.98 |
| GMRES | AMG | 50³ | 5 | 0.68 | 1.84 |
| GMRES | ILU | 50³ | 71 | 0.38 | 1.13 |
| GMRES | ASM | 50³ | 71 | 0.40 | 1.24 |
| CG | AMG | 100³ | 5 | 6.81 | 16.95 |
| CG | ILU | 100³ | 116 | 3.42 | 10.35 |
| CG | ASM | 100³ | 116 | 3.89 | 11.48 |
| GMRES | AMG | 100³ | 5 | 6.57 | 16.87 |
| GMRES | ILU | 100³ | 196 | 6.67 | 18.81 |
| GMRES | ASM | 100³ | 196 | 7.11 | 20.81 |

### 2. Convection-Diffusion — FEniCS CPU Benchmark (GMRES only)

| Precond | Mesh | Iters | AMD Z2 CPU(s) | HPC CPU(s) |
|---------|------|-------|---------------|------------|
| AMG | 20³ | 5 | 0.04 | 0.11 |
| ILU | 20³ | 23 | 0.01 | 0.05 |
| ASM | 20³ | 23 | 0.01 | 0.05 |
| AMG | 50³ | 5 | 0.71 | 2.00 |
| ILU | 50³ | 57 | 0.33 | 1.08 |
| ASM | 50³ | 57 | 0.36 | 1.17 |
| AMG | 100³ | 6 | 7.39 | 18.38 |
| ILU | 100³ | 188 | 6.39 | 18.66 |
| ASM | 100³ | 188 | 6.91 | 20.50 |

### 3. Poisson — AMD GPU vs CPU (PETSc bench_kspsolve, 27-point stencil)

| Solver | Precond | Mesh | Iters | CPU(s) | GPU(s) | Speedup |
|--------|---------|------|-------|--------|--------|---------|
| GMRES | ASM | 20³ | 12 | 0.007 | 0.063 | CPU faster |
| GMRES | ASM | 50³ | 23 | 0.164 | 0.134 | 1.2x |
| GMRES | ASM | 100³ | 41 | 1.811 | 0.802 | 2.3x |
| GMRES | ILU | 20³ | 12 | 0.005 | 0.048 | CPU faster |
| GMRES | ILU | 50³ | 23 | 0.134 | 0.092 | 1.5x |
| GMRES | ILU | 100³ | 41 | 1.575 | 0.615 | 2.6x |
| CG | ASM | 20³ | 12 | 0.007 | 0.063 | CPU faster |
| CG | ASM | 50³ | 23 | 0.141 | 0.136 | 1.0x |
| CG | ASM | 100³ | 40 | 1.451 | 0.741 | 2.0x |
| CG | ILU | 20³ | 12 | 0.004 | 0.047 | CPU faster |
| CG | ILU | 50³ | 23 | 0.111 | 0.092 | 1.2x |
| CG | ILU | 100³ | 40 | 1.224 | 0.564 | 2.2x |

### 4. Convection-Diffusion — AMD GPU vs CPU (PETSc bench_convdiff, GMRES only)

| Precond | Mesh | Iters | CPU(s) | GPU(s) | Speedup |
|---------|------|-------|--------|--------|---------|
| ASM | 20³ | 19 | 0.004 | 0.074 | CPU faster |
| ASM | 50³ | 55 | 0.183 | 0.143 | 1.3x |
| ASM | 100³ | 131 | 3.164 | 1.566 | 2.0x |
| ILU | 20³ | 19 | 0.003 | 0.034 | CPU faster |
| ILU | 50³ | 55 | 0.167 | 0.092 | 1.8x |
| ILU | 100³ | 131 | 2.904 | 1.479 | 2.0x |

---

## Key Findings
1. AMG converges in 5 iterations regardless of mesh size — optimal for symmetric problems
2. CG fails on non-symmetric problems (hits 1000 iteration limit ❌) — GMRES required
3. GPU slower than CPU for small problems (20³) due to initialization overhead
4. GPU 2-3x faster than CPU for large problems (100³) — unified memory advantage
5. Crossover point is around 50³ (125,000 unknowns)
6. ILU benefits more from GPU acceleration than ASM

## Setup

### CPU Benchmarks (FEniCS)
```bash
docker run -it dolfinx/dolfinx:v0.8.0
python3 benchmark.py
```

### GPU Benchmarks (PETSc + HIP)
```bash
# Build PETSc with HIP support
./configure --with-hip=1 --with-hipc=hipcc --prefix=/home/user/petsc-hip
make all install

# Compile benchmarks
mpicc -o bench bench_kspsolve.c -I$PETSC_DIR/include -L$PETSC_DIR/lib -lpetsc -lm
mpicc -o bench_convdiff bench_convdiff.c -I$PETSC_DIR/include -L$PETSC_DIR/lib -lpetsc -lm

# Run on GPU
mpiexec -n 1 ./bench -mat_type aijhipsparse -ksp_type gmres -pc_type ilu -n 100
mpiexec -n 1 ./bench_convdiff -mat_type aijhipsparse -ksp_type gmres -pc_type ilu -n 100
```

## References
- Saad, Y. (2003). Iterative Methods for Sparse Linear Systems, 2nd ed. SIAM.
- Balay et al. PETSc/TAO User's Manual, Argonne National Laboratory.
- Baratta et al. DOLFINx: The next generation FEniCS problem solving environment, 2023.
