/* tiff_fmt.h */

/* Part of xopenimage project */
/*  portions derived from xloadimage */
/*   copyright 1993 Jim Frost, "X Consortium license" */
/*  modified under that license */
/*   modifications copyright 2026 Nicholas Maus, Douglas Maus */
/* xopenimage project under the "ISC license" */
/*  see LICENSE.txt */ 

#ifndef tiff_fmt_h
#define tiff_fmt_h

/**
 * @defgroup tiff TIFF routines
 * functions to handle TIFF format images
 *
 * \#include "tiff_fmt.h"
 */

#include "../gimage.h" /* 'gImage' struct */


/** tiffload
 * @ingroup tiff
 * @param[in] filename filename
 * @param[in] verbose flag for verbose output
 * @return new gImage from TIFF file, or NULL if error
 *
 * load a TIFF file to gImage
 */
gImage* tiffLoad(const char *filename, unsigned int verbose);


#endif

