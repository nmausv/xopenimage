/* jpeg_fmt.h */

/* Part of xopenimage project */
/*  portions derived from xloadimage */
/*   copyright 1993 Jim Frost, "X Consortium license" */
/*  modified under that license */
/*   modifications copyright 2026 Nicholas Maus, Douglas Maus */
/* xopenimage project under the "ISC license" */
/*  see LICENSE.txt */ 

#ifndef jpeg_fmt_h
#define jpeg_fmt_h

/**
 * @defgroup jpeg JPEG routines
 * functions to handle JPEG format images
 *
 * \#include "jpeg_fmt.h"
 */

#include "../gimage.h" /* 'gImage' struct */


/** jpegload
 * @ingroup jpeg
 * @param[in] filename filename
 * @param[in] verbose flag for verbose output
 * @return new gImage from JPEG file, or NULL if error
 *
 * load a JPEG file to gImage
 */
gImage* jpegLoad(const char *filename, unsigned int verbose);


#endif

