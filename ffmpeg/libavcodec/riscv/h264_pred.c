#include "h264_pred.h"

#if HAVE_RVV
#include <riscv_vector.h>
void pred8x8_vert_8_rvv(uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t *p_src_iter = p_src;
    int width = 8;

    while (width > 0)
    {
        int vl = __riscv_vsetvl_e8m1(width);
        uint8_t *p_src_iter_next = p_src_iter + vl;

        vuint8m1_t top = __riscv_vle8_v_u8m1(p_src_iter - stride, vl);

        __riscv_vse8_v_u8m1(p_src_iter, top, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, top, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, top, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, top, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, top, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, top, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, top, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, top, vl);
        p_src_iter += stride;

        width -= vl;
        p_src_iter = p_src_iter_next;
    }
}

void pred8x8_hor_8_rvv(uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t *p_src_iter = p_src;
    int width = 8;

    while (width > 0)
    {
        int vl = __riscv_vsetvl_e8m1(width);
        vuint8m1_t left = __riscv_vlse8_v_u8m1(p_src_iter - 1, stride, width);

        __riscv_vssseg8e8_v_u8m1(p_src_iter, stride, left, left, left, left, left, left, left, left, width);

        width -= vl;
        p_src_iter = p_src_iter + vl * stride;
    }
}

