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

#ifndef AVCODEC_RISCV_H264_IDCT_H
#define AVCODEC_RISCV_H264_IDCT_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include "config.h"

#if HAVE_INTRINSICS_RVV
typedef unsigned char pixel;

void h264_idct_add_8_rvv(uint8_t *dst, int16_t *block, int stride);
void h264_idct_dc_add_8_rvv(uint8_t *p_dst, int16_t *p_block, int stride);
void h264_idct_add16_8_rvv(uint8_t *p_dst, const int *p_block_offset, int16_t *p_block, int stride,
                           const uint8_t nnzc[5 * 8]);
void h264_idct_add16_intra_8_rvv(uint8_t *p_dst, const int *p_block_offset, int16_t *p_block, int stride,
                                 const uint8_t nnzc[5 * 8]);
void h264_idct_add8_8_rvv(uint8_t **p_dst, const int *p_block_offset, int16_t *p_block, int stride,
                          const uint8_t nnzc[15*8]);
void h264_idct8_add_8_rvv(uint8_t *_dst, int16_t *_block, int stride);
void h264_idct8_dc_add_8_rvv(uint8_t *p_dst, int16_t *p_block, int stride);
void h264_idct8_add4_8_rvv(uint8_t *dst, const int *block_offset,
                           int16_t *block, int stride, const uint8_t nnzc[5 * 8]);
#endif
#endif