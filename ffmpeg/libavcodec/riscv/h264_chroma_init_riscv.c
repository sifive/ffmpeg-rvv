#include <stdint.h>

#include "libavutil/attributes.h"
#include "libavutil/cpu.h"
#include "libavcodec/h264chroma.h"
#include "config.h"
#include "h264_mc_chroma.h"

av_cold void ff_h264chroma_init_riscv(H264ChromaContext *c, int bit_depth)
{
#if HAVE_RVV
    const int high_bit_depth = bit_depth > 8;

    if (!high_bit_depth) {
        c->put_h264_chroma_pixels_tab[0] = h264_put_chroma_mc8_rvv;
        c->avg_h264_chroma_pixels_tab[0] = h264_avg_chroma_mc8_rvv;

        c->put_h264_chroma_pixels_tab[1] = h264_put_chroma_mc4_rvv;
        c->avg_h264_chroma_pixels_tab[1] = h264_avg_chroma_mc4_rvv;

        c->put_h264_chroma_pixels_tab[2] = h264_put_chroma_mc2_rvv;
        c->avg_h264_chroma_pixels_tab[2] = h264_avg_chroma_mc2_rvv;
    }
#endif
}