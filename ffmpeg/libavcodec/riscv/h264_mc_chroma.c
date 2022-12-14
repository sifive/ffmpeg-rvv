#include "h264_mc_chroma.h"
#if HAVE_RVV
#include <riscv_vector.h>
typedef unsigned char pixel;

__attribute__((always_inline)) static void h264_put_chroma_unroll4(uint8_t *p_dst, uint8_t *p_src, ptrdiff_t stride, int w, int h, int x, int y)
{
    uint8_t *p_dst_iter = p_dst;
    uint8_t *p_src_iter = p_src;

    const int xy = x * y;
    const int x8 = x << 3;
    const int y8 = y << 3;
    const int a = 64 - x8 - y8 + xy;
    const int b = x8 - xy;
    const int c = y8 -xy;
    const int d = xy;

    int vl = vsetvl_e8m1(w);

    if (d != 0)
    {
        for (int j = 0; j < h; j += 4)
        {
            // dst 1st row
            vuint8m1_t row00 = vle8_v_u8m1(p_src_iter, vl + 1);

            vuint8m1_t row01;
            row01 = vslidedown_vx_u8m1(row01, row00, 1, vl + 1);

            vuint16m2_t dst0 = vwmulu_vx_u16m2(row00, a, vl);
            dst0 = vwmaccu_vx_u16m2(dst0, b, row01, vl);

            vuint8m1_t row10 = vle8_v_u8m1(p_src_iter + stride, vl + 1);
            dst0 = vwmaccu_vx_u16m2(dst0, c, row10, vl);

            vuint8m1_t row11;
            row11 = vslidedown_vx_u8m1(row11, row10, 1, vl + 1);
            dst0 = vwmaccu_vx_u16m2(dst0, d, row11, vl);

            // dst 2nd row
            p_src_iter += (stride << 1);

            vuint16m2_t dst1 = vwmulu_vx_u16m2(row10, a, vl);
            dst1 = vwmaccu_vx_u16m2(dst1, b, row11, vl);

            vuint8m1_t row20 = vle8_v_u8m1(p_src_iter, vl + 1);
            dst1 = vwmaccu_vx_u16m2(dst1, c, row20, vl);

            vuint8m1_t row21;
            row21 = vslidedown_vx_u8m1(row21, row20, 1, vl + 1);
            dst1 = vwmaccu_vx_u16m2(dst1, d, row21, vl);

            // dst 3rd row
            p_src_iter += stride;

            vuint16m2_t dst2 = vwmulu_vx_u16m2(row20, a, vl);
            dst2 = vwmaccu_vx_u16m2(dst2, b, row21, vl);

            vuint8m1_t row30 = vle8_v_u8m1(p_src_iter, vl + 1);
            dst2 = vwmaccu_vx_u16m2(dst2, c, row30, vl);

            vuint8m1_t row31;
            row31 = vslidedown_vx_u8m1(row31, row30, 1, vl + 1);
            dst2 = vwmaccu_vx_u16m2(dst2, d, row31, vl);

            // dst 4rd row
            p_src_iter += stride;

            vuint16m2_t dst3 = vwmulu_vx_u16m2(row30, a, vl);
            dst3 = vwmaccu_vx_u16m2(dst3, b, row31, vl);

            vuint8m1_t row40 = vle8_v_u8m1(p_src_iter, vl + 1);
            dst3 = vwmaccu_vx_u16m2(dst3, c, row40, vl);

            vuint8m1_t row41;
            row41 = vslidedown_vx_u8m1(row41, row40, 1, vl + 1);
            dst3 = vwmaccu_vx_u16m2(dst3, d, row41, vl);

            vse8_v_u8m1(p_dst_iter, vnclipu_wx_u8m1(dst0, 6, vl), vl);
            p_dst_iter += stride;

            vse8_v_u8m1(p_dst_iter, vnclipu_wx_u8m1(dst1, 6, vl), vl);
            p_dst_iter += stride;

            vse8_v_u8m1(p_dst_iter, vnclipu_wx_u8m1(dst2, 6, vl), vl);
            p_dst_iter += stride;

            vse8_v_u8m1(p_dst_iter, vnclipu_wx_u8m1(dst3, 6, vl), vl);
            p_dst_iter += stride;
        }
    }
    else if (b == 0 && c != 0)
    {
        const unsigned short e = b + c;

        for (int j = 0; j < h; j += 4)
        {
            vuint8m1_t row0 = vle8_v_u8m1(p_src_iter, vl);
            vuint16m2_t dst0 = vwmulu_vx_u16m2(row0, a, vl);
            p_src_iter += stride;

            vuint8m1_t row1 = vle8_v_u8m1(p_src_iter, vl);
            dst0 = vwmaccu_vx_u16m2(dst0, e, row1, vl);
            p_src_iter += stride;

            vuint8m1_t row2 = vle8_v_u8m1(p_src_iter, vl);
            vuint16m2_t dst1 = vwmulu_vx_u16m2(row1, a, vl);
            dst1 = vwmaccu_vx_u16m2(dst1, e, row2, vl);
            p_src_iter += stride;

            vuint8m1_t row3 = vle8_v_u8m1(p_src_iter, vl);
            vuint16m2_t dst2 = vwmulu_vx_u16m2(row2, a, vl);
            dst2 = vwmaccu_vx_u16m2(dst2, e, row3, vl);
            p_src_iter += stride;

            vuint8m1_t row4 = vle8_v_u8m1(p_src_iter, vl);
            vuint16m2_t dst3 = vwmulu_vx_u16m2(row3, a, vl);
            dst3 = vwmaccu_vx_u16m2(dst3, e, row4, vl);

            vse8_v_u8m1(p_dst_iter, vnclipu_wx_u8m1(dst0, 6, vl), vl);
            p_dst_iter += stride;

            vse8_v_u8m1(p_dst_iter, vnclipu_wx_u8m1(dst1, 6, vl), vl);
            p_dst_iter += stride;

            vse8_v_u8m1(p_dst_iter, vnclipu_wx_u8m1(dst2, 6, vl), vl);
            p_dst_iter += stride;

            vse8_v_u8m1(p_dst_iter, vnclipu_wx_u8m1(dst3, 6, vl), vl);
            p_dst_iter += stride;
        }
    }
    else if (b !=0 && c == 0)
    {
        const unsigned short e = b + c;

        for (int j = 0; j < h; j += 4)
        {
            // 1st
            vuint8m1_t row00 = vle8_v_u8m1(p_src_iter, vl + 1);
            p_src_iter += stride;

            vuint8m1_t row01;
            row01 = vslidedown_vx_u8m1(row01, row00, 1, vl + 1);

            vuint16m2_t dst0 = vwmulu_vx_u16m2(row00, a, vl);
            dst0 = vwmaccu_vx_u16m2(dst0, e, row01, vl);

            // 2nd
            vuint8m1_t row10 = vle8_v_u8m1(p_src_iter, vl + 1);
            p_src_iter += stride;

            vuint8m1_t row11;
            row11 = vslidedown_vx_u8m1(row11, row10, 1, vl + 1);

            vuint16m2_t dst1 = vwmulu_vx_u16m2(row10, a, vl);
            dst1 = vwmaccu_vx_u16m2(dst1, e, row11, vl);

            // 3rd
            vuint8m1_t row20 = vle8_v_u8m1(p_src_iter, vl + 1);
            p_src_iter += stride;

            vuint8m1_t row21;
            row21 = vslidedown_vx_u8m1(row21, row20, 1, vl + 1);

            vuint16m2_t dst2 = vwmulu_vx_u16m2(row20, a, vl);
            dst2 = vwmaccu_vx_u16m2(dst2, e, row21, vl);

            // 3rd
            vuint8m1_t row30 = vle8_v_u8m1(p_src_iter, vl + 1);
            p_src_iter += stride;

            vuint8m1_t row31;
            row31 = vslidedown_vx_u8m1(row31, row30, 1, vl + 1);

            vuint16m2_t dst3 = vwmulu_vx_u16m2(row30, a, vl);
            dst3 = vwmaccu_vx_u16m2(dst3, e, row31, vl);

            vse8_v_u8m1(p_dst_iter, vnclipu_wx_u8m1(dst0, 6, vl), vl);
            p_dst_iter += stride;

            vse8_v_u8m1(p_dst_iter, vnclipu_wx_u8m1(dst1, 6, vl), vl);
            p_dst_iter += stride;

            vse8_v_u8m1(p_dst_iter, vnclipu_wx_u8m1(dst2, 6, vl), vl);
            p_dst_iter += stride;

            vse8_v_u8m1(p_dst_iter, vnclipu_wx_u8m1(dst3, 6, vl), vl);
            p_dst_iter += stride;
        }
    }
    else
    {
        for (int j = 0; j < h; j += 4)
        {
            vuint8m1_t row0 = vle8_v_u8m1(p_src_iter, vl);
            p_src_iter += stride;
            vuint16m2_t dst0 = vwmulu_vx_u16m2(row0, a, vl);

            vuint8m1_t row1 = vle8_v_u8m1(p_src_iter, vl);
            p_src_iter += stride;
            vuint16m2_t dst1 = vwmulu_vx_u16m2(row1, a, vl);

            vuint8m1_t row2 = vle8_v_u8m1(p_src_iter, vl);
            p_src_iter += stride;
            vuint16m2_t dst2 = vwmulu_vx_u16m2(row2, a, vl);

            vuint8m1_t row3 = vle8_v_u8m1(p_src_iter, vl);
            p_src_iter += stride;
            vuint16m2_t dst3 = vwmulu_vx_u16m2(row3, a, vl);

            vse8_v_u8m1(p_dst_iter, vnclipu_wx_u8m1(dst0, 6, vl), vl);
            p_dst_iter += stride;

            vse8_v_u8m1(p_dst_iter, vnclipu_wx_u8m1(dst1, 6, vl), vl);
            p_dst_iter += stride;

            vse8_v_u8m1(p_dst_iter, vnclipu_wx_u8m1(dst2, 6, vl), vl);
            p_dst_iter += stride;

            vse8_v_u8m1(p_dst_iter, vnclipu_wx_u8m1(dst3, 6, vl), vl);
            p_dst_iter += stride;
        }
    }
}

