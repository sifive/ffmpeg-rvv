#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include "config.h"

#if HAVE_RVV
typedef unsigned char pixel;

void pred8x8_vert_8_rvv(uint8_t *p_src, ptrdiff_t stride);
void pred8x8_hor_8_rvv(uint8_t *p_src, ptrdiff_t stride);
void pred8x8_plane_8_rvv(uint8_t *p_src, ptrdiff_t stride);
void pred8x8_128_dc_8_rvv(uint8_t *p_src, ptrdiff_t stride);
void pred8x8_top_dc_rvv(uint8_t *p_src, ptrdiff_t stride);
void pred8x8_left_dc_rvv(uint8_t *p_src, ptrdiff_t stride);
void pred8x8_dc_rvv(uint8_t *p_src, ptrdiff_t stride);
void pred8x8_l0t_dc_rvv(uint8_t *p_src, ptrdiff_t stride);
void pred8x8_0lt_dc_rvv(uint8_t *p_src, ptrdiff_t stride);
void pred8x8_l00_dc_rvv(uint8_t *p_src, ptrdiff_t stride);
void pred8x8_0l0_dc_rvv(uint8_t *p_src, ptrdiff_t stride);

void pred16x16_dc_8_rvv(uint8_t *p_src, ptrdiff_t stride);
void pred16x16_top_dc_8_rvv(uint8_t *p_src, ptrdiff_t stride);
void pred16x16_left_dc_8_rvv(uint8_t *p_src, ptrdiff_t stride);
void pred16x16_128_dc_8_rvv(uint8_t *p_src, ptrdiff_t stride);
void pred16x16_vert_8_rvv(uint8_t *p_src, ptrdiff_t stride);
void pred16x16_hor_8_rvv(uint8_t *p_src, ptrdiff_t stride);
void pred16x16_plane_8_rvv(uint8_t *p_src, ptrdiff_t stride);
#endif