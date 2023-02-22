#include <riscv_vector.h>
#include "h264_utility.h"
#include "h264_lowpass.h"

__attribute__((always_inline)) static void avg_copy_block16(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    const uint8_t *p_src_iter = p_src;
    uint8_t *p_dst_iter = p_dst;
    int len = 16;

    while (len > 0)
    {
        int vl = vsetvl_e8m1(len);
        const uint8_t *p_src_begin = p_src_iter;
        uint8_t *p_dst_begin = p_dst_iter;

        for (int j = 0; j < 16; j += 8)
        {
            vuint8m1_t src0 = vle8_v_u8m1(p_src_iter, vl);
            p_src_iter += stride;
            vuint8m1_t src1 = vle8_v_u8m1(p_src_iter, vl);
            p_src_iter += stride;
            vuint8m1_t src2 = vle8_v_u8m1(p_src_iter, vl);
            p_src_iter += stride;
            vuint8m1_t src3 = vle8_v_u8m1(p_src_iter, vl);
            p_src_iter += stride;
            vuint8m1_t src4 = vle8_v_u8m1(p_src_iter, vl);
            p_src_iter += stride;
            vuint8m1_t src5 = vle8_v_u8m1(p_src_iter, vl);
            p_src_iter += stride;
            vuint8m1_t src6 = vle8_v_u8m1(p_src_iter, vl);
            p_src_iter += stride;
            vuint8m1_t src7 = vle8_v_u8m1(p_src_iter, vl);
            p_src_iter += stride;

            vuint8m1_t dst0 = vle8_v_u8m1(p_dst_iter, vl);
            vuint8m1_t dst1 = vle8_v_u8m1(p_dst_iter + stride, vl);
            vuint8m1_t dst2 = vle8_v_u8m1(p_dst_iter + stride * 2, vl);
            vuint8m1_t dst3 = vle8_v_u8m1(p_dst_iter + stride * 3, vl);
            vuint8m1_t dst4 = vle8_v_u8m1(p_dst_iter + stride * 4, vl);
            vuint8m1_t dst5 = vle8_v_u8m1(p_dst_iter + stride * 5, vl);
            vuint8m1_t dst6 = vle8_v_u8m1(p_dst_iter + stride * 6, vl);
            vuint8m1_t dst7 = vle8_v_u8m1(p_dst_iter + stride * 7, vl);

            dst0 = vaaddu_vv_u8m1(dst0, src0, vl);
            dst1 = vaaddu_vv_u8m1(dst1, src1, vl);
            dst2 = vaaddu_vv_u8m1(dst2, src2, vl);
            dst3 = vaaddu_vv_u8m1(dst3, src3, vl);
            dst4 = vaaddu_vv_u8m1(dst4, src4, vl);
            dst5 = vaaddu_vv_u8m1(dst5, src5, vl);
            dst6 = vaaddu_vv_u8m1(dst6, src6, vl);
            dst7 = vaaddu_vv_u8m1(dst7, src7, vl);

            vse8_v_u8m1(p_dst_iter, dst0, vl);
            p_dst_iter += stride;
            vse8_v_u8m1(p_dst_iter, dst1, vl);
            p_dst_iter += stride;
            vse8_v_u8m1(p_dst_iter, dst2, vl);
            p_dst_iter += stride;
            vse8_v_u8m1(p_dst_iter, dst3, vl);
            p_dst_iter += stride;
            vse8_v_u8m1(p_dst_iter, dst4, vl);
            p_dst_iter += stride;
            vse8_v_u8m1(p_dst_iter, dst5, vl);
            p_dst_iter += stride;
            vse8_v_u8m1(p_dst_iter, dst6, vl);
            p_dst_iter += stride;
            vse8_v_u8m1(p_dst_iter, dst7, vl);
            p_dst_iter += stride;                                                                                      
        }

        p_src_iter = p_src_begin + vl;
        p_dst_iter = p_dst_begin + vl;
        len -= vl;
    }
}

__attribute__((always_inline)) static void avg_h264_qpel16_h_lowpass(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t dst_stride, ptrdiff_t src_stride)
{
    const uint8_t *p_src_iter = p_src;
    uint8_t *p_dst_iter = p_dst;
    int len = 16;

    while (len > 0)
    {
        int vl = vsetvl_e8m1(len);
        const uint8_t *p_src_begin = p_src_iter;
        uint8_t *p_dst_begin = p_dst_iter;

        for (int j = 0; j < 16; j += 2)
        {
            vuint16m2_t dst0_u, dst1_u;
            h_lowpass_u16m2(&dst0_u, &dst1_u, &p_src_iter, src_stride, vl);
            
            vuint8m1_t dst0_nrw = vnclipu_wx_u8m1(dst0_u, 5, vl);
            vuint8m1_t dst1_nrw = vnclipu_wx_u8m1(dst1_u, 5, vl);

            vuint8m1_t avg0 = vle8_v_u8m1(p_dst_iter, vl);
            vuint8m1_t avg1 = vle8_v_u8m1(p_dst_iter + dst_stride, vl);

            avg0 = vaaddu_vv_u8m1(avg0, dst0_nrw, vl);
            avg1 = vaaddu_vv_u8m1(avg1, dst1_nrw, vl);

            vse8_v_u8m1(p_dst_iter, avg0, vl);
            p_dst_iter += dst_stride;
            vse8_v_u8m1(p_dst_iter, avg1, vl);
            p_dst_iter += dst_stride;
        }

        p_dst_iter = p_dst_begin + vl;
        p_src_iter = p_src_begin + vl;
        len -= vl;
    }
}

