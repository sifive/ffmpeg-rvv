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
    .text

    .globl    h264_put_chroma_mc8_rvv
    .p2align    1
    .type    h264_put_chroma_mc8_rvv,@function
h264_put_chroma_mc8_rvv:
    slliw    t2, a5, 3
    mulw    t1, a5, a4
    sh3add    a5, a4, t2
    slliw    a4, a4, 3
    subw    a5, t1, a5
    subw    a7, a4, t1
    addiw    a6, a5, 64
    subw    t0, t2, t1
    vsetivli    t3, 8, e8, m1, ta, mu
    beqz    t1, .LBB0_4
    blez    a3, .LBB0_17
    li    t4, 0
    li    t2, 0
    addi    a5, t3, 1
    slli    t3, a2, 2
.LBB0_3:                                # if (xy != 0)
    add    a4, a1, t4
    vsetvli    zero, a5, e8, m1, ta, ma
    addiw    t2, t2, 4
    vle8.v    v10, (a4)
    add    a4, a4, a2
    vslidedown.vi    v11, v10, 1
    vsetivli    zero, 8, e8, m1, ta, ma
    vwmulu.vx    v8, v10, a6
    vwmaccu.vx    v8, a7, v11
    vsetvli    zero, a5, e8, m1, ta, ma
    vle8.v    v12, (a4)
    vsetivli    zero, 8, e8, m1, ta, ma
    add    a4, a4, a2
    vwmaccu.vx    v8, t0, v12
    vsetvli    zero, a5, e8, m1, ta, ma
    vslidedown.vi    v13, v12, 1
    vsetivli    zero, 8, e8, m1, ta, ma
    vwmulu.vx    v10, v12, a6
    vwmaccu.vx    v8, t1, v13
    vwmaccu.vx    v10, a7, v13
    vsetvli    zero, a5, e8, m1, ta, ma
    vle8.v    v14, (a4)
    vsetivli    zero, 8, e8, m1, ta, ma
    add    a4, a4, a2
    vwmaccu.vx    v10, t0, v14
    vsetvli    zero, a5, e8, m1, ta, ma
    vslidedown.vi    v15, v14, 1
    vsetivli    zero, 8, e8, m1, ta, ma
    vwmulu.vx    v12, v14, a6
    vwmaccu.vx    v10, t1, v15
    vwmaccu.vx    v12, a7, v15
    vsetvli    zero, a5, e8, m1, ta, ma
    vle8.v    v14, (a4)
    vsetivli    zero, 8, e8, m1, ta, ma
    add    a4, a4, a2
    vwmaccu.vx    v12, t0, v14
    vsetvli    zero, a5, e8, m1, ta, ma
    vslidedown.vi    v15, v14, 1
    vsetivli    zero, 8, e8, m1, ta, ma
    vwmulu.vx    v16, v14, a6
    vwmaccu.vx    v12, t1, v15
    vwmaccu.vx    v16, a7, v15
    vsetvli    zero, a5, e8, m1, ta, ma
    vle8.v    v14, (a4)
    vsetivli    zero, 8, e8, m1, ta, ma
    add    a4, a0, t4
    add    t4, t4, t3
    vwmaccu.vx    v16, t0, v14
    vsetvli    zero, a5, e8, m1, ta, ma
    vslidedown.vi    v14, v14, 1
    vsetivli    zero, 8, e8, m1, ta, ma
    vnclipu.wi    v15, v8, 6
    vwmaccu.vx    v16, t1, v14
    vse8.v    v15, (a4)
    add    a4, a4, a2
    vnclipu.wi    v8, v10, 6
    vse8.v    v8, (a4)
    add    a4, a4, a2
    vnclipu.wi    v8, v12, 6
    vse8.v    v8, (a4)
    add    a4, a4, a2
    vnclipu.wi    v8, v16, 6
    vse8.v    v8, (a4)
    blt    t2, a3, .LBB0_3
    j    .LBB0_17
.LBB0_4:
    bnez    a4, .LBB0_9
    beqz    t2, .LBB0_9
    blez    a3, .LBB0_17
    li    a4, 0
    li    t1, 0
    slli    a7, a2, 2
