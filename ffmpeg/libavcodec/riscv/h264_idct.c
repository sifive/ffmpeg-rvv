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

#include "h264_idct.h"
#if HAVE_INTRINSICS_RVV
#include <riscv_vector.h>

static const uint8_t scan8[16 * 3 + 3] =
{
    4 +  1 * 8, 5 +  1 * 8, 4 +  2 * 8, 5 +  2 * 8,
    6 +  1 * 8, 7 +  1 * 8, 6 +  2 * 8, 7 +  2 * 8,
    4 +  3 * 8, 5 +  3 * 8, 4 +  4 * 8, 5 +  4 * 8,
    6 +  3 * 8, 7 +  3 * 8, 6 +  4 * 8, 7 +  4 * 8,
    4 +  6 * 8, 5 +  6 * 8, 4 +  7 * 8, 5 +  7 * 8,
    6 +  6 * 8, 7 +  6 * 8, 6 +  7 * 8, 7 +  7 * 8,
    4 +  8 * 8, 5 +  8 * 8, 4 +  9 * 8, 5 +  9 * 8,
    6 +  8 * 8, 7 +  8 * 8, 6 +  9 * 8, 7 +  9 * 8,
    4 + 11 * 8, 5 + 11 * 8, 4 + 12 * 8, 5 + 12 * 8,
    6 + 11 * 8, 7 + 11 * 8, 6 + 12 * 8, 7 + 12 * 8,
    4 + 13 * 8, 5 + 13 * 8, 4 + 14 * 8, 5 + 14 * 8,
    6 + 13 * 8, 7 + 13 * 8, 6 + 14 * 8, 7 + 14 * 8,
    0 +  0 * 8, 0 +  5 * 8, 0 + 10 * 8
};