__attribute__((always_inline)) static void avg_h264_qpel16_hv_lowpass(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    const uint8_t *p_src_iter = p_src;
    uint8_t *p_dst_iter = p_dst;
    int len = 16;

    while (len > 0)
    {
        int vl = vsetvl_e8mf2(len);
        const uint8_t *p_src_begin = p_src_iter;
        uint8_t *p_dst_begin = p_dst_iter;

        p_src_iter -= (stride << 1);

        vint16m1_t h_row0, h_row1, h_row2, h_row3, h_row4, h_row5, h_row6, h_row7;
        vint16m1_t h_row8, h_row9, h_row10, h_row11, h_row12, h_row13;

        h_lowpass_i16m1(&h_row0, &h_row1, &p_src_iter, stride, vl);
        h_lowpass_i16m1(&h_row2, &h_row3, &p_src_iter, stride, vl);
        h_lowpass_i16m1(&h_row4, &h_row5, &p_src_iter, stride, vl);
        h_lowpass_i16m1(&h_row6, &h_row7, &p_src_iter, stride, vl);
        h_lowpass_i16m1(&h_row8, &h_row9, &p_src_iter, stride, vl);
        h_lowpass_i16m1(&h_row10, &h_row11, &p_src_iter, stride, vl);
        h_lowpass_i16m1(&h_row12, &h_row13, &p_src_iter, stride, vl);

        vuint32m2_t dst0, dst1;
        v_lowpass_u32m2(&dst0, &dst1, &h_row0, &h_row1, &h_row2, &h_row3, &h_row4, &h_row5, &h_row6, stride, vl);

        vuint8mf2_t dst0_u8, dst1_u8;
        u32_to_u8(&dst0_u8, &dst1_u8, dst0, dst1, vl);

        vuint8mf2_t avg0 = vle8_v_u8mf2(p_dst_iter, vl);
        vuint8mf2_t avg1 = vle8_v_u8mf2(p_dst_iter + stride, vl);
        avg0 = vaaddu_vv_u8mf2(avg0, dst0_u8, vl);
        avg1 = vaaddu_vv_u8mf2(avg1, dst1_u8, vl);

        vse8_v_u8mf2(p_dst_iter, avg0, vl);
        p_dst_iter += stride;
        vse8_v_u8mf2(p_dst_iter, avg1, vl);
        p_dst_iter += stride;

        v_lowpass_u32m2(&dst0, &dst1, &h_row2, &h_row3, &h_row4, &h_row5, &h_row6, &h_row7, &h_row8, stride, vl);
        
        u32_to_u8(&dst0_u8, &dst1_u8, dst0, dst1, vl);

        avg0 = vle8_v_u8mf2(p_dst_iter, vl);
        avg1 = vle8_v_u8mf2(p_dst_iter + stride, vl);
        avg0 = vaaddu_vv_u8mf2(avg0, dst0_u8, vl);
        avg1 = vaaddu_vv_u8mf2(avg1, dst1_u8, vl);

        vse8_v_u8mf2(p_dst_iter, avg0, vl);
        p_dst_iter += stride;
        vse8_v_u8mf2(p_dst_iter, avg1, vl);
        p_dst_iter += stride;

        v_lowpass_u32m2(&dst0, &dst1, &h_row4, &h_row5, &h_row6, &h_row7, &h_row8, &h_row9, &h_row10, stride, vl);

        u32_to_u8(&dst0_u8, &dst1_u8, dst0, dst1, vl);

        avg0 = vle8_v_u8mf2(p_dst_iter, vl);
        avg1 = vle8_v_u8mf2(p_dst_iter + stride, vl);
        avg0 = vaaddu_vv_u8mf2(avg0, dst0_u8, vl);
        avg1 = vaaddu_vv_u8mf2(avg1, dst1_u8, vl);

        vse8_v_u8mf2(p_dst_iter, avg0, vl);
        p_dst_iter += stride;
        vse8_v_u8mf2(p_dst_iter, avg1, vl);
        p_dst_iter += stride;

        v_lowpass_u32m2(&dst0, &dst1, &h_row6, &h_row7, &h_row8, &h_row9, &h_row10, &h_row11, &h_row12, stride, vl);
 
        u32_to_u8(&dst0_u8, &dst1_u8, dst0, dst1, vl);
        
        avg0 = vle8_v_u8mf2(p_dst_iter, vl);
        avg1 = vle8_v_u8mf2(p_dst_iter + stride, vl);
        avg0 = vaaddu_vv_u8mf2(avg0, dst0_u8, vl);
        avg1 = vaaddu_vv_u8mf2(avg1, dst1_u8, vl);

        vse8_v_u8mf2(p_dst_iter, avg0, vl);
        p_dst_iter += stride;
        vse8_v_u8mf2(p_dst_iter, avg1, vl);
        p_dst_iter += stride;

        vint16m1_t h_row14, h_row15, h_row16, h_row17, h_row18, h_row19, h_row20, h_row21;

        h_lowpass_i16m1(&h_row14, &h_row15, &p_src_iter, stride, vl);
        h_lowpass_i16m1(&h_row16, &h_row17, &p_src_iter, stride, vl);
        h_lowpass_i16m1(&h_row18, &h_row19, &p_src_iter, stride, vl);
        h_lowpass_i16m1(&h_row20, &h_row21, &p_src_iter, stride, vl);

        v_lowpass_u32m2(&dst0, &dst1, &h_row8, &h_row9, &h_row10, &h_row11, &h_row12, &h_row13, &h_row14, stride, vl);

        u32_to_u8(&dst0_u8, &dst1_u8, dst0, dst1, vl);

        avg0 = vle8_v_u8mf2(p_dst_iter, vl);
        avg1 = vle8_v_u8mf2(p_dst_iter + stride, vl);
        avg0 = vaaddu_vv_u8mf2(avg0, dst0_u8, vl);
        avg1 = vaaddu_vv_u8mf2(avg1, dst1_u8, vl);

        vse8_v_u8mf2(p_dst_iter, avg0, vl);
        p_dst_iter += stride;
        vse8_v_u8mf2(p_dst_iter, avg1, vl);
        p_dst_iter += stride;

        v_lowpass_u32m2(&dst0, &dst1, &h_row10, &h_row11, &h_row12, &h_row13, &h_row14, &h_row15, &h_row16, stride, vl);

        u32_to_u8(&dst0_u8, &dst1_u8, dst0, dst1, vl);
        
        avg0 = vle8_v_u8mf2(p_dst_iter, vl);
        avg1 = vle8_v_u8mf2(p_dst_iter + stride, vl);
        avg0 = vaaddu_vv_u8mf2(avg0, dst0_u8, vl);
        avg1 = vaaddu_vv_u8mf2(avg1, dst1_u8, vl);

        vse8_v_u8mf2(p_dst_iter, avg0, vl);
        p_dst_iter += stride;
        vse8_v_u8mf2(p_dst_iter, avg1, vl);
        p_dst_iter += stride;

        v_lowpass_u32m2(&dst0, &dst1, &h_row12, &h_row13, &h_row14, &h_row15, &h_row16, &h_row17, &h_row18, stride, vl);

        u32_to_u8(&dst0_u8, &dst1_u8, dst0, dst1, vl);

        avg0 = vle8_v_u8mf2(p_dst_iter, vl);
        avg1 = vle8_v_u8mf2(p_dst_iter + stride, vl);
        avg0 = vaaddu_vv_u8mf2(avg0, dst0_u8, vl);
        avg1 = vaaddu_vv_u8mf2(avg1, dst1_u8, vl);

        vse8_v_u8mf2(p_dst_iter, avg0, vl);
        p_dst_iter += stride;
        vse8_v_u8mf2(p_dst_iter, avg1, vl);
        p_dst_iter += stride;

        v_lowpass_u32m2(&dst0, &dst1, &h_row14, &h_row15, &h_row16, &h_row17, &h_row18, &h_row19, &h_row20, stride, vl);

        u32_to_u8(&dst0_u8, &dst1_u8, dst0, dst1, vl);

        avg0 = vle8_v_u8mf2(p_dst_iter, vl);
        avg1 = vle8_v_u8mf2(p_dst_iter + stride, vl);
        avg0 = vaaddu_vv_u8mf2(avg0, dst0_u8, vl);
        avg1 = vaaddu_vv_u8mf2(avg1, dst1_u8, vl);

        vse8_v_u8mf2(p_dst_iter, avg0, vl);
        p_dst_iter += stride;
        vse8_v_u8mf2(p_dst_iter, avg1, vl);

        p_dst_iter = p_dst_begin + vl;
        p_src_iter = p_src_begin + vl;
        len -= vl;
    }
}

