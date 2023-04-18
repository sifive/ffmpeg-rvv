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

#ifndef AVCODEC_RISCV_H264_PRED_H
#define AVCODEC_RISCV_H264_PRED_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include "config.h"

#if HAVE_INTRINSICS_RVV
typedef unsigned char pixel;

void pred8x8_vert_8_rvv(uint8_t *p_src, ptrdiff_t stride);
void pred8x8_hor_8_rvv(uint8_t *p_src, ptrdiff_t stride);
void pred8x8_plane_8_rvv(uint8_t *p_src, ptrdiff_t stride);
void pred8x8_128_dc_8_rvv(uint8_t *p_src, ptrdiff_t stride);
void pred8x8_top_dc_rvv(uint8_t *p_src, ptrdiff_t stride);
void pred8x8_left_dc_rvv(uint8_t *p_src, ptrdiff_t stride);
void pred8x8_dc_rvv(uint8_t *p_src, ptrdiff_t stride);
void pred8x8_l0t_dc_rvv(uint8_t *p_src, ptrdiff_t stride);
void pred8x8_0lt_dc_rvv(uint8_t *p_src, ptrdiff_t stride);
void pred8x8_l00_dc_rvv(uint8_t *p_src, ptrdiff_t stride);
void pred8x8_0l0_dc_rvv(uint8_t *p_src, ptrdiff_t stride);

void pred16x16_dc_8_rvv(uint8_t *p_src, ptrdiff_t stride);
void pred16x16_top_dc_8_rvv(uint8_t *p_src, ptrdiff_t stride);
void pred16x16_left_dc_8_rvv(uint8_t *p_src, ptrdiff_t stride);
void pred16x16_128_dc_8_rvv(uint8_t *p_src, ptrdiff_t stride);
void pred16x16_vert_8_rvv(uint8_t *p_src, ptrdiff_t stride);
void pred16x16_hor_8_rvv(uint8_t *p_src, ptrdiff_t stride);
void pred16x16_plane_8_rvv(uint8_t *p_src, ptrdiff_t stride);
#endif
#endif