void h264_idct_add_8_rvv(uint8_t *p_dst, int16_t *p_block, int stride)
{
    int16_t temp[16];
    int vl = __riscv_vsetvl_e16m1(4);

    p_block[0] += 32;

    vint16m1_t row0 = __riscv_vle16_v_i16m1(p_block, vl);
    vint16m1_t row1 = __riscv_vle16_v_i16m1(p_block + 4, vl);
    vint16m1_t row2 = __riscv_vle16_v_i16m1(p_block + 8, vl);
    vint16m1_t row3 = __riscv_vle16_v_i16m1(p_block + 12, vl);

    // 1-D row idct
    vint16m1_t z0 = __riscv_vadd_vv_i16m1(row0, row2, vl);
    vint16m1_t z1 = __riscv_vsub_vv_i16m1(row0, row2, vl);
    vint16m1_t z2 = __riscv_vsra_vx_i16m1(row1, 1, vl);
    z2 = __riscv_vsub_vv_i16m1(z2, row3, vl);
    vint16m1_t z3 = __riscv_vsra_vx_i16m1(row3, 1, vl);
    z3 = __riscv_vadd_vv_i16m1(z3, row1, vl);

    vint16m1_t result0 = __riscv_vadd_vv_i16m1(z0, z3, vl);
    vint16m1_t result1 = __riscv_vadd_vv_i16m1(z1, z2, vl);
    vint16m1_t result2 = __riscv_vsub_vv_i16m1(z1, z2, vl);
    vint16m1_t result3 = __riscv_vsub_vv_i16m1(z0, z3, vl);

    // transpose
    __riscv_vse16_v_i16m1(&temp[0], result0, vl);
    __riscv_vse16_v_i16m1(&temp[4], result1, vl);
    __riscv_vse16_v_i16m1(&temp[8], result2, vl);
    __riscv_vse16_v_i16m1(&temp[12], result3, vl);
    vint16m1x4_t rowx4 = __riscv_vlseg4e16_v_i16m1x4(&temp[0], vl);
    row0 = __riscv_vget_v_i16m1x4_i16m1(rowx4, 0);
    row1 = __riscv_vget_v_i16m1x4_i16m1(rowx4, 1);
    row2 = __riscv_vget_v_i16m1x4_i16m1(rowx4, 2);
    row3 = __riscv_vget_v_i16m1x4_i16m1(rowx4, 3);

    // 1-D column idct
    z0 = __riscv_vadd_vv_i16m1(row0, row2, vl);
    z1 = __riscv_vsub_vv_i16m1(row0, row2, vl);
    z2 = __riscv_vsra_vx_i16m1(row1, 1, vl);
    z2 = __riscv_vsub_vv_i16m1(z2, row3, vl);
    z3 = __riscv_vsra_vx_i16m1(row3, 1, vl);
    z3 = __riscv_vadd_vv_i16m1(z3, row1, vl);

    result0 = __riscv_vadd_vv_i16m1(z0, z3, vl);
    result1 = __riscv_vadd_vv_i16m1(z1, z2, vl);
    result2 = __riscv_vsub_vv_i16m1(z1, z2, vl);
    result3 = __riscv_vsub_vv_i16m1(z0, z3, vl);

    result0 = __riscv_vsra_vx_i16m1(result0, 6, vl);
    result1 = __riscv_vsra_vx_i16m1(result1, 6, vl);
    result2 = __riscv_vsra_vx_i16m1(result2, 6, vl);
    result3 = __riscv_vsra_vx_i16m1(result3, 6, vl);

    vuint8mf2_t dst0 = __riscv_vle8_v_u8mf2(p_dst, vl);
    vuint8mf2_t dst1 = __riscv_vle8_v_u8mf2(p_dst + stride, vl);
    vuint8mf2_t dst2 = __riscv_vle8_v_u8mf2(p_dst + stride * 2, vl);
    vuint8mf2_t dst3 = __riscv_vle8_v_u8mf2(p_dst + stride * 3, vl);

    vint16m1_t dst0_w = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwcvtu_x_x_v_u16m1(dst0, vl));
    vint16m1_t dst1_w = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwcvtu_x_x_v_u16m1(dst1, vl));
    vint16m1_t dst2_w = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwcvtu_x_x_v_u16m1(dst2, vl));
    vint16m1_t dst3_w = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwcvtu_x_x_v_u16m1(dst3, vl));

    result0 = __riscv_vadd_vv_i16m1(result0, dst0_w, vl);
    result1 = __riscv_vadd_vv_i16m1(result1, dst1_w, vl);
    result2 = __riscv_vadd_vv_i16m1(result2, dst2_w, vl);
    result3 = __riscv_vadd_vv_i16m1(result3, dst3_w, vl);

    result0 = __riscv_vmax_vx_i16m1(result0, 0, vl);
    result1 = __riscv_vmax_vx_i16m1(result1, 0, vl);
    result2 = __riscv_vmax_vx_i16m1(result2, 0, vl);
    result3 = __riscv_vmax_vx_i16m1(result3, 0, vl);

    vuint8mf2_t result0_n = __riscv_vnclipu_wx_u8mf2(__riscv_vreinterpret_v_i16m1_u16m1(result0), 0, __RISCV_FRM_RNE, vl);
    vuint8mf2_t result1_n = __riscv_vnclipu_wx_u8mf2(__riscv_vreinterpret_v_i16m1_u16m1(result1), 0, __RISCV_FRM_RNE, vl);
    vuint8mf2_t result2_n = __riscv_vnclipu_wx_u8mf2(__riscv_vreinterpret_v_i16m1_u16m1(result2), 0, __RISCV_FRM_RNE, vl);
    vuint8mf2_t result3_n = __riscv_vnclipu_wx_u8mf2(__riscv_vreinterpret_v_i16m1_u16m1(result3), 0, __RISCV_FRM_RNE, vl);

    __riscv_vse8_v_u8mf2(p_dst, result0_n, vl);
    __riscv_vse8_v_u8mf2(p_dst + stride, result1_n, vl);
    __riscv_vse8_v_u8mf2(p_dst + stride * 2, result2_n, vl);
    __riscv_vse8_v_u8mf2(p_dst + stride * 3, result3_n, vl);

    memset(p_block, 0, sizeof(int16_t) * 16);
}