void pred8x8_plane_8_rvv(uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t *p_src_iter = p_src;
    int vl = __riscv_vsetvl_e8mf2(4);

    const uint8_t index_data[] = {3, 2, 1, 0};
    const int16_t weight1_data[] = {1, 2, 3, 4};
    const int16_t weight2_data[] = {0, 1, 2, 3, 4, 5, 6, 7};

    vuint8mf2_t index = __riscv_vle8_v_u8mf2(index_data, vl);

    vuint8mf2_t h_half2 = __riscv_vle8_v_u8mf2(p_src - stride + 4, vl);
    vuint8mf2_t h_half1 = __riscv_vle8_v_u8mf2(p_src - stride - 1, vl);
    h_half1 = __riscv_vrgather_vv_u8mf2(h_half1, index, vl);

    vuint8mf2_t v_half2 = __riscv_vlse8_v_u8mf2(p_src - 1 + 4 * stride, stride, vl);
    vuint8mf2_t v_half1 = __riscv_vlse8_v_u8mf2(p_src - 1 - stride, stride, vl);
    v_half1 = __riscv_vrgather_vv_u8mf2(v_half1, index, vl);

    vint16m1_t h_half2_w = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwaddu_vx_u16m1(h_half2, 0, vl));
    vint16m1_t h_half1_w = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwaddu_vx_u16m1(h_half1, 0, vl));

    vint16m1_t v_half2_w = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwaddu_vx_u16m1(v_half2, 0, vl));
    vint16m1_t v_half1_w = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwaddu_vx_u16m1(v_half1, 0, vl));

    // calculate H
    vint16m1_t h = __riscv_vsub_vv_i16m1(h_half2_w, h_half1_w, vl);
    vint16m1_t weight1 = __riscv_vle16_v_i16m1(weight1_data, vl);
    h = __riscv_vmul_vv_i16m1(h, weight1, vl);

    // calculate V
    vint16m1_t v = __riscv_vsub_vv_i16m1(v_half2_w, v_half1_w, vl);
    v = __riscv_vmul_vv_i16m1(v, weight1, vl);

    vint32m1_t v_sum = __riscv_vand_vx_i32m1(v_sum, 0, vl);
    vint32m1_t h_sum = __riscv_vand_vx_i32m1(h_sum, 0, vl);
    v_sum = __riscv_vwredsum_vs_i16m1_i32m1(v, v_sum, vl);
    h_sum = __riscv_vwredsum_vs_i16m1_i32m1(h, h_sum, vl);

    int32_t h_sum_scalar = __riscv_vmv_x_s_i32m1_i32(h_sum);
    h_sum_scalar = (17 * h_sum_scalar + 16) >> 5;
    int32_t v_sum_scalar = __riscv_vmv_x_s_i32m1_i32(v_sum);
    v_sum_scalar = (17 * v_sum_scalar + 16) >> 5;

    // linear combination of H, V, and src
    int32_t a = ((p_src[7 * stride - 1] + p_src[-stride + 7] + 1) << 4) - (3 * (v_sum_scalar + h_sum_scalar));

    size_t vxrm = __builtin_rvv_vgetvxrm();
    __builtin_rvv_vsetvxrm(VE_DOWNWARD);

    vint16m1_t weight2 = __riscv_vle16_v_i16m1(weight2_data, 8);
    vint16m1_t h_weighted = __riscv_vmv_v_x_i16m1(h_sum_scalar, 8);
    h_weighted = __riscv_vmul_vv_i16m1(h_weighted, weight2, 8);

    vint16m1_t result1 = __riscv_vadd_vx_i16m1(h_weighted, a, 8);
    result1 = __riscv_vmax_vx_i16m1(result1, 0, 8);
    a += v_sum_scalar;

    vint16m1_t result2 = __riscv_vadd_vx_i16m1(h_weighted, a, 8);
    result2 = __riscv_vmax_vx_i16m1(result2, 0, 8);
    a += v_sum_scalar;

    vint16m1_t result3 = __riscv_vadd_vx_i16m1(h_weighted, a, 8);
    result3 = __riscv_vmax_vx_i16m1(result3, 0, 8);
    a += v_sum_scalar;

    vint16m1_t result4 = __riscv_vadd_vx_i16m1(h_weighted, a, 8);
    result4 = __riscv_vmax_vx_i16m1(result4, 0, 8);
    a += v_sum_scalar;

    vint16m1_t result5 = __riscv_vadd_vx_i16m1(h_weighted, a, 8);
    result5 = __riscv_vmax_vx_i16m1(result5, 0, 8);
    a += v_sum_scalar;

    vint16m1_t result6 = __riscv_vadd_vx_i16m1(h_weighted, a, 8);
    result6 = __riscv_vmax_vx_i16m1(result6, 0, 8);
    a += v_sum_scalar;

    vint16m1_t result7 = __riscv_vadd_vx_i16m1(h_weighted, a, 8);
    result7 = __riscv_vmax_vx_i16m1(result7, 0, 8);
    a += v_sum_scalar;

    vint16m1_t result8 = __riscv_vadd_vx_i16m1(h_weighted, a, 8);
    result8 = __riscv_vmax_vx_i16m1(result8, 0, 8);
    a += v_sum_scalar;

    vuint8mf2_t result1_n = __riscv_vnclipu_wx_u8mf2(__riscv_vreinterpret_v_i16m1_u16m1(result1), 5, 8);
    vuint8mf2_t result2_n = __riscv_vnclipu_wx_u8mf2(__riscv_vreinterpret_v_i16m1_u16m1(result2), 5, 8);
    vuint8mf2_t result3_n = __riscv_vnclipu_wx_u8mf2(__riscv_vreinterpret_v_i16m1_u16m1(result3), 5, 8);
    vuint8mf2_t result4_n = __riscv_vnclipu_wx_u8mf2(__riscv_vreinterpret_v_i16m1_u16m1(result4), 5, 8);
    vuint8mf2_t result5_n = __riscv_vnclipu_wx_u8mf2(__riscv_vreinterpret_v_i16m1_u16m1(result5), 5, 8);
    vuint8mf2_t result6_n = __riscv_vnclipu_wx_u8mf2(__riscv_vreinterpret_v_i16m1_u16m1(result6), 5, 8);
    vuint8mf2_t result7_n = __riscv_vnclipu_wx_u8mf2(__riscv_vreinterpret_v_i16m1_u16m1(result7), 5, 8);
    vuint8mf2_t result8_n = __riscv_vnclipu_wx_u8mf2(__riscv_vreinterpret_v_i16m1_u16m1(result8), 5, 8);

    __riscv_vse8_v_u8mf2(p_src_iter, result1_n, 8);
    p_src_iter += stride;
    __riscv_vse8_v_u8mf2(p_src_iter, result2_n, 8);
    p_src_iter += stride;
    __riscv_vse8_v_u8mf2(p_src_iter, result3_n, 8);
    p_src_iter += stride;
    __riscv_vse8_v_u8mf2(p_src_iter, result4_n, 8);
    p_src_iter += stride;
    __riscv_vse8_v_u8mf2(p_src_iter, result5_n, 8);
    p_src_iter += stride;
    __riscv_vse8_v_u8mf2(p_src_iter, result6_n, 8);
    p_src_iter += stride;
    __riscv_vse8_v_u8mf2(p_src_iter, result7_n, 8);
    p_src_iter += stride;
    __riscv_vse8_v_u8mf2(p_src_iter, result8_n, 8);
    p_src_iter += stride;

    __builtin_rvv_vsetvxrm(vxrm);
}

