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

#include "config.h"
#include "libavutil/attributes.h"
#include "libavutil/cpu.h"
#include "libavutil/aarch64/cpu.h"
#include "libavcodec/h264qpel.h"
#include "h264_mc_luma.h"

av_cold void ff_h264qpel_init_riscv(H264QpelContext *c, int bit_depth)
{
#if HAVE_INTRINSICS_RVV
    const int high_bit_depth = bit_depth > 8;
    int cpu_flags = av_get_cpu_flags();

    if (!high_bit_depth)
    {
        c->put_h264_qpel_pixels_tab[0][0] = put_h264_qpel16_mc00_8_rvv;
        c->put_h264_qpel_pixels_tab[0][1] = put_h264_qpel16_mc10_8_rvv;
        c->put_h264_qpel_pixels_tab[0][2] = put_h264_qpel16_mc20_8_rvv;
        c->put_h264_qpel_pixels_tab[0][3] = put_h264_qpel16_mc30_8_rvv;
        c->put_h264_qpel_pixels_tab[0][4] = put_h264_qpel16_mc01_8_rvv;
        c->put_h264_qpel_pixels_tab[0][5] = put_h264_qpel16_mc11_8_rvv;
        c->put_h264_qpel_pixels_tab[0][6] = put_h264_qpel16_mc21_8_rvv;
        c->put_h264_qpel_pixels_tab[0][7] = put_h264_qpel16_mc31_8_rvv;
        c->put_h264_qpel_pixels_tab[0][8] = put_h264_qpel16_mc02_8_rvv;
        c->put_h264_qpel_pixels_tab[0][9] = put_h264_qpel16_mc12_8_rvv;
        c->put_h264_qpel_pixels_tab[0][10] = put_h264_qpel16_mc22_8_rvv;
        c->put_h264_qpel_pixels_tab[0][11] = put_h264_qpel16_mc32_8_rvv;
        c->put_h264_qpel_pixels_tab[0][12] = put_h264_qpel16_mc03_8_rvv;
        c->put_h264_qpel_pixels_tab[0][13] = put_h264_qpel16_mc13_8_rvv;
        c->put_h264_qpel_pixels_tab[0][14] = put_h264_qpel16_mc23_8_rvv;
        c->put_h264_qpel_pixels_tab[0][15] = put_h264_qpel16_mc33_8_rvv;

        c->put_h264_qpel_pixels_tab[1][0] = put_h264_qpel8_mc00_8_rvv;
        c->put_h264_qpel_pixels_tab[1][1] = put_h264_qpel8_mc10_8_rvv;
        c->put_h264_qpel_pixels_tab[1][2] = put_h264_qpel8_mc20_8_rvv;
        c->put_h264_qpel_pixels_tab[1][3] = put_h264_qpel8_mc30_8_rvv;
        c->put_h264_qpel_pixels_tab[1][4] = put_h264_qpel8_mc01_8_rvv;
        c->put_h264_qpel_pixels_tab[1][5] = put_h264_qpel8_mc11_8_rvv;
        c->put_h264_qpel_pixels_tab[1][6] = put_h264_qpel8_mc21_8_rvv;
        c->put_h264_qpel_pixels_tab[1][7] = put_h264_qpel8_mc31_8_rvv;
        c->put_h264_qpel_pixels_tab[1][8] = put_h264_qpel8_mc02_8_rvv;
        c->put_h264_qpel_pixels_tab[1][9] = put_h264_qpel8_mc12_8_rvv;
        c->put_h264_qpel_pixels_tab[1][10] = put_h264_qpel8_mc22_8_rvv;
        c->put_h264_qpel_pixels_tab[1][11] = put_h264_qpel8_mc32_8_rvv;
        c->put_h264_qpel_pixels_tab[1][12] = put_h264_qpel8_mc03_8_rvv;
        c->put_h264_qpel_pixels_tab[1][13] = put_h264_qpel8_mc13_8_rvv;
        c->put_h264_qpel_pixels_tab[1][14] = put_h264_qpel8_mc23_8_rvv;
        c->put_h264_qpel_pixels_tab[1][15] = put_h264_qpel8_mc33_8_rvv;

        c->avg_h264_qpel_pixels_tab[0][0] = avg_h264_qpel16_mc00_8_rvv;
        c->avg_h264_qpel_pixels_tab[0][1] = avg_h264_qpel16_mc10_8_rvv;
        c->avg_h264_qpel_pixels_tab[0][2] = avg_h264_qpel16_mc20_8_rvv;
        c->avg_h264_qpel_pixels_tab[0][3] = avg_h264_qpel16_mc30_8_rvv;
        c->avg_h264_qpel_pixels_tab[0][4] = avg_h264_qpel16_mc01_8_rvv;
        c->avg_h264_qpel_pixels_tab[0][5] = avg_h264_qpel16_mc11_8_rvv;
        c->avg_h264_qpel_pixels_tab[0][6] = avg_h264_qpel16_mc21_8_rvv;
        c->avg_h264_qpel_pixels_tab[0][7] = avg_h264_qpel16_mc31_8_rvv;
        c->avg_h264_qpel_pixels_tab[0][8] = avg_h264_qpel16_mc02_8_rvv;
        c->avg_h264_qpel_pixels_tab[0][9] = avg_h264_qpel16_mc12_8_rvv;
        c->avg_h264_qpel_pixels_tab[0][10] = avg_h264_qpel16_mc22_8_rvv;
        c->avg_h264_qpel_pixels_tab[0][11] = avg_h264_qpel16_mc32_8_rvv;
        c->avg_h264_qpel_pixels_tab[0][12] = avg_h264_qpel16_mc03_8_rvv;
        c->avg_h264_qpel_pixels_tab[0][13] = avg_h264_qpel16_mc13_8_rvv;
        c->avg_h264_qpel_pixels_tab[0][14] = avg_h264_qpel16_mc23_8_rvv;
        c->avg_h264_qpel_pixels_tab[0][15] = avg_h264_qpel16_mc33_8_rvv;

        c->avg_h264_qpel_pixels_tab[1][0] = avg_h264_qpel8_mc00_8_rvv;
        c->avg_h264_qpel_pixels_tab[1][1] = avg_h264_qpel8_mc10_8_rvv;
        c->avg_h264_qpel_pixels_tab[1][2] = avg_h264_qpel8_mc20_8_rvv;
        c->avg_h264_qpel_pixels_tab[1][3] = avg_h264_qpel8_mc30_8_rvv;
        c->avg_h264_qpel_pixels_tab[1][4] = avg_h264_qpel8_mc01_8_rvv;
        c->avg_h264_qpel_pixels_tab[1][5] = avg_h264_qpel8_mc11_8_rvv;
        c->avg_h264_qpel_pixels_tab[1][6] = avg_h264_qpel8_mc21_8_rvv;
        c->avg_h264_qpel_pixels_tab[1][7] = avg_h264_qpel8_mc31_8_rvv;
        c->avg_h264_qpel_pixels_tab[1][8] = avg_h264_qpel8_mc02_8_rvv;
        c->avg_h264_qpel_pixels_tab[1][9] = avg_h264_qpel8_mc12_8_rvv;
        c->avg_h264_qpel_pixels_tab[1][10] = avg_h264_qpel8_mc22_8_rvv;
        c->avg_h264_qpel_pixels_tab[1][11] = avg_h264_qpel8_mc32_8_rvv;
        c->avg_h264_qpel_pixels_tab[1][12] = avg_h264_qpel8_mc03_8_rvv;
        c->avg_h264_qpel_pixels_tab[1][13] = avg_h264_qpel8_mc13_8_rvv;
        c->avg_h264_qpel_pixels_tab[1][14] = avg_h264_qpel8_mc23_8_rvv;
        c->avg_h264_qpel_pixels_tab[1][15] = avg_h264_qpel8_mc33_8_rvv;
    }
#endif
}