.LBB0_8:                                # if ((x8 - xy) == 0 && (y8 -xy) != 0)
    add    a5, a1, a4
    vsetvli    zero, zero, e8, m1, ta, ma
    addiw    t1, t1, 4
    vle8.v    v8, (a5)
    add    a5, a5, a2
    add    t2, a5, a2
    vwmulu.vx    v10, v8, a6
    vle8.v    v8, (a5)
    vwmulu.vx    v12, v8, a6
    vle8.v    v9, (t2)
    add    t2, t2, a2
    add    a5, t2, a2
    vwmaccu.vx    v10, t0, v8
    vle8.v    v8, (t2)
    vle8.v    v14, (a5)
    add    a5, a0, a4
    add    a4, a4, a7
    vwmaccu.vx    v12, t0, v9
    vnclipu.wi    v15, v10, 6
    vwmulu.vx    v10, v9, a6
    vse8.v    v15, (a5)
    add    a5, a5, a2
    vnclipu.wi    v9, v12, 6
    vwmaccu.vx    v10, t0, v8
    vwmulu.vx    v12, v8, a6
    vse8.v    v9, (a5)
    add    a5, a5, a2
    vnclipu.wi    v8, v10, 6
    vwmaccu.vx    v12, t0, v14
    vse8.v    v8, (a5)
    add    a5, a5, a2
    vnclipu.wi    v8, v12, 6
    vse8.v    v8, (a5)
    blt    t1, a3, .LBB0_8
    j    .LBB0_17
.LBB0_9:
    beqz    a4, .LBB0_14
    bnez    t2, .LBB0_14
    blez    a3, .LBB0_17
    li    a4, 0
    li    t2, 0
    addi    t0, t3, 1
    slli    t1, a2, 2
.LBB0_13:                               # if ((x8 - xy) != 0 && (y8 -xy) == 0)
    add    a5, a1, a4
    vsetvli    zero, t0, e8, m1, ta, ma
    addiw    t2, t2, 4
    vle8.v    v8, (a5)
    add    a5, a5, a2
    vslidedown.vi    v9, v8, 1
    vsetivli    zero, 8, e8, m1, ta, ma
    vwmulu.vx    v10, v8, a6
    vwmaccu.vx    v10, a7, v9
    vsetvli    zero, t0, e8, m1, ta, ma
    vle8.v    v8, (a5)
    add    a5, a5, a2
    vslidedown.vi    v9, v8, 1
    vsetivli    zero, 8, e8, m1, ta, ma
    vwmulu.vx    v12, v8, a6
    vwmaccu.vx    v12, a7, v9
    vsetvli    zero, t0, e8, m1, ta, ma
    vle8.v    v8, (a5)
    add    a5, a5, a2
    vslidedown.vi    v9, v8, 1
    vsetivli    zero, 8, e8, m1, ta, ma
    vwmulu.vx    v14, v8, a6
    vwmaccu.vx    v14, a7, v9
    vsetvli    zero, t0, e8, m1, ta, ma
    vle8.v    v8, (a5)
    add    a5, a0, a4
    add    a4, a4, t1
    vslidedown.vi    v9, v8, 1
    vsetivli    zero, 8, e8, m1, ta, ma
    vnclipu.wi    v16, v10, 6
    vse8.v    v16, (a5)
    add    a5, a5, a2
    vnclipu.wi    v10, v12, 6
    vwmulu.vx    v12, v8, a6
    vse8.v    v10, (a5)
    add    a5, a5, a2
    vnclipu.wi    v8, v14, 6
    vwmaccu.vx    v12, a7, v9
    vse8.v    v8, (a5)
    add    a5, a5, a2
    vnclipu.wi    v8, v12, 6
    vse8.v    v8, (a5)
    blt    t2, a3, .LBB0_13
    j    .LBB0_17
.LBB0_14:
    blez    a3, .LBB0_17
    li    a4, 0
    li    t2, 0
    slli    a7, a2, 2