void pred8x8_128_dc_8_rvv(uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t *p_src_iter = p_src;
    int width = 8;

    while (width > 0)
    {
        int vl = __riscv_vsetvl_e8m1(width);

        vuint8m1_t dc = __riscv_vmv_v_x_u8m1(128, vl);

        __riscv_vse8_v_u8m1(p_src_iter, dc, vl);
        __riscv_vse8_v_u8m1(p_src_iter + stride, dc, vl);
        __riscv_vse8_v_u8m1(p_src_iter + stride * 2, dc, vl);
        __riscv_vse8_v_u8m1(p_src_iter + stride * 3, dc, vl);
        __riscv_vse8_v_u8m1(p_src_iter + stride * 4, dc, vl);
        __riscv_vse8_v_u8m1(p_src_iter + stride * 5, dc, vl);
        __riscv_vse8_v_u8m1(p_src_iter + stride * 6, dc, vl);
        __riscv_vse8_v_u8m1(p_src_iter + stride * 7, dc, vl);

        width -= vl;
        p_src_iter = p_src_iter + vl;
    }
}

void pred8x8_top_dc_rvv(uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t *p_src_iter = p_src;

    const uint8_t index_data[] = {0, 0, 0, 0, 1, 1, 1, 1};

    vuint8m1_t top0, top1, top2, top3;
    __riscv_vlseg4e8_v_u8m1(&top0, &top1, &top2, &top3, p_src - stride, 2);

    vuint16m2_t sum1 = __riscv_vwaddu_vv_u16m2(top0, top1, 2);
    vuint16m2_t sum2 = __riscv_vwaddu_vv_u16m2(top2, top3, 2);
    vuint16m2_t sum = __riscv_vadd_vv_u16m2(sum1, sum2, 2);

    vuint8m1_t dc01 = __riscv_vnclipu_wx_u8m1(sum, 2, 2);

    vuint8m1_t index = __riscv_vle8_v_u8m1(index_data, 8);
    dc01 = __riscv_vrgather_vv_u8m1(dc01, index, 8);

    __riscv_vse8_v_u8m1(p_src_iter, dc01, 8);
    __riscv_vse8_v_u8m1(p_src_iter + stride, dc01, 8);
    __riscv_vse8_v_u8m1(p_src_iter + stride * 2, dc01, 8);
    __riscv_vse8_v_u8m1(p_src_iter + stride * 3, dc01, 8);
    __riscv_vse8_v_u8m1(p_src_iter + stride * 4, dc01, 8);
    __riscv_vse8_v_u8m1(p_src_iter + stride * 5, dc01, 8);
    __riscv_vse8_v_u8m1(p_src_iter + stride * 6, dc01, 8);
    __riscv_vse8_v_u8m1(p_src_iter + stride * 7, dc01, 8);
}

void pred8x8_left_dc_rvv(uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t *p_src_iter = p_src;
 
    int dc0_data = (p_src[-1] + p_src[-1 + stride] + p_src[-1 + 2 * stride] + p_src[-1 + 3 * stride] + 2) >> 2;
    int dc2_data = (p_src[-1 + 4 * stride] + p_src[-1 + 5 * stride] + p_src[-1 + 6 * stride] + p_src[-1 + 7 * stride] + 2) >> 2;

    vuint8m1_t dc0 = __riscv_vmv_v_x_u8m1(dc0_data, 8);
    vuint8m1_t dc2 = __riscv_vmv_v_x_u8m1(dc2_data, 8);

    __riscv_vse8_v_u8m1(p_src_iter, dc0, 8);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc0, 8);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc0, 8);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc0, 8);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc2, 8);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc2, 8);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc2, 8);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc2, 8);
}

