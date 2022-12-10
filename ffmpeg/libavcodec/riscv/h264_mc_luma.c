#include "h264_mc_luma.h"
#if HAVE_RVV
#include <riscv_vector.h>
#include "h264_mc_luma_put16.h"
#include "h264_mc_luma_avg16.h"
#include "h264_mc_luma_put8.h"
#include "h264_mc_luma_avg8.h"

void put_h264_qpel16_mc00_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    put_copy_block16(p_dst, p_src, stride);
}

void put_h264_qpel16_mc01_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    put_h264_qpel16_v_lowpass_l2src(p_dst, p_src, stride);
}

void put_h264_qpel16_mc02_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    put_h264_qpel16_v_lowpass(p_dst, p_src, stride, stride);
}

void put_h264_qpel16_mc03_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    put_h264_qpel16_v_lowpass_l2src_shift(p_dst, p_src, stride);
}

void put_h264_qpel16_mc10_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    put_h264_qpel16_h_lowpass_l2src(p_dst, p_src, stride);
}

void put_h264_qpel16_mc11_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t temp[256] = {0};
    put_h264_qpel16_h_lowpass(temp, p_src, 16, stride);
    put_h264_qpel16_v_lowpass_l2(p_dst, p_src, temp, stride, 16);
}

void put_h264_qpel16_mc12_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t temp[256] = {0};
    put_h264_qpel16_v_lowpass(temp, p_src, 16, stride);
    put_h264_qpel16_hv_lowpass_l2(p_dst, p_src, temp, stride, 16);
}

void put_h264_qpel16_mc13_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t temp[256] = {0};
    put_h264_qpel16_h_lowpass(temp, p_src + stride, 16, stride);
    put_h264_qpel16_v_lowpass_l2(p_dst, p_src, temp, stride, 16);
}

void put_h264_qpel16_mc20_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    put_h264_qpel16_h_lowpass(p_dst, p_src, stride, stride);
}

void put_h264_qpel16_mc21_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t temp[256] = {0};
    put_h264_qpel16_h_lowpass(temp, p_src, 16, stride);
    put_h264_qpel16_hv_lowpass_l2(p_dst, p_src, temp, stride, 16);
}

void put_h264_qpel16_mc22_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    put_h264_qpel16_hv_lowpass(p_dst, p_src, stride);
}

void put_h264_qpel16_mc23_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t temp[256] = {0};
    put_h264_qpel16_h_lowpass(temp, p_src + stride, 16, stride);
    put_h264_qpel16_hv_lowpass_l2(p_dst, p_src, temp, stride, 16);
}

void put_h264_qpel16_mc30_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    put_h264_qpel16_h_lowpass_l2src_shift(p_dst, p_src, stride);
}

void put_h264_qpel16_mc31_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t temp[256] = {0};
    put_h264_qpel16_h_lowpass(temp, p_src, 16, stride);
    put_h264_qpel16_v_lowpass_l2(p_dst, p_src + 1, temp, stride, 16);    
}

void put_h264_qpel16_mc32_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t temp[256] = {0};
    put_h264_qpel16_v_lowpass(temp, p_src + 1, 16, stride);
    put_h264_qpel16_hv_lowpass_l2(p_dst, p_src, temp, stride, 16);
}

void put_h264_qpel16_mc33_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t temp[256] = {0};
    put_h264_qpel16_h_lowpass(temp, p_src + stride, 16, stride);
    put_h264_qpel16_v_lowpass_l2(p_dst, p_src + 1, temp, stride, 16);
}

void avg_h264_qpel16_mc00_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    avg_copy_block16(p_dst, p_src, stride);
}

void avg_h264_qpel16_mc01_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    avg_h264_qpel16_v_lowpass_l2src(p_dst, p_src, stride);
}

void avg_h264_qpel16_mc02_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    avg_h264_qpel16_v_lowpass(p_dst, p_src, stride, stride);
}

void avg_h264_qpel16_mc03_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    avg_h264_qpel16_v_lowpass_l2src_shift(p_dst, p_src, stride);
}

void avg_h264_qpel16_mc10_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    avg_h264_qpel16_h_lowpass_l2src(p_dst, p_src, stride);
}

void avg_h264_qpel16_mc11_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t temp[256] = {0};
    put_h264_qpel16_h_lowpass(temp, p_src, 16, stride);
    avg_h264_qpel16_v_lowpass_l2(p_dst, p_src, temp, stride, 16);
}

