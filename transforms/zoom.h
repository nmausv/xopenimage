/* zoom.h */

/* Part of xopenimage project */
/*  portions derived from xloadimage */
/*   copyright 1993 Jim Frost, "X Consortium license" */
/*  modified under that license */
/*   modifications copyright 2026 Nicholas Maus, Douglas Maus */
/* xopenimage project under the "ISC license" */
/*  see LICENSE.txt */ 

#ifndef zoom_h
#define zoom_h

/**
 * @defgroup zoom  zoom routines
 * functions to zoom
 *
 * \#include "zoom.h"
 */

#include "../gimage.h" /* 'gImage' struct */

/** zoom 
 * @ingroup zoom
 * @param[in,out] ingimageP gImage to zoom
 * @param[in] xzoom  percentage ("200" is 200%)
 * @param[in] yzoom  percentage ("50" is 50%)
 * @param[in] verbose flag for verbose output
 * @return new gImage that was zoom'd
 * 
 */
gImage* zoom(gImage *ingimageP, unsigned int xzoom, unsigned int yzoom, unsigned int verbose);

#endif