void pred8x8_dc_rvv(uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t *p_src_iter = p_src;
    uint8_t *p_top = p_src - stride;
    uint8_t *p_left = p_src - 1;

    uint16_t dc0 = p_top[0] + p_top[1] + p_top[2] + p_top[3];
    uint16_t dc1 = p_top[4] + p_top[5] + p_top[6] + p_top[7];

    dc0 += (p_left[0] + p_left[stride] + p_left[stride * 2] + p_left[stride * 3]);
    uint16_t dc2 = p_left[stride * 4] + p_left[stride * 5] + p_left[stride * 6] + p_left[stride * 7];

    dc0 = (dc0 + 4) >> 3;
    uint16_t dc3 = (dc1 + dc2 + 4) >> 3;
    dc1 = (dc1 + 2) >> 2;
    dc2 = (dc2 + 2) >> 2;

    uint8_t weight_data[] = {0, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF};
    vuint8m1_t weight = __riscv_vle8_v_u8m1(weight_data, 8);
    vuint8m1_t weight2 = __riscv_vxor_vx_u8m1(weight, 0xFF, 8);

    vuint8m1_t dc1_splat = __riscv_vmv_v_x_u8m1(dc1, 8);
    vuint8m1_t dc3_splat = __riscv_vmv_v_x_u8m1(dc3, 8);

    vuint8m1_t dc0_splat = __riscv_vmv_v_x_u8m1(dc0, 8);    
    vuint8m1_t dc2_splat = __riscv_vmv_v_x_u8m1(dc2, 8);
    
    dc0_splat = __riscv_vand_vv_u8m1(dc0_splat, weight2, 8);
    dc1_splat = __riscv_vand_vv_u8m1(dc1_splat, weight, 8);
    vuint8m1_t dc01_splat = __riscv_vor_vv_u8m1(dc0_splat, dc1_splat, 8);

    dc2_splat = __riscv_vand_vv_u8m1(dc2_splat, weight2, 8);
    dc3_splat = __riscv_vand_vv_u8m1(dc3_splat, weight, 8);
    vuint8m1_t dc23_splat = __riscv_vor_vv_u8m1(dc2_splat, dc3_splat, 8);

    __riscv_vse8_v_u8m1(p_src_iter, dc01_splat, 8);
    p_src_iter += stride;

    __riscv_vse8_v_u8m1(p_src_iter, dc01_splat, 8);
    p_src_iter += stride;

    __riscv_vse8_v_u8m1(p_src_iter, dc01_splat, 8);
    p_src_iter += stride;

    __riscv_vse8_v_u8m1(p_src_iter, dc01_splat, 8);
    p_src_iter += stride;
        
    __riscv_vse8_v_u8m1(p_src_iter, dc23_splat, 8);
    p_src_iter += stride;

    __riscv_vse8_v_u8m1(p_src_iter, dc23_splat, 8);
    p_src_iter += stride;

    __riscv_vse8_v_u8m1(p_src_iter, dc23_splat, 8);
    p_src_iter += stride;

    __riscv_vse8_v_u8m1(p_src_iter, dc23_splat, 8);
}

void pred8x8_l0t_dc_rvv(uint8_t *p_src, ptrdiff_t stride)
{
    const uint16_t mask_data[] = {0xFFFF, 0, 0, 0, 0, 0, 0, 0};
    const uint8_t index_data[] = {0, 0, 0, 0, 4, 4, 4, 4};
    const uint8_t shift_data[] = {3, 3, 3, 3, 2, 2, 2, 2};

    uint8_t *p_src_iter = p_src;
    uint8_t *p_left = p_src - 1;
    uint8_t *p_top = p_src - stride;

    uint16_t left_sum = p_left[0] + p_left[stride] + p_left[stride << 1] + p_left[(stride << 1) + stride];

    vuint8m1_t top = __riscv_vle8_v_u8m1(p_top, 8);

    vuint8m1_t top_shift1 = __riscv_vslidedown_vx_u8m1(top, 1, 8);
    vuint16m2_t dc01 = __riscv_vwaddu_vv_u16m2(top, top_shift1, 8);
    vuint16m2_t top_shift2 = __riscv_vslidedown_vx_u16m2(dc01, 2, 8);
    dc01 = __riscv_vadd_vv_u16m2(dc01, top_shift2, 8);

    vuint16m2_t mask = __riscv_vle16_v_u16m2(mask_data, 8);  
    vuint16m2_t dc021 = __riscv_vmv_v_x_u16m2(left_sum, 8);
    dc021 = __riscv_vand_vv_u16m2(dc021, mask, 8);
    dc021 = __riscv_vadd_vv_u16m2(dc021, dc01 , 8);

    vuint8m1_t shift = __riscv_vle8_v_u8m1(shift_data, 8);
    vuint8m1_t dc01_splat = __riscv_vnclipu_wx_u8m1(dc01, 2, 8);
    vuint8m1_t dc021_splat = __riscv_vnclipu_wv_u8m1(dc021, shift, 8);

    vuint8m1_t index = __riscv_vle8_v_u8m1(index_data, 8);
    dc01_splat = __riscv_vrgather_vv_u8m1(dc01_splat, index, 8);
    dc021_splat = __riscv_vrgather_vv_u8m1(dc021_splat, index, 8);

    __riscv_vse8_v_u8m1(p_src_iter, dc021_splat, 8);
    p_src_iter += stride;

    __riscv_vse8_v_u8m1(p_src_iter, dc021_splat, 8);
    p_src_iter += stride;

    __riscv_vse8_v_u8m1(p_src_iter, dc021_splat, 8);
    p_src_iter += stride;

    __riscv_vse8_v_u8m1(p_src_iter, dc021_splat, 8);
    p_src_iter += stride;
        
    __riscv_vse8_v_u8m1(p_src_iter, dc01_splat, 8);
    p_src_iter += stride;

    __riscv_vse8_v_u8m1(p_src_iter, dc01_splat, 8);
    p_src_iter += stride;

    __riscv_vse8_v_u8m1(p_src_iter, dc01_splat, 8);
    p_src_iter += stride;

    __riscv_vse8_v_u8m1(p_src_iter, dc01_splat, 8);
}

