#!/bin/bash
# Sweep polynomial order p for 3D diffusion (libCEED bp3 benchmark)
# Factor size is p; optimal window predicted by shared-mem / cache capacity.

CEED_DIR=../../deps/libceed
OUT=libceed_psweep.csv

BACKEND_CPU="/cpu/self/opt/serial"
BACKEND_GPU="/gpu/cuda/gen"

echo "backend,p,dofs_per_s,gflops" > $OUT

for backend in "$BACKEND_CPU" "$BACKEND_GPU"; do
  for p in 1 2 3 4 5 6 7 8 9 10 12 14 16; do
    result=$($CEED_DIR/build/benchmarks/bps \
      -ceed "$backend" \
      -problem bp3 \
      -degree $p \
      -local 8192 \
      2>&1 | grep -E 'DOFs/sec|GFlops' | awk '{print $NF}' | paste - -)
    [ -z "$result" ] && result="0 0"
    echo "${backend},${p},${result// /,}" | tee -a $OUT
  done
done
