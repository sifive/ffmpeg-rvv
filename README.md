# ffmpeg-rvv
The project is based on the [revision](https://git.ffmpeg.org/gitweb/ffmpeg.git/commit/1440bf15e2b911cb41b88f2340878ca596d987c0) of FFmpeg,
aimimg to improve the performance of the H.264 decoder on RISC-V platform through RVV intrinsic code.
The optimization includes tasks such as motion compensation, intra prediction, IDCT, weighted prediction, and in-loop filtering.
The performance was conducted using 720P videos, executed on an FPGA platform. The FPS is more than 2x as fast.

# Toolchain
The project is verified using the toolchain, riscv64-glibc-ubuntu-22.04-llvm-nightly-2023.10.17-nightly.tar.gz.
Please find it on the page.
https://github.com/riscv-collab/riscv-gnu-toolchain/releases/tag/2023.10.17/

# Build Steps
**Clone project**
```
git clone git@github.com:sifive/ffmpeg-rvv.git && mkdir -p ffmpeg-rvv/ffmpeg/build && cd ffmpeg-rvv/ffmpeg/build
```
**Configure**  
Please replace the --sysroot=xxx with the path where your toolchain is located and -mcpu=xxx to your target CPU.
```
../configure --enable-cross-compile --cross-prefix=riscv64-unknown-linux-gnu- --sysroot=/PATH_TO_TOOLCHAIN/sysroot --target-os=linux --arch=riscv --enable-static --extra-cflags='-mcpu=YOUR_CPU -O3' --cc=riscv64-unknown-linux-gnu-clang --cxx=riscv64-unknown-linux-gnu-clang++ --disable-swscale --disable-ffplay --disable-doc
```
**Make**
```
make clean;make
```

# Regression Test ([FATE](http://ffmpeg.org/fate.html)) on QEMU
The FATE is used to verify the correctness of the patches. The steps below show how to run it on QEMU.

**Get samples**
```
make fate-rsync SAMPLES=fate-suite/
```

**Set QEMU command**  
Here is an example. Please modify the options for your CPU target.
```
run_qemu='qemu-riscv64 -cpu rv64,zba=true,zbb=true,zbc=true,zbs=true,v=true,Zfh=false,Zfhmin=true,vlen=512,elen=64,vext_spec=v1.0 -L /PATH_TO_TOOLCHAIN/sysroot/'
```

**Run FATE related to H.264**  
Since I only made changed to the H.264 portion, I focused solely on tests related to H.264 codec.
```
items=$(make fate-list SAMPLES=fate-sute | grep h264)
for item in $items; do make $item  TARGET_EXEC="${run_qemu}" SAMPLES=fate-suite ; done
```

