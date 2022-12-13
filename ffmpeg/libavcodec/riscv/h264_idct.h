#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include "config.h"

#if HAVE_RVV
typedef unsigned char pixel;

void h264_idct_add_8_rvv(uint8_t *dst, int16_t *block, int stride);
void h264_idct_dc_add_8_rvv(uint8_t *p_dst, int16_t *p_block, int stride);
void h264_idct_add16_8_rvv(uint8_t *p_dst, const int *p_block_offset, int16_t *p_block, int stride,
                           const uint8_t nnzc[5 * 8]);
void h264_idct_add16_intra_8_rvv(uint8_t *p_dst, const int *p_block_offset, int16_t *p_block, int stride,
                                 const uint8_t nnzc[5 * 8]);
void h264_idct_add8_8_rvv(uint8_t **p_dst, const int *p_block_offset, int16_t *p_block, int stride,
                          const uint8_t nnzc[15*8]);
void h264_idct8_add_8_rvv(uint8_t *_dst, int16_t *_block, int stride);
void h264_idct8_dc_add_8_rvv(uint8_t *p_dst, int16_t *p_block, int stride);
void h264_idct8_add4_8_rvv(uint8_t *dst, const int *block_offset,
                           int16_t *block, int stride, const uint8_t nnzc[5 * 8]);
#endif