__attribute__((always_inline)) static void avg_h264_qpel16_hv_lowpass_l2(uint8_t *p_dst, const uint8_t *p_src, uint8_t *p_l2_src, ptrdiff_t stride, ptrdiff_t l2_stride)
{
    const uint8_t *p_src_iter = p_src;
    const uint8_t *p_l2_src_iter = p_l2_src;
    uint8_t *p_dst_iter = p_dst;
    int len = 16;

    while (len > 0)
    {
        int vl = vsetvl_e8mf2(len);
        const uint8_t *p_src_begin = p_src_iter;
        const uint8_t *p_l2_src_begin = p_l2_src_iter;
        uint8_t *p_dst_begin = p_dst_iter;

        p_src_iter -= (stride << 1);

        vint16m1_t h_row0, h_row1, h_row2, h_row3, h_row4, h_row5, h_row6, h_row7;
        vint16m1_t h_row8, h_row9, h_row10, h_row11, h_row12, h_row13;

        h_lowpass_i16m1(&h_row0, &h_row1, &p_src_iter, stride, vl);
        h_lowpass_i16m1(&h_row2, &h_row3, &p_src_iter, stride, vl);
        h_lowpass_i16m1(&h_row4, &h_row5, &p_src_iter, stride, vl);
        h_lowpass_i16m1(&h_row6, &h_row7, &p_src_iter, stride, vl);
        h_lowpass_i16m1(&h_row8, &h_row9, &p_src_iter, stride, vl);
        h_lowpass_i16m1(&h_row10, &h_row11, &p_src_iter, stride, vl);
        h_lowpass_i16m1(&h_row12, &h_row13, &p_src_iter, stride, vl);

        vuint32m2_t dst0, dst1;
        vuint8mf2_t dst0_u8, dst1_u8;

        v_lowpass_u32m2(&dst0, &dst1, &h_row0, &h_row1, &h_row2, &h_row3, &h_row4, &h_row5, &h_row6, stride, vl);        
        u32_to_u8(&dst0_u8, &dst1_u8, dst0, dst1, vl);
        avg_average_l2(&p_dst_iter, &p_l2_src_iter, stride, l2_stride, dst0_u8, dst1_u8, vl);

        v_lowpass_u32m2(&dst0, &dst1, &h_row2, &h_row3, &h_row4, &h_row5, &h_row6, &h_row7, &h_row8, stride, vl);        
        u32_to_u8(&dst0_u8, &dst1_u8, dst0, dst1, vl);
        avg_average_l2(&p_dst_iter, &p_l2_src_iter, stride, l2_stride, dst0_u8, dst1_u8, vl);

        v_lowpass_u32m2(&dst0, &dst1, &h_row4, &h_row5, &h_row6, &h_row7, &h_row8, &h_row9, &h_row10, stride, vl);
        u32_to_u8(&dst0_u8, &dst1_u8, dst0, dst1, vl);
        avg_average_l2(&p_dst_iter, &p_l2_src_iter, stride, l2_stride, dst0_u8, dst1_u8, vl);

        v_lowpass_u32m2(&dst0, &dst1, &h_row6, &h_row7, &h_row8, &h_row9, &h_row10, &h_row11, &h_row12, stride, vl);
        u32_to_u8(&dst0_u8, &dst1_u8, dst0, dst1, vl);
        avg_average_l2(&p_dst_iter, &p_l2_src_iter, stride, l2_stride, dst0_u8, dst1_u8, vl);

        vint16m1_t h_row14, h_row15, h_row16, h_row17, h_row18, h_row19, h_row20, h_row21;
        h_lowpass_i16m1(&h_row14, &h_row15, &p_src_iter, stride, vl);
        h_lowpass_i16m1(&h_row16, &h_row17, &p_src_iter, stride, vl);
        h_lowpass_i16m1(&h_row18, &h_row19, &p_src_iter, stride, vl);
        h_lowpass_i16m1(&h_row20, &h_row21, &p_src_iter, stride, vl);

        v_lowpass_u32m2(&dst0, &dst1, &h_row8, &h_row9, &h_row10, &h_row11, &h_row12, &h_row13, &h_row14, stride, vl);
        u32_to_u8(&dst0_u8, &dst1_u8, dst0, dst1, vl);
        avg_average_l2(&p_dst_iter, &p_l2_src_iter, stride, l2_stride, dst0_u8, dst1_u8, vl);

        v_lowpass_u32m2(&dst0, &dst1, &h_row10, &h_row11, &h_row12, &h_row13, &h_row14, &h_row15, &h_row16, stride, vl);
        u32_to_u8(&dst0_u8, &dst1_u8, dst0, dst1, vl);
        avg_average_l2(&p_dst_iter, &p_l2_src_iter, stride, l2_stride, dst0_u8, dst1_u8, vl);

        v_lowpass_u32m2(&dst0, &dst1, &h_row12, &h_row13, &h_row14, &h_row15, &h_row16, &h_row17, &h_row18, stride, vl);
        u32_to_u8(&dst0_u8, &dst1_u8, dst0, dst1, vl);
        avg_average_l2(&p_dst_iter, &p_l2_src_iter, stride, l2_stride, dst0_u8, dst1_u8, vl);

        v_lowpass_u32m2(&dst0, &dst1, &h_row14, &h_row15, &h_row16, &h_row17, &h_row18, &h_row19, &h_row20, stride, vl);
        u32_to_u8(&dst0_u8, &dst1_u8, dst0, dst1, vl);
        avg_average_l2(&p_dst_iter, &p_l2_src_iter, stride, l2_stride, dst0_u8, dst1_u8, vl);

        p_dst_iter = p_dst_begin + vl;
        p_src_iter = p_src_begin + vl;
        p_l2_src_iter = p_l2_src_begin + vl;
        len -= vl;
    }
}

