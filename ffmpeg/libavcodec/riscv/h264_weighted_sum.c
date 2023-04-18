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

#include "h264_weighted_sum.h"
#if HAVE_INTRINSICS_RVV
#include <riscv_vector.h>

typedef unsigned char pixel;

__attribute__((always_inline)) static void h264_weight_128(uint8_t *p_block, ptrdiff_t stride, int width,
                                                           int height, int log2_den, int offset)

{
    uint8_t *p_block_iter = p_block;

    const unsigned char weight = 128;
    short value = (unsigned)offset << log2_den;
    value += (1 << (log2_den - 1));

    int shift = log2_den;

    size_t vxrm = __builtin_rvv_vgetvxrm();
    __builtin_rvv_vsetvxrm(VE_DOWNWARD);

    int count = width;

    while (count > 0)
    {
        int vl = __riscv_vsetvl_e8m1(count);
        uint8_t *p_begin = p_block_iter;

        for (int j = 0; j < height; j += 2)
        {
            vuint8m1_t row0 = __riscv_vle8_v_u8m1(p_block_iter, vl);
            vuint8m1_t row1 = __riscv_vle8_v_u8m1(p_block_iter + stride, vl);

            vint16m2_t result0_w, result1_w;

            result0_w = __riscv_vreinterpret_v_u16m2_i16m2(__riscv_vzext_vf2_u16m2(row0, vl));
            result1_w = __riscv_vreinterpret_v_u16m2_i16m2(__riscv_vzext_vf2_u16m2(row1, vl));

            result0_w = __riscv_vsll_vx_i16m2(result0_w, 7, vl);
            result1_w = __riscv_vsll_vx_i16m2(result1_w, 7, vl);

            result0_w = __riscv_vadd_vx_i16m2(result0_w, offset, vl);
            result1_w = __riscv_vadd_vx_i16m2(result1_w, offset, vl);

            result0_w = __riscv_vmax_vx_i16m2(result0_w, 0, vl);
            result1_w = __riscv_vmax_vx_i16m2(result1_w, 0, vl);

            vuint8m1_t result0_n = __riscv_vnclipu_wx_u8m1(__riscv_vreinterpret_v_i16m2_u16m2(result0_w), shift, vl);
            vuint8m1_t result1_n = __riscv_vnclipu_wx_u8m1(__riscv_vreinterpret_v_i16m2_u16m2(result1_w), shift, vl);

            __riscv_vse8_v_u8m1(p_block_iter, result0_n, vl);
            p_block_iter += stride;
            __riscv_vse8_v_u8m1(p_block_iter, result1_n, vl);
            p_block_iter += stride;
        }

        p_block_iter = p_begin + vl;
        count -= vl;
    }

    __builtin_rvv_vsetvxrm(vxrm);
}

__attribute__((always_inline)) static void h264_weight_normal(uint8_t *p_block, ptrdiff_t stride,
                                                              int width, int height, int log2_den,
                                                              int weight, int offset)

{
    uint8_t *p_block_iter = p_block;

    short value = (unsigned)offset << log2_den;

    if (log2_den)
        value += (1 << (log2_den - 1));

    int shift = log2_den;

    size_t vxrm = __builtin_rvv_vgetvxrm();
    __builtin_rvv_vsetvxrm(VE_DOWNWARD);

    int count = width;

    while (count > 0)
    {
        int vl = __riscv_vsetvl_e8m1(count);
        uint8_t *p_begin = p_block_iter;

        vint8m1_t weight_v = __riscv_vmv_v_x_i8m1(weight, vl);

        for (int j = 0; j < height; j += 2)
        {
            vuint8m1_t row0 = __riscv_vle8_v_u8m1(p_block_iter, vl);
            vuint8m1_t row1 = __riscv_vle8_v_u8m1(p_block_iter + stride, vl);

            vint16m2_t result0_w, result1_w;

            result0_w = __riscv_vwmulsu_vv_i16m2(weight_v, row0, vl);
            result1_w = __riscv_vwmulsu_vv_i16m2(weight_v, row1, vl);

            result0_w = __riscv_vsadd_vx_i16m2(result0_w, value, vl);
            result1_w = __riscv_vsadd_vx_i16m2(result1_w, value, vl);

            result0_w = __riscv_vmax_vx_i16m2(result0_w, 0, vl);
            result1_w = __riscv_vmax_vx_i16m2(result1_w, 0, vl);

            vuint8m1_t result0_n = __riscv_vnclipu_wx_u8m1(__riscv_vreinterpret_v_i16m2_u16m2(result0_w), shift, vl);
            vuint8m1_t result1_n = __riscv_vnclipu_wx_u8m1(__riscv_vreinterpret_v_i16m2_u16m2(result1_w), shift, vl);

            __riscv_vse8_v_u8m1(p_block_iter, result0_n, vl);
            p_block_iter += stride;
            __riscv_vse8_v_u8m1(p_block_iter, result1_n, vl);
            p_block_iter += stride;
        }

        p_block_iter = p_begin + vl;
        count -= vl;
    }

    __builtin_rvv_vsetvxrm(vxrm);
}

