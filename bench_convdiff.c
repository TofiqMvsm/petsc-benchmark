#include <petscksp.h>

static char help[] = "Solves 3D Convection-Diffusion using KSP.\n";

int main(int argc, char **argv)
{
  KSP            ksp;
  Mat            A;
  Vec            x, b;
  PetscInt       n = 50, N, Istart, Iend, i, j, k, II, ncols;
  PetscReal      wind = 10.0, h, val;
  PetscScalar    v;
  PetscErrorCode ierr;
  PetscLogDouble t1, t2;
  PetscInt       its;
  PetscReal      rnorm;

  ierr = PetscInitialize(&argc, &argv, NULL, help);
  ierr = PetscOptionsGetInt(NULL, NULL, "-n", &n, NULL);

  N = n * n * n;
  h = 1.0 / (n + 1);

  ierr = MatCreate(PETSC_COMM_WORLD, &A);
  ierr = MatSetSizes(A, PETSC_DECIDE, PETSC_DECIDE, N, N);
  ierr = MatSetFromOptions(A);
  ierr = MatSetUp(A);

  ierr = MatGetOwnershipRange(A, &Istart, &Iend);

  for (II = Istart; II < Iend; II++) {
    i = II / (n * n);
    j = (II % (n * n)) / n;
    k = II % n;

    /* Diagonal */
    v = 6.0 / (h * h);
    ierr = MatSetValues(A, 1, &II, 1, &II, &v, ADD_VALUES);

    /* x-direction diffusion */
    if (k > 0)     { PetscInt jj = II-1; v = -1.0/(h*h) - wind/(2*h); ierr = MatSetValues(A,1,&II,1,&jj,&v,ADD_VALUES); }
    if (k < n-1)   { PetscInt jj = II+1; v = -1.0/(h*h) + wind/(2*h); ierr = MatSetValues(A,1,&II,1,&jj,&v,ADD_VALUES); }

    /* y-direction diffusion */
    if (j > 0)     { PetscInt jj = II-n;   v = -1.0/(h*h); ierr = MatSetValues(A,1,&II,1,&jj,&v,ADD_VALUES); }
    if (j < n-1)   { PetscInt jj = II+n;   v = -1.0/(h*h); ierr = MatSetValues(A,1,&II,1,&jj,&v,ADD_VALUES); }

    /* z-direction diffusion */
    if (i > 0)     { PetscInt jj = II-n*n; v = -1.0/(h*h); ierr = MatSetValues(A,1,&II,1,&jj,&v,ADD_VALUES); }
    if (i < n-1)   { PetscInt jj = II+n*n; v = -1.0/(h*h); ierr = MatSetValues(A,1,&II,1,&jj,&v,ADD_VALUES); }
  }

  ierr = MatAssemblyBegin(A, MAT_FINAL_ASSEMBLY);
  ierr = MatAssemblyEnd(A, MAT_FINAL_ASSEMBLY);

  ierr = MatCreateVecs(A, &x, &b);
  ierr = VecSet(b, 1.0);
  ierr = VecSet(x, 0.0);

  ierr = KSPCreate(PETSC_COMM_WORLD, &ksp);
  ierr = KSPSetOperators(ksp, A, A);
  ierr = KSPSetFromOptions(ksp);
  ierr = KSPSetUp(ksp);

  PetscPrintf(PETSC_COMM_WORLD, "===========================================\n");
  PetscPrintf(PETSC_COMM_WORLD, "Test: Convection-Diffusion 3D\n");
  PetscPrintf(PETSC_COMM_WORLD, "        -n %d\n", n);
  PetscPrintf(PETSC_COMM_WORLD, "        DoFs = %d\n", N);
  PetscPrintf(PETSC_COMM_WORLD, "        wind = %.1f\n", wind);

  ierr = PetscTime(&t1);
  ierr = KSPSolve(ksp, b, x);
  ierr = PetscTime(&t2);

  ierr = KSPGetIterationNumber(ksp, &its);
  ierr = KSPGetResidualNorm(ksp, &rnorm);

  PetscPrintf(PETSC_COMM_WORLD, "KSP iters:     %d\n", its);
  PetscPrintf(PETSC_COMM_WORLD, "Residual norm: %g\n", rnorm);
  PetscPrintf(PETSC_COMM_WORLD, "KSPSolve:      %.5f seconds\n", t2-t1);
  PetscPrintf(PETSC_COMM_WORLD, "===========================================\n");

  ierr = KSPDestroy(&ksp);
  ierr = MatDestroy(&A);
  ierr = VecDestroy(&x);
  ierr = VecDestroy(&b);
  ierr = PetscFinalize();
  return 0;
}