__attribute__((always_inline)) static void avg_h264_qpel16_v_lowpass(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t dst_stride, ptrdiff_t src_stride)
{
    const uint8_t *p_src_iter = p_src;
    uint8_t *p_dst_iter = p_dst;
    int len = 16;

    while (len > 0)
    {
        int vl = vsetvl_e8m1(len);
        const uint8_t *p_src_begin = p_src_iter;
        uint8_t *p_dst_begin = p_dst_iter;

        p_src_iter -= (src_stride * 2);

        vuint8m1_t row0 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += src_stride;
        vuint8m1_t row1 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += src_stride;
        vuint8m1_t row2 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += src_stride;
        vuint8m1_t row3 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += src_stride;
        vuint8m1_t row4 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += src_stride;
        vuint8m1_t row5 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += src_stride;
        vuint8m1_t row6 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += src_stride;

        vuint8m1_t dst0, dst1;
        v_lowpass_u8m1(&dst0, &dst1, row0, row1, row2, row3, row4, row5, row6, vl);
        vuint8m1_t avg0 = vle8_v_u8m1(p_dst_iter, vl);
        vuint8m1_t avg1 = vle8_v_u8m1(p_dst_iter + dst_stride, vl);
        avg0 = vaaddu_vv_u8m1(avg0, dst0, vl);
        avg1 = vaaddu_vv_u8m1(avg1, dst1, vl);
       
        vse8_v_u8m1(p_dst_iter, avg0, vl);
        p_dst_iter += dst_stride;
        vse8_v_u8m1(p_dst_iter, avg1, vl);
        p_dst_iter += dst_stride;

        // 3rd, 4th dst
        row0 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += src_stride;
        row1 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += src_stride;

        v_lowpass_u8m1(&dst0, &dst1, row2, row3, row4, row5, row6, row0, row1, vl);
        avg0 = vle8_v_u8m1(p_dst_iter, vl);
        avg1 = vle8_v_u8m1(p_dst_iter + dst_stride, vl);
        avg0 = vaaddu_vv_u8m1(avg0, dst0, vl);
        avg1 = vaaddu_vv_u8m1(avg1, dst1, vl);

        vse8_v_u8m1(p_dst_iter, avg0, vl);
        p_dst_iter += dst_stride;
        vse8_v_u8m1(p_dst_iter, avg1, vl);
        p_dst_iter += dst_stride;

        // 5th, 6th dst
        row2 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += src_stride;
        row3 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += src_stride;
        
        v_lowpass_u8m1(&dst0, &dst1, row4, row5, row6, row0, row1, row2, row3, vl);
        avg0 = vle8_v_u8m1(p_dst_iter, vl);
        avg1 = vle8_v_u8m1(p_dst_iter + dst_stride, vl);
        avg0 = vaaddu_vv_u8m1(avg0, dst0, vl);
        avg1 = vaaddu_vv_u8m1(avg1, dst1, vl);
        
        vse8_v_u8m1(p_dst_iter, avg0, vl);
        p_dst_iter += dst_stride;
        vse8_v_u8m1(p_dst_iter, avg1, vl);
        p_dst_iter += dst_stride;

        // 7th, 8th dst
        row4 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += src_stride;
        row5 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += src_stride;

        v_lowpass_u8m1(&dst0, &dst1, row6, row0, row1, row2, row3, row4, row5, vl);
        avg0 = vle8_v_u8m1(p_dst_iter, vl);
        avg1 = vle8_v_u8m1(p_dst_iter + dst_stride, vl);
        avg0 = vaaddu_vv_u8m1(avg0, dst0, vl);
        avg1 = vaaddu_vv_u8m1(avg1, dst1, vl);

        vse8_v_u8m1(p_dst_iter, avg0, vl);
        p_dst_iter += dst_stride;
        vse8_v_u8m1(p_dst_iter, avg1, vl);
        p_dst_iter += dst_stride;

        // 9th, 10th dst
        row6 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += src_stride;
        row0 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += src_stride;

        v_lowpass_u8m1(&dst0, &dst1, row1, row2, row3, row4, row5, row6, row0, vl);
        avg0 = vle8_v_u8m1(p_dst_iter, vl);
        avg1 = vle8_v_u8m1(p_dst_iter + dst_stride, vl);
        avg0 = vaaddu_vv_u8m1(avg0, dst0, vl);
        avg1 = vaaddu_vv_u8m1(avg1, dst1, vl);
        
        vse8_v_u8m1(p_dst_iter, avg0, vl);
        p_dst_iter += dst_stride;
        vse8_v_u8m1(p_dst_iter, avg1, vl);
        p_dst_iter += dst_stride;

        // 11th, 12th dst
        row1 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += src_stride;
        row2 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += src_stride;

        v_lowpass_u8m1(&dst0, &dst1, row3, row4, row5, row6, row0, row1, row2, vl);
        avg0 = vle8_v_u8m1(p_dst_iter, vl);
        avg1 = vle8_v_u8m1(p_dst_iter + dst_stride, vl);
        avg0 = vaaddu_vv_u8m1(avg0, dst0, vl);
        avg1 = vaaddu_vv_u8m1(avg1, dst1, vl);
        
        vse8_v_u8m1(p_dst_iter, avg0, vl);
        p_dst_iter += dst_stride;
        vse8_v_u8m1(p_dst_iter, avg1, vl);
        p_dst_iter += dst_stride;

        // 13th, 14th dst
        row3 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += src_stride;
        row4 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += src_stride;

        v_lowpass_u8m1(&dst0, &dst1, row5, row6, row0, row1, row2, row3, row4, vl);
        avg0 = vle8_v_u8m1(p_dst_iter, vl);
        avg1 = vle8_v_u8m1(p_dst_iter + dst_stride, vl);
        avg0 = vaaddu_vv_u8m1(avg0, dst0, vl);
        avg1 = vaaddu_vv_u8m1(avg1, dst1, vl);
        
        vse8_v_u8m1(p_dst_iter, avg0, vl);
        p_dst_iter += dst_stride;
        vse8_v_u8m1(p_dst_iter, avg1, vl);
        p_dst_iter += dst_stride;

        // 15th, 16th dst
        row5 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += src_stride;
        row6 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += src_stride;

        v_lowpass_u8m1(&dst0, &dst1, row0, row1, row2, row3, row4, row5, row6, vl);
        avg0 = vle8_v_u8m1(p_dst_iter, vl);
        avg1 = vle8_v_u8m1(p_dst_iter + dst_stride, vl);
        avg0 = vaaddu_vv_u8m1(avg0, dst0, vl);
        avg1 = vaaddu_vv_u8m1(avg1, dst1, vl);
        
        vse8_v_u8m1(p_dst_iter, avg0, vl);
        p_dst_iter += dst_stride;
        vse8_v_u8m1(p_dst_iter, avg1, vl);

        p_dst_iter = p_dst_begin + vl;
        p_src_iter = p_src_begin + vl;
        len -= vl;        
    }
}

