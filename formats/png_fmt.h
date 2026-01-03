/* png_fmt.h */

/* Part of xopenimage project */
/*  portions derived from xloadimage */
/*   copyright 1993 Jim Frost, "X Consortium license" */
/*  modified under that license */
/*   modifications copyright 2026 Nicholas Maus, Douglas Maus */
/* xopenimage project under the "ISC license" */
/*  see LICENSE.txt */ 

#ifndef png_fmt_h
#define png_fmt_h

/**
 * @defgroup png PNG routines
 * functions to handle PNG format images
 *
 * \#include "png_fmt.h"
 */

#include "../gimage.h" /* 'gImage' struct */


/** pngload
 * @ingroup png
 * @param[in] filename filename
 * @param[in] verbose flag for verbose output
 * @return new gImage from PNG file, or NULL if error
 *
 * load a PNG file to gImage
 */
gImage* pngLoad(const char *filename, unsigned int verbose);


#endif

