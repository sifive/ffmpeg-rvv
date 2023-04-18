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

#include "h264_inloop.h"
#if HAVE_INTRINSICS_RVV
#include <riscv_vector.h>

__attribute__((always_inline)) static void extend_tc0_2(vint8mf2_t *p_tc0_i8, int8_t *p_tc0, size_t start, int vl)
{
    if (p_tc0[0] == p_tc0[1] && p_tc0[1] == p_tc0[2] && p_tc0[2] == p_tc0[3])
    {
        *p_tc0_i8 = __riscv_vmv_v_x_i8mf2(p_tc0[0], vl);
    }
    else
    {
        const uint8_t tc02_index[] = {0, 0, 1, 1, 2, 2, 3, 3};
        vint8mf2_t tc8 = __riscv_vle8_v_i8mf2(p_tc0, 4);
        vuint8mf2_t v_index = __riscv_vle8_v_u8mf2(tc02_index + start, vl);
        *p_tc0_i8 = __riscv_vrgather_vv_i8mf2(tc8, v_index, vl);
    }
}

__attribute__((always_inline)) static void extend_tc0(vint8mf2_t *p_tc0_i8, int8_t *p_tc0, size_t start, int vl)
{
    if (p_tc0[0] == p_tc0[1] && p_tc0[1] == p_tc0[2] && p_tc0[2] == p_tc0[3])
    {
        *p_tc0_i8 = __riscv_vmv_v_x_i8mf2(p_tc0[0], vl);
    }
    else
    {
        const uint8_t tc01_index[] = {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3};
        vint8mf2_t tc8 = __riscv_vle8_v_i8mf2(p_tc0, 4);
        vuint8mf2_t v_index = __riscv_vle8_v_u8mf2(tc01_index + start, vl);
        *p_tc0_i8 = __riscv_vrgather_vv_i8mf2(tc8, v_index, vl);
    }
}

