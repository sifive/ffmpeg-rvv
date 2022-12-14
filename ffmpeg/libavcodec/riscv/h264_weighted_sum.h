#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include "config.h"

#if HAVE_RVV
typedef unsigned char pixel;

void weight_h264_pixels_16_8_rvv(uint8_t *p_block, ptrdiff_t stride,
                                 int height, int log2_den, int weight, int offset);
void weight_h264_pixels_8_8_rvv(uint8_t *p_block, ptrdiff_t stride,
                                 int height, int log2_den, int weight, int offset);
void weight_h264_pixels_4_8_rvv(uint8_t *p_block, ptrdiff_t stride,
                                 int height, int log2_den, int weight, int offset);  
                               
void biweight_h264_pixels_16_8_rvv(uint8_t *p_dst, uint8_t *p_src, ptrdiff_t stride,
                                   int height, int log2_den, int weightd, int weights, int offset);
void biweight_h264_pixels_8_8_rvv(uint8_t *p_dst, uint8_t *p_src, ptrdiff_t stride,
                                  int height, int log2_den, int weightd, int weights, int offset);
void biweight_h264_pixels_4_8_rvv(uint8_t *p_dst, uint8_t *p_src, ptrdiff_t stride,
                                  int height, int log2_den, int weightd, int weights, int offset);
#endif