.LBB0_16:                               # the final else, none of the above conditions are met
    add    t0, a1, a4
    vsetvli    zero, zero, e8, m1, ta, ma
    add    a5, a0, a4
    add    a4, a4, a7
    addiw    t2, t2, 4
    vle8.v    v8, (t0)
    add    t0, t0, a2
    add    t1, t0, a2
    vwmulu.vx    v10, v8, a6
    vle8.v    v8, (t0)
    add    t0, t1, a2
    vle8.v    v9, (t1)
    vle8.v    v12, (t0)
    vnclipu.wi    v13, v10, 6
    vwmulu.vx    v10, v8, a6
    vse8.v    v13, (a5)
    add    a5, a5, a2
    vnclipu.wi    v8, v10, 6
    vwmulu.vx    v10, v9, a6
    vse8.v    v8, (a5)
    add    a5, a5, a2
    vnclipu.wi    v8, v10, 6
    vwmulu.vx    v10, v12, a6
    vse8.v    v8, (a5)
    add    a5, a5, a2
    vnclipu.wi    v8, v10, 6
    vse8.v    v8, (a5)
    blt    t2, a3, .LBB0_16
.LBB0_17:                               # Exit h264_put_chroma_mc8_rvv
    ret
.Lfunc_end0:
    .size    h264_put_chroma_mc8_rvv, .Lfunc_end0-h264_put_chroma_mc8_rvv

    .globl    h264_avg_chroma_mc8_rvv
    .p2align    1
    .type    h264_avg_chroma_mc8_rvv,@function
h264_avg_chroma_mc8_rvv:
    slliw    t2, a5, 3
    mulw    t1, a5, a4
    sh3add    a5, a4, t2
    slliw    a4, a4, 3
    subw    a5, t1, a5
    subw    a7, a4, t1
    addiw    a6, a5, 64
    subw    t0, t2, t1
    vsetivli    t3, 8, e8, m1, ta, mu
    beqz    t1, .LBB1_4
    blez    a3, .LBB1_17
    li    t4, 0
    li    t2, 0
    addi    a5, t3, 1
    slli    t3, a2, 2
.LBB1_3:                                # if (xy != 0)
    add    a4, a1, t4
    vsetvli    zero, a5, e8, m1, ta, ma
    addiw    t2, t2, 4
    vle8.v    v10, (a4)
    add    a4, a4, a2
    vslidedown.vi    v11, v10, 1
    vsetivli    zero, 8, e8, m1, ta, ma
    vwmulu.vx    v8, v10, a6
    vwmaccu.vx    v8, a7, v11
    vsetvli    zero, a5, e8, m1, ta, ma
    vle8.v    v12, (a4)
    vsetivli    zero, 8, e8, m1, ta, ma
    add    a4, a4, a2
    vwmaccu.vx    v8, t0, v12
    vsetvli    zero, a5, e8, m1, ta, ma
    vslidedown.vi    v13, v12, 1
    vsetivli    zero, 8, e8, m1, ta, ma
    vwmulu.vx    v10, v12, a6
    vwmaccu.vx    v8, t1, v13
    vwmaccu.vx    v10, a7, v13
    vsetvli    zero, a5, e8, m1, ta, ma
    vle8.v    v14, (a4)
    vsetivli    zero, 8, e8, m1, ta, ma
    add    a4, a4, a2
    vwmaccu.vx    v10, t0, v14
    vsetvli    zero, a5, e8, m1, ta, ma
    vslidedown.vi    v15, v14, 1
    vsetivli    zero, 8, e8, m1, ta, ma
    vwmulu.vx    v12, v14, a6
    vwmaccu.vx    v10, t1, v15
    vwmaccu.vx    v12, a7, v15
    vsetvli    zero, a5, e8, m1, ta, ma
    vle8.v    v14, (a4)
    vsetivli    zero, 8, e8, m1, ta, ma
    add    a4, a4, a2
    vwmaccu.vx    v12, t0, v14
    vsetvli    zero, a5, e8, m1, ta, ma
    vslidedown.vi    v15, v14, 1
    vsetivli    zero, 8, e8, m1, ta, ma
    vwmulu.vx    v16, v14, a6
    vwmaccu.vx    v12, t1, v15
    vwmaccu.vx    v16, a7, v15
    vsetvli    zero, a5, e8, m1, ta, ma
    vle8.v    v14, (a4)
    vsetivli    zero, 8, e8, m1, ta, ma
    add    a4, a0, t4
    add    t4, t4, t3
    vwmaccu.vx    v16, t0, v14
    vsetvli    zero, a5, e8, m1, ta, ma
    vslidedown.vi    v14, v14, 1
    vsetivli    zero, 8, e8, m1, ta, ma
    vnclipu.wi    v15, v8, 6
    vle8.v    v8, (a4)
    vwmaccu.vx    v16, t1, v14
    vaaddu.vv    v8, v15, v8
    vse8.v    v8, (a4)
    add    a4, a4, a2
    vnclipu.wi    v8, v10, 6
    vle8.v    v9, (a4)
    vaaddu.vv    v8, v8, v9
    vse8.v    v8, (a4)
    add    a4, a4, a2
    vnclipu.wi    v8, v12, 6
    vle8.v    v9, (a4)
    vaaddu.vv    v8, v8, v9
    vse8.v    v8, (a4)
    add    a4, a4, a2
    vnclipu.wi    v8, v16, 6
    vle8.v    v9, (a4)
    vaaddu.vv    v8, v8, v9
    vse8.v    v8, (a4)
    blt    t2, a3, .LBB1_3
    j    .LBB1_17
