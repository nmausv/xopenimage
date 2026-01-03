/* gamma.h */

/* Part of xopenimage project */
/*  portions derived from xloadimage */
/*   copyright 1993 Jim Frost, "X Consortium license" */
/*  modified under that license */
/*   modifications copyright 2026 Nicholas Maus, Douglas Maus */
/* xopenimage project under the "ISC license" */
/*  see LICENSE.txt */ 

#ifndef gamma_h
#define gamma_h

/**
 * @defgroup gamma  gamma routines
 * functions to change gamma 
 *
 * \#include "gamma.h"
 */

#include "../gimage.h" /* 'gImage' struct */

/** gammacorrect 
 * @ingroup gamma
 * @param[in,out] gimageP Image to correct gamma 
 * @param[in] gamma gamma value
 * @param[in] verbose flag for verbose output
 *
 * gamma corrects an Image (in-place)
 */
void gammacorrect(gImage *gimageP, float gamma, unsigned int verbose);

#endif

