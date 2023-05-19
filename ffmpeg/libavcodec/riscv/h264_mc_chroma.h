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

#ifndef AVCODEC_RISCV_H264_MC_CHROMA_H
#define AVCODEC_RISCV_H264_MC_CHROMA_H
#include "config.h"

#if HAVE_RVV
void h264_put_chroma_mc8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride, int h, int x, int y);
void h264_avg_chroma_mc8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride, int h, int x, int y);
#endif
#endif