.LBB1_4:
    bnez    a4, .LBB1_9
    beqz    t2, .LBB1_9
    blez    a3, .LBB1_17
    li    t2, 0
    li    t1, 0
    slli    a7, a2, 2
.LBB1_8:                                # if ((x8 - xy) == 0 && (y8 -xy) != 0)
    add    a4, a1, t2
    vsetvli    zero, zero, e8, m1, ta, ma
    addiw    t1, t1, 4
    vle8.v    v8, (a4)
    add    a4, a4, a2
    vwmulu.vx    v10, v8, a6
    vle8.v    v8, (a4)
    add    a4, a4, a2
    add    a5, a4, a2
    vle8.v    v9, (a4)
    add    a4, a5, a2
    vle8.v    v12, (a5)
    vwmaccu.vx    v10, t0, v8
    vle8.v    v13, (a4)
    add    a4, a0, t2
    add    t2, t2, a7
    vnclipu.wi    v14, v10, 6
    vwmulu.vx    v10, v8, a6
    vle8.v    v8, (a4)
    vaaddu.vv    v8, v14, v8
    vwmaccu.vx    v10, t0, v9
    vse8.v    v8, (a4)
    add    a4, a4, a2
    vnclipu.wi    v8, v10, 6
    vwmulu.vx    v10, v9, a6
    vle8.v    v9, (a4)
    vaaddu.vv    v8, v8, v9
    vwmaccu.vx    v10, t0, v12
    vse8.v    v8, (a4)
    add    a4, a4, a2
    vnclipu.wi    v8, v10, 6
    vwmulu.vx    v10, v12, a6
    vle8.v    v9, (a4)
    vaaddu.vv    v8, v8, v9
    vwmaccu.vx    v10, t0, v13
    vse8.v    v8, (a4)
    add    a4, a4, a2
    vnclipu.wi    v8, v10, 6
    vle8.v    v9, (a4)
    vaaddu.vv    v8, v8, v9
    vse8.v    v8, (a4)
    blt    t1, a3, .LBB1_8
    j    .LBB1_17
.LBB1_9:
    beqz    a4, .LBB1_14
    bnez    t2, .LBB1_14
    blez    a3, .LBB1_17
    li    a5, 0
    li    t2, 0
    addi    t0, t3, 1
    slli    t1, a2, 2
