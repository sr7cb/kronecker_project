#include <blis.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static double bench(dim_t MC, dim_t KC, dim_t NC, int n, int reps) {
    /* copy global context then override block sizes */
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
    double gflops = 2.0 * n * n * n * reps / secs / 1e9;

    free(A); free(B); free(C);
    return gflops;
}

int main(int argc, char **argv) {
    int n    = argc > 1 ? atoi(argv[1]) : 2048;
    int reps = argc > 2 ? atoi(argv[2]) : 10;

    printf("# n=%d reps=%d\n", n, reps);
    printf("# MC KC NC GFLOPS\n");

    dim_t KCs[] = {64, 128, 160, 256, 384, 512};
    dim_t MCs[] = {36, 72, 120, 152, 200, 256, 384, 448, 512, 640};
    dim_t NC    = 4080;

    for (int ki = 0; ki < 6; ki++)
    for (int mi = 0; mi < 10; mi++) {
        double g = bench(MCs[mi], KCs[ki], NC, n, reps);
        printf("%4d %4d %4d %.3f\n", (int)MCs[mi], (int)KCs[ki], (int)NC, g);
        fflush(stdout);
    }
    return 0;
}
