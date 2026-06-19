#include <fftw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

static double bench_fftw(int n, unsigned flags, int reps) {
    fftw_complex *in  = fftw_malloc(n * sizeof(fftw_complex));
    fftw_complex *out = fftw_malloc(n * sizeof(fftw_complex));
    fftw_plan p = fftw_plan_dft_1d(n, in, out, FFTW_FORWARD, flags);
    for (int i = 0; i < n; i++) { in[i][0] = 1.0; in[i][1] = 0.0; }

    fftw_execute(p);  /* warmup */

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    for (int r = 0; r < reps; r++) fftw_execute(p);
    clock_gettime(CLOCK_MONOTONIC, &t1);

    double secs = (t1.tv_sec - t0.tv_sec) + 1e-9*(t1.tv_nsec - t0.tv_nsec);
    double gflops = 5.0 * n * log2(n) * reps / secs / 1e9;

    fftw_destroy_plan(p);
    fftw_free(in); fftw_free(out);
    return gflops;
}

int main(void) {
    /* sizes: powers of 2, mixed radix (2*3, 2*5, 2*7), and composites */
    int sizes[] = {
        64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384,
        96, 192, 384, 768, 1536, 3072, 6144,   /* 2^k * 3 */
        160, 320, 640, 1280, 2560, 5120,        /* 2^k * 5 */
        224, 448, 896, 1792, 3584,              /* 2^k * 7 */
        0
    };

    int reps = 50;
    printf("# n plan gflops\n");

    for (int i = 0; sizes[i]; i++) {
        int n = sizes[i];

        /* ESTIMATE: what FFTW will choose with full planning */
        fftw_complex *in  = fftw_malloc(n * sizeof(fftw_complex));
        fftw_complex *out = fftw_malloc(n * sizeof(fftw_complex));
        fftw_plan p = fftw_plan_dft_1d(n, in, out, FFTW_FORWARD, FFTW_MEASURE);
        char *plan_str = fftw_sprint_plan(p);
        fftw_destroy_plan(p);
        fftw_free(in); fftw_free(out);

        double g = bench_fftw(n, FFTW_MEASURE, reps);
        printf("%6d \"%s\" %.3f\n", n, plan_str, g);
        fflush(stdout);
        free(plan_str);
    }
    return 0;
}