__attribute__((always_inline)) static void luma_core(vuint8mf2_t *p_p1_dst, vuint8mf2_t *p_p0_dst,
                                                     vuint8mf2_t *p_q0_dst, vuint8mf2_t *p_q1_dst,
                                                     vuint8mf2_t p2, vuint8mf2_t p1, vuint8mf2_t p0,
                                                     vuint8mf2_t q0, vuint8mf2_t q1, vuint8mf2_t q2,
                                                     vint8mf2_t tc8, int alpha, int beta, int vl)
{
    vint16m1_t p2_i16 = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwaddu_vx_u16m1(p2, 0, vl));
    vint16m1_t p1_i16 = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwaddu_vx_u16m1(p1, 0, vl));
    vint16m1_t p0_i16 = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwaddu_vx_u16m1(p0, 0, vl));
    vint16m1_t q0_i16 = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwaddu_vx_u16m1(q0, 0, vl));
    vint16m1_t q1_i16 = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwaddu_vx_u16m1(q1, 0, vl));
    vint16m1_t q2_i16 = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwaddu_vx_u16m1(q2, 0, vl));

    vint16m1_t sub_q0_p0 = __riscv_vsub_vv_i16m1(q0_i16, p0_i16, vl);
    vint16m1_t sub_p1_p0 = __riscv_vsub_vv_i16m1(p1_i16, p0_i16, vl);
    vint16m1_t sub_q1_q0 = __riscv_vsub_vv_i16m1(q1_i16, q0_i16, vl);
    vint16m1_t sub_p2_p0 = __riscv_vsub_vv_i16m1(p2_i16, p0_i16, vl);
    vint16m1_t sub_q2_q0 = __riscv_vsub_vv_i16m1(q2_i16, q0_i16, vl);

    vint16m1_t minus_sub_q0_p0 = __riscv_vrsub_vx_i16m1(sub_q0_p0, 0, vl);
    vint16m1_t minus_sub_p1_p0 = __riscv_vrsub_vx_i16m1(sub_p1_p0, 0, vl);
    vint16m1_t minus_sub_q1_q0 = __riscv_vrsub_vx_i16m1(sub_q1_q0, 0, vl);
    vint16m1_t minus_sub_p2_p0 = __riscv_vrsub_vx_i16m1(sub_p2_p0, 0, vl);
    vint16m1_t minus_sub_q2_q0 = __riscv_vrsub_vx_i16m1(sub_q2_q0, 0, vl);

    vint16m1_t abs_diff11 = __riscv_vmax_vv_i16m1(sub_q0_p0, minus_sub_q0_p0, vl);
    vint16m1_t abs_diff12 = __riscv_vmax_vv_i16m1(sub_p1_p0, minus_sub_p1_p0, vl);
    vint16m1_t abs_diff13 = __riscv_vmax_vv_i16m1(sub_q1_q0, minus_sub_q1_q0, vl);
    vint16m1_t abs_diff2 = __riscv_vmax_vv_i16m1(sub_p2_p0, minus_sub_p2_p0, vl);
    vint16m1_t abs_diff3 = __riscv_vmax_vv_i16m1(sub_q2_q0, minus_sub_q2_q0, vl);

    vint16m1_t tc = __riscv_vwadd_vx_i16m1(tc8, 0, vl);
    vbool16_t cond_mask = __riscv_vmsge_vx_i16m1_b16(tc, 0, vl);
    vbool16_t cond11 = __riscv_vmslt_vx_i16m1_b16(abs_diff11, alpha, vl);
    vbool16_t cond12 = __riscv_vmslt_vx_i16m1_b16(abs_diff12, beta, vl);
    vbool16_t cond13 = __riscv_vmslt_vx_i16m1_b16(abs_diff13, beta, vl);
    vbool16_t cond2 = __riscv_vmslt_vx_i16m1_b16(abs_diff2, beta, vl);
    vbool16_t cond3 = __riscv_vmslt_vx_i16m1_b16(abs_diff3, beta, vl);

    vbool16_t cond1 = __riscv_vmand_mm_b16(cond11, cond_mask, vl);
    cond1 = __riscv_vmand_mm_b16(cond1, cond12, vl);
    cond1 = __riscv_vmand_mm_b16(cond1, cond13, vl);
    cond2 = __riscv_vmand_mm_b16(cond2, cond1, vl);
    cond3 = __riscv_vmand_mm_b16(cond3, cond1, vl);

    // p1
    vint16m1_t sum_p0_q0 = __riscv_vaadd_vv_i16m1(p0_i16, q0_i16, vl);
    vint16m1_t p1_new_i16 = __riscv_vadd_vv_i16m1(sum_p0_q0, p2_i16, vl);
    p1_new_i16 = __riscv_vsra_vx_i16m1(p1_new_i16, 1, vl);
    vint16m1_t p1_new_upper = __riscv_vadd_vv_i16m1(p1_i16, tc, vl);
    vint16m1_t p1_new_lower = __riscv_vsub_vv_i16m1(p1_i16, tc, vl);
    p1_new_i16 = __riscv_vmax_vv_i16m1(p1_new_i16, p1_new_lower, vl);
    p1_new_i16 = __riscv_vmin_vv_i16m1(p1_new_i16, p1_new_upper, vl);
    *p_p1_dst = __riscv_vncvt_x_x_w_u8mf2_mu(cond2, p1, __riscv_vreinterpret_v_i16m1_u16m1(p1_new_i16), vl);
    vint16m1_t tc_adjust = __riscv_vadc_vxm_i16m1(tc, 0, cond2, vl);

    // q1
    vint16m1_t q1_new_i16 = __riscv_vadd_vv_i16m1(sum_p0_q0, q2_i16, vl);
    q1_new_i16 = __riscv_vsra_vx_i16m1(q1_new_i16, 1, vl);
    vint16m1_t q1_new_upper = __riscv_vadd_vv_i16m1(q1_i16, tc, vl);
    vint16m1_t q1_new_lower = __riscv_vsub_vv_i16m1(q1_i16, tc, vl);
    q1_new_i16 = __riscv_vmax_vv_i16m1(q1_new_i16, q1_new_lower, vl);
    q1_new_i16 = __riscv_vmin_vv_i16m1(q1_new_i16, q1_new_upper, vl);
    *p_q1_dst = __riscv_vncvt_x_x_w_u8mf2_mu(cond3, q1, __riscv_vreinterpret_v_i16m1_u16m1(q1_new_i16), vl);
    tc_adjust = __riscv_vadc_vxm_i16m1(tc_adjust, 0, cond3, vl);

    // p0, q0
    vint16m1_t sub_p1_q1 = __riscv_vsub_vv_i16m1(p1_i16, q1_i16, vl);
    vint16m1_t delta_i16 = __riscv_vsll_vx_i16m1(sub_q0_p0, 2, vl);
    delta_i16 = __riscv_vadd_vv_i16m1(delta_i16, sub_p1_q1, vl);
    delta_i16 = __riscv_vssra_vx_i16m1(delta_i16, 3, vl);
    delta_i16 = __riscv_vmin_vv_i16m1(delta_i16, tc_adjust, vl);
    delta_i16 = __riscv_vmax_vv_i16m1(delta_i16, __riscv_vrsub_vx_i16m1(tc_adjust, 0, vl), vl);

    vint16m1_t p0_new_i16 = __riscv_vadd_vv_i16m1(p0_i16, delta_i16, vl);
    vint16m1_t q0_new_i16 = __riscv_vsub_vv_i16m1(q0_i16, delta_i16, vl);
    p0_new_i16 = __riscv_vmax_vx_i16m1(p0_new_i16, 0, vl);
    q0_new_i16 = __riscv_vmax_vx_i16m1(q0_new_i16, 0, vl);

    *p_p0_dst= __riscv_vnclipu_wx_u8mf2_mu(cond1, p0, __riscv_vreinterpret_v_i16m1_u16m1(p0_new_i16), 0, vl);
    *p_q0_dst = __riscv_vnclipu_wx_u8mf2_mu(cond1, q0, __riscv_vreinterpret_v_i16m1_u16m1(q0_new_i16), 0, vl);
}