void h264_idct_dc_add_8_rvv(uint8_t *p_dst, int16_t *p_block, int stride)
{
    int vl = __riscv_vsetvl_e16m1(4);

    int dc = (p_block[0] + 32) >> 6;

    if (dc > 255)
        dc = 255;

    if (dc < -255)
        dc = -255;

    p_block[0] = 0;

    vuint8m1_t dst0 = __riscv_vle8_v_u8m1(p_dst, vl);
    vuint8m1_t dst1 = __riscv_vle8_v_u8m1(p_dst + stride, vl);
    vuint8m1_t dst2 = __riscv_vle8_v_u8m1(p_dst + stride * 2, vl);
    vuint8m1_t dst3 = __riscv_vle8_v_u8m1(p_dst + stride * 3, vl);

    if (dc >= 0)
    {
        dst0 = __riscv_vsaddu_vx_u8m1(dst0, dc, vl);
        dst1 = __riscv_vsaddu_vx_u8m1(dst1, dc, vl);
        dst2 = __riscv_vsaddu_vx_u8m1(dst2, dc, vl);
        dst3 = __riscv_vsaddu_vx_u8m1(dst3, dc, vl);
    }
    else
    {
        dst0 = __riscv_vssubu_vx_u8m1(dst0, -dc, vl);
        dst1 = __riscv_vssubu_vx_u8m1(dst1, -dc, vl);
        dst2 = __riscv_vssubu_vx_u8m1(dst2, -dc, vl);
        dst3 = __riscv_vssubu_vx_u8m1(dst3, -dc, vl);
    }

    __riscv_vse8_v_u8m1(p_dst, dst0, vl);
    __riscv_vse8_v_u8m1(p_dst + stride, dst1, vl);
    __riscv_vse8_v_u8m1(p_dst + stride * 2, dst2, vl);
    __riscv_vse8_v_u8m1(p_dst + stride * 3, dst3, vl);
}

void h264_idct_add16_8_rvv(uint8_t *p_dst, const int *p_block_offset, int16_t *p_block, int stride,
                           const uint8_t nnzc[5 * 8])
{
    for(int i = 0; i < 16; i++)
    {
        int nnz = nnzc[scan8[i]];

        if(nnz)
        {
            if(nnz==1 && p_block[i*16])
                h264_idct_dc_add_8_rvv(p_dst + p_block_offset[i], p_block + i * 16 * sizeof(pixel), stride);
            else
                h264_idct_add_8_rvv(p_dst + p_block_offset[i], p_block + i * 16 * sizeof(pixel), stride);
        }
    }
}

void h264_idct_add16_intra_8_rvv(uint8_t *p_dst, const int *p_block_offset, int16_t *p_block, int stride,
                                 const uint8_t nnzc[5 * 8])
{
    for(int i = 0; i < 16; i++)
    {
        if(nnzc[scan8[i]])
            h264_idct_add_8_rvv(p_dst + p_block_offset[i], p_block + i * 16 * sizeof(pixel), stride);
        else if(p_block[i*16])
            h264_idct_dc_add_8_rvv(p_dst + p_block_offset[i], p_block + i * 16 * sizeof(pixel), stride);
    }
}

void h264_idct_add8_8_rvv(uint8_t **p_dst, const int *p_block_offset, int16_t *p_block, int stride,
                          const uint8_t nnzc[15*8])
{
    for(int j = 1; j < 3; j++)
    {
        for(int i = j * 16; i < j * 16 + 4; i++)
        {
            if(nnzc[scan8[i]])
                h264_idct_add_8_rvv(p_dst[j - 1] + p_block_offset[i], p_block + i * 16 * sizeof(pixel), stride);
            else if(p_block[i * 16])
                h264_idct_dc_add_8_rvv(p_dst[j - 1] + p_block_offset[i], p_block + i * 16 * sizeof(pixel), stride);
        }
    }
}