__attribute__((always_inline)) static void avg_h264_qpel16_v_lowpass_l2(uint8_t *p_dst, const uint8_t *p_src, const uint8_t *p_l2_src, int stride, int l2_stride)
{
    const uint8_t *p_src_iter = p_src;
    const uint8_t *p_l2_src_iter = p_l2_src;
    uint8_t *p_dst_iter = p_dst;
    int len = 16;

    while (len > 0)
    {
        int vl = vsetvl_e8m1(len);
        const uint8_t *p_src_begin = p_src_iter;
        const uint8_t *p_l2_src_begin = p_l2_src_iter;
        uint8_t *p_dst_begin = p_dst_iter;

        p_src_iter -= (stride * 2);

        vuint8m1_t row0 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        vuint8m1_t row1 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        vuint8m1_t row2 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        vuint8m1_t row3 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        vuint8m1_t row4 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        vuint8m1_t row5 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        vuint8m1_t row6 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        
        vuint8m1_t l2_row0 = vle8_v_u8m1(p_l2_src_iter, vl);
        p_l2_src_iter += l2_stride;
        vuint8m1_t l2_row1 = vle8_v_u8m1(p_l2_src_iter, vl);
        p_l2_src_iter += l2_stride;

        vuint8m1_t dst0, dst1; 
        v_lowpass_u8m1(&dst0, &dst1, row0, row1, row2, row3, row4, row5, row6, vl);
        dst0 = vaaddu_vv_u8m1(dst0, l2_row0, vl);
        dst1 = vaaddu_vv_u8m1(dst1, l2_row1, vl);
        vuint8m1_t avg0 = vle8_v_u8m1(p_dst_iter, vl);
        vuint8m1_t avg1 = vle8_v_u8m1(p_dst_iter + stride, vl);
        avg0 = vaaddu_vv_u8m1(avg0, dst0, vl);
        avg1 = vaaddu_vv_u8m1(avg1, dst1, vl);

        vse8_v_u8m1(p_dst_iter, avg0, vl);
        p_dst_iter += stride;
        vse8_v_u8m1(p_dst_iter, avg1, vl);
        p_dst_iter += stride;

        // 3rd, 4th dst
        row0 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        row1 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;

        l2_row0 = vle8_v_u8m1(p_l2_src_iter, vl);
        p_l2_src_iter += l2_stride;
        l2_row1 = vle8_v_u8m1(p_l2_src_iter, vl);
        p_l2_src_iter += l2_stride;

        v_lowpass_u8m1(&dst0, &dst1, row2, row3, row4, row5, row6, row0, row1, vl);
        dst0 = vaaddu_vv_u8m1(dst0, l2_row0, vl);
        dst1 = vaaddu_vv_u8m1(dst1, l2_row1, vl);
        avg0 = vle8_v_u8m1(p_dst_iter, vl);
        avg1 = vle8_v_u8m1(p_dst_iter + stride, vl);
        avg0 = vaaddu_vv_u8m1(avg0, dst0, vl);
        avg1 = vaaddu_vv_u8m1(avg1, dst1, vl);

        vse8_v_u8m1(p_dst_iter, avg0, vl);
        p_dst_iter += stride;
        vse8_v_u8m1(p_dst_iter, avg1, vl);
        p_dst_iter += stride;

        // 5th, 6th dst
        row2 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        row3 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;

        l2_row0 = vle8_v_u8m1(p_l2_src_iter, vl);
        p_l2_src_iter += l2_stride;
        l2_row1 = vle8_v_u8m1(p_l2_src_iter, vl);
        p_l2_src_iter += l2_stride;

        v_lowpass_u8m1(&dst0, &dst1, row4, row5, row6, row0, row1, row2, row3, vl);
        dst0 = vaaddu_vv_u8m1(dst0, l2_row0, vl);
        dst1 = vaaddu_vv_u8m1(dst1, l2_row1, vl);
        avg0 = vle8_v_u8m1(p_dst_iter, vl);
        avg1 = vle8_v_u8m1(p_dst_iter + stride, vl);
        avg0 = vaaddu_vv_u8m1(avg0, dst0, vl);
        avg1 = vaaddu_vv_u8m1(avg1, dst1, vl);

        vse8_v_u8m1(p_dst_iter, avg0, vl);
        p_dst_iter += stride;
        vse8_v_u8m1(p_dst_iter, avg1, vl);
        p_dst_iter += stride;

        // 7th, 8th dst
        row4 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        row5 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;

        l2_row0 = vle8_v_u8m1(p_l2_src_iter, vl);
        p_l2_src_iter += l2_stride;
        l2_row1 = vle8_v_u8m1(p_l2_src_iter, vl);
        p_l2_src_iter += l2_stride;

        v_lowpass_u8m1(&dst0, &dst1, row6, row0, row1, row2, row3, row4, row5, vl);
        dst0 = vaaddu_vv_u8m1(dst0, l2_row0, vl);
        dst1 = vaaddu_vv_u8m1(dst1, l2_row1, vl);
        avg0 = vle8_v_u8m1(p_dst_iter, vl);
        avg1 = vle8_v_u8m1(p_dst_iter + stride, vl);
        avg0 = vaaddu_vv_u8m1(avg0, dst0, vl);
        avg1 = vaaddu_vv_u8m1(avg1, dst1, vl);

        vse8_v_u8m1(p_dst_iter, avg0, vl);
        p_dst_iter += stride;
        vse8_v_u8m1(p_dst_iter, avg1, vl);
        p_dst_iter += stride;

        // 9th, 10th dst
        row6 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        row0 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;

        l2_row0 = vle8_v_u8m1(p_l2_src_iter, vl);
        p_l2_src_iter += l2_stride;
        l2_row1 = vle8_v_u8m1(p_l2_src_iter, vl);
        p_l2_src_iter += l2_stride;

        v_lowpass_u8m1(&dst0, &dst1, row1, row2, row3, row4, row5, row6, row0, vl);
        dst0 = vaaddu_vv_u8m1(dst0, l2_row0, vl);
        dst1 = vaaddu_vv_u8m1(dst1, l2_row1, vl);
        avg0 = vle8_v_u8m1(p_dst_iter, vl);
        avg1 = vle8_v_u8m1(p_dst_iter + stride, vl);
        avg0 = vaaddu_vv_u8m1(avg0, dst0, vl);
        avg1 = vaaddu_vv_u8m1(avg1, dst1, vl);

        vse8_v_u8m1(p_dst_iter, avg0, vl);
        p_dst_iter += stride;
        vse8_v_u8m1(p_dst_iter, avg1, vl);
        p_dst_iter += stride;

        // 11th, 12th dst
        row1 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        row2 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;

        l2_row0 = vle8_v_u8m1(p_l2_src_iter, vl);
        p_l2_src_iter += l2_stride;
        l2_row1 = vle8_v_u8m1(p_l2_src_iter, vl);
        p_l2_src_iter += l2_stride;

        v_lowpass_u8m1(&dst0, &dst1, row3, row4, row5, row6, row0, row1, row2, vl);
        dst0 = vaaddu_vv_u8m1(dst0, l2_row0, vl);
        dst1 = vaaddu_vv_u8m1(dst1, l2_row1, vl);
        avg0 = vle8_v_u8m1(p_dst_iter, vl);
        avg1 = vle8_v_u8m1(p_dst_iter + stride, vl);
        avg0 = vaaddu_vv_u8m1(avg0, dst0, vl);
        avg1 = vaaddu_vv_u8m1(avg1, dst1, vl);

        vse8_v_u8m1(p_dst_iter, avg0, vl);
        p_dst_iter += stride;
        vse8_v_u8m1(p_dst_iter, avg1, vl);
        p_dst_iter += stride;

        // 13th, 14th dst
        row3 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        row4 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;

        l2_row0 = vle8_v_u8m1(p_l2_src_iter, vl);
        p_l2_src_iter += l2_stride;
        l2_row1 = vle8_v_u8m1(p_l2_src_iter, vl);
        p_l2_src_iter += l2_stride;

        v_lowpass_u8m1(&dst0, &dst1, row5, row6, row0, row1, row2, row3, row4, vl);
        dst0 = vaaddu_vv_u8m1(dst0, l2_row0, vl);
        dst1 = vaaddu_vv_u8m1(dst1, l2_row1, vl);
        avg0 = vle8_v_u8m1(p_dst_iter, vl);
        avg1 = vle8_v_u8m1(p_dst_iter + stride, vl);
        avg0 = vaaddu_vv_u8m1(avg0, dst0, vl);
        avg1 = vaaddu_vv_u8m1(avg1, dst1, vl);

        vse8_v_u8m1(p_dst_iter, avg0, vl);
        p_dst_iter += stride;
        vse8_v_u8m1(p_dst_iter, avg1, vl);
        p_dst_iter += stride;

        // 15th, 16th dst
        row5 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        row6 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;

        l2_row0 = vle8_v_u8m1(p_l2_src_iter, vl);
        p_l2_src_iter += l2_stride;
        l2_row1 = vle8_v_u8m1(p_l2_src_iter, vl);
        p_l2_src_iter += l2_stride;

        v_lowpass_u8m1(&dst0, &dst1, row0, row1, row2, row3, row4, row5, row6, vl);
        dst0 = vaaddu_vv_u8m1(dst0, l2_row0, vl);
        dst1 = vaaddu_vv_u8m1(dst1, l2_row1, vl);
        avg0 = vle8_v_u8m1(p_dst_iter, vl);
        avg1 = vle8_v_u8m1(p_dst_iter + stride, vl);
        avg0 = vaaddu_vv_u8m1(avg0, dst0, vl);
        avg1 = vaaddu_vv_u8m1(avg1, dst1, vl);

        vse8_v_u8m1(p_dst_iter, avg0, vl);
        p_dst_iter += stride;
        vse8_v_u8m1(p_dst_iter, avg1, vl);

        p_dst_iter = p_dst_begin + vl;
        p_src_iter = p_src_begin + vl;
        p_l2_src_iter = p_l2_src_begin + vl;
        len -= vl;        
    }
}

