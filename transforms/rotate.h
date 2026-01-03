/* rotate.h */

/* Part of xopenimage project */
/*  portions derived from xloadimage */
/*   copyright 1993 Jim Frost, "X Consortium license" */
/*  modified under that license */
/*   modifications copyright 2026 Nicholas Maus, Douglas Maus */
/* xopenimage project under the "ISC license" */
/*  see LICENSE.txt */ 

#ifndef rotate_h
#define rotate_h

/**
 * @defgroup rotate  rotate routines
 * functions to rotate
 *
 * \#include "rotate.h"
 */

#include "../gimage.h" /* 'gImage' struct */

/** rotate
 * @ingroup rotate
 * @param[in] gimageP gImage to rotate
 * @param[in] degrees 
 * @param[in] verbose flag for verbose output
 * @return new gImage that was rotate'd
 * 
 */
gImage* rotate(gImage *gimageP, unsigned int degrees, unsigned int verbose);

#endif