__attribute__((always_inline)) static void v_loop_filter_luma(uint8_t *p_pix, ptrdiff_t stride,
                                                              int width, int alpha, int beta, int8_t *p_tc0)
{
    uint8_t *p_iter = p_pix;

    size_t vxrm = __builtin_rvv_vgetvxrm();
    __builtin_rvv_vsetvxrm(VE_TONEARESTUP);

    int count = width;
    int tc_offset = 0;

    while (count > 0)
    {
        int vl = __riscv_vsetvl_e8mf2(width);

        vint8mf2_t tc8;
        extend_tc0(&tc8, p_tc0, tc_offset, vl);

        vuint8mf2_t p2 = __riscv_vle8_v_u8mf2(p_iter - 3 * stride, vl);
        vuint8mf2_t p1 = __riscv_vle8_v_u8mf2(p_iter - 2 * stride, vl);
        vuint8mf2_t p0 = __riscv_vle8_v_u8mf2(p_iter - stride, vl);
        vuint8mf2_t q0 = __riscv_vle8_v_u8mf2(p_iter, vl);
        vuint8mf2_t q1 = __riscv_vle8_v_u8mf2(p_iter + stride, vl);
        vuint8mf2_t q2 = __riscv_vle8_v_u8mf2(p_iter + 2 * stride, vl);

        vuint8mf2_t p1_dst, p0_dst, q0_dst, q1_dst;
        luma_core(&p1_dst, &p0_dst, &q0_dst, &q1_dst, p2, p1, p0, q0, q1, q2, tc8, alpha, beta, vl);

        __riscv_vse8_v_u8mf2(p_iter - stride * 2, p1_dst, vl);
        __riscv_vse8_v_u8mf2(p_iter - stride, p0_dst, vl);
        __riscv_vse8_v_u8mf2(p_iter, q0_dst, vl);
        __riscv_vse8_v_u8mf2(p_iter + stride, q1_dst, vl);

        count -= vl;
        tc_offset = tc_offset + vl;
        p_iter = p_iter + vl;
    }

    __builtin_rvv_vsetvxrm(vxrm);
}

__attribute__((always_inline)) static void h_loop_filter_luma(uint8_t *p_pix, ptrdiff_t stride,
                                                              int width, int alpha, int beta, int8_t *p_tc0)
{
    uint8_t *p_iter = p_pix;

    size_t vxrm = __builtin_rvv_vgetvxrm();
    __builtin_rvv_vsetvxrm(VE_TONEARESTUP);

    int count = width;
    int tc_offset = 0;

    while (count > 0)
    {
        int vl = __riscv_vsetvl_e8mf2(width);

        vint8mf2_t tc8;
        extend_tc0(&tc8, p_tc0, tc_offset, vl);

        vuint8mf2_t p2, p1, p0, q0, q1, q2;
        __riscv_vlsseg6e8_v_u8mf2(&p2, &p1, &p0, &q0, &q1, &q2, p_iter - 3, stride, width);

        vuint8mf2_t p1_dst, p0_dst, q0_dst, q1_dst;
        luma_core(&p1_dst, &p0_dst, &q0_dst, &q1_dst, p2, p1, p0, q0, q1, q2, tc8, alpha, beta, vl);

        __riscv_vssseg4e8_v_u8mf2(p_iter - 2, stride, p1_dst, p0_dst, q0_dst, q1_dst, 16);

        count -= vl;
        tc_offset = tc_offset + vl;
        p_iter = p_iter + vl * stride;
    }

    __builtin_rvv_vsetvxrm(vxrm);
}

__attribute__((always_inline)) static void chroma_core(vuint8mf2_t *p_p0_dst, vuint8mf2_t *p_q0_dst,
                                                       vuint8mf2_t p1, vuint8mf2_t p0, vuint8mf2_t q0,
                                                       vuint8mf2_t q1, vint8mf2_t tc8, int alpha,
                                                       int beta, int vl)
{
    vint16m1_t p1_i16 = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwaddu_vx_u16m1(p1, 0, vl));
    vint16m1_t p0_i16 = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwaddu_vx_u16m1(p0, 0, vl));
    vint16m1_t q0_i16 = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwaddu_vx_u16m1(q0, 0, vl));
    vint16m1_t q1_i16 = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwaddu_vx_u16m1(q1, 0, vl));

    vint16m1_t sub_q0_p0 = __riscv_vsub_vv_i16m1(q0_i16, p0_i16, vl);
    vint16m1_t sub_p1_p0 = __riscv_vsub_vv_i16m1(p1_i16, p0_i16, vl);
    vint16m1_t sub_q1_q0 = __riscv_vsub_vv_i16m1(q1_i16, q0_i16, vl);

    vint16m1_t rsub_q0_p0 = __riscv_vrsub_vx_i16m1(sub_q0_p0, 0, vl);
    vint16m1_t rsub_p1_p0 = __riscv_vrsub_vx_i16m1(sub_p1_p0, 0, vl);
    vint16m1_t rsub_q1_q0 = __riscv_vrsub_vx_i16m1(sub_q1_q0, 0, vl);

    vint16m1_t abs_diff11 = __riscv_vmax_vv_i16m1(sub_q0_p0, rsub_q0_p0, vl);
    vint16m1_t abs_diff12 = __riscv_vmax_vv_i16m1(sub_p1_p0, rsub_p1_p0, vl);
    vint16m1_t abs_diff13 = __riscv_vmax_vv_i16m1(sub_q1_q0, rsub_q1_q0, vl);

    vint16m1_t tc = __riscv_vwadd_vx_i16m1(tc8, 0, vl);
    vbool16_t cond_mask = __riscv_vmsge_vx_i16m1_b16(tc, 0, vl);
    vbool16_t cond11 = __riscv_vmslt_vx_i16m1_b16_mu(cond_mask, cond_mask, abs_diff11, alpha, vl);
    vbool16_t cond12 = __riscv_vmslt_vx_i16m1_b16_mu(cond11, cond11, abs_diff12, beta, vl);
    vbool16_t cond13 = __riscv_vmslt_vx_i16m1_b16_mu(cond12, cond12, abs_diff13, beta, vl);

    vint16m1_t sub_p1_q1 = __riscv_vsub_vv_i16m1(p1_i16, q1_i16, vl);
    vint16m1_t delta = __riscv_vsll_vx_i16m1(sub_q0_p0, 2, vl);
    delta = __riscv_vadd_vv_i16m1(delta, sub_p1_q1, vl);
    delta = __riscv_vssra_vx_i16m1(delta, 3, vl);
    delta = __riscv_vmin_vv_i16m1(delta, tc, vl);
    delta = __riscv_vmax_vv_i16m1(delta, __riscv_vrsub_vx_i16m1(tc, 0, vl), vl);

    vint16m1_t p0_new_i16 = __riscv_vadd_vv_i16m1(p0_i16, delta, vl);
    vint16m1_t q0_new_i16 = __riscv_vsub_vv_i16m1(q0_i16, delta, vl);
    p0_new_i16 = __riscv_vmax_vx_i16m1(p0_new_i16, 0, vl);
    q0_new_i16 = __riscv_vmax_vx_i16m1(q0_new_i16, 0, vl);

    *p_p0_dst = __riscv_vnclipu_wx_u8mf2_mu(cond13, p0, __riscv_vreinterpret_v_i16m1_u16m1(p0_new_i16), 0, vl);
    *p_q0_dst = __riscv_vnclipu_wx_u8mf2_mu(cond13, q0, __riscv_vreinterpret_v_i16m1_u16m1(q0_new_i16), 0, vl);
}

