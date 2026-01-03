/* netpbm_fmt.h */

/* Part of xopenimage project */
/*  portions derived from xloadimage */
/*   copyright 1993 Jim Frost, "X Consortium license" */
/*  modified under that license */
/*   modifications copyright 2026 Nicholas Maus, Douglas Maus */
/* xopenimage project under the "ISC license" */
/*  see LICENSE.txt */ 

#ifndef netpbm_fmt_h
#define netpbm_fmt_h

/**
 * @defgroup netpbm  NetPBM routines
 * functions for NetPBM format images
 *
 * \#include "netpbm_fmt.h"
 */

#include "../gimage.h" /* 'gImage' struct */


/** pbmLoad
 * @ingroup netpbm
 * @param[in] filename filename
 * @param[in] verbose flag for verbose output
 * @return new gImage from NetPBM file, or NULL if error
 * 
 * load an NetPBM file to gImage
 */
gImage* pbmLoad(const char *filename, unsigned int verbose);


#endif

