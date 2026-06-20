#include <blis.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Sweep KC with MC fixed at the known L2-optimal value and NC = n (full
   width), so the B panel = KC * n * 8 bytes.  This drives the B panel
   through L3 and into DRAM as KC grows:
     KC=256, n=4096: B panel = 8 MB  (L3)
     KC=512,  n=4096: B panel = 16 MB (L3)
     KC=1024, n=4096: B panel = 33 MB (L3 boundary ~36 MB)
     KC=2048, n=4096: B panel = 67 MB (DRAM)
   The A panel (MC * KC * 8) stays in or near L2 for MC=256, KC<=1024. */

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
    double gflops = 2.0 * n * n * n * reps / secs / 1e9;

    free(A); free(B); free(C);
    return gflops;
}

int main(int argc, char **argv) {
    int n    = argc > 1 ? atoi(argv[1]) : 4096;
    int reps = argc > 2 ? atoi(argv[2]) : 5;

    /* MC fixed at L2-optimal; NC = n so B panel = KC * n bytes */
    dim_t MC = 256;
    dim_t NC = (dim_t)n;

    printf("# n=%d reps=%d MC=%d NC=n\n", n, reps, (int)MC);
    printf("# KC B_panel_MB GFLOPS\n");

    dim_t KCs[] = {64, 128, 256, 384, 512, 768, 1024, 1280, 1536, 2048};
    int nKC = 10;
    for (int ki = 0; ki < nKC; ki++) {
        dim_t KC = KCs[ki];
        if (KC > (dim_t)n) break;
        double b_panel_mb = (double)KC * n * sizeof(double) / (1024.0*1024.0);
        double g = bench(MC, KC, NC, n, reps);
        printf("%4d %8.1f %.3f\n", (int)KC, b_panel_mb, g);
        fflush(stdout);
    }
    return 0;
}
