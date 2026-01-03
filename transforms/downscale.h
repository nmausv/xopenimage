/* downscale.h */

/* Part of xopenimage project */
/*  portions derived from xloadimage */
/*   copyright 1993 Jim Frost, "X Consortium license" */
/*  modified under that license */
/*   modifications copyright 2026 Nicholas Maus, Douglas Maus */
/* xopenimage project under the "ISC license" */
/*  see LICENSE.txt */ 

#ifndef downscale_h
#define downscale_h

/** downscale */
/* assumes RGB in range [0,1] (not integer 0-255) */
/* ideally would be linearized RGB floating point */
/* assumes 2 dimensional square pixels */
/* assumes 3 components (no good for images with alpha or grayscale) */
/* process along rowlines = x-direction */
/* return 0 on success (no error), -1 on error */
int downscale(unsigned char samp_per_pixel,
 const float *src, unsigned int src_xdim, unsigned int src_ydim,
 float *dst, unsigned int dst_xdim, unsigned int dst_ydim);


#endif