__attribute__((always_inline)) static void v_loop_filter_chroma(uint8_t *p_pix, ptrdiff_t stride,
                                                                int width, int alpha, int beta, int8_t *p_tc0)
{
    uint8_t *p_iter = p_pix;

    size_t vxrm = __builtin_rvv_vgetvxrm();
    __builtin_rvv_vsetvxrm(VE_TONEARESTUP);

    int count = width;
    int tc_offset = 0;

    while (count > 0)
    {
        int vl = __riscv_vsetvl_e8mf2(width);

        vint8mf2_t tc8;
        extend_tc0_2(&tc8, p_tc0, tc_offset, vl);

        vuint8mf2_t p1 = __riscv_vle8_v_u8mf2(p_iter - 2 * stride, vl);
        vuint8mf2_t p0 = __riscv_vle8_v_u8mf2(p_iter - stride, vl);
        vuint8mf2_t q0 = __riscv_vle8_v_u8mf2(p_iter, vl);
        vuint8mf2_t q1 = __riscv_vle8_v_u8mf2(p_iter + stride, vl);

        vuint8mf2_t p0_dst, q0_dst;
        chroma_core(&p0_dst, &q0_dst, p1, p0, q0, q1, tc8, alpha, beta, vl);

        __riscv_vse8_v_u8mf2(p_iter - stride, p0_dst, vl);
        __riscv_vse8_v_u8mf2(p_iter, q0_dst, vl);

        count -= vl;
        tc_offset += vl;
        p_iter = p_iter + vl;
    }

    __builtin_rvv_vsetvxrm(vxrm);
}

__attribute__((always_inline)) static void h_loop_filter_chroma(uint8_t *p_pix, ptrdiff_t stride,
                                                                int width, int alpha, int beta, int8_t *p_tc0)
{
    uint8_t *p_iter = p_pix;

    size_t vxrm = __builtin_rvv_vgetvxrm();
    __builtin_rvv_vsetvxrm(VE_TONEARESTUP);

    int count = width;
    int tc_offset = 0;

    while (count > 0)
    {
        int vl = __riscv_vsetvl_e8mf2(width);

        vint8mf2_t tc8;
        extend_tc0_2(&tc8, p_tc0, tc_offset, vl);

        vuint8mf2_t p1, p0, q0, q1;
        __riscv_vlsseg4e8_v_u8mf2(&p1, &p0, &q0, &q1, p_iter - 2, stride, vl);

        vuint8mf2_t p0_dst, q0_dst;
        chroma_core(&p0_dst, &q0_dst, p1, p0, q0, q1, tc8, alpha, beta, vl);

        __riscv_vssseg2e8_v_u8mf2(p_iter - 1, stride, p0_dst, q0_dst, vl);

        count -= vl;
        tc_offset = tc_offset + vl;
        p_iter = p_iter + vl * stride;
    }

    __builtin_rvv_vsetvxrm(vxrm);
}

