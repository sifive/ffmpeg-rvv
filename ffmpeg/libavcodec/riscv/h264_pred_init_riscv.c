#include <stdint.h>
#include "libavutil/attributes.h"
#include "libavutil/aarch64/cpu.h"
#include "libavcodec/avcodec.h"
#include "libavcodec/h264pred.h"
#include "config.h"
#include "h264_pred.h"

static av_cold void h264_pred_init_riscv(H264PredContext *h, int codec_id,
                                         const int bit_depth,
                                         const int chroma_format_idc)
{
#if HAVE_RVV
    if (bit_depth == 8) {
        if (chroma_format_idc <= 1) {
            h->pred8x8[VERT_PRED8x8     ] = pred8x8_vert_8_rvv;
            h->pred8x8[HOR_PRED8x8      ] = pred8x8_hor_8_rvv;
            if (codec_id != AV_CODEC_ID_VP7 && codec_id != AV_CODEC_ID_VP8)
                h->pred8x8[PLANE_PRED8x8] = pred8x8_plane_8_rvv;
            h->pred8x8[DC_128_PRED8x8   ] = pred8x8_128_dc_8_rvv;
            if (codec_id != AV_CODEC_ID_RV40 && codec_id != AV_CODEC_ID_VP7 &&
                codec_id != AV_CODEC_ID_VP8) {
                h->pred8x8[TOP_DC_PRED8x8 ] = pred8x8_top_dc_rvv;
                h->pred8x8[ALZHEIMER_DC_L0T_PRED8x8] = pred8x8_l0t_dc_rvv;
                h->pred8x8[ALZHEIMER_DC_0LT_PRED8x8] = pred8x8_0lt_dc_rvv;
                h->pred8x8[ALZHEIMER_DC_L00_PRED8x8] = pred8x8_l00_dc_rvv;
                h->pred8x8[ALZHEIMER_DC_0L0_PRED8x8] = pred8x8_0l0_dc_rvv;
            }
        }

        h->pred16x16[DC_PRED8x8     ] = pred16x16_dc_8_rvv;
        h->pred16x16[VERT_PRED8x8   ] = pred16x16_vert_8_rvv;
        h->pred16x16[HOR_PRED8x8    ] = pred16x16_hor_8_rvv;
        h->pred16x16[LEFT_DC_PRED8x8] = pred16x16_left_dc_8_rvv;
        h->pred16x16[TOP_DC_PRED8x8 ] = pred16x16_top_dc_8_rvv;
        h->pred16x16[DC_128_PRED8x8 ] = pred16x16_128_dc_8_rvv;
        if (codec_id != AV_CODEC_ID_SVQ3 && codec_id != AV_CODEC_ID_RV40 &&
            codec_id != AV_CODEC_ID_VP7 && codec_id != AV_CODEC_ID_VP8)
            h->pred16x16[PLANE_PRED8x8  ] = pred16x16_plane_8_rvv;
    }
#endif
}

av_cold void ff_h264_pred_init_riscv(H264PredContext *h, int codec_id,
                                       int bit_depth, const int chroma_format_idc)
{
#if HAVE_RVV
    h264_pred_init_riscv(h, codec_id, bit_depth, chroma_format_idc);
#endif
}
