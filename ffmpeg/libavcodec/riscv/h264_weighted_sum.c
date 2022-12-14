#include "h264_weighted_sum.h"
#if HAVE_RVV
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
        int vl = vsetvl_e8m1(count);
        uint8_t *p_begin = p_block_iter;

        for (int j = 0; j < height; j += 4)
        {
            vuint8m1_t row0 = vle8_v_u8m1(p_block_iter, vl);
            vuint8m1_t row1 = vle8_v_u8m1(p_block_iter + stride, vl);
            vuint8m1_t row2 = vle8_v_u8m1(p_block_iter + stride * 2, vl);
            vuint8m1_t row3 = vle8_v_u8m1(p_block_iter + stride * 3, vl);
            
            vint16m2_t result0_w, result1_w, result2_w, result3_w;

            result0_w = vreinterpret_v_u16m2_i16m2(vzext_vf2_u16m2(row0, vl));
            result1_w = vreinterpret_v_u16m2_i16m2(vzext_vf2_u16m2(row1, vl));
            result2_w = vreinterpret_v_u16m2_i16m2(vzext_vf2_u16m2(row2, vl));
            result3_w = vreinterpret_v_u16m2_i16m2(vzext_vf2_u16m2(row3, vl));

            result0_w = vsll_vx_i16m2(result0_w, 7, vl);
            result1_w = vsll_vx_i16m2(result1_w, 7, vl);
            result2_w = vsll_vx_i16m2(result2_w, 7, vl);
            result3_w = vsll_vx_i16m2(result3_w, 7, vl);

            result0_w = vadd_vx_i16m2(result0_w, offset, vl);
            result1_w = vadd_vx_i16m2(result1_w, offset, vl);
            result2_w = vadd_vx_i16m2(result2_w, offset, vl);
            result3_w = vadd_vx_i16m2(result3_w, offset, vl);                                    

            result0_w = vmax_vx_i16m2(result0_w, 0, vl);
            result1_w = vmax_vx_i16m2(result1_w, 0, vl);
            result2_w = vmax_vx_i16m2(result2_w, 0, vl);
            result3_w = vmax_vx_i16m2(result3_w, 0, vl);

            vuint8m1_t result0_n = vnclipu_wx_u8m1(vreinterpret_v_i16m2_u16m2(result0_w), shift, vl);
            vuint8m1_t result1_n = vnclipu_wx_u8m1(vreinterpret_v_i16m2_u16m2(result1_w), shift, vl);
            vuint8m1_t result2_n = vnclipu_wx_u8m1(vreinterpret_v_i16m2_u16m2(result2_w), shift, vl);
            vuint8m1_t result3_n = vnclipu_wx_u8m1(vreinterpret_v_i16m2_u16m2(result3_w), shift, vl);

            vse8_v_u8m1(p_block_iter, result0_n, vl);
            p_block_iter += stride;
            vse8_v_u8m1(p_block_iter, result1_n, vl);
            p_block_iter += stride;
            vse8_v_u8m1(p_block_iter, result2_n, vl);
            p_block_iter += stride;
            vse8_v_u8m1(p_block_iter, result3_n, vl);
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
        int vl = vsetvl_e8m1(count);
        uint8_t *p_begin = p_block_iter;

        vint8m1_t weight_v = vmv_v_x_i8m1(weight, vl);

        for (int j = 0; j < height; j += 4)
        {
            vuint8m1_t row0 = vle8_v_u8m1(p_block_iter, vl);
            vuint8m1_t row1 = vle8_v_u8m1(p_block_iter + stride, vl);
            vuint8m1_t row2 = vle8_v_u8m1(p_block_iter + stride * 2, vl);
            vuint8m1_t row3 = vle8_v_u8m1(p_block_iter + stride * 3, vl);
            
            vint16m2_t result0_w, result1_w, result2_w, result3_w;

            result0_w = vwmulsu_vv_i16m2(weight_v, row0, vl);
            result1_w = vwmulsu_vv_i16m2(weight_v, row1, vl);
            result2_w = vwmulsu_vv_i16m2(weight_v, row2, vl);
            result3_w = vwmulsu_vv_i16m2(weight_v, row3, vl);

            result0_w = vsadd_vx_i16m2(result0_w, value, vl);
            result1_w = vsadd_vx_i16m2(result1_w, value, vl);
            result2_w = vsadd_vx_i16m2(result2_w, value, vl);
            result3_w = vsadd_vx_i16m2(result3_w, value, vl);

            result0_w = vmax_vx_i16m2(result0_w, 0, vl);
            result1_w = vmax_vx_i16m2(result1_w, 0, vl);
            result2_w = vmax_vx_i16m2(result2_w, 0, vl);
            result3_w = vmax_vx_i16m2(result3_w, 0, vl);

            vuint8m1_t result0_n = vnclipu_wx_u8m1(vreinterpret_v_i16m2_u16m2(result0_w), shift, vl);
            vuint8m1_t result1_n = vnclipu_wx_u8m1(vreinterpret_v_i16m2_u16m2(result1_w), shift, vl);
            vuint8m1_t result2_n = vnclipu_wx_u8m1(vreinterpret_v_i16m2_u16m2(result2_w), shift, vl);
            vuint8m1_t result3_n = vnclipu_wx_u8m1(vreinterpret_v_i16m2_u16m2(result3_w), shift, vl);

            vse8_v_u8m1(p_block_iter, result0_n, vl);
            p_block_iter += stride;
            vse8_v_u8m1(p_block_iter, result1_n, vl);
            p_block_iter += stride;
            vse8_v_u8m1(p_block_iter, result2_n, vl);
            p_block_iter += stride;
            vse8_v_u8m1(p_block_iter, result3_n, vl);
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
        int vl = vsetvl_e8m1(count);
        uint8_t *p_src_begin = p_src_iter;
        uint8_t *p_dst_begin = p_dst_iter;

        for (int j = 0; j < height; j += 4)
        {
            vuint8m1_t src0 = vle8_v_u8m1(p_src_iter, vl);
            p_src_iter += stride;
            vuint8m1_t src1 = vle8_v_u8m1(p_src_iter, vl);
            p_src_iter += stride;
            vuint8m1_t src2 = vle8_v_u8m1(p_src_iter, vl);
            p_src_iter += stride;
            vuint8m1_t src3 = vle8_v_u8m1(p_src_iter, vl);
            p_src_iter += stride;
            
            vuint8m1_t dst0 = vle8_v_u8m1(p_dst_iter, vl);
            vuint8m1_t dst1 = vle8_v_u8m1(p_dst_iter + stride, vl);
            vuint8m1_t dst2 = vle8_v_u8m1(p_dst_iter + stride * 2, vl);
            vuint8m1_t dst3 = vle8_v_u8m1(p_dst_iter + stride * 3, vl);
            
            vint16m2_t result0_w, result1_w, result2_w, result3_w;

            result0_w = vmv_v_x_i16m2(value, vl);
            result1_w = vmv_v_x_i16m2(value, vl);
            result2_w = vmv_v_x_i16m2(value, vl);
            result3_w = vmv_v_x_i16m2(value, vl);
/*
            result0_w = vand_vx_i16m2(result0_w, 0, vl);
            result1_w = vand_vx_i16m2(result1_w, 0, vl);
            result2_w = vand_vx_i16m2(result2_w, 0, vl);
            result3_w = vand_vx_i16m2(result3_w, 0, vl);

            result0_w = vor_vx_i16m2(result0_w, value, vl);
            result1_w = vor_vx_i16m2(result1_w, value, vl);
            result2_w = vor_vx_i16m2(result2_w, value, vl);
            result3_w = vor_vx_i16m2(result3_w, value, vl);
*/
            result0_w = vwmaccsu_vx_i16m2(result0_w, weightd, dst0, vl);
            result1_w = vwmaccsu_vx_i16m2(result1_w, weightd, dst1, vl);
            result2_w = vwmaccsu_vx_i16m2(result2_w, weightd, dst2, vl);
            result3_w = vwmaccsu_vx_i16m2(result3_w, weightd, dst3, vl);

            result0_w = vwmaccsu_vx_i16m2(result0_w, weights, src0, vl);
            result1_w = vwmaccsu_vx_i16m2(result1_w, weights, src1, vl);
            result2_w = vwmaccsu_vx_i16m2(result2_w, weights, src2, vl);
            result3_w = vwmaccsu_vx_i16m2(result3_w, weights, src3, vl);

            result0_w = vmax_vx_i16m2(result0_w, 0, vl);
            result1_w = vmax_vx_i16m2(result1_w, 0, vl);
            result2_w = vmax_vx_i16m2(result2_w, 0, vl);
            result3_w = vmax_vx_i16m2(result3_w, 0, vl);

            vuint8m1_t result0_n = vnclipu_wx_u8m1(vreinterpret_v_i16m2_u16m2(result0_w), shift, vl);
            vuint8m1_t result1_n = vnclipu_wx_u8m1(vreinterpret_v_i16m2_u16m2(result1_w), shift, vl);
            vuint8m1_t result2_n = vnclipu_wx_u8m1(vreinterpret_v_i16m2_u16m2(result2_w), shift, vl);
            vuint8m1_t result3_n = vnclipu_wx_u8m1(vreinterpret_v_i16m2_u16m2(result3_w), shift, vl);

            vse8_v_u8m1(p_dst_iter, result0_n, vl);
            p_dst_iter += stride;
            vse8_v_u8m1(p_dst_iter, result1_n, vl);
            p_dst_iter += stride;
            vse8_v_u8m1(p_dst_iter, result2_n, vl);
            p_dst_iter += stride;
            vse8_v_u8m1(p_dst_iter, result3_n, vl);
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
    if (weightd == 1 && weights == 1 && log2_den == 0 && offset == 0)
        return;

    h264_biweight(p_dst, p_src, stride, 16, height, log2_den, weightd, weights, offset);
}

void biweight_h264_pixels_8_8_rvv(uint8_t *p_dst, uint8_t *p_src, ptrdiff_t stride,
                                  int height, int log2_den, int weightd,
                                  int weights, int offset)
{
    if (weightd == 1 && weights == 1 && log2_den == 0 && offset == 0)
        return;
    
    h264_biweight(p_dst, p_src, stride, 8, height, log2_den, weightd, weights, offset);
}

void biweight_h264_pixels_4_8_rvv(uint8_t *p_dst, uint8_t *p_src, ptrdiff_t stride,
                                  int height, int log2_den, int weightd,
                                  int weights, int offset)
{
    if (weightd == 1 && weights == 1 && log2_den == 0 && offset == 0)
        return;
    
    h264_biweight(p_dst, p_src, stride, 4, height, log2_den, weightd, weights, offset);
}
#endif