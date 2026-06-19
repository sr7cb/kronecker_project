#!/bin/bash
# Sweep CUTLASS tile shapes for FP16 GEMM on sm_89 (RTX 4070 Ti)
# Output: CSV with ThreadblockShape, WarpShape, Stages, GFLOPS

PROFILER=../../deps/cutlass-build/tools/profiler/cutlass_profiler
OUT=cutlass_sweep.csv

# Matrix size: large enough to be compute-bound at optimum
N=8192

echo "tb_m,tb_n,tb_k,warp_m,warp_n,warp_k,stages,gflops" > $OUT

TB_MS="64 128 256"
TB_NS="64 128 256"
TB_KS="32 64"
WARP_MS="32 64"
WARP_NS="32 64"
STAGES="2 3 4"

for tb_m in $TB_MS; do
for tb_n in $TB_NS; do
for tb_k in $TB_KS; do
for warp_m in $WARP_MS; do
for warp_n in $WARP_NS; do
  # skip warp > threadblock
  [ $warp_m -gt $tb_m ] && continue
  [ $warp_n -gt $tb_n ] && continue
for stages in $STAGES; do
  # shared mem check: (tb_m*tb_k + tb_k*tb_n)*2bytes*stages <= 100KB
  shmem=$(( (tb_m*tb_k + tb_k*tb_n) * 2 * stages ))
  [ $shmem -gt 102400 ] && continue

  result=$($PROFILER \
    --operation=gemm \
    --Gemm_kind=universal \
    --m=$N --n=$N --k=$N \
    --A=f16:column --B=f16:row --C=f16:column \
    --threadblock_shape=${tb_m}x${tb_n}x${tb_k} \
    --warp_shape=${warp_m}x${warp_n}x${tb_k} \
    --stages=$stages \
    --warmup-iterations=2 --iterations=20 \
    --output=stdout 2>/dev/null \
    | grep -E '^[0-9]' | awk -F',' '{print $NF}' | tail -1)

  [ -z "$result" ] && result=0
  echo "${tb_m},${tb_n},${tb_k},${warp_m},${warp_n},${tb_k},${stages},${result}" | tee -a $OUT
done
done
done
done
done
done