__attribute__((always_inline)) static void avg_h264_qpel16_v_lowpass_l2src(uint8_t *p_dst, const uint8_t *p_src, int stride)
{
    const uint8_t *p_src_iter = p_src;
    uint8_t *p_dst_iter = p_dst;
    int len = 16;

    while (len > 0)
    {
        int vl = vsetvl_e8m1(len);    
        const uint8_t *p_src_begin = p_src_iter;
        uint8_t *p_dst_begin = p_dst_iter;

        p_src_iter -= (stride * 2);

        vuint8m1_t row0 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        vuint8m1_t row1 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        vuint8m1_t row2 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        vuint8m1_t row3 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        vuint8m1_t row4 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        vuint8m1_t row5 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        vuint8m1_t row6 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        
        vuint8m1_t dst0, dst1; 
        v_lowpass_u8m1(&dst0, &dst1, row0, row1, row2, row3, row4, row5, row6, vl);
        dst0 = vaaddu_vv_u8m1(dst0, row2, vl);
        dst1 = vaaddu_vv_u8m1(dst1, row3, vl);
        vuint8m1_t avg0 = vle8_v_u8m1(p_dst_iter, vl);
        vuint8m1_t avg1 = vle8_v_u8m1(p_dst_iter + stride, vl);
        avg0 = vaaddu_vv_u8m1(avg0, dst0, vl);
        avg1 = vaaddu_vv_u8m1(avg1, dst1, vl);

        vse8_v_u8m1(p_dst_iter, avg0, vl);
        p_dst_iter += stride;
        vse8_v_u8m1(p_dst_iter, avg1, vl);
        p_dst_iter += stride;

        // 3rd, 4th dst
        row0 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        row1 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;

        v_lowpass_u8m1(&dst0, &dst1, row2, row3, row4, row5, row6, row0, row1, vl);
        dst0 = vaaddu_vv_u8m1(dst0, row4, vl);
        dst1 = vaaddu_vv_u8m1(dst1, row5, vl);
        avg0 = vle8_v_u8m1(p_dst_iter, vl);
        avg1 = vle8_v_u8m1(p_dst_iter + stride, vl);
        avg0 = vaaddu_vv_u8m1(avg0, dst0, vl);
        avg1 = vaaddu_vv_u8m1(avg1, dst1, vl);

        vse8_v_u8m1(p_dst_iter, avg0, vl);
        p_dst_iter += stride;
        vse8_v_u8m1(p_dst_iter, avg1, vl);
        p_dst_iter += stride;

        // 5th, 6th dst
        row2 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        row3 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;

        v_lowpass_u8m1(&dst0, &dst1, row4, row5, row6, row0, row1, row2, row3, vl);
        dst0 = vaaddu_vv_u8m1(dst0, row6, vl);
        dst1 = vaaddu_vv_u8m1(dst1, row0, vl);
        avg0 = vle8_v_u8m1(p_dst_iter, vl);
        avg1 = vle8_v_u8m1(p_dst_iter + stride, vl);
        avg0 = vaaddu_vv_u8m1(avg0, dst0, vl);
        avg1 = vaaddu_vv_u8m1(avg1, dst1, vl);

        vse8_v_u8m1(p_dst_iter, avg0, vl);
        p_dst_iter += stride;
        vse8_v_u8m1(p_dst_iter, avg1, vl);
        p_dst_iter += stride;

        // 7th, 8th dst
        row4 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        row5 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        
        v_lowpass_u8m1(&dst0, &dst1, row6, row0, row1, row2, row3, row4, row5, vl);
        dst0 = vaaddu_vv_u8m1(dst0, row1, vl);
        dst1 = vaaddu_vv_u8m1(dst1, row2, vl);
        avg0 = vle8_v_u8m1(p_dst_iter, vl);
        avg1 = vle8_v_u8m1(p_dst_iter + stride, vl);
        avg0 = vaaddu_vv_u8m1(avg0, dst0, vl);
        avg1 = vaaddu_vv_u8m1(avg1, dst1, vl);

        vse8_v_u8m1(p_dst_iter, avg0, vl);
        p_dst_iter += stride;
        vse8_v_u8m1(p_dst_iter, avg1, vl);
        p_dst_iter += stride;

        // 9th, 10th dst
        row6 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        row0 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;

        v_lowpass_u8m1(&dst0, &dst1, row1, row2, row3, row4, row5, row6, row0, vl);
        dst0 = vaaddu_vv_u8m1(dst0, row3, vl);
        dst1 = vaaddu_vv_u8m1(dst1, row4, vl);
        avg0 = vle8_v_u8m1(p_dst_iter, vl);
        avg1 = vle8_v_u8m1(p_dst_iter + stride, vl);
        avg0 = vaaddu_vv_u8m1(avg0, dst0, vl);
        avg1 = vaaddu_vv_u8m1(avg1, dst1, vl);

        vse8_v_u8m1(p_dst_iter, avg0, vl);
        p_dst_iter += stride;
        vse8_v_u8m1(p_dst_iter, avg1, vl);
        p_dst_iter += stride;

        // 11th, 12th dst
        row1 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        row2 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;

        v_lowpass_u8m1(&dst0, &dst1, row3, row4, row5, row6, row0, row1, row2, vl);
        dst0 = vaaddu_vv_u8m1(dst0, row5, vl);
        dst1 = vaaddu_vv_u8m1(dst1, row6, vl);
        avg0 = vle8_v_u8m1(p_dst_iter, vl);
        avg1 = vle8_v_u8m1(p_dst_iter + stride, vl);
        avg0 = vaaddu_vv_u8m1(avg0, dst0, vl);
        avg1 = vaaddu_vv_u8m1(avg1, dst1, vl);

        vse8_v_u8m1(p_dst_iter, avg0, vl);
        p_dst_iter += stride;
        vse8_v_u8m1(p_dst_iter, avg1, vl);
        p_dst_iter += stride;

        // 13th, 14th dst
        row3 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        row4 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;

        v_lowpass_u8m1(&dst0, &dst1, row5, row6, row0, row1, row2, row3, row4, vl);
        dst0 = vaaddu_vv_u8m1(dst0, row0, vl);
        dst1 = vaaddu_vv_u8m1(dst1, row1, vl);
        avg0 = vle8_v_u8m1(p_dst_iter, vl);
        avg1 = vle8_v_u8m1(p_dst_iter + stride, vl);
        avg0 = vaaddu_vv_u8m1(avg0, dst0, vl);
        avg1 = vaaddu_vv_u8m1(avg1, dst1, vl);

        vse8_v_u8m1(p_dst_iter, avg0, vl);
        p_dst_iter += stride;
        vse8_v_u8m1(p_dst_iter, avg1, vl);
        p_dst_iter += stride;

        // 15th, 16th dst
        row5 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        row6 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;

        v_lowpass_u8m1(&dst0, &dst1, row0, row1, row2, row3, row4, row5, row6, vl);
        dst0 = vaaddu_vv_u8m1(dst0, row2, vl);
        dst1 = vaaddu_vv_u8m1(dst1, row3, vl);
        avg0 = vle8_v_u8m1(p_dst_iter, vl);
        avg1 = vle8_v_u8m1(p_dst_iter + stride, vl);
        avg0 = vaaddu_vv_u8m1(avg0, dst0, vl);
        avg1 = vaaddu_vv_u8m1(avg1, dst1, vl);

        vse8_v_u8m1(p_dst_iter, avg0, vl);
        p_dst_iter += stride;
        vse8_v_u8m1(p_dst_iter, avg1, vl);

        p_dst_iter = p_dst_begin + vl;
        p_src_iter = p_src_begin + vl;
        len -= vl;        
    }
}

