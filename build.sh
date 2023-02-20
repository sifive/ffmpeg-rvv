#!/bin/bash
mkdir -p ffmpeg/build_riscv
cd ffmpeg/build_riscv
../configure --enable-cross-compile --cross-prefix=riscv64-unknown-linux-gnu-llvm- --sysroot=/nfs/teams/sw/static/frameworks/riscv_tool/toolsuite-linux/sifive-internal/2022.08.0/riscv64-unknown-linux-gnu-toolsuite-14.9.0-2022.08.0-x86_64-linux-redhat8/sysroot/ --disable-doc --target-os=linux --arch=riscv --disable-ffplay --disable-sse2 --disable-sse --disable-x86asm --disable-avx --disable-avx2 --disable-avx512 --enable-static --disable-ffprobe --extra-cflags='-O3 -mcpu=sifive-x280 -ffast-math -mllvm -riscv-v-vector-bits-min=512 -static' --cc=riscv64-unknown-linux-gnu-clang --cxx=riscv64-unknown-linux-gnu-clang++ --strip=riscv64-unknown-linux-gnu-strip --disable-decoder=aac --disable-swscale
make clean; make -j8