void avg_h264_qpel16_mc12_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t temp[256] = {0};
    put_h264_qpel16_v_lowpass(temp, p_src, 16, stride);
    avg_h264_qpel16_hv_lowpass_l2(p_dst, p_src, temp, stride, 16);
}

void avg_h264_qpel16_mc13_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t temp[256] = {0};
    put_h264_qpel16_h_lowpass(temp, p_src + stride, 16, stride);
    avg_h264_qpel16_v_lowpass_l2(p_dst, p_src, temp, stride, 16);
}

void avg_h264_qpel16_mc20_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    avg_h264_qpel16_h_lowpass(p_dst, p_src, stride, stride);
}

void avg_h264_qpel16_mc21_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t temp[256] = {0};
    put_h264_qpel16_h_lowpass(temp, p_src, 16, stride);
    avg_h264_qpel16_hv_lowpass_l2(p_dst, p_src, temp, stride, 16);
}

void avg_h264_qpel16_mc22_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    avg_h264_qpel16_hv_lowpass(p_dst, p_src, stride);
}

void avg_h264_qpel16_mc23_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t temp[256] = {0};
    put_h264_qpel16_h_lowpass(temp, p_src + stride, 16, stride);
    avg_h264_qpel16_hv_lowpass_l2(p_dst, p_src, temp, stride, 16);
}

void avg_h264_qpel16_mc30_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    avg_h264_qpel16_h_lowpass_l2src_shift(p_dst, p_src, stride);
}

void avg_h264_qpel16_mc31_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t temp[256] = {0};
    put_h264_qpel16_h_lowpass(temp, p_src, 16, stride);
    avg_h264_qpel16_v_lowpass_l2(p_dst, p_src + 1, temp, stride, 16);    
}

void avg_h264_qpel16_mc32_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t temp[256] = {0};
    put_h264_qpel16_v_lowpass(temp, p_src + 1, 16, stride);
    avg_h264_qpel16_hv_lowpass_l2(p_dst, p_src, temp, stride, 16);
}

void avg_h264_qpel16_mc33_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t temp[256] = {0};
    put_h264_qpel16_h_lowpass(temp, p_src + stride, 16, stride);
    avg_h264_qpel16_v_lowpass_l2(p_dst, p_src + 1, temp, stride, 16);
}

void put_h264_qpel8_mc00_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    put_copy_block8(p_dst, p_src, stride);
}

void put_h264_qpel8_mc01_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    put_h264_qpel8_v_lowpass_l2src(p_dst, p_src, stride);
}

void put_h264_qpel8_mc02_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    put_h264_qpel8_v_lowpass(p_dst, p_src, stride, stride);
}

void put_h264_qpel8_mc03_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    put_h264_qpel8_v_lowpass_l2src_shift(p_dst, p_src, stride);
}

void put_h264_qpel8_mc10_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    put_h264_qpel8_h_lowpass_l2src(p_dst, p_src, stride);
}

void put_h264_qpel8_mc11_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t temp[64] = {0};
    put_h264_qpel8_h_lowpass(temp, p_src, 8, stride);
    put_h264_qpel8_v_lowpass_l2(p_dst, p_src, temp, stride, 8);
}

void put_h264_qpel8_mc12_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t temp[64] = {0};
    put_h264_qpel8_v_lowpass(temp, p_src, 8, stride);
    put_h264_qpel8_hv_lowpass_l2(p_dst, p_src, temp, stride, 8);
}

void put_h264_qpel8_mc13_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t temp[64] = {0};
    put_h264_qpel8_h_lowpass(temp, p_src + stride, 8, stride);
    put_h264_qpel8_v_lowpass_l2(p_dst, p_src, temp, stride, 8);
}

void put_h264_qpel8_mc20_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    put_h264_qpel8_h_lowpass(p_dst, p_src, stride, stride);
}

void put_h264_qpel8_mc21_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t temp[64] = {0};
    put_h264_qpel8_h_lowpass(temp, p_src, 8, stride);
    put_h264_qpel8_hv_lowpass_l2(p_dst, p_src, temp, stride, 8);
}

void put_h264_qpel8_mc22_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    put_h264_qpel8_hv_lowpass(p_dst, p_src, stride);
}