void h264_idct8_add_8_rvv(uint8_t *p_dst, int16_t *p_block, int stride)
{
    int16_t temp[64];
    int vl = __riscv_vsetvl_e16m1(8);

    p_block[0] += 32;

    vint16m1_t row0 = __riscv_vle16_v_i16m1(p_block, vl);
    vint16m1_t row1 = __riscv_vle16_v_i16m1(p_block + 8, vl);
    vint16m1_t row2 = __riscv_vle16_v_i16m1(p_block + 16, vl);
    vint16m1_t row3 = __riscv_vle16_v_i16m1(p_block + 24, vl);
    vint16m1_t row4 = __riscv_vle16_v_i16m1(p_block + 32, vl);
    vint16m1_t row5 = __riscv_vle16_v_i16m1(p_block + 40, vl);
    vint16m1_t row6 = __riscv_vle16_v_i16m1(p_block + 48, vl);
    vint16m1_t row7 = __riscv_vle16_v_i16m1(p_block + 56, vl);

    // 1-D row idct
    vint16m1_t a0 = __riscv_vadd_vv_i16m1(row0, row4, vl);
    vint16m1_t a2 = __riscv_vsub_vv_i16m1(row0, row4, vl);
    vint16m1_t a4 = __riscv_vsra_vx_i16m1(row2, 1, vl);
    a4 = __riscv_vsub_vv_i16m1(a4, row6, vl);
    vint16m1_t a6 = __riscv_vsra_vx_i16m1(row6, 1, vl);
    a6 = __riscv_vadd_vv_i16m1(row2, a6, vl);

    vint16m1_t b0 = __riscv_vadd_vv_i16m1(a0, a6, vl);
    vint16m1_t b2 = __riscv_vadd_vv_i16m1(a2, a4, vl);
    vint16m1_t b4 = __riscv_vsub_vv_i16m1(a2, a4, vl);
    vint16m1_t b6 = __riscv_vsub_vv_i16m1(a0, a6, vl);

    vint16m1_t a1 = __riscv_vsra_vx_i16m1(row7, 1, vl);
    a1 = __riscv_vsub_vv_i16m1(row5, a1, vl);
    a1 = __riscv_vsub_vv_i16m1(a1, row3, vl);
    a1 = __riscv_vsub_vv_i16m1(a1, row7, vl);
    vint16m1_t a3 = __riscv_vsra_vx_i16m1(row3, 1, vl);
    a3 = __riscv_vsub_vv_i16m1(row7, a3, vl);
    a3 = __riscv_vadd_vv_i16m1(a3, row1, vl);
    a3 = __riscv_vsub_vv_i16m1(a3, row3, vl);
    vint16m1_t a5 = __riscv_vsra_vx_i16m1(row5, 1, vl);
    a5 = __riscv_vsub_vv_i16m1(a5, row1, vl);
    a5 = __riscv_vadd_vv_i16m1(a5, row7, vl);
    a5 = __riscv_vadd_vv_i16m1(a5, row5, vl);
    vint16m1_t a7 = __riscv_vsra_vx_i16m1(row1, 1, vl);
    a7 = __riscv_vadd_vv_i16m1(a7, row3, vl);
    a7 = __riscv_vadd_vv_i16m1(a7, row5, vl);
    a7 = __riscv_vadd_vv_i16m1(a7, row1, vl);

    vint16m1_t b1 = __riscv_vsra_vx_i16m1(a7, 2, vl);
    b1 = __riscv_vadd_vv_i16m1(b1, a1, vl);
    vint16m1_t b3 = __riscv_vsra_vx_i16m1(a5, 2, vl);
    b3 = __riscv_vadd_vv_i16m1(b3, a3, vl);
    vint16m1_t b5 = __riscv_vsra_vx_i16m1(a3, 2, vl);
    b5 = __riscv_vsub_vv_i16m1(b5, a5, vl);
    vint16m1_t b7 = __riscv_vsra_vx_i16m1(a1, 2, vl);
    b7 = __riscv_vsub_vv_i16m1(a7, b7, vl);

    vint16m1_t result0 = __riscv_vadd_vv_i16m1(b0, b7, vl);
    vint16m1_t result7 = __riscv_vsub_vv_i16m1(b0, b7, vl);
    vint16m1_t result1 = __riscv_vadd_vv_i16m1(b2, b5, vl);
    vint16m1_t result6 = __riscv_vsub_vv_i16m1(b2, b5, vl);
    vint16m1_t result2 = __riscv_vadd_vv_i16m1(b4, b3, vl);
    vint16m1_t result5 = __riscv_vsub_vv_i16m1(b4, b3, vl);
    vint16m1_t result3 = __riscv_vadd_vv_i16m1(b6, b1, vl);
    vint16m1_t result4 = __riscv_vsub_vv_i16m1(b6, b1, vl);

    // transpose
    __riscv_vse16_v_i16m1(&temp[0], result0, vl);
    __riscv_vse16_v_i16m1(&temp[8], result1, vl);
    __riscv_vse16_v_i16m1(&temp[16], result2, vl);
    __riscv_vse16_v_i16m1(&temp[24], result3, vl);
    __riscv_vse16_v_i16m1(&temp[32], result4, vl);
    __riscv_vse16_v_i16m1(&temp[40], result5, vl);
    __riscv_vse16_v_i16m1(&temp[48], result6, vl);
    __riscv_vse16_v_i16m1(&temp[56], result7, vl);

    vint16m1x8_t rowx8 = __riscv_vlseg8e16_v_i16m1x8(&temp[0], vl);
    row0 = __riscv_vget_v_i16m1x8_i16m1(rowx8, 0);
    row1 = __riscv_vget_v_i16m1x8_i16m1(rowx8, 1);
    row2 = __riscv_vget_v_i16m1x8_i16m1(rowx8, 2);
    row3 = __riscv_vget_v_i16m1x8_i16m1(rowx8, 3);
    row4 = __riscv_vget_v_i16m1x8_i16m1(rowx8, 4);
    row5 = __riscv_vget_v_i16m1x8_i16m1(rowx8, 5);
    row6 = __riscv_vget_v_i16m1x8_i16m1(rowx8, 6);
    row7 = __riscv_vget_v_i16m1x8_i16m1(rowx8, 7);

    // 1-D column idct
    a0 = __riscv_vadd_vv_i16m1(row0, row4, vl);
    a2 = __riscv_vsub_vv_i16m1(row0, row4, vl);
    a4 = __riscv_vsra_vx_i16m1(row2, 1, vl);
    a4 = __riscv_vsub_vv_i16m1(a4, row6, vl);
    a6 = __riscv_vsra_vx_i16m1(row6, 1, vl);
    a6 = __riscv_vadd_vv_i16m1(row2, a6, vl);

    b0 = __riscv_vadd_vv_i16m1(a0, a6, vl);
    b2 = __riscv_vadd_vv_i16m1(a2, a4, vl);
    b4 = __riscv_vsub_vv_i16m1(a2, a4, vl);
    b6 = __riscv_vsub_vv_i16m1(a0, a6, vl);

    a1 = __riscv_vsra_vx_i16m1(row7, 1, vl);
    a1 = __riscv_vsub_vv_i16m1(row5, a1, vl);
    a1 = __riscv_vsub_vv_i16m1(a1, row3, vl);
    a1 = __riscv_vsub_vv_i16m1(a1, row7, vl);
    a3 = __riscv_vsra_vx_i16m1(row3, 1, vl);
    a3 = __riscv_vsub_vv_i16m1(row7, a3, vl);
    a3 = __riscv_vadd_vv_i16m1(a3, row1, vl);
    a3 = __riscv_vsub_vv_i16m1(a3, row3, vl);
    a5 = __riscv_vsra_vx_i16m1(row5, 1, vl);
    a5 = __riscv_vsub_vv_i16m1(a5, row1, vl);
    a5 = __riscv_vadd_vv_i16m1(a5, row7, vl);
    a5 = __riscv_vadd_vv_i16m1(a5, row5, vl);
    a7 = __riscv_vsra_vx_i16m1(row1, 1, vl);
    a7 = __riscv_vadd_vv_i16m1(a7, row3, vl);
    a7 = __riscv_vadd_vv_i16m1(a7, row5, vl);
    a7 = __riscv_vadd_vv_i16m1(a7, row1, vl);

    b1 = __riscv_vsra_vx_i16m1(a7, 2, vl);
    b1 = __riscv_vadd_vv_i16m1(b1, a1, vl);
    b3 = __riscv_vsra_vx_i16m1(a5, 2, vl);
    b3 = __riscv_vadd_vv_i16m1(b3, a3, vl);
    b5 = __riscv_vsra_vx_i16m1(a3, 2, vl);
    b5 = __riscv_vsub_vv_i16m1(b5, a5, vl);
    b7 = __riscv_vsra_vx_i16m1(a1, 2, vl);
    b7 = __riscv_vsub_vv_i16m1(a7, b7, vl);

    result0 = __riscv_vadd_vv_i16m1(b0, b7, vl);
    result1 = __riscv_vadd_vv_i16m1(b2, b5, vl);
    result2 = __riscv_vadd_vv_i16m1(b4, b3, vl);
    result3 = __riscv_vadd_vv_i16m1(b6, b1, vl);
    result4 = __riscv_vsub_vv_i16m1(b6, b1, vl);
    result5 = __riscv_vsub_vv_i16m1(b4, b3, vl);
    result6 = __riscv_vsub_vv_i16m1(b2, b5, vl);
    result7 = __riscv_vsub_vv_i16m1(b0, b7, vl);

    // normalize and write to destination
    result0 = __riscv_vsra_vx_i16m1(result0, 6, vl);
    result1 = __riscv_vsra_vx_i16m1(result1, 6, vl);
    result2 = __riscv_vsra_vx_i16m1(result2, 6, vl);
    result3 = __riscv_vsra_vx_i16m1(result3, 6, vl);
    result4 = __riscv_vsra_vx_i16m1(result4, 6, vl);
    result5 = __riscv_vsra_vx_i16m1(result5, 6, vl);
    result6 = __riscv_vsra_vx_i16m1(result6, 6, vl);
    result7 = __riscv_vsra_vx_i16m1(result7, 6, vl);

    vuint8mf2_t dst0 = __riscv_vle8_v_u8mf2(p_dst, vl);
    vuint8mf2_t dst1 = __riscv_vle8_v_u8mf2(p_dst + stride, vl);
    vuint8mf2_t dst2 = __riscv_vle8_v_u8mf2(p_dst + stride * 2, vl);
    vuint8mf2_t dst3 = __riscv_vle8_v_u8mf2(p_dst + stride * 3, vl);
    vuint8mf2_t dst4 = __riscv_vle8_v_u8mf2(p_dst + stride * 4, vl);
    vuint8mf2_t dst5 = __riscv_vle8_v_u8mf2(p_dst + stride * 5, vl);
    vuint8mf2_t dst6 = __riscv_vle8_v_u8mf2(p_dst + stride * 6, vl);
    vuint8mf2_t dst7 = __riscv_vle8_v_u8mf2(p_dst + stride * 7, vl);

    vint16m1_t dst0_w = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwcvtu_x_x_v_u16m1(dst0, vl));
    vint16m1_t dst1_w = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwcvtu_x_x_v_u16m1(dst1, vl));
    vint16m1_t dst2_w = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwcvtu_x_x_v_u16m1(dst2, vl));
    vint16m1_t dst3_w = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwcvtu_x_x_v_u16m1(dst3, vl));
    vint16m1_t dst4_w = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwcvtu_x_x_v_u16m1(dst4, vl));
    vint16m1_t dst5_w = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwcvtu_x_x_v_u16m1(dst5, vl));
    vint16m1_t dst6_w = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwcvtu_x_x_v_u16m1(dst6, vl));
    vint16m1_t dst7_w = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwcvtu_x_x_v_u16m1(dst7, vl));

    result0 = __riscv_vadd_vv_i16m1(result0, dst0_w, vl);
    result1 = __riscv_vadd_vv_i16m1(result1, dst1_w, vl);
    result2 = __riscv_vadd_vv_i16m1(result2, dst2_w, vl);
    result3 = __riscv_vadd_vv_i16m1(result3, dst3_w, vl);
    result4 = __riscv_vadd_vv_i16m1(result4, dst4_w, vl);
    result5 = __riscv_vadd_vv_i16m1(result5, dst5_w, vl);
    result6 = __riscv_vadd_vv_i16m1(result6, dst6_w, vl);
    result7 = __riscv_vadd_vv_i16m1(result7, dst7_w, vl);

    result0 = __riscv_vmax_vx_i16m1(result0, 0, vl);
    result1 = __riscv_vmax_vx_i16m1(result1, 0, vl);
    result2 = __riscv_vmax_vx_i16m1(result2, 0, vl);
    result3 = __riscv_vmax_vx_i16m1(result3, 0, vl);
    result4 = __riscv_vmax_vx_i16m1(result4, 0, vl);
    result5 = __riscv_vmax_vx_i16m1(result5, 0, vl);
    result6 = __riscv_vmax_vx_i16m1(result6, 0, vl);
    result7 = __riscv_vmax_vx_i16m1(result7, 0, vl);

    vuint8mf2_t result0_n = __riscv_vnclipu_wx_u8mf2(__riscv_vreinterpret_v_i16m1_u16m1(result0), 0, __RISCV_FRM_RNE, vl);
    vuint8mf2_t result1_n = __riscv_vnclipu_wx_u8mf2(__riscv_vreinterpret_v_i16m1_u16m1(result1), 0, __RISCV_FRM_RNE, vl);
    vuint8mf2_t result2_n = __riscv_vnclipu_wx_u8mf2(__riscv_vreinterpret_v_i16m1_u16m1(result2), 0, __RISCV_FRM_RNE, vl);
    vuint8mf2_t result3_n = __riscv_vnclipu_wx_u8mf2(__riscv_vreinterpret_v_i16m1_u16m1(result3), 0, __RISCV_FRM_RNE, vl);
    vuint8mf2_t result4_n = __riscv_vnclipu_wx_u8mf2(__riscv_vreinterpret_v_i16m1_u16m1(result4), 0, __RISCV_FRM_RNE, vl);
    vuint8mf2_t result5_n = __riscv_vnclipu_wx_u8mf2(__riscv_vreinterpret_v_i16m1_u16m1(result5), 0, __RISCV_FRM_RNE, vl);
    vuint8mf2_t result6_n = __riscv_vnclipu_wx_u8mf2(__riscv_vreinterpret_v_i16m1_u16m1(result6), 0, __RISCV_FRM_RNE, vl);
    vuint8mf2_t result7_n = __riscv_vnclipu_wx_u8mf2(__riscv_vreinterpret_v_i16m1_u16m1(result7), 0, __RISCV_FRM_RNE, vl);

    __riscv_vse8_v_u8mf2(p_dst, result0_n, vl);
    __riscv_vse8_v_u8mf2(p_dst + stride, result1_n, vl);
    __riscv_vse8_v_u8mf2(p_dst + stride * 2, result2_n, vl);
    __riscv_vse8_v_u8mf2(p_dst + stride * 3, result3_n, vl);
    __riscv_vse8_v_u8mf2(p_dst + stride * 4, result4_n, vl);
    __riscv_vse8_v_u8mf2(p_dst + stride * 5, result5_n, vl);
    __riscv_vse8_v_u8mf2(p_dst + stride * 6, result6_n, vl);
    __riscv_vse8_v_u8mf2(p_dst + stride * 7, result7_n, vl);

    memset(p_block, 0, sizeof(int16_t) * 64);
}