void pred8x8_0lt_dc_rvv(uint8_t *p_src, ptrdiff_t stride)
{
    const uint16_t mask_data[] = {0, 0, 0, 0, 0xFFFF, 0, 0, 0};
    const uint8_t index_data[] = {0, 0, 0, 0, 4, 4, 4, 4};
    const uint8_t shift_data[] = {2, 2, 2, 2, 3, 3, 3, 3};

    uint8_t *p_src_iter = p_src;
    uint8_t *p_left = p_src - 1 + (stride << 2);
    uint8_t *p_top = p_src - stride;

    uint16_t left2_sum = p_left[0] + p_left[stride] + p_left[stride << 1] + p_left[(stride << 1) + stride];

    vuint8m1_t top = __riscv_vle8_v_u8m1(p_top, 8);

    vuint8m1_t top_shift1 = __riscv_vslidedown_vx_u8m1(top, 1, 8);
    vuint16m2_t top_sum = __riscv_vwaddu_vv_u16m2(top, top_shift1, 8);
    vuint16m2_t top_shift2 = __riscv_vslidedown_vx_u16m2(top_sum, 2, 8);
    top_sum = __riscv_vadd_vv_u16m2(top_sum, top_shift2, 8);

    vuint16m2_t mask = __riscv_vle16_v_u16m2(mask_data, 8);  

    vuint16m2_t dc23_sum = __riscv_vand_vv_u16m2(top_sum, mask, 8);
    dc23_sum = __riscv_vadd_vx_u16m2(dc23_sum, left2_sum , 8);

    vuint8m1_t shift = __riscv_vle8_v_u8m1(shift_data, 8);
    vuint8m1_t dc01 = __riscv_vnclipu_wx_u8m1(top_sum, 2, 8);
    vuint8m1_t dc23 = __riscv_vnclipu_wv_u8m1(dc23_sum, shift, 8);

    vuint8m1_t index = __riscv_vle8_v_u8m1(index_data, 8);
    dc01 = __riscv_vrgather_vv_u8m1(dc01, index, 8);
    dc23 = __riscv_vrgather_vv_u8m1(dc23, index, 8);

    __riscv_vse8_v_u8m1(p_src_iter, dc01, 8);
    p_src_iter += stride;

    __riscv_vse8_v_u8m1(p_src_iter, dc01, 8);
    p_src_iter += stride;

    __riscv_vse8_v_u8m1(p_src_iter, dc01, 8);
    p_src_iter += stride;

    __riscv_vse8_v_u8m1(p_src_iter, dc01, 8);
    p_src_iter += stride;
        
    __riscv_vse8_v_u8m1(p_src_iter, dc23, 8);
    p_src_iter += stride;

    __riscv_vse8_v_u8m1(p_src_iter, dc23, 8);
    p_src_iter += stride;

    __riscv_vse8_v_u8m1(p_src_iter, dc23, 8);
    p_src_iter += stride;

    __riscv_vse8_v_u8m1(p_src_iter, dc23, 8);
}

void pred8x8_l00_dc_rvv(uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t *p_src_iter = p_src;
    uint8_t *p_left = p_src - 1;

    uint16_t left_sum = p_left[0] + p_left[stride] + p_left[stride << 1] + p_left[(stride << 1) + stride];

    vuint8m1_t dc0 = __riscv_vmv_v_x_u8m1((left_sum + 2) >> 2, 8);
    vuint8m1_t dc128 = __riscv_vmv_v_x_u8m1(128, 8);

    __riscv_vse8_v_u8m1(p_src_iter, dc0, 8);
    p_src_iter += stride;

    __riscv_vse8_v_u8m1(p_src_iter, dc0, 8);
    p_src_iter += stride;

    __riscv_vse8_v_u8m1(p_src_iter, dc0, 8);
    p_src_iter += stride;

    __riscv_vse8_v_u8m1(p_src_iter, dc0, 8);
    p_src_iter += stride;
        
    __riscv_vse8_v_u8m1(p_src_iter, dc128, 8);
    p_src_iter += stride;

    __riscv_vse8_v_u8m1(p_src_iter, dc128, 8);
    p_src_iter += stride;

    __riscv_vse8_v_u8m1(p_src_iter, dc128, 8);
    p_src_iter += stride;

    __riscv_vse8_v_u8m1(p_src_iter, dc128, 8);
}