void put_h264_qpel8_mc23_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t temp[64] = {0};
    put_h264_qpel8_h_lowpass(temp, p_src + stride, 8, stride);
    put_h264_qpel8_hv_lowpass_l2(p_dst, p_src, temp, stride, 8);
}

void put_h264_qpel8_mc30_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    put_h264_qpel8_h_lowpass_l2src_shift(p_dst, p_src, stride);
}

void put_h264_qpel8_mc31_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t temp[64] = {0};
    put_h264_qpel8_h_lowpass(temp, p_src, 8, stride);
    put_h264_qpel8_v_lowpass_l2(p_dst, p_src + 1, temp, stride, 8);
}

void put_h264_qpel8_mc32_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t temp[64] = {0};
    put_h264_qpel8_v_lowpass(temp, p_src + 1, 8, stride);
    put_h264_qpel8_hv_lowpass_l2(p_dst, p_src, temp, stride, 8);
}

void put_h264_qpel8_mc33_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t temp[64] = {0};
    put_h264_qpel8_h_lowpass(temp, p_src + stride, 8, stride);
    put_h264_qpel8_v_lowpass_l2(p_dst, p_src + 1, temp, stride, 8);
}

void avg_h264_qpel8_mc00_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    avg_copy_block8(p_dst, p_src, stride);
}

void avg_h264_qpel8_mc01_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    avg_h264_qpel8_v_lowpass_l2src(p_dst, p_src, stride);
}

void avg_h264_qpel8_mc02_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    avg_h264_qpel8_v_lowpass(p_dst, p_src, stride, stride);
}

void avg_h264_qpel8_mc03_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    avg_h264_qpel8_v_lowpass_l2src_shift(p_dst, p_src, stride);
}

void avg_h264_qpel8_mc10_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    avg_h264_qpel8_h_lowpass_l2src(p_dst, p_src, stride);
}

void avg_h264_qpel8_mc11_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t temp[64] = {0};
    put_h264_qpel8_h_lowpass(temp, p_src, 8, stride);
    avg_h264_qpel8_v_lowpass_l2(p_dst, p_src, temp, stride, 8);
}

void avg_h264_qpel8_mc12_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t temp[64] = {0};
    put_h264_qpel8_v_lowpass(temp, p_src, 8, stride);
    avg_h264_qpel8_hv_lowpass_l2(p_dst, p_src, temp, stride, 8);
}

void avg_h264_qpel8_mc13_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t temp[64] = {0};
    put_h264_qpel8_h_lowpass(temp, p_src + stride, 8, stride);
    avg_h264_qpel8_v_lowpass_l2(p_dst, p_src, temp, stride, 8);
}

void avg_h264_qpel8_mc20_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    avg_h264_qpel8_h_lowpass(p_dst, p_src, stride, stride);
}

void avg_h264_qpel8_mc21_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t temp[64] = {0};
    put_h264_qpel8_h_lowpass(temp, p_src, 8, stride);
    avg_h264_qpel8_hv_lowpass_l2(p_dst, p_src, temp, stride, 8);
}

void avg_h264_qpel8_mc22_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    avg_h264_qpel8_hv_lowpass(p_dst, p_src, stride);
}

void avg_h264_qpel8_mc23_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t temp[64] = {0};
    put_h264_qpel8_h_lowpass(temp, p_src + stride, 8, stride);
    avg_h264_qpel8_hv_lowpass_l2(p_dst, p_src, temp, stride, 8);
}

void avg_h264_qpel8_mc30_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    avg_h264_qpel8_h_lowpass_l2src_shift(p_dst, p_src, stride);
}

void avg_h264_qpel8_mc31_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t temp[64] = {0};
    put_h264_qpel8_h_lowpass(temp, p_src, 8, stride);
    avg_h264_qpel8_v_lowpass_l2(p_dst, p_src + 1, temp, stride, 8);
}

void avg_h264_qpel8_mc32_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t temp[64] = {0};
    put_h264_qpel8_v_lowpass(temp, p_src + 1, 8, stride);
    avg_h264_qpel8_hv_lowpass_l2(p_dst, p_src, temp, stride, 8);
}

void avg_h264_qpel8_mc33_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride)
{
    uint8_t temp[64] = {0};
    put_h264_qpel8_h_lowpass(temp, p_src + stride, 8, stride);
    avg_h264_qpel8_v_lowpass_l2(p_dst, p_src + 1, temp, stride, 8);
}
#endif