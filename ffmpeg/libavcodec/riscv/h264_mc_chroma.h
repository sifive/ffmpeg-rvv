#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include "config.h"

#if HAVE_RVV
typedef unsigned char pixel;

void h264_put_chroma_mc8_rvv(uint8_t *p_dst, uint8_t *p_src, ptrdiff_t stride, int h, int x, int y);
void h264_avg_chroma_mc8_rvv(uint8_t *p_dst, uint8_t *p_src, ptrdiff_t stride, int h, int x, int y);
void h264_put_chroma_mc4_rvv(uint8_t *p_dst, uint8_t *p_src, ptrdiff_t stride, int h, int x, int y);
void h264_avg_chroma_mc4_rvv(uint8_t *p_dst, uint8_t *p_src, ptrdiff_t stride, int h, int x, int y);
void h264_put_chroma_mc2_rvv(uint8_t *p_dst, uint8_t *p_src, ptrdiff_t stride, int h, int x, int y);
void h264_avg_chroma_mc2_rvv(uint8_t *p_dst, uint8_t *p_src, ptrdiff_t stride, int h, int x, int y);
#endif