.LBB1_13:                               # if ((x8 - xy) != 0 && (y8 -xy) == 0)
    add    a4, a1, a5
    vsetvli    zero, t0, e8, m1, ta, ma
    addiw    t2, t2, 4
    vle8.v    v8, (a4)
    add    a4, a4, a2
    vslidedown.vi    v9, v8, 1
    vsetivli    zero, 8, e8, m1, ta, ma
    vwmulu.vx    v10, v8, a6
    vwmaccu.vx    v10, a7, v9
    vsetvli    zero, t0, e8, m1, ta, ma
    vle8.v    v8, (a4)
    add    a4, a4, a2
    vslidedown.vi    v9, v8, 1
    vsetivli    zero, 8, e8, m1, ta, ma
    vwmulu.vx    v12, v8, a6
    vwmaccu.vx    v12, a7, v9
    vsetvli    zero, t0, e8, m1, ta, ma
    vle8.v    v8, (a4)
    add    a4, a4, a2
    vslidedown.vi    v9, v8, 1
    vsetivli    zero, 8, e8, m1, ta, ma
    vwmulu.vx    v14, v8, a6
    vwmaccu.vx    v14, a7, v9
    vsetvli    zero, t0, e8, m1, ta, ma
    vle8.v    v8, (a4)
    add    a4, a0, a5
    add    a5, a5, t1
    vslidedown.vi    v9, v8, 1
    vsetivli    zero, 8, e8, m1, ta, ma
    vnclipu.wi    v16, v10, 6
    vle8.v    v10, (a4)
    vaaddu.vv    v10, v16, v10
    vse8.v    v10, (a4)
    add    a4, a4, a2
    vnclipu.wi    v10, v12, 6
    vle8.v    v11, (a4)
    vwmulu.vx    v12, v8, a6
    vaaddu.vv    v10, v10, v11
    vwmaccu.vx    v12, a7, v9
    vse8.v    v10, (a4)
    add    a4, a4, a2
    vnclipu.wi    v10, v14, 6
    vle8.v    v8, (a4)
    vaaddu.vv    v8, v10, v8
    vse8.v    v8, (a4)
    add    a4, a4, a2
    vnclipu.wi    v8, v12, 6
    vle8.v    v9, (a4)
    vaaddu.vv    v8, v8, v9
    vse8.v    v8, (a4)
    blt    t2, a3, .LBB1_13
    j    .LBB1_17
.LBB1_14:
    blez    a3, .LBB1_17
    li    a4, 0
    li    t0, 0
    slli    a7, a2, 2
.LBB1_16:                               # the final else, none of the above conditions are met
    add    a5, a1, a4
    vsetvli    zero, zero, e8, m1, ta, ma
    addiw    t0, t0, 4
    vle8.v    v8, (a5)
    add    a5, a5, a2
    add    t1, a5, a2
    vwmulu.vx    v10, v8, a6
    vle8.v    v8, (a5)
    add    a5, t1, a2
    vle8.v    v9, (t1)
    vle8.v    v12, (a5)
    add    a5, a0, a4
    add    a4, a4, a7
    vnclipu.wi    v13, v10, 6
    vle8.v    v10, (a5)
    vwmulu.vx    v14, v8, a6
    vaaddu.vv    v10, v13, v10
    vse8.v    v10, (a5)
    add    a5, a5, a2
    vnclipu.wi    v8, v14, 6
    vle8.v    v10, (a5)
    vaaddu.vv    v8, v8, v10
    vwmulu.vx    v10, v9, a6
    vse8.v    v8, (a5)
    add    a5, a5, a2
    vnclipu.wi    v8, v10, 6
    vle8.v    v9, (a5)
    vwmulu.vx    v10, v12, a6
    vaaddu.vv    v8, v8, v9
    vse8.v    v8, (a5)
    add    a5, a5, a2
    vnclipu.wi    v8, v10, 6
    vle8.v    v9, (a5)
    vaaddu.vv    v8, v8, v9
    vse8.v    v8, (a5)
    blt    t0, a3, .LBB1_16
.LBB1_17:                               # Exit h264_avg_chroma_mc8_rvv
    ret
.Lfunc_end1:
    .size    h264_avg_chroma_mc8_rvv, .Lfunc_end1-h264_avg_chroma_mc8_rvv
