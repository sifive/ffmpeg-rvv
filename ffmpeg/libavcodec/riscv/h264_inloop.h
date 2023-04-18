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

#ifndef AVCODEC_RISCV_H264_INLOOP_H
#define AVCODEC_RISCV_H264_INLOOP_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include "config.h"

#if HAVE_INTRINSICS_RVV
typedef unsigned char pixel;

void h264_v_loop_filter_luma_8_rvv(uint8_t *pix, ptrdiff_t stride, int alpha, int beta, int8_t *tc0);
void h264_h_loop_filter_luma_8_rvv(uint8_t *pix, ptrdiff_t stride, int alpha, int beta, int8_t *tc0);

void h264_v_loop_filter_chroma_8_rvv(uint8_t *pix, ptrdiff_t stride, int alpha, int beta, int8_t *tc0);
void h264_h_loop_filter_chroma_8_rvv(uint8_t *pix, ptrdiff_t stride, int alpha, int beta, int8_t *tc0);

void h264_v_loop_filter_luma_intra_8_rvv(uint8_t *p_pix, ptrdiff_t stride, int alpha, int beta);
void h264_h_loop_filter_luma_intra_8_rvv(uint8_t *p_pix, ptrdiff_t stride, int alpha, int beta);

void h264_v_loop_filter_chroma_intra_8_rvv(uint8_t *p_pix, ptrdiff_t stride, int alpha, int beta);
void h264_h_loop_filter_chroma_intra_8_rvv(uint8_t *p_pix, ptrdiff_t stride, int alpha, int beta);

void h264_h_loop_filter_chroma_mbaff_intra_8_rvv(uint8_t *p_pix, ptrdiff_t stride, int alpha, int beta);
#endif
#endif