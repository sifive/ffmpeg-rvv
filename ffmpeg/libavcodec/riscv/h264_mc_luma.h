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

#ifndef AVCODEC_RISCV_H264_MC_LUMA_H
#define AVCODEC_RISCV_H264_MC_LUMA_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include "config.h"

#if HAVE_INTRINSICS_RVV
typedef unsigned char pixel;

void put_h264_qpel16_mc00_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel16_mc01_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel16_mc02_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel16_mc03_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel16_mc10_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel16_mc11_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel16_mc12_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel16_mc13_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel16_mc20_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel16_mc21_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel16_mc22_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel16_mc23_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel16_mc30_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel16_mc31_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel16_mc32_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel16_mc33_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);

void avg_h264_qpel16_mc00_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel16_mc01_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel16_mc02_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel16_mc03_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel16_mc10_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel16_mc11_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel16_mc12_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel16_mc13_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel16_mc20_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel16_mc21_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel16_mc22_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel16_mc23_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel16_mc30_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel16_mc31_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel16_mc32_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel16_mc33_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);

void put_h264_qpel8_mc00_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel8_mc01_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel8_mc02_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel8_mc03_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel8_mc10_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel8_mc11_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel8_mc12_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel8_mc13_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel8_mc20_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel8_mc21_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel8_mc22_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel8_mc23_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel8_mc30_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel8_mc31_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel8_mc32_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel8_mc33_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);

void avg_h264_qpel8_mc00_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel8_mc01_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel8_mc02_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel8_mc03_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel8_mc10_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel8_mc11_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel8_mc12_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel8_mc13_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel8_mc20_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel8_mc21_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel8_mc22_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel8_mc23_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel8_mc30_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel8_mc31_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel8_mc32_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel8_mc33_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
#endif
#endif