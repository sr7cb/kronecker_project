#include <blis.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Single-point probe: blis_probe <MC> <KC> <NC> <n> <reps> */

static double bench(dim_t MC, dim_t KC, dim_t NC, int n, int reps) {
    const cntx_t *gcntx = bli_gks_query_cntx();
    cntx_t cntx;
    memcpy(&cntx, gcntx, sizeof(cntx_t));
    bli_cntx_set_blksz_def_dt(BLIS_DOUBLE, BLIS_MC, MC, &cntx);
    bli_cntx_set_blksz_max_dt(BLIS_DOUBLE, BLIS_MC, MC, &cntx);
    bli_cntx_set_blksz_def_dt(BLIS_DOUBLE, BLIS_KC, KC, &cntx);
    bli_cntx_set_blksz_max_dt(BLIS_DOUBLE, BLIS_KC, KC, &cntx);
    bli_cntx_set_blksz_def_dt(BLIS_DOUBLE, BLIS_NC, NC, &cntx);
    bli_cntx_set_blksz_max_dt(BLIS_DOUBLE, BLIS_NC, NC, &cntx);

    rntm_t rntm;
    bli_rntm_init_from_global(&rntm);
    bli_rntm_set_num_threads(1, &rntm);

    double *A = malloc((size_t)n*n*sizeof(double));
    double *B = malloc((size_t)n*n*sizeof(double));
    double *C = calloc((size_t)n*n, sizeof(double));
    for (int i = 0; i < n*n; i++) { A[i] = 1.0; B[i] = 1.0; }

    obj_t oa, ob, oc, oalpha, obeta;
    bli_obj_create_1x1(BLIS_DOUBLE, &oalpha); bli_setsc(1.0, 0.0, &oalpha);
    bli_obj_create_1x1(BLIS_DOUBLE, &obeta);  bli_setsc(1.0, 0.0, &obeta);
    bli_obj_create_with_attached_buffer(BLIS_DOUBLE, n, n, A, n, 1, &oa);
    bli_obj_create_with_attached_buffer(BLIS_DOUBLE, n, n, B, n, 1, &ob);
    bli_obj_create_with_attached_buffer(BLIS_DOUBLE, n, n, C, n, 1, &oc);

    bli_gemm_ex(&oalpha, &oa, &ob, &obeta, &oc, &cntx, &rntm);

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    for (int r = 0; r < reps; r++)
        bli_gemm_ex(&oalpha, &oa, &ob, &obeta, &oc, &cntx, &rntm);
    clock_gettime(CLOCK_MONOTONIC, &t1);

    double secs = (t1.tv_sec - t0.tv_sec) + 1e-9*(t1.tv_nsec - t0.tv_nsec);
    free(A); free(B); free(C);
    return 2.0 * n * n * n * reps / secs / 1e9;
}

int main(int argc, char **argv) {
    if (argc < 6) {
        fprintf(stderr, "usage: blis_probe <MC> <KC> <NC> <n> <reps>\n");
        return 1;
    }
    dim_t MC = atoi(argv[1]);
    dim_t KC = atoi(argv[2]);
    dim_t NC = atoi(argv[3]);
    int   n  = atoi(argv[4]);
    int reps = atoi(argv[5]);

    double g = bench(MC, KC, NC, n, reps);
    printf("MC=%d KC=%d NC=%d n=%d reps=%d GFLOPS=%.3f\n",
           (int)MC, (int)KC, (int)NC, n, reps, g);
    return 0;
}
