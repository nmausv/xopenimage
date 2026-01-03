/* webp_fmt.h */

/* Part of xopenimage project */
/*  portions derived from xloadimage */
/*   copyright 1993 Jim Frost, "X Consortium license" */
/*  modified under that license */
/*   modifications copyright 2026 Nicholas Maus, Douglas Maus */
/* xopenimage project under the "ISC license" */
/*  see LICENSE.txt */ 

#ifndef webp_fmt_h
#define webp_fmt_h

/**
 * @defgroup webp WebP routines
 * functions to handle WebP format images
 *
 * \#include "webp_fmt.h"
 */

#include "../gimage.h" /* 'gImage' struct */


/** webpLoad
 * @ingroup webp
 * @param[in] filename filename
 * @param[in] verbose flag for verbose output
 * @return new gImage from WebP file, or NULL if error
 *
 * load a WebP file to gImage
 */
gImage* webpLoad(const char *filename, unsigned int verbose);


#endif

