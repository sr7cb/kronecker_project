CEED_DIR = ../../deps/libceed
CC       = gcc
CFLAGS   = -O3 -march=native -I$(CEED_DIR)/include
LDFLAGS  = -L$(CEED_DIR)/lib -Wl,-rpath,$(CEED_DIR)/lib -lceed -lm

ceed_psweep: ceed_psweep.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f ceed_psweep
