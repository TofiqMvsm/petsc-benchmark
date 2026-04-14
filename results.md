\# Benchmark Results



\## Setup

\- Framework: DOLFINx (FEniCS) v0.8.0

\- Solver: PETSc

\- Problem 1: 3D Poisson equation (symmetric)

\- Problem 2: 3D Convection-Diffusion equation (non-symmetric)



\## Results



\### POISSON

| Solver | Precond | Mesh | Vertices | Iters | Time(s) |

|--------|---------|------|----------|-------|---------|

| CG | AMG | 20³ | 9,261 | 5 | 0.06 |

| CG | ILU | 20³ | 9,261 | 28 | 0.02 |

| CG | ASM | 20³ | 9,261 | 28 | 0.02 |

| GMRES | AMG | 20³ | 9,261 | 5 | 0.05 |

| GMRES | ILU | 20³ | 9,261 | 28 | 0.02 |

| GMRES | ASM | 20³ | 9,261 | 28 | 0.03 |

| CG | AMG | 50³ | 132,651 | 5 | 1.04 |

| CG | ILU | 50³ | 132,651 | 61 | 0.41 |

| CG | ASM | 50³ | 132,651 | 61 | 0.48 |

| GMRES | AMG | 50³ | 132,651 | 5 | 1.03 |

| GMRES | ILU | 50³ | 132,651 | 71 | 0.52 |

| GMRES | ASM | 50³ | 132,651 | 71 | 0.62 |



\### CONVECTION-DIFFUSION

| Solver | Precond | Mesh | Vertices | Iters | Time(s) |

|--------|---------|------|----------|-------|---------|

| CG | AMG | 20³ | 9,261 | 5 | 0.07 |

| CG | ILU | 20³ | 9,261 | 1000 | 0.17 |

| CG | ASM | 20³ | 9,261 | 1000 | 0.17 |

| GMRES | AMG | 20³ | 9,261 | 5 | 0.05 |

| GMRES | ILU | 20³ | 9,261 | 23 | 0.02 |

| GMRES | ASM | 20³ | 9,261 | 23 | 0.02 |

| CG | AMG | 50³ | 132,651 | 93 | 6.07 |

| CG | ILU | 50³ | 132,651 | 1000 | 3.39 |

| CG | ASM | 50³ | 132,651 | 1000 | 4.01 |

| GMRES | AMG | 50³ | 132,651 | 5 | 1.04 |

| GMRES | ILU | 50³ | 132,651 | 57 | 0.49 |

| GMRES | ASM | 50³ | 132,651 | 57 | 0.53 |



\## Key Findings

1\. AMG converges in 5 iterations regardless of mesh size

2\. CG fails on non-symmetric problems (hits 1000 iteration limit)

3\. GMRES + AMG is the best combination for non-symmetric problems

4\. ILU and ASM behave similarly on single CPU