__attribute__((always_inline)) static void luma_intra_core(vuint8mf2_t *p_p2_dst, vuint8mf2_t *p_p1_dst,
                                                           vuint8mf2_t *p_p0_dst, vuint8mf2_t *p_q0_dst,
                                                           vuint8mf2_t *p_q1_dst, vuint8mf2_t *p_q2_dst,
                                                           vuint8mf2_t p3, vuint8mf2_t p2, vuint8mf2_t p1,
                                                           vuint8mf2_t p0, vuint8mf2_t q0, vuint8mf2_t q1,
                                                           vuint8mf2_t q2, vuint8mf2_t q3, int alpha,
                                                           int beta, int vl)
{
        vint16m1_t p3_i16 = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwaddu_vx_u16m1(p3, 0, vl));
        vint16m1_t p2_i16 = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwaddu_vx_u16m1(p2, 0, vl));
        vint16m1_t p1_i16 = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwaddu_vx_u16m1(p1, 0, vl));
        vint16m1_t p0_i16 = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwaddu_vx_u16m1(p0, 0, vl));
        vint16m1_t q0_i16 = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwaddu_vx_u16m1(q0, 0, vl));
        vint16m1_t q1_i16 = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwaddu_vx_u16m1(q1, 0, vl));
        vint16m1_t q2_i16 = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwaddu_vx_u16m1(q2, 0, vl));
        vint16m1_t q3_i16 = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwaddu_vx_u16m1(q3, 0, vl));

        // p0
        vint16m1_t sum_p1p0q0 = __riscv_vadd_vv_i16m1(p0_i16, p1_i16, vl);
        sum_p1p0q0 = __riscv_vadd_vv_i16m1(sum_p1p0q0, q0_i16, vl);

        vint16m1_t p0_new1_i16 = __riscv_vadd_vv_i16m1(p0_i16, q1_i16, vl);
        vint16m1_t p0_new2_i16 = __riscv_vadd_vv_i16m1(p2_i16, q1_i16, vl);

        // p1
        vint16m1_t p1_new1_i16 = __riscv_vadd_vv_i16m1(sum_p1p0q0, p2_i16, vl);

        // q0
        vint16m1_t sum_p0q0q1 = __riscv_vadd_vv_i16m1(p0_i16, q0_i16, vl);
        sum_p0q0q1 = __riscv_vadd_vv_i16m1(sum_p0q0q1, q1_i16, vl);

        vint16m1_t q0_new1_i16 = __riscv_vadd_vv_i16m1(q0_i16, p1_i16, vl);
        vint16m1_t q0_new2_i16 = __riscv_vadd_vv_i16m1(q2_i16, p1_i16, vl);

        // q1
        vint16m1_t q1_new1_i16 = __riscv_vadd_vv_i16m1(sum_p0q0q1, q2_i16, vl);

        p0_new1_i16 = __riscv_vmacc_vx_i16m1(p0_new1_i16, 2, p1_i16, vl);
        p0_new2_i16 = __riscv_vmacc_vx_i16m1(p0_new2_i16, 2, sum_p1p0q0, vl);
        vint16m1_t p2_new1_i16 = __riscv_vmadd_vx_i16m1(p3_i16, 2, sum_p1p0q0, vl);
        p2_new1_i16 = __riscv_vmacc_vx_i16m1(p2_new1_i16, 3, p2_i16, vl);
        q0_new1_i16 = __riscv_vmacc_vx_i16m1(q0_new1_i16, 2, q1_i16, vl);
        q0_new2_i16 = __riscv_vmacc_vx_i16m1(q0_new2_i16, 2, sum_p0q0q1, vl);
        vint16m1_t q2_new1_i16 = __riscv_vmadd_vx_i16m1(q3_i16, 2, sum_p0q0q1, vl);
        q2_new1_i16 = __riscv_vmacc_vx_i16m1(q2_new1_i16, 3, q2_i16, vl);

        vint16m1_t sub_q0p0 = __riscv_vsub_vv_i16m1(q0_i16, p0_i16, vl);
        vint16m1_t sub_p1p0 = __riscv_vsub_vv_i16m1(p1_i16, p0_i16, vl);
        vint16m1_t sub_q1q0 = __riscv_vsub_vv_i16m1(q1_i16, q0_i16, vl);
        vint16m1_t sub_p2p0 = __riscv_vsub_vv_i16m1(p2_i16, p0_i16, vl);
        vint16m1_t sub_q2q0 = __riscv_vsub_vv_i16m1(q2_i16, q0_i16, vl);

        vint16m1_t rsub_q0p0 = __riscv_vrsub_vx_i16m1(sub_q0p0, 0, vl);
        vint16m1_t rsub_p1p0 = __riscv_vrsub_vx_i16m1(sub_p1p0, 0, vl);
        vint16m1_t rsub_q1q0 = __riscv_vrsub_vx_i16m1(sub_q1q0, 0, vl);
        vint16m1_t rsub_p2p0 = __riscv_vrsub_vx_i16m1(sub_p2p0, 0, vl);
        vint16m1_t rsub_q2q0 = __riscv_vrsub_vx_i16m1(sub_q2q0, 0, vl);

        vint16m1_t abd_q0p0 = __riscv_vmax_vv_i16m1(rsub_q0p0, sub_q0p0, vl);
        vint16m1_t abd_p1p0_ = __riscv_vmax_vv_i16m1(rsub_p1p0, sub_p1p0, vl);
        vint16m1_t abd_q1q0 = __riscv_vmax_vv_i16m1(rsub_q1q0, sub_q1q0, vl);
        vint16m1_t abd_p2p0 = __riscv_vmax_vv_i16m1(rsub_p2p0, sub_p2p0, vl);
        vint16m1_t abd_q2q0 = __riscv_vmax_vv_i16m1(rsub_q2q0, sub_q2q0, vl);

        vbool16_t cond11 = __riscv_vmslt_vx_i16m1_b16(abd_q0p0, alpha, vl);
        vbool16_t cond12 = __riscv_vmslt_vx_i16m1_b16(abd_p1p0_, beta, vl);
        vbool16_t cond13 = __riscv_vmslt_vx_i16m1_b16(abd_q1q0, beta, vl);
        vbool16_t cond2 = __riscv_vmslt_vx_i16m1_b16(abd_q0p0, (alpha >> 2) + 2, vl);
        vbool16_t cond3 = __riscv_vmslt_vx_i16m1_b16(abd_p2p0, beta, vl);
        vbool16_t cond4 = __riscv_vmslt_vx_i16m1_b16(abd_q2q0, beta, vl);

        vbool16_t cond1 = __riscv_vmand_mm_b16(cond11, cond12, vl);
        cond1 = __riscv_vmand_mm_b16(cond1, cond13, vl);
        cond2 = __riscv_vmand_mm_b16(cond2, cond1, vl);
        cond3 = __riscv_vmand_mm_b16(cond3, cond2, vl);
        cond4 = __riscv_vmand_mm_b16(cond4, cond2, vl);

        vuint8mf2_t p0_new1_u8 = __riscv_vnclipu_wx_u8mf2(__riscv_vreinterpret_v_i16m1_u16m1(p0_new1_i16), 2, vl);
        vuint8mf2_t p0_new2_u8 = __riscv_vnclipu_wx_u8mf2(__riscv_vreinterpret_v_i16m1_u16m1(p0_new2_i16), 3, vl);
        vuint8mf2_t p1_new1_u8 = __riscv_vnclipu_wx_u8mf2(__riscv_vreinterpret_v_i16m1_u16m1(p1_new1_i16), 2, vl);
        vuint8mf2_t p2_new1_u8 = __riscv_vnclipu_wx_u8mf2(__riscv_vreinterpret_v_i16m1_u16m1(p2_new1_i16), 3, vl);
        vuint8mf2_t q0_new1_u8 = __riscv_vnclipu_wx_u8mf2(__riscv_vreinterpret_v_i16m1_u16m1(q0_new1_i16), 2, vl);
        vuint8mf2_t q0_new2_u8 = __riscv_vnclipu_wx_u8mf2(__riscv_vreinterpret_v_i16m1_u16m1(q0_new2_i16), 3, vl);
        vuint8mf2_t q1_new1_u8 = __riscv_vnclipu_wx_u8mf2(__riscv_vreinterpret_v_i16m1_u16m1(q1_new1_i16), 2, vl);
        vuint8mf2_t q2_new1_u8 = __riscv_vnclipu_wx_u8mf2(__riscv_vreinterpret_v_i16m1_u16m1(q2_new1_i16), 3, vl);

        *p_p1_dst = __riscv_vmerge_vvm_u8mf2(p1, p1_new1_u8, cond3, vl);
        *p_p2_dst = __riscv_vmerge_vvm_u8mf2(p2, p2_new1_u8, cond3, vl);
        *p_p0_dst = __riscv_vmerge_vvm_u8mf2(p0_new1_u8, p0_new2_u8, cond3, vl);
        *p_p0_dst = __riscv_vmerge_vvm_u8mf2(p0, *p_p0_dst, cond1, vl);

        *p_q0_dst = __riscv_vmerge_vvm_u8mf2(q0, q0_new1_u8, cond1, vl);
        *p_q0_dst = __riscv_vmerge_vvm_u8mf2(*p_q0_dst, q0_new2_u8, cond4, vl);
        *p_q1_dst = __riscv_vmerge_vvm_u8mf2(q1, q1_new1_u8, cond4, vl);
        *p_q2_dst = __riscv_vmerge_vvm_u8mf2(q2, q2_new1_u8, cond4, vl);
}

