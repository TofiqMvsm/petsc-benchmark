# PETSc Benchmark: Parallel Sparse Linear Solvers

## Overview
Benchmarking Krylov solvers and preconditioners for large-scale 
PDE problems using FEniCS (DOLFINx) and PETSc.

## Problems
- 3D Poisson equation (symmetric)
- 3D Convection-Diffusion equation (non-symmetric)

## Methods Tested
- Solvers: CG, GMRES
- Preconditioners: AMG (BoomerAMG), ILU, Additive Schwarz (ASM)

## Key Findings
- AMG converges in 5 iterations regardless of mesh size
- CG fails on non-symmetric problems (1000 iteration limit hit)
- GMRES + AMG is optimal for non-symmetric problems

## Setup
- FEniCS DOLFINx v0.8.0
- PETSc (via Docker)
- Python 3

## Run
docker run -it dolfinx/dolfinx:v0.8.0
python3 benchmark.py