__attribute__((always_inline)) static void h264_biweight(uint8_t *p_dst, uint8_t *p_src, ptrdiff_t stride,
                                                         int width, int height, int log2_den,
                                                         int weightd, int weights, int offset)
{
    uint8_t *p_dst_iter = p_dst;
    uint8_t *p_src_iter = p_src;
    short value = (unsigned int)((offset + 1) | 1) << log2_den;
    int shift = log2_den + 1;

    size_t vxrm = __builtin_rvv_vgetvxrm();
    __builtin_rvv_vsetvxrm(VE_DOWNWARD);

    int count = width;

    while (count > 0)
    {
        int vl = __riscv_vsetvl_e8m1(count);
        uint8_t *p_src_begin = p_src_iter;
        uint8_t *p_dst_begin = p_dst_iter;

        for (int j = 0; j < height; j += 2)
        {
            vuint8m1_t src0 = __riscv_vle8_v_u8m1(p_src_iter, vl);
            p_src_iter += stride;
            vuint8m1_t src1 = __riscv_vle8_v_u8m1(p_src_iter, vl);
            p_src_iter += stride;

            vuint8m1_t dst0 = __riscv_vle8_v_u8m1(p_dst_iter, vl);
            vuint8m1_t dst1 = __riscv_vle8_v_u8m1(p_dst_iter + stride, vl);

            vint16m2_t result0_w, result1_w;

            result0_w = __riscv_vmv_v_x_i16m2(value, vl);
            result1_w = __riscv_vmv_v_x_i16m2(value, vl);

            result0_w = __riscv_vwmaccsu_vx_i16m2(result0_w, weightd, dst0, vl);
            result1_w = __riscv_vwmaccsu_vx_i16m2(result1_w, weightd, dst1, vl);

            result0_w = __riscv_vwmaccsu_vx_i16m2(result0_w, weights, src0, vl);
            result1_w = __riscv_vwmaccsu_vx_i16m2(result1_w, weights, src1, vl);

            result0_w = __riscv_vmax_vx_i16m2(result0_w, 0, vl);
            result1_w = __riscv_vmax_vx_i16m2(result1_w, 0, vl);

            vuint8m1_t result0_n = __riscv_vnclipu_wx_u8m1(__riscv_vreinterpret_v_i16m2_u16m2(result0_w), shift, vl);
            vuint8m1_t result1_n = __riscv_vnclipu_wx_u8m1(__riscv_vreinterpret_v_i16m2_u16m2(result1_w), shift, vl);

            __riscv_vse8_v_u8m1(p_dst_iter, result0_n, vl);
            p_dst_iter += stride;
            __riscv_vse8_v_u8m1(p_dst_iter, result1_n, vl);
            p_dst_iter += stride;
        }

        p_src_iter = p_src_begin + vl;
        p_dst_iter = p_dst_begin + vl;
        count -= vl;
    }

    __builtin_rvv_vsetvxrm(vxrm);
}

void weight_h264_pixels_16_8_rvv(uint8_t *p_block, ptrdiff_t stride,
                                 int height, int log2_den, int weight, int offset)
{
    if (weight == 1 && offset == 0 && log2_den == 0)
        return;

    if (weight == 128)
    {
        h264_weight_128(p_block, stride, 16, height, log2_den, offset);
    }
    else
    {
        h264_weight_normal(p_block, stride, 16, height, log2_den, weight, offset);
    }
}

void weight_h264_pixels_8_8_rvv(uint8_t *p_block, ptrdiff_t stride,
                                int height, int log2_den, int weight, int offset)
{
    if (weight == 1 && offset == 0 && log2_den == 0)
        return;

    if (weight == 128)
    {
        h264_weight_128(p_block, stride, 8, height, log2_den, offset);
    }
    else
    {
        h264_weight_normal(p_block, stride, 8, height, log2_den, weight, offset);
    }
}

void weight_h264_pixels_4_8_rvv(uint8_t *p_block, ptrdiff_t stride,
                                int height, int log2_den, int weight, int offset)
{
    if (weight == 1 && offset == 0 && log2_den == 0)
        return;

    if (weight == 128)
    {
        h264_weight_128(p_block, stride, 4, height, log2_den, offset);
    }
    else
    {
        h264_weight_normal(p_block, stride, 4, height, log2_den, weight, offset);
    }
}

void biweight_h264_pixels_16_8_rvv(uint8_t *p_dst, uint8_t *p_src, ptrdiff_t stride,
                                   int height, int log2_den, int weightd,
                                   int weights, int offset)
{
    h264_biweight(p_dst, p_src, stride, 16, height, log2_den, weightd, weights, offset);
}

void biweight_h264_pixels_8_8_rvv(uint8_t *p_dst, uint8_t *p_src, ptrdiff_t stride,
                                  int height, int log2_den, int weightd,
                                  int weights, int offset)
{

    h264_biweight(p_dst, p_src, stride, 8, height, log2_den, weightd, weights, offset);
}

void biweight_h264_pixels_4_8_rvv(uint8_t *p_dst, uint8_t *p_src, ptrdiff_t stride,
                                  int height, int log2_den, int weightd,
                                  int weights, int offset)
{

    h264_biweight(p_dst, p_src, stride, 4, height, log2_den, weightd, weights, offset);
}
#endif