__attribute__((always_inline)) static void v_loop_filter_luma_intra(uint8_t *p_pix, ptrdiff_t stride,
                                                                    int width, int alpha, int beta)
{
    uint8_t *p_iter = p_pix;

    size_t vxrm = __builtin_rvv_vgetvxrm();
    __builtin_rvv_vsetvxrm(VE_TONEARESTUP);

    int count = width;

    while (count > 0)
    {
        int vl = __riscv_vsetvl_e8mf2(width);

        vuint8mf2_t p3 = __riscv_vle8_v_u8mf2(p_iter - 4 * stride, vl);
        vuint8mf2_t p2 = __riscv_vle8_v_u8mf2(p_iter - 3 * stride, vl);
        vuint8mf2_t p1 = __riscv_vle8_v_u8mf2(p_iter - 2 * stride, vl);
        vuint8mf2_t p0 = __riscv_vle8_v_u8mf2(p_iter - stride, vl);
        vuint8mf2_t q0 = __riscv_vle8_v_u8mf2(p_iter, vl);
        vuint8mf2_t q1 = __riscv_vle8_v_u8mf2(p_iter + stride, vl);
        vuint8mf2_t q2 = __riscv_vle8_v_u8mf2(p_iter + 2 * stride, vl);
        vuint8mf2_t q3 = __riscv_vle8_v_u8mf2(p_iter + 3 * stride, vl);

        vuint8mf2_t p2_dst, p1_dst, p0_dst, q0_dst, q1_dst, q2_dst;

        luma_intra_core(&p2_dst, &p1_dst, &p0_dst, &q0_dst, &q1_dst, &q2_dst,
                        p3, p2, p1, p0, q0, q1, q2, q3, alpha, beta, vl);

        __riscv_vse8_v_u8mf2(p_iter - stride * 3, p2_dst, vl);
        __riscv_vse8_v_u8mf2(p_iter - stride * 2, p1_dst, vl);
        __riscv_vse8_v_u8mf2(p_iter - stride, p0_dst, vl);
        __riscv_vse8_v_u8mf2(p_iter, q0_dst, vl);
        __riscv_vse8_v_u8mf2(p_iter + stride, q1_dst, vl);
        __riscv_vse8_v_u8mf2(p_iter + stride * 2, q2_dst, vl);

        count -= vl;
        p_iter = p_iter + vl;
    }

    __builtin_rvv_vsetvxrm(vxrm);
}

