FFTW_DIR = ../../deps/fftw-install
CC       = gcc
CFLAGS   = -O3 -march=native -I$(FFTW_DIR)/include
LDFLAGS  = -L$(FFTW_DIR)/lib -lfftw3 -lm

fftw_sweep: fftw_sweep.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f fftw_sweep