__attribute__((always_inline)) static void h264_put_chroma_unroll2(uint8_t *p_dst, uint8_t *p_src, ptrdiff_t stride, int w, int h, int x, int y)
{
    uint8_t *p_dst_iter = p_dst;
    uint8_t *p_src_iter = p_src;

    const int xy = x * y;
    const int x8 = x << 3;
    const int y8 = y << 3;
    const int a = 64 - x8 - y8 + xy;
    const int b = x8 - xy;
    const int c = y8 -xy;
    const int d = xy;

    int vl = vsetvl_e8m1(w);

    if (d != 0)
    {
        for (int j = 0; j < h; j += 2)
        {
            // dst 1st row
            vuint8m1_t row00 = vle8_v_u8m1(p_src_iter, vl + 1);

            vuint8m1_t row01;
            row01 = vslidedown_vx_u8m1(row01, row00, 1, vl + 1);

            vuint16m2_t dst0 = vwmulu_vx_u16m2(row00, a, vl);
            dst0 = vwmaccu_vx_u16m2(dst0, b, row01, vl);

            vuint8m1_t row10 = vle8_v_u8m1(p_src_iter + stride, vl + 1);
            dst0 = vwmaccu_vx_u16m2(dst0, c, row10, vl);

            vuint8m1_t row11;
            row11 = vslidedown_vx_u8m1(row11, row10, 1, vl + 1);
            dst0 = vwmaccu_vx_u16m2(dst0, d, row11, vl);

            // dst 2nd row
            p_src_iter += (stride << 1);

            vuint16m2_t dst1 = vwmulu_vx_u16m2(row10, a, vl);
            dst1 = vwmaccu_vx_u16m2(dst1, b, row11, vl);

            vuint8m1_t row20 = vle8_v_u8m1(p_src_iter, vl + 1);
            dst1 = vwmaccu_vx_u16m2(dst1, c, row20, vl);

            vuint8m1_t row21;
            row21 = vslidedown_vx_u8m1(row21, row20, 1, vl + 1);
            dst1 = vwmaccu_vx_u16m2(dst1, d, row21, vl);

            vse8_v_u8m1(p_dst_iter, vnclipu_wx_u8m1(dst0, 6, vl), vl);
            p_dst_iter += stride;

            vse8_v_u8m1(p_dst_iter, vnclipu_wx_u8m1(dst1, 6, vl), vl);
            p_dst_iter += stride;
        }
    }
    else if (b == 0 && c != 0)
    {
        const unsigned short e = b + c;

        for (int j = 0; j < h; j += 2)
        {
            vuint8m1_t row0 = vle8_v_u8m1(p_src_iter, vl);
            vuint16m2_t dst0 = vwmulu_vx_u16m2(row0, a, vl);
            p_src_iter += stride;

            vuint8m1_t row1 = vle8_v_u8m1(p_src_iter, vl);
            dst0 = vwmaccu_vx_u16m2(dst0, e, row1, vl);
            p_src_iter += stride;

            vuint8m1_t row2 = vle8_v_u8m1(p_src_iter, vl);
            vuint16m2_t dst1 = vwmulu_vx_u16m2(row1, a, vl);
            dst1 = vwmaccu_vx_u16m2(dst1, e, row2, vl);
            p_src_iter += stride;

            vse8_v_u8m1(p_dst_iter, vnclipu_wx_u8m1(dst0, 6, vl), vl);
            p_dst_iter += stride;

            vse8_v_u8m1(p_dst_iter, vnclipu_wx_u8m1(dst1, 6, vl), vl);
            p_dst_iter += stride;
        }
    }
    else if (b !=0 && c == 0)
    {
        const unsigned short e = b + c;

        for (int j = 0; j < h; j += 2)
        {
            // 1st
            vuint8m1_t row00 = vle8_v_u8m1(p_src_iter, vl + 1);
            p_src_iter += stride;

            vuint8m1_t row01;
            row01 = vslidedown_vx_u8m1(row01, row00, 1, vl + 1);

            vuint16m2_t dst0 = vwmulu_vx_u16m2(row00, a, vl);
            dst0 = vwmaccu_vx_u16m2(dst0, e, row01, vl);

            // 2nd
            vuint8m1_t row10 = vle8_v_u8m1(p_src_iter, vl + 1);
            p_src_iter += stride;

            vuint8m1_t row11;
            row11 = vslidedown_vx_u8m1(row11, row10, 1, vl + 1);

            vuint16m2_t dst1 = vwmulu_vx_u16m2(row10, a, vl);
            dst1 = vwmaccu_vx_u16m2(dst1, e, row11, vl);

            vse8_v_u8m1(p_dst_iter, vnclipu_wx_u8m1(dst0, 6, vl), vl);
            p_dst_iter += stride;

            vse8_v_u8m1(p_dst_iter, vnclipu_wx_u8m1(dst1, 6, vl), vl);
            p_dst_iter += stride;
        }
    }
    else
    {
        for (int j = 0; j < h; j += 2)
        {
            vuint8m1_t row0 = vle8_v_u8m1(p_src_iter, vl);
            p_src_iter += stride;
            vuint16m2_t dst0 = vwmulu_vx_u16m2(row0, a, vl);

            vuint8m1_t row1 = vle8_v_u8m1(p_src_iter, vl);
            p_src_iter += stride;
            vuint16m2_t dst1 = vwmulu_vx_u16m2(row1, a, vl);

            vse8_v_u8m1(p_dst_iter, vnclipu_wx_u8m1(dst0, 6, vl), vl);
            p_dst_iter += stride;

            vse8_v_u8m1(p_dst_iter, vnclipu_wx_u8m1(dst1, 6, vl), vl);
            p_dst_iter += stride;
        }
    }
}

