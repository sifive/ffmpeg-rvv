#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include "config.h"

#if HAVE_RVV
typedef unsigned char pixel;

void h264_v_loop_filter_luma_8_rvv(uint8_t *pix, ptrdiff_t stride, int alpha, int beta, int8_t *tc0);
void h264_h_loop_filter_luma_8_rvv(uint8_t *pix, ptrdiff_t stride, int alpha, int beta, int8_t *tc0);

void h264_v_loop_filter_chroma_8_rvv(uint8_t *pix, ptrdiff_t stride, int alpha, int beta, int8_t *tc0);
void h264_h_loop_filter_chroma_8_rvv(uint8_t *pix, ptrdiff_t stride, int alpha, int beta, int8_t *tc0);

void h264_v_loop_filter_luma_intra_8_rvv(uint8_t *p_pix, ptrdiff_t stride, int alpha, int beta);
void h264_h_loop_filter_luma_intra_8_rvv(uint8_t *p_pix, ptrdiff_t stride, int alpha, int beta);

void h264_v_loop_filter_chroma_intra_8_rvv(uint8_t *p_pix, ptrdiff_t stride, int alpha, int beta);
void h264_h_loop_filter_chroma_intra_8_rvv(uint8_t *p_pix, ptrdiff_t stride, int alpha, int beta);

void h264_h_loop_filter_chroma_mbaff_intra_8_rvv(uint8_t *p_pix, ptrdiff_t stride, int alpha, int beta);
#endif