void pred8x8_0l0_dc_rvv(uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t *p_src_iter = p_src;
    uint8_t *p_left2 = p_src - 1 + (stride << 2);

    uint16_t left_sum = p_left2[0] + p_left2[stride] + p_left2[stride << 1] + p_left2[(stride << 1) + stride];

    vuint8m1_t dc2 = __riscv_vmv_v_x_u8m1((left_sum + 2) >> 2, 8);
    vuint8m1_t dc128 = __riscv_vmv_v_x_u8m1(128, 8);

    __riscv_vse8_v_u8m1(p_src_iter, dc128, 8);
    p_src_iter += stride;

    __riscv_vse8_v_u8m1(p_src_iter, dc128, 8);
    p_src_iter += stride;

    __riscv_vse8_v_u8m1(p_src_iter, dc128, 8);
    p_src_iter += stride;

    __riscv_vse8_v_u8m1(p_src_iter, dc128, 8);
    p_src_iter += stride;
        
    __riscv_vse8_v_u8m1(p_src_iter, dc2, 8);
    p_src_iter += stride;

    __riscv_vse8_v_u8m1(p_src_iter, dc2, 8);
    p_src_iter += stride;

    __riscv_vse8_v_u8m1(p_src_iter, dc2, 8);
    p_src_iter += stride;

    __riscv_vse8_v_u8m1(p_src_iter, dc2, 8);
}

void pred16x16_dc_8_rvv(uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t *p_src_iter = p_src;

    __builtin_rvv_vsetvxrm(VE_TONEARESTUP);
    vuint8m1_t left = __riscv_vlse8_v_u8m1(p_src_iter - 1, stride, 16);
    vuint8m1_t top = __riscv_vle8_v_u8m1(p_src_iter - stride, 16);

    vuint16m1_t sum = __riscv_vand_vx_u16m1(sum, 0, 8);

    sum = __riscv_vwredsumu_vs_u8m1_u16m1(left, sum, 16);
    sum = __riscv_vwredsumu_vs_u8m1_u16m1(top, sum, 16);

    vuint16m1_t sum_n = __riscv_vssrl_vx_u16m1(sum, 5, 8);
    vuint8m1_t dc_splat = __riscv_vrgather_vx_u8m1(__riscv_vreinterpret_v_u16m1_u8m1(sum_n), 0, 16);

    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);    
}

void pred16x16_left_dc_8_rvv(uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t *p_src_iter = p_src;

    __builtin_rvv_vsetvxrm(VE_TONEARESTUP);
    vuint8m1_t left = __riscv_vlse8_v_u8m1(p_src_iter - 1, stride, 16);

    vuint16m1_t sum = __riscv_vand_vx_u16m1(sum, 0, 16);
    sum = __riscv_vwredsumu_vs_u8m1_u16m1(left, sum, 16);

    vuint16m1_t dc = __riscv_vssrl_vx_u16m1(sum, 4, 8);
    vuint8m1_t dc_splat = __riscv_vrgather_vx_u8m1(__riscv_vreinterpret_v_u16m1_u8m1(dc), 0, 16);

    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
}

void pred16x16_top_dc_8_rvv(uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t *p_src_iter = p_src;
    __builtin_rvv_vsetvxrm(VE_TONEARESTUP);
    vuint8m1_t top = __riscv_vle8_v_u8m1(p_src_iter - stride, 16);

    vuint16m1_t sum = __riscv_vand_vx_u16m1(sum, 0, 16);
    sum = __riscv_vwredsumu_vs_u8m1_u16m1(top, sum, 16);

    vuint16m1_t dc = __riscv_vssrl_vx_u16m1(sum, 4, 8);
    vuint8m1_t dc_splat = __riscv_vrgather_vx_u8m1(__riscv_vreinterpret_v_u16m1_u8m1(dc), 0, 16);

    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
    p_src_iter += stride;
    __riscv_vse8_v_u8m1(p_src_iter, dc_splat, 16);
}

void pred16x16_128_dc_8_rvv(uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t *p_src_iter = p_src;
    int width = 16;

    while (width > 0)
    {
        int vl = __riscv_vsetvl_e8m1(width);
        uint8_t *p_src_iter_next = p_src + vl;

        vuint8m1_t dc = __riscv_vmv_v_x_u8m1(128, vl);

        __riscv_vse8_v_u8m1(p_src_iter, dc, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, dc, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, dc, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, dc, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, dc, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, dc, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, dc, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, dc, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, dc, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, dc, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, dc, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, dc, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, dc, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, dc, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, dc, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, dc, vl);

        width -= vl;
        p_src_iter = p_src_iter_next;
    }
}