__attribute__((always_inline)) static void h264_avg_chroma_unroll4(uint8_t *p_dst, uint8_t *p_src, ptrdiff_t stride, int w, int h, int x, int y)
{
    uint8_t *p_dst_iter = p_dst;
    uint8_t *p_src_iter = p_src;

    const int xy = x * y;
    const int x8 = x << 3;
    const int y8 = y << 3;
    const int a = 64 - x8 - y8 + xy;
    const int b = x8 - xy;
    const int c = y8 - xy;
    const int d = xy;

    int vl = vsetvl_e8m1(w);

    if (d != 0)
    {
        for (int j = 0; j < h; j += 4)
        {
            // dst 1st row
            vuint8m1_t row00 = vle8_v_u8m1(p_src_iter, vl + 1);

            vuint8m1_t row01;
            row01 = vslidedown_vx_u8m1(row01, row00, 1, vl + 1);

            vuint16m2_t dst0 = vwmulu_vx_u16m2(row00, a, vl);
            dst0 = vwmaccu_vx_u16m2(dst0, b, row01, vl);

            vuint8m1_t row10 = vle8_v_u8m1(p_src_iter + stride, vl + 1);
            dst0 = vwmaccu_vx_u16m2(dst0, c, row10, vl);

            vuint8m1_t row11;
            row11 = vslidedown_vx_u8m1(row11, row10, 1, vl + 1);
            dst0 = vwmaccu_vx_u16m2(dst0, d, row11, vl);

            // dst 2nd row
            p_src_iter += (stride << 1);

            vuint16m2_t dst1 = vwmulu_vx_u16m2(row10, a, vl);
            dst1 = vwmaccu_vx_u16m2(dst1, b, row11, vl);

            vuint8m1_t row20 = vle8_v_u8m1(p_src_iter, vl + 1);
            dst1 = vwmaccu_vx_u16m2(dst1, c, row20, vl);

            vuint8m1_t row21;
            row21 = vslidedown_vx_u8m1(row21, row20, 1, vl + 1);
            dst1 = vwmaccu_vx_u16m2(dst1, d, row21, vl);

            // dst 3rd row
            p_src_iter += stride;

            vuint16m2_t dst2 = vwmulu_vx_u16m2(row20, a, vl);
            dst2 = vwmaccu_vx_u16m2(dst2, b, row21, vl);

            vuint8m1_t row30 = vle8_v_u8m1(p_src_iter, vl + 1);
            dst2 = vwmaccu_vx_u16m2(dst2, c, row30, vl);

            vuint8m1_t row31;
            row31 = vslidedown_vx_u8m1(row31, row30, 1, vl + 1);
            dst2 = vwmaccu_vx_u16m2(dst2, d, row31, vl);

            // dst 4rd row
            p_src_iter += stride;

            vuint16m2_t dst3 = vwmulu_vx_u16m2(row30, a, vl);
            dst3 = vwmaccu_vx_u16m2(dst3, b, row31, vl);

            vuint8m1_t row40 = vle8_v_u8m1(p_src_iter, vl + 1);
            dst3 = vwmaccu_vx_u16m2(dst3, c, row40, vl);

            vuint8m1_t row41;
            row41 = vslidedown_vx_u8m1(row41, row40, 1, vl + 1);
            dst3 = vwmaccu_vx_u16m2(dst3, d, row41, vl);

            vuint8m1_t avg0 = vaaddu_vv_u8m1(vnclipu_wx_u8m1(dst0, 6, vl), vle8_v_u8m1(p_dst_iter, vl), vl);
            vse8_v_u8m1(p_dst_iter, avg0, vl);
            p_dst_iter += stride;

            vuint8m1_t avg1 = vaaddu_vv_u8m1(vnclipu_wx_u8m1(dst1, 6, vl), vle8_v_u8m1(p_dst_iter, vl), vl);
            vse8_v_u8m1(p_dst_iter, avg1, vl);
            p_dst_iter += stride;

            vuint8m1_t avg2 = vaaddu_vv_u8m1(vnclipu_wx_u8m1(dst2, 6, vl), vle8_v_u8m1(p_dst_iter, vl), vl);
            vse8_v_u8m1(p_dst_iter, avg2, vl);
            p_dst_iter += stride;

            vuint8m1_t avg3 = vaaddu_vv_u8m1(vnclipu_wx_u8m1(dst3, 6, vl), vle8_v_u8m1(p_dst_iter, vl), vl);
            vse8_v_u8m1(p_dst_iter, avg3, vl);
            p_dst_iter += stride;
        }
    }
    else if (b == 0 && c != 0)
    {
        const unsigned short e = b + c;

        for (int j = 0; j < h; j += 4)
        {
            vuint8m1_t row0 = vle8_v_u8m1(p_src_iter, vl);
            vuint16m2_t dst0 = vwmulu_vx_u16m2(row0, a, vl);
            p_src_iter += stride;

            vuint8m1_t row1 = vle8_v_u8m1(p_src_iter, vl);
            dst0 = vwmaccu_vx_u16m2(dst0, e, row1, vl);
            p_src_iter += stride;

            vuint8m1_t row2 = vle8_v_u8m1(p_src_iter, vl);
            vuint16m2_t dst1 = vwmulu_vx_u16m2(row1, a, vl);
            dst1 = vwmaccu_vx_u16m2(dst1, e, row2, vl);
            p_src_iter += stride;

            vuint8m1_t row3 = vle8_v_u8m1(p_src_iter, vl);
            vuint16m2_t dst2 = vwmulu_vx_u16m2(row2, a, vl);
            dst2 = vwmaccu_vx_u16m2(dst2, e, row3, vl);
            p_src_iter += stride;

            vuint8m1_t row4 = vle8_v_u8m1(p_src_iter, vl);
            vuint16m2_t dst3 = vwmulu_vx_u16m2(row3, a, vl);
            dst3 = vwmaccu_vx_u16m2(dst3, e, row4, vl);

            vuint8m1_t avg0 = vaaddu_vv_u8m1(vnclipu_wx_u8m1(dst0, 6, vl), vle8_v_u8m1(p_dst_iter, vl), vl);
            vse8_v_u8m1(p_dst_iter, avg0, vl);
            p_dst_iter += stride;

            vuint8m1_t avg1 = vaaddu_vv_u8m1(vnclipu_wx_u8m1(dst1, 6, vl), vle8_v_u8m1(p_dst_iter, vl), vl);
            vse8_v_u8m1(p_dst_iter, avg1, vl);
            p_dst_iter += stride;

            vuint8m1_t avg2 = vaaddu_vv_u8m1(vnclipu_wx_u8m1(dst2, 6, vl), vle8_v_u8m1(p_dst_iter, vl), vl);
            vse8_v_u8m1(p_dst_iter, avg2, vl);
            p_dst_iter += stride;

            vuint8m1_t avg3 = vaaddu_vv_u8m1(vnclipu_wx_u8m1(dst3, 6, vl), vle8_v_u8m1(p_dst_iter, vl), vl);
            vse8_v_u8m1(p_dst_iter, avg3, vl);
            p_dst_iter += stride;
        }
    }
    else if (b != 0 && c == 0)
    {
        const unsigned short e = b + c;

        for (int j = 0; j < h; j += 4)
        {
            // 1st
            vuint8m1_t row00 = vle8_v_u8m1(p_src_iter, vl + 1);
            p_src_iter += stride;

            vuint8m1_t row01;
            row01 = vslidedown_vx_u8m1(row01, row00, 1, vl + 1);

            vuint16m2_t dst0 = vwmulu_vx_u16m2(row00, a, vl);
            dst0 = vwmaccu_vx_u16m2(dst0, e, row01, vl);

            // 2nd
            vuint8m1_t row10 = vle8_v_u8m1(p_src_iter, vl + 1);
            p_src_iter += stride;

            vuint8m1_t row11;
            row11 = vslidedown_vx_u8m1(row11, row10, 1, vl + 1);

            vuint16m2_t dst1 = vwmulu_vx_u16m2(row10, a, vl);
            dst1 = vwmaccu_vx_u16m2(dst1, e, row11, vl);

            // 3rd
            vuint8m1_t row20 = vle8_v_u8m1(p_src_iter, vl + 1);
            p_src_iter += stride;

            vuint8m1_t row21;
            row21 = vslidedown_vx_u8m1(row21, row20, 1, vl + 1);

            vuint16m2_t dst2 = vwmulu_vx_u16m2(row20, a, vl);
            dst2 = vwmaccu_vx_u16m2(dst2, e, row21, vl);

            // 4th
            vuint8m1_t row30 = vle8_v_u8m1(p_src_iter, vl + 1);
            p_src_iter += stride;

            vuint8m1_t row31;
            row31 = vslidedown_vx_u8m1(row31, row30, 1, vl + 1);

            vuint16m2_t dst3 = vwmulu_vx_u16m2(row30, a, vl);
            dst3 = vwmaccu_vx_u16m2(dst3, e, row31, vl);

            vuint8m1_t avg0 = vaaddu_vv_u8m1(vnclipu_wx_u8m1(dst0, 6, vl), vle8_v_u8m1(p_dst_iter, vl), vl);
            vse8_v_u8m1(p_dst_iter, avg0, vl);
            p_dst_iter += stride;

            vuint8m1_t avg1 = vaaddu_vv_u8m1(vnclipu_wx_u8m1(dst1, 6, vl), vle8_v_u8m1(p_dst_iter, vl), vl);
            vse8_v_u8m1(p_dst_iter, avg1, vl);
            p_dst_iter += stride;

            vuint8m1_t avg2 = vaaddu_vv_u8m1(vnclipu_wx_u8m1(dst2, 6, vl), vle8_v_u8m1(p_dst_iter, vl), vl);
            vse8_v_u8m1(p_dst_iter, avg2, vl);
            p_dst_iter += stride;

            vuint8m1_t avg3 = vaaddu_vv_u8m1(vnclipu_wx_u8m1(dst3, 6, vl), vle8_v_u8m1(p_dst_iter, vl), vl);
            vse8_v_u8m1(p_dst_iter, avg3, vl);
            p_dst_iter += stride;
        }
    }
    else
    {
        for (int j = 0; j < h; j += 4)
        {
            vuint8m1_t row0 = vle8_v_u8m1(p_src_iter, vl);
            p_src_iter += stride;
            vuint16m2_t dst0 = vwmulu_vx_u16m2(row0, a, vl);

            vuint8m1_t row1 = vle8_v_u8m1(p_src_iter, vl);
            p_src_iter += stride;
            vuint16m2_t dst1 = vwmulu_vx_u16m2(row1, a, vl);

            vuint8m1_t row2 = vle8_v_u8m1(p_src_iter, vl);
            p_src_iter += stride;
            vuint16m2_t dst2 = vwmulu_vx_u16m2(row2, a, vl);

            vuint8m1_t row3 = vle8_v_u8m1(p_src_iter, vl);
            p_src_iter += stride;
            vuint16m2_t dst3 = vwmulu_vx_u16m2(row3, a, vl);

            vuint8m1_t avg0 = vaaddu_vv_u8m1(vnclipu_wx_u8m1(dst0, 6, vl), vle8_v_u8m1(p_dst_iter, vl), vl);
            vse8_v_u8m1(p_dst_iter, avg0, vl);
            p_dst_iter += stride;

            vuint8m1_t avg1 = vaaddu_vv_u8m1(vnclipu_wx_u8m1(dst1, 6, vl), vle8_v_u8m1(p_dst_iter, vl), vl);
            vse8_v_u8m1(p_dst_iter, avg1, vl);
            p_dst_iter += stride;

            vuint8m1_t avg2 = vaaddu_vv_u8m1(vnclipu_wx_u8m1(dst2, 6, vl), vle8_v_u8m1(p_dst_iter, vl), vl);
            vse8_v_u8m1(p_dst_iter, avg2, vl);
            p_dst_iter += stride;

            vuint8m1_t avg3 = vaaddu_vv_u8m1(vnclipu_wx_u8m1(dst3, 6, vl), vle8_v_u8m1(p_dst_iter, vl), vl);
            vse8_v_u8m1(p_dst_iter, avg3, vl);
            p_dst_iter += stride;
        }
    }
}