__attribute__((always_inline)) static void avg_h264_qpel16_h_lowpass_l2src(uint8_t *p_dst, const uint8_t *p_src, int stride)
{
    const uint8_t *p_src_iter = p_src;
    uint8_t *p_dst_iter = p_dst;
    int len = 16;

    while (len > 0)
    {
        int vl = vsetvl_e8m1(len);
        const uint8_t *p_src_begin = p_src_iter;
        uint8_t *p_dst_begin = p_dst_iter;

        for (int j = 0; j < 16; j += 2)
        {
            vuint8m1_t dst0, dst1;
            h_lowpass_u8m1_l2src(&dst0, &dst1, &p_src_iter, stride, vl);

            vuint8m1_t avg0 = vle8_v_u8m1(p_dst_iter, vl);
            vuint8m1_t avg1 = vle8_v_u8m1(p_dst_iter + stride, vl);
            avg0 = vaaddu_vv_u8m1(avg0, dst0, vl);
            avg1 = vaaddu_vv_u8m1(avg1, dst1, vl);

            vse8_v_u8m1(p_dst_iter, avg0, vl);
            p_dst_iter += stride;
            vse8_v_u8m1(p_dst_iter, avg1, vl);
            p_dst_iter += stride;
        }

        p_dst_iter = p_dst_begin + vl;
        p_src_iter = p_src_begin + vl;
        len -= vl;
    }
}

__attribute__((always_inline)) static void avg_h264_qpel16_h_lowpass_l2src_shift(uint8_t *p_dst, const uint8_t *p_src, int stride)
{
    const uint8_t *p_src_iter = p_src;
    uint8_t *p_dst_iter = p_dst;
    int len = 16;

    while (len > 0)
    {
        int vl = vsetvl_e8m1(len);
        const uint8_t *p_src_begin = p_src_iter;
        uint8_t *p_dst_begin = p_dst_iter;

        for (int j = 0; j < 16; j += 2)
        {
            vuint8m1_t dst0, dst1;
            h_lowpass_u8m1_l2src_shift(&dst0, &dst1, &p_src_iter, stride, vl);

            vuint8m1_t avg0 = vle8_v_u8m1(p_dst_iter, vl);
            vuint8m1_t avg1 = vle8_v_u8m1(p_dst_iter + stride, vl);
            avg0 = vaaddu_vv_u8m1(avg0, dst0, vl);
            avg1 = vaaddu_vv_u8m1(avg1, dst1, vl);

            vse8_v_u8m1(p_dst_iter, avg0, vl);
            p_dst_iter += stride;
            vse8_v_u8m1(p_dst_iter, avg1, vl);
            p_dst_iter += stride;
        }

        p_dst_iter = p_dst_begin + vl;
        p_src_iter = p_src_begin + vl;
        len -= vl;
    }
}

