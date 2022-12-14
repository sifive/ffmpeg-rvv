#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include "config.h"

#if HAVE_RVV
typedef unsigned char pixel;

void put_h264_qpel16_mc00_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel16_mc01_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel16_mc02_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel16_mc03_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel16_mc10_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel16_mc11_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel16_mc12_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel16_mc13_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel16_mc20_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel16_mc21_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel16_mc22_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel16_mc23_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel16_mc30_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel16_mc31_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel16_mc32_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel16_mc33_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);

void avg_h264_qpel16_mc00_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel16_mc01_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel16_mc02_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel16_mc03_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel16_mc10_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel16_mc11_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel16_mc12_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel16_mc13_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel16_mc20_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel16_mc21_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel16_mc22_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel16_mc23_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel16_mc30_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel16_mc31_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel16_mc32_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel16_mc33_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);

void put_h264_qpel8_mc00_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel8_mc01_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel8_mc02_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel8_mc03_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel8_mc10_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel8_mc11_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel8_mc12_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel8_mc13_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel8_mc20_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel8_mc21_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel8_mc22_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel8_mc23_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel8_mc30_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel8_mc31_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel8_mc32_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void put_h264_qpel8_mc33_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);

void avg_h264_qpel8_mc00_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel8_mc01_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel8_mc02_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel8_mc03_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel8_mc10_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel8_mc11_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel8_mc12_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel8_mc13_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel8_mc20_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel8_mc21_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel8_mc22_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel8_mc23_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel8_mc30_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel8_mc31_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel8_mc32_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
void avg_h264_qpel8_mc33_8_rvv(uint8_t *p_dst, const uint8_t *p_src, ptrdiff_t stride);
#endif