void h264_idct8_dc_add_8_rvv(uint8_t *p_dst, int16_t *p_block, int stride)
{
    int count = 8;
    uint8_t *p_dst_iter = p_dst;

    int dc = (p_block[0] + 32) >> 6;

    if (dc > 255)
        dc = 255;

    if (dc < -255)
        dc = -255;

    p_block[0] = 0;

    while (count > 0)
    {
        int vl = __riscv_vsetvl_e16m1(8);

        vuint8m1_t dst0 = __riscv_vle8_v_u8m1(p_dst_iter, vl);
        vuint8m1_t dst1 = __riscv_vle8_v_u8m1(p_dst_iter + stride, vl);
        vuint8m1_t dst2 = __riscv_vle8_v_u8m1(p_dst_iter + stride * 2, vl);
        vuint8m1_t dst3 = __riscv_vle8_v_u8m1(p_dst_iter + stride * 3, vl);
        vuint8m1_t dst4 = __riscv_vle8_v_u8m1(p_dst_iter + stride * 4, vl);
        vuint8m1_t dst5 = __riscv_vle8_v_u8m1(p_dst_iter + stride * 5, vl);
        vuint8m1_t dst6 = __riscv_vle8_v_u8m1(p_dst_iter + stride * 6, vl);
        vuint8m1_t dst7 = __riscv_vle8_v_u8m1(p_dst_iter + stride * 7, vl);

        if (dc >= 0)
        {
            dst0 = __riscv_vsaddu_vx_u8m1(dst0, dc, vl);
            dst1 = __riscv_vsaddu_vx_u8m1(dst1, dc, vl);
            dst2 = __riscv_vsaddu_vx_u8m1(dst2, dc, vl);
            dst3 = __riscv_vsaddu_vx_u8m1(dst3, dc, vl);
            dst4 = __riscv_vsaddu_vx_u8m1(dst4, dc, vl);
            dst5 = __riscv_vsaddu_vx_u8m1(dst5, dc, vl);
            dst6 = __riscv_vsaddu_vx_u8m1(dst6, dc, vl);
            dst7 = __riscv_vsaddu_vx_u8m1(dst7, dc, vl);
        }
        else
        {
            dst0 = __riscv_vssubu_vx_u8m1(dst0, -dc, vl);
            dst1 = __riscv_vssubu_vx_u8m1(dst1, -dc, vl);
            dst2 = __riscv_vssubu_vx_u8m1(dst2, -dc, vl);
            dst3 = __riscv_vssubu_vx_u8m1(dst3, -dc, vl);
            dst4 = __riscv_vssubu_vx_u8m1(dst4, -dc, vl);
            dst5 = __riscv_vssubu_vx_u8m1(dst5, -dc, vl);
            dst6 = __riscv_vssubu_vx_u8m1(dst6, -dc, vl);
            dst7 = __riscv_vssubu_vx_u8m1(dst7, -dc, vl);
        }

        __riscv_vse8_v_u8m1(p_dst_iter, dst0, vl);
        __riscv_vse8_v_u8m1(p_dst_iter + stride, dst1, vl);
        __riscv_vse8_v_u8m1(p_dst_iter + stride * 2, dst2, vl);
        __riscv_vse8_v_u8m1(p_dst_iter + stride * 3, dst3, vl);
        __riscv_vse8_v_u8m1(p_dst_iter + stride * 4, dst4, vl);
        __riscv_vse8_v_u8m1(p_dst_iter + stride * 5, dst5, vl);
        __riscv_vse8_v_u8m1(p_dst_iter + stride * 6, dst6, vl);
        __riscv_vse8_v_u8m1(p_dst_iter + stride * 7, dst7, vl);

        count -= vl;
        p_dst_iter += vl;
    }
}

void h264_idct8_add4_8_rvv(uint8_t *p_dst, const int *p_block_offset,
                           int16_t *p_block, int stride, const uint8_t nnzc[5 * 8])
{
    for(int i = 0; i < 16; i += 4)
    {
        int nnz = nnzc[scan8[i]];

        if(nnz)
        {
            if(nnz == 1 && p_block[i * 16])
                h264_idct8_dc_add_8_rvv(p_dst + p_block_offset[i], p_block + i * 16 * sizeof(pixel), stride);
            else
                h264_idct8_add_8_rvv(p_dst + p_block_offset[i], p_block + i * 16 * sizeof(pixel), stride);
        }
    }
}
#endif

