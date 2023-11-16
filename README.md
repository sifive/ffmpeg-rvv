# ffmpeg-rvv
The project is based on the [revision](https://git.ffmpeg.org/gitweb/ffmpeg.git/commit/1440bf15e2b911cb41b88f2340878ca596d987c0) of FFmpeg,
aiming to improve the performance of the H.264 decoder on RISC-V platform through RVV intrinsic code.
The optimization includes tasks such as motion compensation, intra prediction, IDCT, weighted prediction, and in-loop filtering.
The performance was conducted using 720P videos on an FPGA platform, resulting in an average FPS that is more than 2x faster.

The patches([84e9b40](https://github.com/sifive/ffmpeg-rvv/commit/84e9b40464ba343b5426b89cd2cad95fe5cce1e8), [717fc7e](https://github.com/sifive/ffmpeg-rvv/commit/717fc7eae965ce6899e9f6d7ba1902baa8fdc97e), [82abb61](https://github.com/sifive/ffmpeg-rvv/commit/82abb61ea15c14c6bf958492bcf0fcc85b5ef9d5), [27b338f](https://github.com/sifive/ffmpeg-rvv/commit/27b338f265c1a54d2684892cd4df451346f898cc), [e4d4e32](https://github.com/sifive/ffmpeg-rvv/commit/e4d4e32d785b55039fd6fc86ae771c8930bdae37)) were also submitted to the [FFmpeg-devel](https://ffmpeg.org/pipermail/ffmpeg-devel/2023-May/309386.html) maillist. Due to the current maintenance team limiting patches to assembly implementation only (no intrinsics), the patches were NOT merged. Following that, part of the patches(chroma MC) has been rewritten and upstreamed ([patch1](https://git.ffmpeg.org/gitweb/ffmpeg.git/commit/c5508f60c255f7c3c17ac84d0d9c0033094d8a96), [patch2](https://git.ffmpeg.org/gitweb/ffmpeg.git/commit/8d1316e51561882a30a3febb77f600a8bb6a5034)).

# Toolchain
The project is verified using the SiFive LLVM toolchain internally. SiFive LLVM toolchain has backward supports for more ISA if optimization contains different implementation such as RVV 0.1 and 0.12. Our expectation is that a particular upstream LLVM should be able to build as well. Please check troubleshooting for more details.

# Build Steps
**Clone project**
```
git clone git@github.com:sifive/ffmpeg-rvv.git && mkdir -p ffmpeg-rvv/ffmpeg/build && cd ffmpeg-rvv/ffmpeg/build
```
**Configure**  
Please replace the `--sysroot=xxx` with the path where your toolchain is located and `-mcpu=xxx` to your target CPU.
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
Since only modifications were made to the H.264 portion, the focus is solely on tests related to the H.264 codec.
```
items=$(make fate-list SAMPLES=fate-sute | grep h264)
for item in $items; do make $item  TARGET_EXEC="${run_qemu}" SAMPLES=fate-suite ; done
```

# Troubleshooting
## Build with upstream LLVM
The project is verified using the toolchain, riscv64-glibc-ubuntu-22.04-llvm-nightly-2023.10.17-nightly.tar.gz.
Please find it on the page.
https://github.com/riscv-collab/riscv-gnu-toolchain/releases/tag/2023.10.17/

**Configure**  
Please replace the --sysroot=xxx with the path where your toolchain is located and -mcpu=xxx to your target CPU.
```
../configure --enable-cross-compile --cross-prefix=riscv64-unknown-linux-gnu- --sysroot=/PATH_TO_TOOLCHAIN/sysroot --target-os=linux --arch=riscv --enable-static --extra-cflags='-mcpu=YOUR_CPU -O3' --cc=riscv64-unknown-linux-gnu-clang --cxx=riscv64-unknown-linux-gnu-clang++ --disable-swscale --disable-ffplay --disable-doc
```

**Make**  
```
make clean;make
```
# Contributors
- Arnie Chang <arnie.chang@sifive.com>

# Known Issues