void pred16x16_vert_8_rvv(uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t *p_src_iter = p_src;
    int width = 16;

    while (width > 0)
    {
        int vl = __riscv_vsetvl_e8m1(width);
        uint8_t *p_src_iter_next = p_src + vl;

        vuint8m1_t top = __riscv_vle8_v_u8m1(p_src_iter - stride, vl);

        __riscv_vse8_v_u8m1(p_src_iter, top, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, top, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, top, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, top, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, top, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, top, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, top, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, top, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, top, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, top, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, top, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, top, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, top, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, top, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, top, vl);
        p_src_iter += stride;
        __riscv_vse8_v_u8m1(p_src_iter, top, vl);

        width -= vl;
        p_src_iter = p_src_iter_next;
    }
}

void pred16x16_hor_8_rvv(uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t *p_src_iter = p_src;
    int width = 16;

    while (width > 0)
    {
        int vl = __riscv_vsetvl_e8m1(width);
        vuint8m1_t left = __riscv_vlse8_v_u8m1(p_src_iter - 1, stride, width);

        __riscv_vssseg8e8_v_u8m1(p_src_iter, stride, left, left, left, left, left, left, left, left, width);
        __riscv_vssseg8e8_v_u8m1(p_src_iter + 8, stride, left, left, left, left, left, left, left, left, width);

        width -= vl;
        p_src_iter = p_src_iter + vl * stride;
    }
}