__attribute__((always_inline)) static void h_loop_filter_luma_intra(uint8_t *p_pix, ptrdiff_t stride,
                                                                    int width, int alpha, int beta)
{
    uint8_t *p_iter = p_pix;

    size_t vxrm = __builtin_rvv_vgetvxrm();
    __builtin_rvv_vsetvxrm(VE_TONEARESTUP);

    int count = width;

    while (count > 0)
    {
        int vl = __riscv_vsetvl_e8mf2(width);

        vuint8mf2_t p3, p2, p1, p0, q0, q1, q2, q3;
        __riscv_vlsseg8e8_v_u8mf2(&p3, &p2, &p1, &p0,
                          &q0, &q1, &q2, &q3, p_iter - 4, stride, 16);

        vuint8mf2_t p2_dst, p1_dst, p0_dst, q0_dst, q1_dst, q2_dst;

        luma_intra_core(&p2_dst, &p1_dst, &p0_dst, &q0_dst, &q1_dst, &q2_dst,
                        p3, p2, p1, p0, q0, q1, q2, q3, alpha, beta, vl);

        __riscv_vssseg6e8_v_u8mf2(p_iter - 3, stride,
                          p2_dst, p1_dst, p0_dst, q0_dst, q1_dst, q2_dst, 16);

        count -= vl;
        p_iter = p_iter + vl * stride;
    }

    __builtin_rvv_vsetvxrm(vxrm);
}

__attribute__((always_inline)) static void chroma_intra_core(vuint8mf2_t *p_p0_dst, vuint8mf2_t *p_q0_dst,
                                                             vuint8mf2_t p1, vuint8mf2_t p0,
                                                             vuint8mf2_t q0, vuint8mf2_t q1,
                                                             int alpha, int beta, int vl)
{
    vint16m1_t p1_i16 = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwaddu_vx_u16m1(p1, 0, vl));
    vint16m1_t p0_i16 = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwaddu_vx_u16m1(p0, 0, vl));
    vint16m1_t q0_i16 = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwaddu_vx_u16m1(q0, 0, vl));
    vint16m1_t q1_i16 = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwaddu_vx_u16m1(q1, 0, vl));

    vint16m1_t sub_q0p0 = __riscv_vsub_vv_i16m1(q0_i16, p0_i16, vl);
    vint16m1_t sub_p1p0 = __riscv_vsub_vv_i16m1(p1_i16, p0_i16, vl);
    vint16m1_t sub_q1q0 = __riscv_vsub_vv_i16m1(q1_i16, q0_i16, vl);

    vint16m1_t rsub_q0p0 = __riscv_vrsub_vx_i16m1(sub_q0p0, 0, vl);
    vint16m1_t rsub_p1p0 = __riscv_vrsub_vx_i16m1(sub_p1p0, 0, vl);
    vint16m1_t rsub_q1q0 = __riscv_vrsub_vx_i16m1(sub_q1q0, 0, vl);

    vint16m1_t abd_q0p0 = __riscv_vmax_vv_i16m1(sub_q0p0, rsub_q0p0, vl);
    vint16m1_t abd_p1p0_ = __riscv_vmax_vv_i16m1(sub_p1p0, rsub_p1p0, vl);
    vint16m1_t abd_q1q0 = __riscv_vmax_vv_i16m1(sub_q1q0, rsub_q1q0, vl);

    vbool16_t cond11 = __riscv_vmslt_vx_i16m1_b16(abd_q0p0, alpha, vl);
    vbool16_t cond12 = __riscv_vmslt_vx_i16m1_b16_mu(cond11, cond11, abd_p1p0_, beta, vl);
    vbool16_t cond13 = __riscv_vmslt_vx_i16m1_b16_mu(cond12, cond12, abd_q1q0, beta, vl);

    vint16m1_t p0_new1_i16 = __riscv_vadd_vv_i16m1(p0_i16, q1_i16, vl);
    vint16m1_t q0_new1_i16 = __riscv_vadd_vv_i16m1(q0_i16, p1_i16, vl);
    p0_new1_i16 = __riscv_vmacc_vx_i16m1(p0_new1_i16, 2, p1_i16, vl);
    q0_new1_i16 = __riscv_vmacc_vx_i16m1(q0_new1_i16, 2, q1_i16, vl);

    *p_p0_dst = __riscv_vnclipu_wx_u8mf2_mu(cond13, p0, __riscv_vreinterpret_v_i16m1_u16m1(p0_new1_i16), 2, vl);
    *p_q0_dst = __riscv_vnclipu_wx_u8mf2_mu(cond13, q0, __riscv_vreinterpret_v_i16m1_u16m1(q0_new1_i16), 2, vl);
}

