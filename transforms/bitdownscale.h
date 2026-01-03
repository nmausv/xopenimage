/* bitdownscale.h */

/* Part of xopenimage project */
/*  portions derived from xloadimage */
/*   copyright 1993 Jim Frost, "X Consortium license" */
/*  modified under that license */
/*   modifications copyright 2026 Nicholas Maus, Douglas Maus */
/* xopenimage project under the "ISC license" */
/*  see LICENSE.txt */ 

#ifndef bitdownscale_h
#define bitdownscale_h

/**
 * formats:
 *  src and dst are bitmaps
 *  image start with top left
 *   first along x direction (left to right)
 *   then along y direction (top to bottom)
 *  left most image bit is the least significant bit
 *   e.g. if first byte is 130
 *    01000001
 *  requirements: none of xdim/ydim can be zero
 *   dst_xdim <= src_xdim
 *   dst_ydim <= src_ydim
 */

int bitdownscale(const unsigned char *src,
 unsigned int src_xdim, unsigned int src_ydim,
 unsigned char *dst,
 unsigned int dst_xdim, unsigned int dst_ydim);

#endif

