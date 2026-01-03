/* xbitmap_fmt.h */

/* Part of xopenimage project */
/*  portions derived from xloadimage */
/*   copyright 1993 Jim Frost, "X Consortium license" */
/*  modified under that license */
/*   modifications copyright 2026 Nicholas Maus, Douglas Maus */
/* xopenimage project under the "ISC license" */
/*  see LICENSE.txt */ 

#ifndef xbitmap_fmt_h
#define xbitmap_fmt_h

/**
 * @defgroup xbitmap  XBitMap(XBM) routines
 * functions to handle XBitMap(XBM) format images
 *
 * \#include "xbitmap_fmt.h"
 */

#include "../gimage.h" /* 'gImage' struct */


/** xbitmapLoad
 * @ingroup xbitmap
 * @param[in] filename filename
 * @param[in] verbose flag for verbose output
 * @return new gImage from X11 BitMap (XBM) file, or NULL if error
 * 
 * load an X11 BitMap (XBM) file to gImage
 */
gImage* xbitmapLoad(const char *filename, unsigned int verbose);

#endif