void pred16x16_plane_8_rvv(uint8_t *p_src, ptrdiff_t stride)
{
    int i = 0;
    uint8_t *p_src_iter = p_src;
    int vl = __riscv_vsetvl_e8mf2(8);

    const uint8_t index_data[] = {7, 6, 5, 4, 3, 2, 1, 0};
    const int16_t weight2_data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

    vuint8mf2_t index = __riscv_vle8_v_u8mf2(index_data, vl);
    vuint16m1_t index_w = __riscv_vwaddu_vx_u16m1(index, 0, vl);

    vuint8mf2_t h_half2 = __riscv_vle8_v_u8mf2(p_src - stride + 8, vl);
    vuint8mf2_t h_half1 = __riscv_vle8_v_u8mf2(p_src - stride - 1, vl);
    h_half1 = __riscv_vrgather_vv_u8mf2(h_half1, index, vl);

    vuint8mf2_t v_half2 = __riscv_vlse8_v_u8mf2(p_src - 1 + 8 * stride, stride, vl);
    vuint8mf2_t v_half1 = __riscv_vlse8_v_u8mf2(p_src - 1 - stride, stride, vl);
    v_half1 = __riscv_vrgather_vv_u8mf2(v_half1, index, vl);

    vint16m1_t h_half2_w = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwaddu_vx_u16m1(h_half2, 0, vl));
    vint16m1_t h_half1_w = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwaddu_vx_u16m1(h_half1, 0, vl));

    vint16m1_t v_half2_w = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwaddu_vx_u16m1(v_half2, 0, vl));
    vint16m1_t v_half1_w = __riscv_vreinterpret_v_u16m1_i16m1(__riscv_vwaddu_vx_u16m1(v_half1, 0, vl));

    // calculate H
    vint16m1_t h = __riscv_vsub_vv_i16m1(h_half2_w, h_half1_w, vl);
    vint16m1_t weight = __riscv_vrsub_vx_i16m1(__riscv_vreinterpret_v_u16m1_i16m1(index_w), 8, vl);
    h = __riscv_vmul_vv_i16m1(h, weight, vl);

    // calculate V
    vint16m1_t v = __riscv_vsub_vv_i16m1(v_half2_w, v_half1_w, vl);
    v = __riscv_vmul_vv_i16m1(v, weight, vl);

    vint32m1_t v_sum = __riscv_vand_vx_i32m1(v_sum, 0, vl);
    vint32m1_t h_sum = __riscv_vand_vx_i32m1(h_sum, 0, vl);
    v_sum = __riscv_vwredsum_vs_i16m1_i32m1(v, v_sum, vl);
    h_sum = __riscv_vwredsum_vs_i16m1_i32m1(h, h_sum, vl);

    int32_t h_sum_scalar = __riscv_vmv_x_s_i32m1_i32(h_sum);
    h_sum_scalar = (5 * h_sum_scalar + 32) >> 6;
    int32_t v_sum_scalar = __riscv_vmv_x_s_i32m1_i32(v_sum);
    v_sum_scalar = (5 * v_sum_scalar + 32) >> 6;

    // linear combination of H, V, and src
    int32_t a = ((p_src[15 * stride - 1] + p_src[-stride + 15] + 1) << 4) - (7 * (v_sum_scalar + h_sum_scalar));

    size_t vxrm = __builtin_rvv_vgetvxrm();
    __builtin_rvv_vsetvxrm(VE_DOWNWARD);

    vint16m1_t weight2 = __riscv_vle16_v_i16m1(weight2_data, 16);
    vint16m1_t h_weighted = __riscv_vmv_v_x_i16m1(h_sum_scalar, 16);
    h_weighted = __riscv_vmul_vv_i16m1(h_weighted, weight2, 16);

    for (i = 0; i < 16; i += 8)
    {
        vint16m1_t result1 = __riscv_vadd_vx_i16m1(h_weighted, a, 16);
        result1 = __riscv_vmax_vx_i16m1(result1, 0, 16);
        a += v_sum_scalar;

        vint16m1_t result2 = __riscv_vadd_vx_i16m1(h_weighted, a, 16);
        result2 = __riscv_vmax_vx_i16m1(result2, 0, 16);
        a += v_sum_scalar;

        vint16m1_t result3 = __riscv_vadd_vx_i16m1(h_weighted, a, 16);
        result3 = __riscv_vmax_vx_i16m1(result3, 0, 16);
        a += v_sum_scalar;

        vint16m1_t result4 = __riscv_vadd_vx_i16m1(h_weighted, a, 16);
        result4 = __riscv_vmax_vx_i16m1(result4, 0, 16);
        a += v_sum_scalar;

        vint16m1_t result5 = __riscv_vadd_vx_i16m1(h_weighted, a, 16);
        result5 = __riscv_vmax_vx_i16m1(result5, 0, 16);
        a += v_sum_scalar;

        vint16m1_t result6 = __riscv_vadd_vx_i16m1(h_weighted, a, 16);
        result6 = __riscv_vmax_vx_i16m1(result6, 0, 16);
        a += v_sum_scalar;

        vint16m1_t result7 = __riscv_vadd_vx_i16m1(h_weighted, a, 16);
        result7 = __riscv_vmax_vx_i16m1(result7, 0, 16);
        a += v_sum_scalar;

        vint16m1_t result8 = __riscv_vadd_vx_i16m1(h_weighted, a, 16);
        result8 = __riscv_vmax_vx_i16m1(result8, 0, 16);
        a += v_sum_scalar;

        vuint8mf2_t result1_n = __riscv_vnclipu_wx_u8mf2(__riscv_vreinterpret_v_i16m1_u16m1(result1), 5, 16);
        vuint8mf2_t result2_n = __riscv_vnclipu_wx_u8mf2(__riscv_vreinterpret_v_i16m1_u16m1(result2), 5, 16);
        vuint8mf2_t result3_n = __riscv_vnclipu_wx_u8mf2(__riscv_vreinterpret_v_i16m1_u16m1(result3), 5, 16);
        vuint8mf2_t result4_n = __riscv_vnclipu_wx_u8mf2(__riscv_vreinterpret_v_i16m1_u16m1(result4), 5, 16);
        vuint8mf2_t result5_n = __riscv_vnclipu_wx_u8mf2(__riscv_vreinterpret_v_i16m1_u16m1(result5), 5, 16);
        vuint8mf2_t result6_n = __riscv_vnclipu_wx_u8mf2(__riscv_vreinterpret_v_i16m1_u16m1(result6), 5, 16);
        vuint8mf2_t result7_n = __riscv_vnclipu_wx_u8mf2(__riscv_vreinterpret_v_i16m1_u16m1(result7), 5, 16);
        vuint8mf2_t result8_n = __riscv_vnclipu_wx_u8mf2(__riscv_vreinterpret_v_i16m1_u16m1(result8), 5, 16);

        __riscv_vse8_v_u8mf2(p_src_iter, result1_n, 16);
        p_src_iter += stride;
        __riscv_vse8_v_u8mf2(p_src_iter, result2_n, 16);
        p_src_iter += stride;
        __riscv_vse8_v_u8mf2(p_src_iter, result3_n, 16);
        p_src_iter += stride;
        __riscv_vse8_v_u8mf2(p_src_iter, result4_n, 16);
        p_src_iter += stride;
        __riscv_vse8_v_u8mf2(p_src_iter, result5_n, 16);
        p_src_iter += stride;
        __riscv_vse8_v_u8mf2(p_src_iter, result6_n, 16);
        p_src_iter += stride;
        __riscv_vse8_v_u8mf2(p_src_iter, result7_n, 16);
        p_src_iter += stride;
        __riscv_vse8_v_u8mf2(p_src_iter, result8_n, 16);
        p_src_iter += stride;
    }

    __builtin_rvv_vsetvxrm(vxrm);
}
#endif