__attribute__((always_inline)) static void h264_avg_chroma_unroll2(uint8_t *p_dst, uint8_t *p_src, ptrdiff_t stride, int w, int h, int x, int y)
{
    uint8_t *p_dst_iter = p_dst;
    uint8_t *p_src_iter = p_src;

    const int xy = x * y;
    const int x8 = x << 3;
    const int y8 = y << 3;
    const int a = 64 - x8 - y8 + xy;
    const int b = x8 - xy;
    const int c = y8 - xy;
    const int d = xy;

    int vl = vsetvl_e8m1(w);

    if (d != 0)
    {
        for (int j = 0; j < h; j += 2)
        {
            // dst 1st row
            vuint8m1_t row00 = vle8_v_u8m1(p_src_iter, vl + 1);

            vuint8m1_t row01;
            row01 = vslidedown_vx_u8m1(row01, row00, 1, vl + 1);

            vuint16m2_t dst0 = vwmulu_vx_u16m2(row00, a, vl);
            dst0 = vwmaccu_vx_u16m2(dst0, b, row01, vl);

            vuint8m1_t row10 = vle8_v_u8m1(p_src_iter + stride, vl + 1);
            dst0 = vwmaccu_vx_u16m2(dst0, c, row10, vl);

            vuint8m1_t row11;
            row11 = vslidedown_vx_u8m1(row11, row10, 1, vl + 1);
            dst0 = vwmaccu_vx_u16m2(dst0, d, row11, vl);

            // dst 2nd row
            p_src_iter += (stride << 1);

            vuint16m2_t dst1 = vwmulu_vx_u16m2(row10, a, vl);
            dst1 = vwmaccu_vx_u16m2(dst1, b, row11, vl);

            vuint8m1_t row20 = vle8_v_u8m1(p_src_iter, vl + 1);
            dst1 = vwmaccu_vx_u16m2(dst1, c, row20, vl);

            vuint8m1_t row21;
            row21 = vslidedown_vx_u8m1(row21, row20, 1, vl + 1);
            dst1 = vwmaccu_vx_u16m2(dst1, d, row21, vl);

            vuint8m1_t avg0 = vaaddu_vv_u8m1(vnclipu_wx_u8m1(dst0, 6, vl), vle8_v_u8m1(p_dst_iter, vl), vl);
            vse8_v_u8m1(p_dst_iter, avg0, vl);
            p_dst_iter += stride;

            vuint8m1_t avg1 = vaaddu_vv_u8m1(vnclipu_wx_u8m1(dst1, 6, vl), vle8_v_u8m1(p_dst_iter, vl), vl);
            vse8_v_u8m1(p_dst_iter, avg1, vl);
            p_dst_iter += stride;
        }
    }
    else if (b == 0 && c != 0)
    {
        const unsigned short e = b + c;

        for (int j = 0; j < h; j += 2)
        {
            vuint8m1_t row0 = vle8_v_u8m1(p_src_iter, vl);
            vuint16m2_t dst0 = vwmulu_vx_u16m2(row0, a, vl);
            p_src_iter += stride;

            vuint8m1_t row1 = vle8_v_u8m1(p_src_iter, vl);
            dst0 = vwmaccu_vx_u16m2(dst0, e, row1, vl);
            p_src_iter += stride;

            vuint8m1_t row2 = vle8_v_u8m1(p_src_iter, vl);
            vuint16m2_t dst1 = vwmulu_vx_u16m2(row1, a, vl);
            dst1 = vwmaccu_vx_u16m2(dst1, e, row2, vl);
            p_src_iter += stride;

            vuint8m1_t avg0 = vaaddu_vv_u8m1(vnclipu_wx_u8m1(dst0, 6, vl), vle8_v_u8m1(p_dst_iter, vl), vl);
            vse8_v_u8m1(p_dst_iter, avg0, vl);
            p_dst_iter += stride;

            vuint8m1_t avg1 = vaaddu_vv_u8m1(vnclipu_wx_u8m1(dst1, 6, vl), vle8_v_u8m1(p_dst_iter, vl), vl);
            vse8_v_u8m1(p_dst_iter, avg1, vl);
            p_dst_iter += stride;
        }
    }
    else if (b != 0 && c == 0)
    {
        const unsigned short e = b + c;

        for (int j = 0; j < h; j += 2)
        {
            // 1st
            vuint8m1_t row00 = vle8_v_u8m1(p_src_iter, vl + 1);
            p_src_iter += stride;

            vuint8m1_t row01;
            row01 = vslidedown_vx_u8m1(row01, row00, 1, vl + 1);

            vuint16m2_t dst0 = vwmulu_vx_u16m2(row00, a, vl);
            dst0 = vwmaccu_vx_u16m2(dst0, e, row01, vl);

            // 2nd
            vuint8m1_t row10 = vle8_v_u8m1(p_src_iter, vl + 1);
            p_src_iter += stride;

            vuint8m1_t row11;
            row11 = vslidedown_vx_u8m1(row11, row10, 1, vl + 1);

            vuint16m2_t dst1 = vwmulu_vx_u16m2(row10, a, vl);
            dst1 = vwmaccu_vx_u16m2(dst1, e, row11, vl);

            vuint8m1_t avg0 = vaaddu_vv_u8m1(vnclipu_wx_u8m1(dst0, 6, vl), vle8_v_u8m1(p_dst_iter, vl), vl);
            vse8_v_u8m1(p_dst_iter, avg0, vl);
            p_dst_iter += stride;

            vuint8m1_t avg1 = vaaddu_vv_u8m1(vnclipu_wx_u8m1(dst1, 6, vl), vle8_v_u8m1(p_dst_iter, vl), vl);
            vse8_v_u8m1(p_dst_iter, avg1, vl);
            p_dst_iter += stride;
        }
    }
    else
    {
        for (int j = 0; j < h; j += 2)
        {
            vuint8m1_t row0 = vle8_v_u8m1(p_src_iter, vl);
            p_src_iter += stride;
            vuint16m2_t dst0 = vwmulu_vx_u16m2(row0, a, vl);

            vuint8m1_t row1 = vle8_v_u8m1(p_src_iter, vl);
            p_src_iter += stride;
            vuint16m2_t dst1 = vwmulu_vx_u16m2(row1, a, vl);

            vuint8m1_t avg0 = vaaddu_vv_u8m1(vnclipu_wx_u8m1(dst0, 6, vl), vle8_v_u8m1(p_dst_iter, vl), vl);
            vse8_v_u8m1(p_dst_iter, avg0, vl);
            p_dst_iter += stride;

            vuint8m1_t avg1 = vaaddu_vv_u8m1(vnclipu_wx_u8m1(dst1, 6, vl), vle8_v_u8m1(p_dst_iter, vl), vl);
            vse8_v_u8m1(p_dst_iter, avg1, vl);
            p_dst_iter += stride;
        }
    }
}

