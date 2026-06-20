BLIS_DIR = ../../deps/blis-install
CC       = gcc
CFLAGS   = -O3 -march=native -I$(BLIS_DIR)/include/blis
LDFLAGS  = -L$(BLIS_DIR)/lib -lblis -lm -lpthread -fopenmp

all: blis_sweep blis_kc_sweep blis_probe

blis_sweep: blis_sweep.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

blis_kc_sweep: blis_kc_sweep.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

blis_probe: blis_probe.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f blis_sweep blis_kc_sweep blis_probe
