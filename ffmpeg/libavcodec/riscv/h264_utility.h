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

#ifndef AVCODEC_RISCV_H264_UTILITY_H
#define AVCODEC_RISCV_H264_UTILITY_H
#include <riscv_vector.h>

__attribute__((always_inline)) static void u32_to_u8(vuint8mf2_t *p_u8_1, vuint8mf2_t *p_u8_2,
                                                     vuint32m2_t i32_1, vuint32m2_t i32_2, int vl)
{
    vuint16m1_t u16_1 = __riscv_vnclipu_wx_u16m1(i32_1, 10, __RISCV_FRM_RNE, vl);
    vuint16m1_t u16_2 = __riscv_vnclipu_wx_u16m1(i32_2, 10, __RISCV_FRM_RNE, vl);
    *p_u8_1 = __riscv_vnclipu_wx_u8mf2(u16_1, 0, __RISCV_FRM_RNE, vl);
    *p_u8_2 = __riscv_vnclipu_wx_u8mf2(u16_2, 0, __RISCV_FRM_RNE, vl);
}

__attribute__((always_inline)) static void put_average_l2(uint8_t **pp_dst, uint8_t **pp_l2_src,
                                                          ptrdiff_t dst_stride, ptrdiff_t l2_stride,
                                                          vuint8mf2_t src_row0, vuint8mf2_t src_row1, int vl)
{
    vuint8mf2_t l2_row0 = __riscv_vle8_v_u8mf2(*pp_l2_src, vl);
    *pp_l2_src += l2_stride;
    vuint8mf2_t l2_row1 = __riscv_vle8_v_u8mf2(*pp_l2_src, vl);
    *pp_l2_src += l2_stride;

    src_row0 = __riscv_vaaddu_vv_u8mf2(src_row0, l2_row0, __RISCV_FRM_RNE, vl);
    src_row1 = __riscv_vaaddu_vv_u8mf2(src_row1, l2_row1, __RISCV_FRM_RNE, vl);

    __riscv_vse8_v_u8mf2(*pp_dst, src_row0, vl);
    *pp_dst += dst_stride;
    __riscv_vse8_v_u8mf2(*pp_dst, src_row1, vl);
    *pp_dst += dst_stride;
}

__attribute__((always_inline)) static void avg_average_l2(uint8_t **pp_dst, uint8_t **pp_l2_src,
                                                          ptrdiff_t dst_stride, ptrdiff_t l2_stride,
                                                          vuint8mf2_t src_row0, vuint8mf2_t src_row1, int vl)
{
    vuint8mf2_t l2_row0 = __riscv_vle8_v_u8mf2(*pp_l2_src, vl);
    *pp_l2_src += l2_stride;
    vuint8mf2_t l2_row1 = __riscv_vle8_v_u8mf2(*pp_l2_src, vl);
    *pp_l2_src += l2_stride;

    vuint8mf2_t dst0 = __riscv_vle8_v_u8mf2(*pp_dst, vl);
    vuint8mf2_t dst1 = __riscv_vle8_v_u8mf2(*pp_dst + dst_stride, vl);

    src_row0 = __riscv_vaaddu_vv_u8mf2(src_row0, l2_row0, __RISCV_FRM_RNE, vl);
    src_row1 = __riscv_vaaddu_vv_u8mf2(src_row1, l2_row1, __RISCV_FRM_RNE, vl);

    src_row0 = __riscv_vaaddu_vv_u8mf2(src_row0, dst0, __RISCV_FRM_RNE, vl);
    src_row1 = __riscv_vaaddu_vv_u8mf2(src_row1, dst1,__RISCV_FRM_RNE,  vl);

    __riscv_vse8_v_u8mf2(*pp_dst, src_row0, vl);
    *pp_dst += dst_stride;
    __riscv_vse8_v_u8mf2(*pp_dst, src_row1, vl);
    *pp_dst += dst_stride;
}
#endif