# PETSc Benchmark: Parallel Sparse Linear Solvers

## Overview
Benchmarking Krylov solvers and preconditioners for large-scale PDE 
problems using FEniCS (DOLFINx) and PETSc, with CPU and GPU 
comparisons on AMD and NVIDIA hardware.

University of Tartu — Distributed Systems Seminar 2026
Tofig Movsumov

## Research Questions
1. Which Krylov solver + preconditioner combination is most scalable?
2. At what problem size does GPU acceleration become beneficial?
3. How does AMD unified memory compare to NVIDIA discrete GPU for sparse solvers?

## Problems
- 3D Poisson equation (symmetric) — FEniCS + PETSc bench
- 3D Convection-Diffusion equation (non-symmetric) — custom PETSc benchmark

## Methods Tested
- Solvers: CG, GMRES
- Preconditioners: AMG (BoomerAMG), ILU, Additive Schwarz (ASM)
- Mesh sizes: 20³ (8k), 50³ (125k), 100³ (1M unknowns)

## Hardware
- AMD HP Z2 Mini — AMD Radeon 8060S, 94GB unified memory (CPU+GPU share same RAM)
- UT HPC falcon2 — NVIDIA Tesla V100 32GB HBM2

## Key Findings
1. AMG converges in 5 iterations regardless of mesh size — ILU/ASM grow to 196
2. CG fails completely on non-symmetric problems — GMRES required
3. GPU crossover point is ~125k unknowns — CPU faster below, GPU faster above
4. NVIDIA V100 is 1.6--4.9x faster than AMD Radeon 8060S at 1M unknowns
5. AMD unified memory competitive at medium scale due to no PCIe overhead

## Repository Structure
- `benchmark.py` — FEniCS CPU benchmark (Poisson + Convection-Diffusion)
- `bench_kspsolve.c` — PETSc 3D Poisson benchmark (CPU + GPU)
- `bench_convdiff.c` — PETSc Convection-Diffusion benchmark (CPU + GPU)

## Why C for GPU benchmarks?
The FEniCS Python DOLFINx container was compiled without GPU support.
PETSc's native C benchmarks include built-in CUDA and HIP backends,
enabling direct GPU execution without recompiling FEniCS.

## Results

### Poisson — CPU Comparison
| Solver | Precond | Mesh | Iters | AMD Z2(s) | HPC(s) |
|--------|---------|------|-------|-----------|--------|
| CG | AMG | 20³ | 5 | 0.04 | 0.10 |
| GMRES | AMG | 20³ | 5 | 0.03 | 0.12 |
| CG | AMG | 50³ | 5 | 0.67 | 1.83 |
| GMRES | AMG | 50³ | 5 | 0.68 | 1.84 |
| CG | AMG | 100³ | 5 | 6.81 | 16.95 |
| GMRES | AMG | 100³ | 5 | 6.57 | 16.87 |
| GMRES | ILU | 100³ | 196 | 6.67 | 18.81 |

### AMD GPU vs NVIDIA V100
| Problem | Precond | Mesh | AMD(s) | NVIDIA(s) | Speedup |
|---------|---------|------|--------|-----------|---------|
| Poisson | ILU | 50³ | 0.092 | 0.106 | 0.9x |
| Poisson | ILU | 100³ | 0.615 | 0.379 | 1.6x |
| ConvDiff | ASM | 100³ | 1.566 | 0.456 | 3.4x |
| ConvDiff | ILU | 100³ | 1.479 | 0.303 | 4.9x |

### CPU vs GPU (AMD Z2 Mini)
| Solver | Precond | Mesh | CPU(s) | GPU(s) | Speedup |
|--------|---------|------|--------|--------|---------|
| GMRES | ILU | 20³ | 0.005 | 0.048 | 0.1x |
| GMRES | ILU | 50³ | 0.134 | 0.092 | 1.5x |
| GMRES | ILU | 100³ | 1.575 | 0.615 | 2.6x |

## Setup

### CPU Benchmarks (FEniCS)
```bash
docker run -it dolfinx/dolfinx:v0.8.0
python3 benchmark.py
```

### GPU Benchmarks (PETSc + HIP — AMD)
```bash
# Build PETSc with HIP
./configure --with-hip=1 --with-hipc=hipcc --prefix=$HOME/petsc-hip
make all install
cd $PETSC_DIR/share/petsc/examples/src/ksp/ksp/tutorials/
mpicc -o bench bench_kspsolve.c -I$PETSC_DIR/include -L$PETSC_DIR/lib -lpetsc -lm
mpiexec -n 1 ./bench -mat_type aijhipsparse -ksp_type gmres -pc_type ilu -n 100
```

### GPU Benchmarks (PETSc + CUDA — NVIDIA)
```bash
# Build PETSc with CUDA
module load gcc/11.4.1 cuda/12.1.1 openmpi/4.1.5 python/3.11.2 cmake/3.26.3
./configure --with-cuda=1 --with-cudac=$(which nvcc) --prefix=$HOME/petsc-cuda
make all install
mpiexec -n 1 ./bench -mat_type aijcusparse -ksp_type gmres -pc_type ilu -n 100
```

## References
- Saad, Y. (2003). Iterative Methods for Sparse Linear Systems, 2nd ed. SIAM.
- Saad & Schultz (1986). GMRES. SIAM Journal on Scientific Computing.
- Falgout & Yang (2002). hypre: A library of high performance preconditioners.
- Balay et al. PETSc/TAO User's Manual, Argonne National Laboratory.
- Baratta et al. DOLFINx: The next generation FEniCS problem solving environment.
