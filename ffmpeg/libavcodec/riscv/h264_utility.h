#ifndef _UTILITY_H_
#define _UTILITY_H_
#include <riscv_vector.h>

__attribute__((always_inline)) static void u32_to_u8(vuint8mf2_t *p_u8_1, vuint8mf2_t *p_u8_2,
                                                     vuint32m2_t i32_1, vuint32m2_t i32_2, int vl)
{
    vuint16m1_t u16_1 = vnclipu_wx_u16m1(i32_1, 10, vl);
    vuint16m1_t u16_2 = vnclipu_wx_u16m1(i32_2, 10, vl);
    *p_u8_1 = vnclipu_wx_u8mf2(u16_1, 0, vl);
    *p_u8_2 = vnclipu_wx_u8mf2(u16_2, 0, vl);
}

__attribute__((always_inline)) static void put_average_l2(uint8_t **pp_dst, uint8_t **pp_l2_src,
                                                          ptrdiff_t dst_stride, ptrdiff_t l2_stride,
                                                          vuint8mf2_t src_row0, vuint8mf2_t src_row1, int vl)
{
    vuint8mf2_t l2_row0 = vle8_v_u8mf2(*pp_l2_src, vl);
    *pp_l2_src += l2_stride;
    vuint8mf2_t l2_row1 = vle8_v_u8mf2(*pp_l2_src, vl);
    *pp_l2_src += l2_stride;

    src_row0 = vaaddu_vv_u8mf2(src_row0, l2_row0, vl);
    src_row1 = vaaddu_vv_u8mf2(src_row1, l2_row1, vl);

    vse8_v_u8mf2(*pp_dst, src_row0, vl);
    *pp_dst += dst_stride;
    vse8_v_u8mf2(*pp_dst, src_row1, vl);
    *pp_dst += dst_stride;
}

__attribute__((always_inline)) static void avg_average_l2(uint8_t **pp_dst, uint8_t **pp_l2_src,
                                                          ptrdiff_t dst_stride, ptrdiff_t l2_stride,
                                                          vuint8mf2_t src_row0, vuint8mf2_t src_row1, int vl)
{
    vuint8mf2_t l2_row0 = vle8_v_u8mf2(*pp_l2_src, vl);
    *pp_l2_src += l2_stride;
    vuint8mf2_t l2_row1 = vle8_v_u8mf2(*pp_l2_src, vl);
    *pp_l2_src += l2_stride;

    vuint8mf2_t dst0 = vle8_v_u8mf2(*pp_dst, vl);
    vuint8mf2_t dst1 = vle8_v_u8mf2(*pp_dst + dst_stride, vl);

    src_row0 = vaaddu_vv_u8mf2(src_row0, l2_row0, vl);
    src_row1 = vaaddu_vv_u8mf2(src_row1, l2_row1, vl);

    src_row0 = vaaddu_vv_u8mf2(src_row0, dst0, vl);
    src_row1 = vaaddu_vv_u8mf2(src_row1, dst1, vl);

    vse8_v_u8mf2(*pp_dst, src_row0, vl);
    *pp_dst += dst_stride;
    vse8_v_u8mf2(*pp_dst, src_row1, vl);
    *pp_dst += dst_stride;
}
#endif