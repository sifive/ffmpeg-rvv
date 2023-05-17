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
    }
#endif
}