__attribute__((always_inline)) static void avg_h264_qpel16_v_lowpass_l2src_shift(uint8_t *p_dst, const uint8_t *p_src, int stride)
{

    const uint8_t *p_src_iter = p_src;
    uint8_t *p_dst_iter = p_dst;
    int len = 16;

    while (len > 0)
    {
        int vl = vsetvl_e8m1(len);
        const uint8_t *p_src_begin = p_src_iter;
        uint8_t *p_dst_begin = p_dst_iter;

        p_src_iter -= (stride * 2);

        vuint8m1_t row0 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        vuint8m1_t row1 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        vuint8m1_t row2 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        vuint8m1_t row3 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        vuint8m1_t row4 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        vuint8m1_t row5 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        vuint8m1_t row6 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        
        vuint8m1_t dst0, dst1; 
        v_lowpass_u8m1(&dst0, &dst1, row0, row1, row2, row3, row4, row5, row6, vl);
        dst0 = vaaddu_vv_u8m1(dst0, row3, vl);
        dst1 = vaaddu_vv_u8m1(dst1, row4, vl);
        vuint8m1_t avg0 = vle8_v_u8m1(p_dst_iter, vl);
        vuint8m1_t avg1 = vle8_v_u8m1(p_dst_iter + stride, vl);
        avg0 = vaaddu_vv_u8m1(avg0, dst0, vl);
        avg1 = vaaddu_vv_u8m1(avg1, dst1, vl);

        vse8_v_u8m1(p_dst_iter, avg0, vl);
        p_dst_iter += stride;
        vse8_v_u8m1(p_dst_iter, avg1, vl);
        p_dst_iter += stride;

        // 3rd, 4th dst
        row0 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        row1 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;

        v_lowpass_u8m1(&dst0, &dst1, row2, row3, row4, row5, row6, row0, row1, vl);
        dst0 = vaaddu_vv_u8m1(dst0, row5, vl);
        dst1 = vaaddu_vv_u8m1(dst1, row6, vl);
        avg0 = vle8_v_u8m1(p_dst_iter, vl);
        avg1 = vle8_v_u8m1(p_dst_iter + stride, vl);
        avg0 = vaaddu_vv_u8m1(avg0, dst0, vl);
        avg1 = vaaddu_vv_u8m1(avg1, dst1, vl);

        vse8_v_u8m1(p_dst_iter, avg0, vl);
        p_dst_iter += stride;
        vse8_v_u8m1(p_dst_iter, avg1, vl);
        p_dst_iter += stride;

        // 5th, 6th dst
        row2 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        row3 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;

        v_lowpass_u8m1(&dst0, &dst1, row4, row5, row6, row0, row1, row2, row3, vl);
        dst0 = vaaddu_vv_u8m1(dst0, row0, vl);
        dst1 = vaaddu_vv_u8m1(dst1, row1, vl);
        avg0 = vle8_v_u8m1(p_dst_iter, vl);
        avg1 = vle8_v_u8m1(p_dst_iter + stride, vl);
        avg0 = vaaddu_vv_u8m1(avg0, dst0, vl);
        avg1 = vaaddu_vv_u8m1(avg1, dst1, vl);

        vse8_v_u8m1(p_dst_iter, avg0, vl);
        p_dst_iter += stride;
        vse8_v_u8m1(p_dst_iter, avg1, vl);
        p_dst_iter += stride;

        // 7th, 8th dst
        row4 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        row5 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        
        v_lowpass_u8m1(&dst0, &dst1, row6, row0, row1, row2, row3, row4, row5, vl);
        dst0 = vaaddu_vv_u8m1(dst0, row2, vl);
        dst1 = vaaddu_vv_u8m1(dst1, row3, vl);
        avg0 = vle8_v_u8m1(p_dst_iter, vl);
        avg1 = vle8_v_u8m1(p_dst_iter + stride, vl);
        avg0 = vaaddu_vv_u8m1(avg0, dst0, vl);
        avg1 = vaaddu_vv_u8m1(avg1, dst1, vl);

        vse8_v_u8m1(p_dst_iter, avg0, vl);
        p_dst_iter += stride;
        vse8_v_u8m1(p_dst_iter, avg1, vl);
        p_dst_iter += stride;

        // 9th, 10th dst
        row6 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        row0 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;

        v_lowpass_u8m1(&dst0, &dst1, row1, row2, row3, row4, row5, row6, row0, vl);
        dst0 = vaaddu_vv_u8m1(dst0, row4, vl);
        dst1 = vaaddu_vv_u8m1(dst1, row5, vl);
        avg0 = vle8_v_u8m1(p_dst_iter, vl);
        avg1 = vle8_v_u8m1(p_dst_iter + stride, vl);
        avg0 = vaaddu_vv_u8m1(avg0, dst0, vl);
        avg1 = vaaddu_vv_u8m1(avg1, dst1, vl);

        vse8_v_u8m1(p_dst_iter, avg0, vl);
        p_dst_iter += stride;
        vse8_v_u8m1(p_dst_iter, avg1, vl);
        p_dst_iter += stride;

        // 11th, 12th dst
        row1 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        row2 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;

        v_lowpass_u8m1(&dst0, &dst1, row3, row4, row5, row6, row0, row1, row2, vl);
        dst0 = vaaddu_vv_u8m1(dst0, row6, vl);
        dst1 = vaaddu_vv_u8m1(dst1, row0, vl);
        avg0 = vle8_v_u8m1(p_dst_iter, vl);
        avg1 = vle8_v_u8m1(p_dst_iter + stride, vl);
        avg0 = vaaddu_vv_u8m1(avg0, dst0, vl);
        avg1 = vaaddu_vv_u8m1(avg1, dst1, vl);

        vse8_v_u8m1(p_dst_iter, avg0, vl);
        p_dst_iter += stride;
        vse8_v_u8m1(p_dst_iter, avg1, vl);
        p_dst_iter += stride;

        // 13th, 14th dst
        row3 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        row4 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;

        v_lowpass_u8m1(&dst0, &dst1, row5, row6, row0, row1, row2, row3, row4, vl);
        dst0 = vaaddu_vv_u8m1(dst0, row1, vl);
        dst1 = vaaddu_vv_u8m1(dst1, row2, vl);
        avg0 = vle8_v_u8m1(p_dst_iter, vl);
        avg1 = vle8_v_u8m1(p_dst_iter + stride, vl);
        avg0 = vaaddu_vv_u8m1(avg0, dst0, vl);
        avg1 = vaaddu_vv_u8m1(avg1, dst1, vl);

        vse8_v_u8m1(p_dst_iter, avg0, vl);
        p_dst_iter += stride;
        vse8_v_u8m1(p_dst_iter, avg1, vl);
        p_dst_iter += stride;

        // 15th, 16th dst
        row5 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;
        row6 = vle8_v_u8m1(p_src_iter, vl);
        p_src_iter += stride;

        v_lowpass_u8m1(&dst0, &dst1, row0, row1, row2, row3, row4, row5, row6, vl);
        dst0 = vaaddu_vv_u8m1(dst0, row3, vl);
        dst1 = vaaddu_vv_u8m1(dst1, row4, vl);
        avg0 = vle8_v_u8m1(p_dst_iter, vl);
        avg1 = vle8_v_u8m1(p_dst_iter + stride, vl);
        avg0 = vaaddu_vv_u8m1(avg0, dst0, vl);
        avg1 = vaaddu_vv_u8m1(avg1, dst1, vl);

        vse8_v_u8m1(p_dst_iter, avg0, vl);
        p_dst_iter += stride;
        vse8_v_u8m1(p_dst_iter, avg1, vl);

        p_dst_iter = p_dst_begin + vl;
        p_src_iter = p_src_begin + vl;
        len -= vl;       
    }
}