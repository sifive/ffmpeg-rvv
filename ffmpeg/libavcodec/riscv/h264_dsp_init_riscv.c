/*
 * Copyright (c) 2023 SiFive, Inc. All rights reserved.
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdint.h>
#include "libavutil/attributes.h"
#include "libavutil/cpu.h"
#include "libavcodec/h264dsp.h"
#include "config.h"
#include "h264_inloop.h"
#include "h264_weighted_sum.h"
#include "h264_idct.h"

av_cold void ff_h264dsp_init_riscv(H264DSPContext *c, const int bit_depth, const int chroma_format_idc)
{
#if HAVE_INTRINSICS_RVV
    if (bit_depth == 8) {
        c->h264_v_loop_filter_luma   = h264_v_loop_filter_luma_8_rvv;
        c->h264_v_loop_filter_luma_intra= h264_v_loop_filter_luma_intra_8_rvv;

        c->h264_h_loop_filter_luma   = h264_h_loop_filter_luma_8_rvv;
        c->h264_h_loop_filter_luma_intra= h264_h_loop_filter_luma_intra_8_rvv;

        c->h264_v_loop_filter_chroma = h264_v_loop_filter_chroma_8_rvv;
        c->h264_v_loop_filter_chroma_intra = h264_v_loop_filter_chroma_intra_8_rvv;

        if (chroma_format_idc <= 1) {
            c->h264_h_loop_filter_chroma = h264_h_loop_filter_chroma_8_rvv;
            c->h264_h_loop_filter_chroma_intra = h264_h_loop_filter_chroma_intra_8_rvv;
            c->h264_h_loop_filter_chroma_mbaff_intra = h264_h_loop_filter_chroma_mbaff_intra_8_rvv;
        }

        c->weight_h264_pixels_tab[0] = weight_h264_pixels_16_8_rvv;
        c->weight_h264_pixels_tab[1] = weight_h264_pixels_8_8_rvv;
        c->weight_h264_pixels_tab[2] = weight_h264_pixels_4_8_rvv;

        c->biweight_h264_pixels_tab[0]= biweight_h264_pixels_16_8_rvv;
        c->biweight_h264_pixels_tab[1]= biweight_h264_pixels_8_8_rvv;
        c->biweight_h264_pixels_tab[2]= biweight_h264_pixels_4_8_rvv;

        c->h264_idct_add        = h264_idct_add_8_rvv;
        c->h264_idct_dc_add     = h264_idct_dc_add_8_rvv;
        c->h264_idct_add16      = h264_idct_add16_8_rvv;
        c->h264_idct_add16intra = h264_idct_add16_intra_8_rvv;
        if (chroma_format_idc <= 1)
            c->h264_idct_add8   = h264_idct_add8_8_rvv;
        c->h264_idct8_add       = h264_idct8_add_8_rvv;
        c->h264_idct8_dc_add    = h264_idct8_dc_add_8_rvv;
        c->h264_idct8_add4      = h264_idct8_add4_8_rvv;
    }
#endif
}
