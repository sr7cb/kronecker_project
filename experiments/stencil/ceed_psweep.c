#include <ceed.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

/* Time CeedBasisApply (sum-factorization) for 3D basis at degree p.
   nelem elements, EVAL_INTERP direction (P->Q nodes). */
static double bench_basis(const char *ceed_spec, int p, int nelem, int reps) {
    Ceed ceed;
    CeedInit(ceed_spec, &ceed);

    int q = p + 2;  /* quadrature pts per dim */
    int P = p + 1;  /* nodes per dim */
    int dim = 3;

    CeedBasis basis;
    CeedBasisCreateTensorH1Lagrange(ceed, dim, 1, P, q,
                                    CEED_GAUSS, &basis);

    CeedInt nqpts;
    CeedBasisGetNumQuadraturePoints(basis, &nqpts);
    CeedInt ndof;
    CeedBasisGetNumNodes(basis, &ndof);

    CeedVector u, v;
    CeedVectorCreate(ceed, (CeedInt)nelem * ndof, &u);
    CeedVectorCreate(ceed, (CeedInt)nelem * nqpts, &v);
    CeedVectorSetValue(u, 1.0);

    /* warmup: 3 calls let the opt backend JIT-compile and stabilize */
    for (int w = 0; w < 3; w++)
        CeedBasisApply(basis, nelem, CEED_NOTRANSPOSE,
                       CEED_EVAL_INTERP, u, v);

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    for (int r = 0; r < reps; r++)
        CeedBasisApply(basis, nelem, CEED_NOTRANSPOSE,
                       CEED_EVAL_INTERP, u, v);
    clock_gettime(CLOCK_MONOTONIC, &t1);

    double secs = (t1.tv_sec - t0.tv_sec) + 1e-9*(t1.tv_nsec - t0.tv_nsec);
    /* sum-factorization flops: dim * nelem * P^(dim-1) * q^dim * 2 */
    double flops = (double)dim * nelem * pow(P, dim-1) * pow(q, dim) * 2.0 * reps;
    double gflops = flops / secs / 1e9;

    CeedVectorDestroy(&u);
    CeedVectorDestroy(&v);
    CeedBasisDestroy(&basis);
    CeedDestroy(&ceed);
    return gflops;
}

int main(int argc, char **argv) {
    const char *spec = argc > 1 ? argv[1] : "/cpu/self/opt/serial";
    int nelem = argc > 2 ? atoi(argv[2]) : 4096;
    int reps  = argc > 3 ? atoi(argv[3]) : 20;

    printf("# ceed=%s nelem=%d reps=%d\n", spec, nelem, reps);
    printf("# p q gflops\n");

    int ps[] = {1,2,3,4,5,6,7,8,9,10,12,14,16,0};
    for (int i = 0; ps[i]; i++) {
        double g = bench_basis(spec, ps[i], nelem, reps);
        printf("%2d %2d %.3f\n", ps[i], ps[i]+2, g);
        fflush(stdout);
    }
    return 0;
}