__attribute__((always_inline)) static void v_loop_filter_chroma_intra(uint8_t *p_pix, ptrdiff_t stride,
                                                                      int width, int alpha, int beta)
{
    uint8_t *p_iter = p_pix;

    size_t vxrm = __builtin_rvv_vgetvxrm();
    __builtin_rvv_vsetvxrm(VE_TONEARESTUP);

    int count = width;

    while (count > 0)
    {
        int vl = __riscv_vsetvl_e8mf2(width);

        vuint8mf2_t p1 = __riscv_vle8_v_u8mf2(p_iter - 2 * stride, vl);
        vuint8mf2_t p0 = __riscv_vle8_v_u8mf2(p_iter - stride, vl);
        vuint8mf2_t q0 = __riscv_vle8_v_u8mf2(p_iter, vl);
        vuint8mf2_t q1 = __riscv_vle8_v_u8mf2(p_iter + stride, vl);

        vuint8mf2_t p0_dst, q0_dst;
        chroma_intra_core(&p0_dst, &q0_dst, p1, p0, q0, q1, alpha, beta, vl);

        __riscv_vse8_v_u8mf2(p_iter - stride, p0_dst, vl);
        __riscv_vse8_v_u8mf2(p_iter, q0_dst, vl);

        count -= vl;
        p_iter = p_iter + vl;
    }

    __builtin_rvv_vsetvxrm(vxrm);
}

__attribute__((always_inline)) static void h_loop_filter_chroma_intra(uint8_t *p_pix, ptrdiff_t stride,
                                                                      int width, int alpha, int beta)
{
    uint8_t *p_iter = p_pix;

    size_t vxrm = __builtin_rvv_vgetvxrm();
    __builtin_rvv_vsetvxrm(VE_TONEARESTUP);

    int count = width;

    while (count > 0)
    {
        int vl = __riscv_vsetvl_e8mf2(width);

        vuint8mf2_t p1, p0, q0, q1;
        __riscv_vlsseg4e8_v_u8mf2(&p1, &p0, &q0, &q1, p_iter - 2, stride, vl);

        vuint8mf2_t p0_dst, q0_dst;
        chroma_intra_core(&p0_dst, &q0_dst, p1, p0, q0, q1, alpha, beta, vl);

        __riscv_vssseg2e8_v_u8mf2(p_iter - 1, stride, p0_dst, q0_dst, vl);

        count -= vl;
        p_iter = p_iter + vl * stride;
    }

    __builtin_rvv_vsetvxrm(vxrm);
}

__attribute__((always_inline)) static void h_loop_filter_chroma_mbaff_intra(uint8_t *p_pix, ptrdiff_t stride,
                                                                            int width, int alpha, int beta)
{
    uint8_t *p_iter = p_pix;

    size_t vxrm = __builtin_rvv_vgetvxrm();
    __builtin_rvv_vsetvxrm(VE_TONEARESTUP);

    int count = width;

    while (count > 0)
    {
        int vl = __riscv_vsetvl_e8mf2(count);

        vuint8mf2_t p1, p0, q0, q1;
        __riscv_vlsseg4e8_v_u8mf2(&p1, &p0, &q0, &q1, p_iter - 2, stride, vl);

        vuint8mf2_t p0_dst, q0_dst;
        chroma_intra_core(&p0_dst, &q0_dst, p1, p0, q0, q1, alpha, beta, vl);

        __riscv_vssseg2e8_v_u8mf2(p_iter - 1, stride, p0_dst, q0_dst, vl);

        count -= vl;
        p_iter = p_iter + vl * stride;
    }

    __builtin_rvv_vsetvxrm(vxrm);
}

void h264_v_loop_filter_luma_8_rvv(uint8_t *pix, ptrdiff_t stride, int alpha, int beta, int8_t *p_tc0)
{
    v_loop_filter_luma(pix, stride, 16, alpha, beta, p_tc0);
}

void h264_h_loop_filter_luma_8_rvv(uint8_t *p_pix, ptrdiff_t stride, int alpha, int beta, int8_t *p_tc0)
{
    h_loop_filter_luma(p_pix, stride, 16, alpha, beta, p_tc0);
}

void h264_v_loop_filter_chroma_8_rvv(uint8_t *p_pix, ptrdiff_t stride, int alpha, int beta, int8_t *p_tc0)
{
    v_loop_filter_chroma(p_pix, stride, 8, alpha, beta, p_tc0);
}

void h264_h_loop_filter_chroma_8_rvv(uint8_t *p_pix, ptrdiff_t stride, int alpha, int beta, int8_t *p_tc0)
{
    h_loop_filter_chroma(p_pix, stride, 8, alpha, beta, p_tc0);
}

void h264_v_loop_filter_luma_intra_8_rvv(uint8_t *p_pix, ptrdiff_t stride, int alpha, int beta)
{
    v_loop_filter_luma_intra(p_pix, stride, 16, alpha, beta);
}

void h264_h_loop_filter_luma_intra_8_rvv(uint8_t *p_pix, ptrdiff_t stride, int alpha, int beta)
{
    h_loop_filter_luma_intra(p_pix, stride, 16, alpha, beta);
}

void h264_v_loop_filter_chroma_intra_8_rvv(uint8_t *p_pix, ptrdiff_t stride, int alpha, int beta)
{
    v_loop_filter_chroma_intra(p_pix, stride, 8, alpha, beta);
}

void h264_h_loop_filter_chroma_intra_8_rvv(uint8_t *p_pix, ptrdiff_t stride, int alpha, int beta)
{
    h_loop_filter_chroma_intra(p_pix, stride, 8, alpha, beta);
}

void h264_h_loop_filter_chroma_mbaff_intra_8_rvv(uint8_t *p_pix, ptrdiff_t stride, int alpha, int beta)
{
    h_loop_filter_chroma_mbaff_intra(p_pix, stride, 4, alpha, beta);
}
#endif