void h264_put_chroma_mc8_rvv(uint8_t *p_dst, uint8_t *p_src, ptrdiff_t stride, int h, int x, int y)
{
    h264_put_chroma_unroll4(p_dst, p_src, stride, 8, h, x, y);
}

void h264_avg_chroma_mc8_rvv(uint8_t *p_dst, uint8_t *p_src, ptrdiff_t stride, int h, int x, int y)
{
    h264_avg_chroma_unroll4(p_dst, p_src, stride, 8, h, x, y);
}

void h264_put_chroma_mc4_rvv(uint8_t *p_dst, uint8_t *p_src, ptrdiff_t stride, int h, int x, int y)
{
    if (h >= 4)
    {
        h264_put_chroma_unroll4(p_dst, p_src, stride, 4, h, x, y);
    }
    else
    {
        h264_put_chroma_unroll2(p_dst, p_src, stride, 4, h, x, y);
    }
}

void h264_avg_chroma_mc4_rvv(uint8_t *p_dst, uint8_t *p_src, ptrdiff_t stride, int h, int x, int y)
{
    if (h >= 4)
    {
        h264_avg_chroma_unroll4(p_dst, p_src, stride, 4, h, x, y);
    }
    else
    {
        h264_avg_chroma_unroll2(p_dst, p_src, stride, 4, h, x, y);
    }
}

void h264_put_chroma_mc2_rvv(uint8_t *p_dst, uint8_t *p_src, ptrdiff_t stride, int h, int x, int y)
{
    if (h >= 4)
    {
        h264_put_chroma_unroll4(p_dst, p_src, stride, 2, h, x, y);
    }
    else
    {
        h264_put_chroma_unroll2(p_dst, p_src, stride, 2, h, x, y);
    }
}

void h264_avg_chroma_mc2_rvv(uint8_t *p_dst, uint8_t *p_src, ptrdiff_t stride, int h, int x, int y)
{
    if (h >= 4)
    {
        h264_avg_chroma_unroll4(p_dst, p_src, stride, 2, h, x, y);
    }
    else
    {
        h264_avg_chroma_unroll2(p_dst, p_src, stride, 2, h, x, y);
    }
}
#endif
