# Pinned dependency versions

Locked at project start. Do not upgrade mid-study.

| Library   | Version      | Source                                      |
|-----------|-------------|----------------------------------------------|
| BLIS      | 2.0          | https://github.com/flame/blis               |
| CUTLASS   | v3.9.2       | https://github.com/NVIDIA/cutlass           |
| FFTW      | 3.3.10       | https://www.fftw.org/fftw-3.3.10.tar.gz    |
| VkFFT     | v1.3.4       | https://github.com/DTolm/VkFFT             |
| libCEED   | v1.0.0-rc.3  | https://github.com/CEED/libCEED            |
| CUDA      | 12.0         | system                                      |
| GCC       | 13.3.0       | system                                      |

## Hardware

| Component | Details                                             |
|-----------|-----------------------------------------------------|
| CPU       | Intel i9-13900F (Raptor Lake), 16P+8E cores         |
| L1d       | 48 KB / P-core, 32 KB / E-core                      |
| L2        | 2 MB / P-core, 4 MB / 4 E-cores (32 MB total)      |
| L3        | 36 MB shared                                        |
| GPU       | NVIDIA RTX 4070 Ti (Ada Lovelace, sm_89)            |
| VRAM      | 12 GB GDDR6X                                        |
| GPU SMs   | 60 SMs, 128 CUDA cores each, 4th-gen tensor cores   |
| SM shmem  | up to 100 KB / SM (configurable)                    |
